/*
 * -----------------------------------------------------------------------------
 * Project: Fossil Logic
 *
 * This file is part of the Fossil Logic project, which aims to develop high-
 * performance, cross-platform applications and libraries. The code contained
 * herein is subject to the terms and conditions defined in the project license.
 *
 * Author: Michael Gene Brockus (Dreamer)
 *
 * Copyright (C) 2024 Fossil Logic. All rights reserved.
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

/* Parse CSV text */
fossil_media_csv_doc_t *
fossil_media_csv_parse(const char *csv_text, char delimiter, fossil_media_csv_error_t *err_out) {
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

    while (*p) {
        char c = *p++;

        if (in_quotes) {
            if (c == '"') {
                if (*p == '"') { /* Escaped quote */
                    buffer[buf_len++] = '"';
                    p++;
                } else {
                    in_quotes = 0; /* End quote */
                }
            } else {
                buffer[buf_len++] = c;
            }
        } else {
            if (c == '"') {
                in_quotes = 1;
            } else if (c == delimiter) {
                buffer[buf_len] = '\0';
                if (csv_row_add_field(&current_row, buffer) < 0) goto fail;
                buf_len = 0;
            } else if (c == '\n' || c == '\r') {
                /* End of row */
                buffer[buf_len] = '\0';
                if (csv_row_add_field(&current_row, buffer) < 0) goto fail;
                buf_len = 0;

                /* Append row */
                fossil_media_csv_row_t *new_rows = realloc(doc->rows, (doc->row_count + 1) * sizeof(*doc->rows));
                if (!new_rows) goto fail;
                doc->rows = new_rows;
                doc->rows[doc->row_count++] = current_row;
                current_row.fields = NULL;
                current_row.field_count = 0;

                /* Skip CRLF pairs */
                if (c == '\r' && *p == '\n') p++;
            } else {
                buffer[buf_len++] = c;
            }
        }

        if (buf_len >= sizeof(buffer) - 1) goto fail; /* Field too long */
    }

    /* Final field/row if not empty */
    if (buf_len > 0 || current_row.field_count > 0) {
        buffer[buf_len] = '\0';
        if (csv_row_add_field(&current_row, buffer) < 0) goto fail;
        fossil_media_csv_row_t *new_rows = realloc(doc->rows, (doc->row_count + 1) * sizeof(*doc->rows));
        if (!new_rows) goto fail;
        doc->rows = new_rows;
        doc->rows[doc->row_count++] = current_row;
    }

    return doc;

fail:
    if (err_out) *err_out = FOSSIL_MEDIA_CSV_ERR_MEMORY;
    fossil_media_csv_free(doc);
    for (size_t i = 0; i < current_row.field_count; i++) free(current_row.fields[i]);
    free(current_row.fields);
    return NULL;
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
