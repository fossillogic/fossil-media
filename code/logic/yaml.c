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
#include "fossil/media/yaml.h"
#include "fossil/media/media.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#if defined(_WIN32) && !defined(__MINGW32__)
    // Windows has strtok_s instead of strtok_r
    #define strtok_r(str, delim, saveptr) strtok_s((str), (delim), (saveptr))
#endif


/*
 * Enhanced YAML parser: supports nested maps (basic indentation-based hierarchy)
 * Updated to survive all listed test cases.
 */
fossil_media_yaml_node_t *fossil_media_yaml_parse(const char *input) {
    if (!input || !*input) return NULL;

    fossil_media_yaml_node_t *root = NULL, *last = NULL;
    fossil_media_yaml_node_t **stack = NULL;
    size_t stack_size = 0, stack_cap = 0;
    int parsed_any = 0;

    char *copy = fossil_media_strdup(input);
    if (!copy) return NULL;

    char *saveptr = NULL;
    char *line = strtok_r(copy, "\n", &saveptr);
    while (line) {
        char *orig_line = line;
        // Count indent (spaces/tabs before first non-space/tab)
        size_t indent = 0;
        while (orig_line[indent] == ' ' || orig_line[indent] == '\t') indent++;
        line = orig_line + indent;

        // Skip lines that are only whitespace
        if (*line == '\0' || strspn(line, " \t") == strlen(line)) {
            line = strtok_r(NULL, "\n", &saveptr);
            continue;
        }

        char *sep = strchr(line, ':');
        if (!sep) {
            line = strtok_r(NULL, "\n", &saveptr);
            continue;
        }

        // Accept "key:" (colon at end) as valid, value is empty string
        *sep = '\0';
        char *key = fossil_media_trim(line);
        char *value = fossil_media_trim(sep + 1);

        // Remove trailing spaces from value
        if (value) {
            size_t vlen = strlen(value);
            while (vlen > 0 && (value[vlen - 1] == ' ' || value[vlen - 1] == '\t')) {
                value[vlen - 1] = '\0';
                vlen--;
            }
        }

        if (!key || !*key) {
            line = strtok_r(NULL, "\n", &saveptr);
            continue;
        }
        if (!value) value = "";

        fossil_media_yaml_node_t *node = calloc(1, sizeof(*node));
        if (!node) break;
        node->key = fossil_media_strdup(key);
        node->value = fossil_media_strdup(value);
        node->indent = indent;
        node->next = NULL;
        node->child = NULL;

        // Stack management for hierarchy
        // Treat tabs and spaces as equivalent for indentation comparison
        while (stack_size > 0) {
            size_t parent_indent = stack[stack_size - 1]->indent;
            if (parent_indent < indent)
                break;
            stack_size--;
        }

        if (stack_size == 0) {
            // Top-level node
            if (!root) root = node;
            else last->next = node;
            last = node;
        } else {
            // Child node
            fossil_media_yaml_node_t *parent = stack[stack_size - 1];
            if (!parent->child) parent->child = node;
            else {
                fossil_media_yaml_node_t *sibling = parent->child;
                while (sibling->next) sibling = sibling->next;
                sibling->next = node;
            }
        }

        // Push to stack
        if (stack_size == stack_cap) {
            stack_cap = stack_cap ? stack_cap * 2 : 8;
            stack = realloc(stack, stack_cap * sizeof(*stack));
        }
        stack[stack_size++] = node;
        parsed_any = 1;

        line = strtok_r(NULL, "\n", &saveptr);
    }

    if (stack)
        free(stack);
    if (copy)
        free(copy);

    // If root is NULL, return NULL (no valid nodes parsed)
    return parsed_any ? root : NULL;
}

void fossil_media_yaml_free(fossil_media_yaml_node_t *head) {
    while (head) {
        fossil_media_yaml_node_t *next = head->next;
        fossil_media_yaml_free(head->child);
        free(head->key);
        free(head->value);
        free(head);
        head = next;
    }
}

const char *fossil_media_yaml_get(const fossil_media_yaml_node_t *head, const char *key) {
    for (; head; head = head->next) {
        if (strcmp(head->key, key) == 0)
            return head->value;
        const char *val = fossil_media_yaml_get(head->child, key);
        if (val) return val;
    }
    return NULL;
}

void fossil_media_yaml_print(const fossil_media_yaml_node_t *head) {
    for (; head; head = head->next) {
        for (size_t i = 0; i < head->indent; i++) printf(" ");
        printf("%s: %s\n", head->key, head->value);
        if (head->child)
            fossil_media_yaml_print(head->child);
    }
}
