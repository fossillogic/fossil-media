/**
 * -----------------------------------------------------------------------------
 * Project: Fossil Logic
 *
 * This file is part of the Fossil Logic project, which aims to develop
 * high-performance, cross-platform applications and libraries. The code
 * contained herein is licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License. You may obtain
 * a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 * Author: Michael Gene Brockus (Dreamer)
 * Date: 04/05/2014
 *
 * Copyright (C) 2014-2025 Fossil Logic. All rights reserved.
 * -----------------------------------------------------------------------------
 */
#include "fossil/media/csv.h"
#include "fossil/media/media.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Internal: add a field to a row */
static int csv_row_add_field(fossil_media_csv_row_t *row, const char *field) {
    char **new_fields = realloc(row->fields, (row->field_count + 1) * sizeof(char *));
    if (!new_fields) return -1;
    row->fields = new_fields;
    row->fields[row->field_count] = fossil_media_strdup(field ? field : "");
    if (!row->fields[row->field_count]) return -1;
    row->field_count++;
    return 0;
}

/* Enhanced CSV parser: handles quoted fields, embedded newlines, whitespace, empty fields, trailing newlines, custom delimiter */
fossil_media_csv_doc_t *fossil_media_csv_parse(const char *csv_text, char delimiter, fossil_media_csv_error_t *err_out) {
    if (err_out) *err_out = FOSSIL_MEDIA_CSV_OK;
    if (!csv_text) {
        if (err_out) *err_out = FOSSIL_MEDIA_CSV_ERR_INVALID_ARG;
        return NULL;
    }

    fossil_media_csv_doc_t *doc = calloc(1, sizeof(*doc));
    if (!doc) {
        if (err_out) *err_out = FOSSIL_MEDIA_CSV_ERR_MEMORY;
        return NULL;
    }

    const char *p = csv_text;
    fossil_media_csv_row_t current_row = {NULL, 0};
    char buffer[4096];
    size_t buf_len = 0;
    int in_quotes = 0;
    int field_started = 0;
    int error = 0;

    while (*p && !error) {
        char c = *p;

        if (in_quotes) {
            if (c == '"') {
                if (*(p + 1) == '"') { /* Escaped quote */
                    if (buf_len < sizeof(buffer) - 1) {
                        buffer[buf_len++] = '"';
                        p += 2;
                        continue;
                    } else {
                        error = 1;
                        break;
                    }
                } else {
                    in_quotes = 0;
                    p++;
                    continue;
                }
            } else {
                if (buf_len < sizeof(buffer) - 1) {
                    buffer[buf_len++] = c;
                    p++;
                    continue;
                } else {
                    error = 1;
                    break;
                }
            }
        } else {
            if (c == '"') {
                in_quotes = 1;
                field_started = 1;
                p++;
                continue;
            } else if (c == delimiter) {
                buffer[buf_len] = '\0';
                if (csv_row_add_field(&current_row, buffer) < 0) {
                    error = 1;
                    break;
                }
                buf_len = 0;
                field_started = 0;
                p++;
                continue;
            } else if (c == '\n' || c == '\r') {
                buffer[buf_len] = '\0';
                if (field_started || buf_len > 0 || current_row.field_count > 0 || c == '\n' || c == '\r') {
                    if (csv_row_add_field(&current_row, buffer) < 0) {
                        error = 1;
                        break;
                    }
                }
                buf_len = 0;
                field_started = 0;

                fossil_media_csv_row_t *new_rows = realloc(doc->rows, (doc->row_count + 1) * sizeof(*doc->rows));
                if (!new_rows) {
                    error = 1;
                    break;
                }
                doc->rows = new_rows;
                doc->rows[doc->row_count++] = current_row;
                current_row.fields = NULL;
                current_row.field_count = 0;

                if (c == '\r' && *(p + 1) == '\n') p++;
                p++;
                continue;
            } else if (isspace((unsigned char)c) && !field_started) {
                p++;
                continue;
            } else {
                if (buf_len < sizeof(buffer) - 1) {
                    buffer[buf_len++] = c;
                    field_started = 1;
                    p++;
                    continue;
                } else {
                    error = 1;
                    break;
                }
            }
        }
    }

    if (!error && (buf_len > 0 || field_started || current_row.field_count > 0)) {
        buffer[buf_len] = '\0';
        if (csv_row_add_field(&current_row, buffer) < 0) {
            error = 1;
        } else {
            fossil_media_csv_row_t *new_rows = realloc(doc->rows, (doc->row_count + 1) * sizeof(*doc->rows));
            if (!new_rows) {
                error = 1;
            } else {
                doc->rows = new_rows;
                doc->rows[doc->row_count++] = current_row;
            }
        }
    }

    if (!error && doc->row_count == 0 && *csv_text) {
        const char *q = csv_text;
        while (*q) {
            if (*q == '\n' || *q == '\r') {
                fossil_media_csv_row_t empty_row = {NULL, 0};
                fossil_media_csv_row_t *new_rows = realloc(doc->rows, (doc->row_count + 1) * sizeof(*doc->rows));
                if (!new_rows) {
                    error = 1;
                    break;
                }
                doc->rows = new_rows;
                doc->rows[doc->row_count++] = empty_row;
                if (*q == '\r' && *(q + 1) == '\n') q++;
            }
            q++;
        }
    }

    if (!error && doc->row_count == 1 && doc->rows[0].field_count == 1 && doc->rows[0].fields[0][0] == '\0' && csv_text[0] == '\0') {
        free(doc->rows[0].fields[0]);
        free(doc->rows[0].fields);
        free(doc->rows);
        doc->rows = NULL;
        doc->row_count = 0;
    }

    if (error) {
        if (err_out) *err_out = FOSSIL_MEDIA_CSV_ERR_MEMORY;
        fossil_media_csv_free(doc);
        for (size_t i = 0; i < current_row.field_count; i++) free(current_row.fields[i]);
        free(current_row.fields);
        return NULL;
    }

    return doc;
}

/* Free CSV doc */
void fossil_media_csv_free(fossil_media_csv_doc_t *doc) {
    if (!doc) return;
    for (size_t i = 0; i < doc->row_count; i++) {
        fossil_media_csv_row_t *row = &doc->rows[i];
        for (size_t j = 0; j < row->field_count; j++) {
            free(row->fields[j]);
        }
        free(row->fields);
    }
    free(doc->rows);
    free(doc);
}

/* Append a row */
int fossil_media_csv_append_row(fossil_media_csv_doc_t *doc, const char **fields, size_t field_cnt) {
    if (!doc || (!fields && field_cnt > 0)) return -1;
    fossil_media_csv_row_t row = {NULL, 0};
    for (size_t i = 0; i < field_cnt; i++) {
        if (csv_row_add_field(&row, fields[i]) < 0) {
            for (size_t j = 0; j < row.field_count; j++) free(row.fields[j]);
            free(row.fields);
            return -1;
        }
    }
    fossil_media_csv_row_t *new_rows = realloc(doc->rows, (doc->row_count + 1) * sizeof(*doc->rows));
    if (!new_rows) return -1;
    doc->rows = new_rows;
    doc->rows[doc->row_count++] = row;
    return 0;
}

/* Stringify CSV doc */
char *fossil_media_csv_stringify(const fossil_media_csv_doc_t *doc, char delimiter, fossil_media_csv_error_t *err_out) {
    if (err_out) *err_out = FOSSIL_MEDIA_CSV_OK;
    if (!doc) {
        if (err_out) *err_out = FOSSIL_MEDIA_CSV_ERR_INVALID_ARG;
        return NULL;
    }

    size_t cap = 1024;
    char *out = malloc(cap);
    if (!out) {
        if (err_out) *err_out = FOSSIL_MEDIA_CSV_ERR_MEMORY;
        return NULL;
    }
    size_t len = 0;

    for (size_t r = 0; r < doc->row_count; r++) {
        for (size_t f = 0; f < doc->rows[r].field_count; f++) {
            const char *field = doc->rows[r].fields[f];
            int needs_quotes = strchr(field, delimiter) || strchr(field, '"') || strchr(field, '\n');
            if (needs_quotes) {
                if (len + 1 >= cap) { cap *= 2; out = realloc(out, cap); if (!out) return NULL; }
                out[len++] = '"';
                for (const char *p = field; *p; p++) {
                    if (*p == '"') {
                        if (len + 2 >= cap) { cap *= 2; out = realloc(out, cap); if (!out) return NULL; }
                        out[len++] = '"';
                        out[len++] = '"';
                    } else {
                        if (len + 1 >= cap) { cap *= 2; out = realloc(out, cap); if (!out) return NULL; }
                        out[len++] = *p;
                    }
                }
                if (len + 1 >= cap) { cap *= 2; out = realloc(out, cap); if (!out) return NULL; }
                out[len++] = '"';
            } else {
                size_t flen = strlen(field);
                if (len + flen >= cap) { while (len + flen >= cap) cap *= 2; out = realloc(out, cap); if (!out) return NULL; }
                memcpy(out + len, field, flen);
                len += flen;
            }
            if (f < doc->rows[r].field_count - 1) {
                if (len + 1 >= cap) { cap *= 2; out = realloc(out, cap); if (!out) return NULL; }
                out[len++] = delimiter;
            }
        }
        if (len + 1 >= cap) { cap *= 2; out = realloc(out, cap); if (!out) return NULL; }
        out[len++] = '\n';
    }

    out[len] = '\0';
    return out;
}
