/**
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
    fossil_media_ini_section_t *current_section = NULL;

    const char *line_start = data;
    while (*line_start) {
        const char *line_end = strchr(line_start, '\n');
        size_t line_len = line_end ? (size_t)(line_end - line_start) : strlen(line_start);

        char *line = (char *)malloc(line_len + 1);
        memcpy(line, line_start, line_len);
        line[line_len] = '\0';

        // Trim
        char *trimmed = strdup_trim(line);
        free(line);

        // Skip blank and comments
        if (*trimmed == '\0' || *trimmed == ';' || *trimmed == '#') {
            free(trimmed);
            goto next_line;
        }

        // Section header
        if (*trimmed == '[') {
            char *end = strchr(trimmed, ']');
            if (end) {
                *end = '\0';
                ini->sections = realloc(ini->sections, sizeof(*ini->sections) * (ini->section_count + 1));
                current_section = &ini->sections[ini->section_count++];
                current_section->name = strdup_trim(trimmed + 1);
                current_section->entries = NULL;
                current_section->entry_count = 0;
            }
        }
        // Key=value pair
        else if (current_section) {
            char *eq = strchr(trimmed, '=');
            if (eq) {
                *eq = '\0';
                char *key = strdup_trim(trimmed);
                char *value = strdup_trim(eq + 1);

                current_section->entries = realloc(
                    current_section->entries,
                    sizeof(*current_section->entries) * (current_section->entry_count + 1)
                );
                fossil_media_ini_entry_t *entry = &current_section->entries[current_section->entry_count++];
                entry->key = key;
                entry->value = value;
            }
        }

        free(trimmed);
    next_line:
        if (!line_end) break;
        line_start = line_end + 1;
    }

    return 0;
}

int fossil_media_ini_load_file(const char *path, fossil_media_ini_t *ini) {
    FILE *f = fopen(path, "rb");
    if (!f) return -1;
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *buf = (char *)malloc(size + 1);
    if (!buf) { fclose(f); return -1; }
    fread(buf, 1, size, f);
    buf[size] = '\0';
    fclose(f);

    int res = fossil_media_ini_load_string(buf, ini);
    free(buf);
    return res;
}

const char *fossil_media_ini_get(const fossil_media_ini_t *ini, const char *section, const char *key) {
    fossil_media_ini_section_t *sec = find_section((fossil_media_ini_t *)ini, section);
    if (!sec) return NULL;
    fossil_media_ini_entry_t *entry = find_entry(sec, key);
    return entry ? entry->value : NULL;
}

int fossil_media_ini_set(fossil_media_ini_t *ini, const char *section, const char *key, const char *value) {
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
    for (size_t i = 0; i < ini->section_count; i++) {
        free(ini->sections[i].name);
        for (size_t j = 0; j < ini->sections[i].entry_count; j++) {
            free(ini->sections[i].entries[j].key);
            free(ini->sections[i].entries[j].value);
        }
        free(ini->sections[i].entries);
    }
    free(ini->sections);
    memset(ini, 0, sizeof(*ini));
}
