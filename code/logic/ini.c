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
#include "fossil/media/ini.h"
#include "fossil/media/media.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static char *strdup_trim(const char *src) {
    if (!src) return NULL;
    while (isspace((unsigned char)*src)) src++; // trim left
    size_t len = strlen(src);
    while (len > 0 && isspace((unsigned char)src[len-1])) len--; // trim right
    char *out = (char *)malloc(len + 1);
    if (!out) return NULL;
    memcpy(out, src, len);
    out[len] = '\0';
    return out;
}

static void remove_inline_comment(char *line) {
    char *comment = strpbrk(line, ";#");
    if (comment) *comment = '\0';
}

static fossil_media_ini_section_t *find_section(fossil_media_ini_t *ini, const char *name) {
    for (size_t i = 0; i < ini->section_count; i++) {
        if (strcmp(ini->sections[i].name, name) == 0) {
            return &ini->sections[i];
        }
    }
    return NULL;
}

static fossil_media_ini_entry_t *find_entry(fossil_media_ini_section_t *section, const char *key) {
    for (size_t i = 0; i < section->entry_count; i++) {
        if (strcmp(section->entries[i].key, key) == 0) {
            return &section->entries[i];
        }
    }
    return NULL;
}

int fossil_media_ini_load_string(const char *data, fossil_media_ini_t *ini) {
    memset(ini, 0, sizeof(*ini));
    if (!data || *data == '\0')
        return 0;

    fossil_media_ini_section_t *current_section = NULL;
    const char *line_start = data;
    char *multiline_key = NULL;
    char *multiline_value = NULL;
    int multiline_quote = 0;

    while (*line_start) {
        const char *line_end = strchr(line_start, '\n');
        size_t line_len = line_end ? (size_t)(line_end - line_start) : strlen(line_start);

        char *line = (char *)malloc(line_len + 1);
        memcpy(line, line_start, line_len);
        line[line_len] = '\0';

        if (!multiline_quote)
            remove_inline_comment(line);

        char *trimmed = strdup_trim(line);
        free(line);

        // --- Handle multiline quoted value ---
        if (multiline_quote) {
            size_t tlen = strlen(trimmed);
            char *end_quote = NULL;
            for (size_t i = 0; i < tlen; ++i) {
                if (trimmed[i] == multiline_quote) {
                    end_quote = &trimmed[i];
                    break;
                }
            }

            size_t oldlen = strlen(multiline_value);
            size_t addlen = strlen(trimmed);
            multiline_value = realloc(multiline_value, oldlen + addlen + 2);
            multiline_value[oldlen] = '\n';
            memcpy(multiline_value + oldlen + 1, trimmed, addlen + 1);

            if (end_quote) {
                *end_quote = '\0';
                // End of multiline value: store key/value
                current_section->entries = realloc(
                    current_section->entries,
                    sizeof(*current_section->entries) * (current_section->entry_count + 1)
                );
                fossil_media_ini_entry_t *entry =
                    &current_section->entries[current_section->entry_count++];
                entry->key = multiline_key;
                entry->value = multiline_value;
                multiline_key = NULL;
                multiline_value = NULL;
                multiline_quote = 0;
            }

            free(trimmed);
            if (!line_end) break;
            line_start = line_end + 1;
            continue; // <-- clean jump to next line
        }

        // --- Skip blanks and comments ---
        if (!trimmed || *trimmed == '\0') {
            free(trimmed);
            if (!line_end) break;
            line_start = line_end + 1;
            continue;
        }

        // --- Section header ---
        if (*trimmed == '[') {
            char *end = strchr(trimmed, ']');
            if (end) {
                *end = '\0';
                char *section_name = strdup_trim(trimmed + 1);
                if (section_name && *section_name) {
                    ini->sections = realloc(
                        ini->sections,
                        sizeof(*ini->sections) * (ini->section_count + 1)
                    );
                    current_section = &ini->sections[ini->section_count++];
                    current_section->name = section_name;
                    current_section->entries = NULL;
                    current_section->entry_count = 0;
                } else {
                    free(section_name);
                    current_section = NULL;
                }
            }
            free(trimmed);
            if (!line_end) break;
            line_start = line_end + 1;
            continue;
        }

        // --- Key=value pair (or ignored key) ---
        if (current_section) {
            char *eq = strchr(trimmed, '=');
            if (eq) {
                *eq = '\0';
                char *key = strdup_trim(trimmed);
                char *value = strdup_trim(eq + 1);

                // Handle quoted (possibly multiline) values
                if (value && (*value == '"' || *value == '\'')) {
                    char quote = *value;
                    size_t vlen = strlen(value);
                    if (vlen > 1 && value[vlen - 1] == quote) {
                        value[vlen - 1] = '\0';
                        memmove(value, value + 1, vlen - 1);
                    } else {
                        memmove(value, value + 1, vlen); // remove leading quote
                        multiline_key = key;
                        multiline_value = fossil_media_strdup(value);
                        multiline_quote = quote;
                        free(value);
                        free(trimmed);
                        if (!line_end) break;
                        line_start = line_end + 1;
                        continue;
                    }
                }

                fossil_media_ini_entry_t *entry = find_entry(current_section, key);
                if (entry) {
                    free(entry->value);
                    entry->value = value;
                    free(key);
                } else {
                    current_section->entries = realloc(
                        current_section->entries,
                        sizeof(*current_section->entries) * (current_section->entry_count + 1)
                    );
                    entry = &current_section->entries[current_section->entry_count++];
                    entry->key = key;
                    entry->value = value;
                }
            }
        }

        free(trimmed);
        if (!line_end) break;
        line_start = line_end + 1;
    }

    // Handle EOF during multiline quoted value
    if (multiline_quote && current_section && multiline_key && multiline_value) {
        current_section->entries = realloc(
            current_section->entries,
            sizeof(*current_section->entries) * (current_section->entry_count + 1)
        );
        fossil_media_ini_entry_t *entry =
            &current_section->entries[current_section->entry_count++];
        entry->key = multiline_key;
        entry->value = multiline_value;
    }

    return 0;
}

int fossil_media_ini_load_file(const char *path, fossil_media_ini_t *ini) {
    FILE *f = fopen(path, "rb");
    if (!f) return -1;

    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        return -1;
    }

    long fsize = ftell(f);
    if (fsize < 0) {
        fclose(f);
        return -1;
    }
    rewind(f);

    size_t size = (size_t)fsize;
    char *buf = malloc(size + 1);
    if (!buf) {
        fclose(f);
        return -1;
    }

    size_t nread = fread(buf, 1, size, f);
    fclose(f);

    if (nread != size) {
        free(buf);
        return -1;
    }

    buf[size] = '\0';

    int res = fossil_media_ini_load_string(buf, ini);
    free(buf);
    return res;
}

const char *fossil_media_ini_get(const fossil_media_ini_t *ini, const char *section, const char *key) {
    if (!ini || !section || !key) return NULL;
    fossil_media_ini_section_t *sec = find_section((fossil_media_ini_t *)ini, section);
    if (!sec) return NULL;
    fossil_media_ini_entry_t *entry = find_entry(sec, key);
    return entry ? entry->value : NULL;
}

int fossil_media_ini_set(fossil_media_ini_t *ini, const char *section, const char *key, const char *value) {
    if (!ini || !section || !key || !value) return -1;
    fossil_media_ini_section_t *sec = find_section(ini, section);
    if (!sec) {
        ini->sections = realloc(ini->sections, sizeof(*ini->sections) * (ini->section_count + 1));
        sec = &ini->sections[ini->section_count++];
        sec->name = fossil_media_strdup(section);
        sec->entries = NULL;
        sec->entry_count = 0;
    }
    fossil_media_ini_entry_t *entry = find_entry(sec, key);
    if (!entry) {
        sec->entries = realloc(sec->entries, sizeof(*sec->entries) * (sec->entry_count + 1));
        entry = &sec->entries[sec->entry_count++];
        entry->key = fossil_media_strdup(key);
        entry->value = fossil_media_strdup(value);
    } else {
        free(entry->value);
        entry->value = fossil_media_strdup(value);
    }
    return 0;
}

int fossil_media_ini_save_file(const char *path, const fossil_media_ini_t *ini) {
    FILE *f = fopen(path, "wb");
    if (!f) return -1;
    for (size_t i = 0; i < ini->section_count; i++) {
        fprintf(f, "[%s]\n", ini->sections[i].name);
        for (size_t j = 0; j < ini->sections[i].entry_count; j++) {
            fprintf(f, "%s=%s\n", ini->sections[i].entries[j].key, ini->sections[i].entries[j].value);
        }
        fprintf(f, "\n");
    }
    fclose(f);
    return 0;
}

void fossil_media_ini_free(fossil_media_ini_t *ini) {
    if (!ini || !ini->sections) return;
    for (size_t i = 0; i < ini->section_count; i++) {
        if (ini->sections[i].name) free(ini->sections[i].name);
        if (ini->sections[i].entries) {
            for (size_t j = 0; j < ini->sections[i].entry_count; j++) {
                if (ini->sections[i].entries[j].key) free(ini->sections[i].entries[j].key);
                if (ini->sections[i].entries[j].value) free(ini->sections[i].entries[j].value);
            }
            free(ini->sections[i].entries);
        }
    }
    free(ini->sections);
    ini->sections = NULL;
    ini->section_count = 0;
    memset(ini, 0, sizeof(*ini));
}
