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
#include "fossil/media/markdown.h"
#include "fossil/media/media.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ---------- Internal helpers ---------- */
static fossil_media_md_node_t *md_new_node(fossil_media_md_type_t type, const char *content, const char *extra) {
    fossil_media_md_node_t *node = calloc(1, sizeof(fossil_media_md_node_t));
    if (!node) return NULL;
    node->type = type;
    node->content = content ? fossil_media_strdup(content) : NULL;
    node->extra = extra ? fossil_media_strdup(extra) : NULL;
    return node;
}

static void md_add_child(fossil_media_md_node_t *parent, fossil_media_md_node_t *child) {
    if (!parent || !child) return;
    parent->children = realloc(parent->children, sizeof(*parent->children) * (parent->child_count + 1));
    parent->children[parent->child_count++] = child;
    child->parent = parent;
}

/* ---------- Public API ---------- */

fossil_media_md_node_t *fossil_media_md_parse(const char *input) {
    if (!input) return NULL;

    fossil_media_md_node_t *root = md_new_node(FOSSIL_MEDIA_MD_PARAGRAPH, NULL, NULL);

    const char *line = input;
    while (*line) {
        if (*line == '#') {
            size_t level = 0;
            while (*line == '#') { level++; line++; }
            while (*line == ' ') line++;

            fossil_media_md_node_t *heading = md_new_node(FOSSIL_MEDIA_MD_HEADING, line, NULL);
            heading->level = (int)level;  /* Store heading level in node */
            md_add_child(root, heading);
        }
        else if (*line == '-' && line[1] == ' ') {
            md_add_child(root, md_new_node(FOSSIL_MEDIA_MD_LIST_ITEM, line + 2, NULL));
        }
        else if (strncmp(line, "```", 3) == 0) {
            line += 3;
            const char *end = strstr(line, "```");
            if (!end) break;
            char *block = fossil_media_strndup(line, (size_t)(end - line));
            md_add_child(root, md_new_node(FOSSIL_MEDIA_MD_CODE_BLOCK, block, NULL));
            free(block);
            line = end + 3;
        }
        else {
            md_add_child(root, md_new_node(FOSSIL_MEDIA_MD_TEXT, line, NULL));
        }

        const char *next = strchr(line, '\n');
        if (!next) break;
        line = next + 1;
    }

    return root;
}

char *fossil_media_md_serialize(const fossil_media_md_node_t *root) {
    if (!root) return NULL;

    size_t buf_size = 1024;
    char *buf = calloc(1, buf_size);
    size_t len = 0;

    for (size_t i = 0; i < root->child_count; i++) {
        const fossil_media_md_node_t *n = root->children[i];
        const char *prefix = "";
        switch (n->type) {
            case FOSSIL_MEDIA_MD_HEADING: prefix = "# "; break;
            case FOSSIL_MEDIA_MD_LIST_ITEM: prefix = "- "; break;
            case FOSSIL_MEDIA_MD_CODE_BLOCK: prefix = "```\n"; break;
            default: break;
        }
        size_t chunk_len = strlen(prefix) + (n->content ? strlen(n->content) : 0) + 5;
        if (len + chunk_len >= buf_size) {
            buf_size *= 2;
            buf = realloc(buf, buf_size);
        }
        strcat(buf, prefix);
        if (n->content) strcat(buf, n->content);
        if (n->type == FOSSIL_MEDIA_MD_CODE_BLOCK) strcat(buf, "\n```");
        strcat(buf, "\n");
        len = strlen(buf);
    }

    return buf;
}

void fossil_media_md_free(fossil_media_md_node_t *node) {
    if (!node) return;
    free(node->content);
    free(node->extra);
    for (size_t i = 0; i < node->child_count; i++) {
        fossil_media_md_free(node->children[i]);
    }
    free(node->children);
    free(node);
}
