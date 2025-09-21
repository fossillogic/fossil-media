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


// Enhanced YAML parser: supports nested maps (basic indentation-based hierarchy)
fossil_media_yaml_node_t *fossil_media_yaml_parse(const char *input) {
    if (!input) return NULL;

    fossil_media_yaml_node_t *root = NULL, *last = NULL;
    fossil_media_yaml_node_t **stack = NULL;
    size_t stack_size = 0, stack_cap = 0;

    char *copy = fossil_media_strdup(input);
    if (!copy) return NULL;

    char *line = strtok(copy, "\n");
    while (line) {
        size_t indent = 0;
        while (*line == ' ' || *line == '\t') {
            indent++;
            line++;
        }

        char *sep = strchr(line, ':');
        if (sep) {
            *sep = '\0';
            char *key = fossil_media_trim(line);
            char *value = fossil_media_trim(sep + 1);

            fossil_media_yaml_node_t *node = calloc(1, sizeof(*node));
            if (!node) break;
            node->key = fossil_media_strdup(key);
            node->value = fossil_media_strdup(value);
            node->indent = indent;
            node->next = NULL;
            node->child = NULL;

            // Stack management for hierarchy
            while (stack_size > 0 && stack[stack_size - 1]->indent >= indent)
                stack_size--;

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
        }
        line = strtok(NULL, "\n");
    }

    free(stack);
    free(copy);
    return root;
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
