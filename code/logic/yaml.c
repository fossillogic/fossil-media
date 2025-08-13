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
#include "fossil/media/yaml.h"
#include "fossil/media/media.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


fossil_media_yaml_node_t *fossil_media_yaml_parse(const char *input) {
    if (!input) return NULL;

    fossil_media_yaml_node_t *head = NULL, *tail = NULL;

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

            if (!head) head = tail = node;
            else { tail->next = node; tail = node; }
        }
        line = strtok(NULL, "\n");
    }

    free(copy);
    return head;
}

void fossil_media_yaml_free(fossil_media_yaml_node_t *head) {
    while (head) {
        fossil_media_yaml_node_t *next = head->next;
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
    }
    return NULL;
}

void fossil_media_yaml_print(const fossil_media_yaml_node_t *head) {
    for (; head; head = head->next) {
        for (size_t i = 0; i < head->indent; i++) printf(" ");
        printf("%s: %s\n", head->key, head->value);
    }
}
