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
#include "fossil/media/text.h"
#include "fossil/media/media.h"
#include <string.h>
#include <ctype.h>

char *fossil_media_text_trim(char *str) {
    if (!str) return NULL;
    char *start = str;
    char *end;

    // Skip leading whitespace
    while (isspace((unsigned char)*start)) start++;

    if (*start == 0) return start; // Empty string

    // Trim trailing whitespace
    end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) end--;

    *(end + 1) = '\0';
    return start;
}

char *fossil_media_text_tolower(char *str) {
    if (!str) return NULL;
    for (char *p = str; *p; ++p) {
        *p = (char)tolower((unsigned char)*p);
    }
    return str;
}

char *fossil_media_text_toupper(char *str) {
    if (!str) return NULL;
    for (char *p = str; *p; ++p) {
        *p = (char)toupper((unsigned char)*p);
    }
    return str;
}

size_t fossil_media_text_replace(char *str, const char *old_sub, const char *new_sub, size_t buf_size) {
    if (!str || !old_sub || !new_sub || buf_size == 0) return 0;

    size_t old_len = strlen(old_sub);
    size_t new_len = strlen(new_sub);
    size_t count = 0;

    if (old_len == 0) return 0;

    char *pos = strstr(str, old_sub);
    while (pos) {
        size_t tail_len = strlen(pos + old_len);

        if ((strlen(str) - old_len + new_len) >= buf_size) {
            // No room for replacement
            break;
        }

        memmove(pos + new_len, pos + old_len, tail_len + 1);
        memcpy(pos, new_sub, new_len);

        count++;
        pos = strstr(pos + new_len, old_sub);
    }
    return count;
}

char *fossil_media_text_find(const char *haystack, const char *needle) {
    if (!haystack || !needle) return NULL;
    return strstr(haystack, needle);
}

// Enhanced split: supports quoted tokens and ignores empty tokens
size_t fossil_media_text_split(char *str, char delim, char **tokens, size_t max_tokens) {
    if (!str || !tokens || max_tokens == 0) return 0;

    size_t count = 0;
    char *p = str;
    char *token_start = NULL;
    int in_quotes = 0;

    while (*p && count < max_tokens) {
        // Skip leading delimiters
        while (*p == delim) p++;
        if (!*p) break;

        token_start = p;
        in_quotes = 0;

        while (*p) {
            if (*p == '"') {
                in_quotes = !in_quotes;
            } else if (*p == delim && !in_quotes) {
                break;
            }
            p++;
        }

        if (*p) {
            *p = '\0';
            p++;
        }

        // Remove surrounding quotes if present
        if (*token_start == '"' && p > token_start + 1 && *(p - 2) == '"') {
            token_start++;
            *(p - 2) = '\0';
        }

        tokens[count++] = token_start;
    }
    return count;
}

// Enhanced: checks if string starts with prefix
int fossil_media_text_starts_with(const char *str, const char *prefix) {
    if (!str || !prefix) return 0;
    size_t len_prefix = strlen(prefix);
    size_t len_str = strlen(str);
    return len_str >= len_prefix && strncmp(str, prefix, len_prefix) == 0;
}

// Enhanced: checks if string ends with suffix
int fossil_media_text_ends_with(const char *str, const char *suffix) {
    if (!str || !suffix) return 0;
    size_t len_suffix = strlen(suffix);
    size_t len_str = strlen(str);
    if (len_str < len_suffix) return 0;
    return strcmp(str + len_str - len_suffix, suffix) == 0;
}
