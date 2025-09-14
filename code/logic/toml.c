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
#include "fossil/media/toml.h"
#include "fossil/media/media.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Trim whitespace in-place */
static char *trim_whitespace(char *str) {
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
    return str;
}

/* Internal function to add a new table */
static fossil_media_toml_table_t *add_table(fossil_media_toml_t *toml, const char *name) {
    toml->tables = realloc(toml->tables, sizeof(fossil_media_toml_table_t) * (toml->table_count + 1));
    fossil_media_toml_table_t *table = &toml->tables[toml->table_count++];
    table->name = name ? fossil_media_strdup(name) : NULL;
    table->entries = NULL;
    table->entry_count = 0;
    return table;
}

/* Internal function to add entry to a table */
static void add_entry(fossil_media_toml_table_t *table, const char *key, const char *value) {
    table->entries = realloc(table->entries, sizeof(fossil_media_toml_entry_t) * (table->entry_count + 1));
    fossil_media_toml_entry_t *entry = &table->entries[table->entry_count++];
    entry->key = fossil_media_strdup(key);
    entry->value = fossil_media_strdup(value);
}

int fossil_media_toml_parse(const char *input, fossil_media_toml_t *out_toml) {
    memset(out_toml, 0, sizeof(*out_toml));

    char *data = fossil_media_strdup(input);
    char *line = strtok(data, "\n");
    fossil_media_toml_table_t *current_table = add_table(out_toml, NULL);

    while (line) {
        char *trimmed = trim_whitespace(line);

        if (*trimmed == '#' || *trimmed == '\0') {
            line = strtok(NULL, "\n");
            continue;
        }

        if (*trimmed == '[' && trimmed[strlen(trimmed) - 1] == ']') {
            trimmed[strlen(trimmed) - 1] = '\0';
            char *table_name = trim_whitespace(trimmed + 1);
            current_table = add_table(out_toml, table_name);
        }
        else {
            char *eq = strchr(trimmed, '=');
            if (eq) {
                *eq = '\0';
                char *key = trim_whitespace(trimmed);
                char *value = trim_whitespace(eq + 1);
                if (*value == '"' && value[strlen(value) - 1] == '"') {
                    value[strlen(value) - 1] = '\0';
                    value++;
                }
                add_entry(current_table, key, value);
            }
        }

        line = strtok(NULL, "\n");
    }

    free(data);
    return 0;
}

const char *fossil_media_toml_get(const fossil_media_toml_t *toml, const char *table_name, const char *key) {
    for (size_t i = 0; i < toml->table_count; i++) {
        fossil_media_toml_table_t *table = &toml->tables[i];
        if ((table_name == NULL && table->name == NULL) ||
            (table_name && table->name && strcmp(table->name, table_name) == 0)) {
            for (size_t j = 0; j < table->entry_count; j++) {
                if (strcmp(table->entries[j].key, key) == 0) {
                    return table->entries[j].value;
                }
            }
        }
    }
    return NULL;
}

void fossil_media_toml_free(fossil_media_toml_t *toml) {
    for (size_t i = 0; i < toml->table_count; i++) {
        fossil_media_toml_table_t *table = &toml->tables[i];
        free(table->name);
        for (size_t j = 0; j < table->entry_count; j++) {
            free(table->entries[j].key);
            free(table->entries[j].value);
        }
        free(table->entries);
    }
    free(toml->tables);
    memset(toml, 0, sizeof(*toml));
}
