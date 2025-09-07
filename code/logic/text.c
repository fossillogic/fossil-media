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
#include "fossil/media/text.h"
#include "fossil/media/media.h"
#include <string.h>
#include <ctype.h>

char *fossil_media_text_trim(char *str) {
    if (!str) return NULL;
    char *start = str;
    char *end;

    // Skip leading spaces
    while (isspace((unsigned char)*start)) start++;

    if (*start == 0) return start; // Empty string

    // Trim trailing spaces
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

size_t fossil_media_text_split(char *str, char delim, char **tokens, size_t max_tokens) {
    if (!str || !tokens || max_tokens == 0) return 0;

    size_t count = 0;
    char *p = str;

    while (count < max_tokens) {
        tokens[count++] = p;
        p = strchr(p, delim);
        if (!p) break;
        *p = '\0';
        p++;
    }
    return count;
}
