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
#include "fossil/media/media.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* -------------------------------------------------------------
 *  fossil_media_strdup
 * -------------------------------------------------------------
 *  Allocates a copy of a null-terminated string.
 *  The returned buffer must be freed by the caller.
 */
char *fossil_media_strdup(const char *src) {
    if (!src) {
        return NULL;
    }
    size_t len = strlen(src);
    char *copy = (char *)malloc(len + 1);
    if (!copy) {
        return NULL;
    }
    memcpy(copy, src, len + 1);
    return copy;
}

char *fossil_media_strndup(const char *s, size_t n) {
    char *copy;
    size_t len;

    if (!s) {
        return NULL;
    }

    /* Determine actual length to copy, which is the smaller of n and strlen(s) */
    len = 0;
    while (len < n && s[len] != '\0') {
        len++;
    }

    copy = (char *)malloc(len + 1); /* +1 for null terminator */
    if (!copy) {
        return NULL;
    }

    if (len > 0) {
        memcpy(copy, s, len);
    }
    copy[len] = '\0';

    return copy;
}

/* -------------------------------------------------------------
 *  fossil_media_read_file
 * -------------------------------------------------------------
 *  Reads an entire file into a newly allocated buffer.
 *  The buffer will be null-terminated for text usage.
 *  The caller must free the returned buffer.
 */
char *fossil_media_read_file(const char *path, size_t *out_size) {
    if (!path) {
        return NULL;
    }
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        return NULL;
    }
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    rewind(fp);

    if (fsize < 0) {
        fclose(fp);
        return NULL;
    }

    char *buffer = (char *)malloc((size_t)fsize + 1);
    if (!buffer) {
        fclose(fp);
        return NULL;
    }

    size_t read_size = fread(buffer, 1, (size_t)fsize, fp);
    fclose(fp);

    buffer[read_size] = '\0'; /* Null-terminate */
    if (out_size) {
        *out_size = read_size;
    }
    return buffer;
}

/* -------------------------------------------------------------
 *  fossil_media_write_file
 * -------------------------------------------------------------
 *  Writes a null-terminated string to a file.
 *  Returns 0 on success, nonzero on error.
 */
int fossil_media_write_file(const char *path, const char *data) {
    if (!path || !data) {
        return -1;
    }
    FILE *fp = fopen(path, "wb");
    if (!fp) {
        return -1;
    }
    size_t len = strlen(data);
    size_t written = fwrite(data, 1, len, fp);
    fclose(fp);
    return (written == len) ? 0 : -1;
}

/* -------------------------------------------------------------
 *  fossil_media_trim
 * -------------------------------------------------------------
 *  Removes leading and trailing whitespace from a mutable string.
 *  Operates in place. Returns the trimmed string pointer.
 */
char *fossil_media_trim(char *str) {
    if (!str) {
        return NULL;
    }

    /* Trim leading whitespace */
    char *start = str;
    while (isspace((unsigned char)*start)) {
        start++;
    }

    /* Trim trailing whitespace */
    char *end = start + strlen(start);
    while (end > start && isspace((unsigned char)*(end - 1))) {
        end--;
    }

    *end = '\0';

    /* If trimmed part is not at original start, shift */
    if (start != str) {
        memmove(str, start, (size_t)(end - start) + 1);
    }

    return str;
}
