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
    fossil_media_md_node_t **new_children = realloc(parent->children, sizeof(*parent->children) * (parent->child_count + 1));
    if (!new_children) return; // realloc failed, do not modify parent
    parent->children = new_children;
    parent->children[parent->child_count++] = child;
    child->parent = parent;
}

/* ---------- Enhanced Markdown Parser ---------- */

static int is_blank_line(const char *line) {
    while (*line) {
        if (*line != ' ' && *line != '\t' && *line != '\r' && *line != '\n')
            return 0;
        line++;
    }
    return 1;
}

static int is_heading(const char *line, size_t *level) {
    *level = 0;
    while (*line == '#') { (*level)++; line++; }
    if (*level > 0 && (*line == ' ' || *line == '\t')) return 1;
    return 0;
}

static int is_list_item(const char *line) {
    return ((*line == '-' || *line == '*' || *line == '+') && (line[1] == ' ' || line[1] == '\t'));
}

static int is_code_fence(const char *line) {
    return (strncmp(line, "```", 3) == 0);
}

static int is_blockquote(const char *line) {
    return (*line == '>' && (line[1] == ' ' || line[1] == '\t'));
}

static char *extract_line(const char *start, const char **next) {
    const char *end = strchr(start, '\n');
    size_t len = end ? (size_t)(end - start) : strlen(start);
    char *line = fossil_media_strndup(start, len);
    *next = end ? end + 1 : start + len;
    return line;
}

fossil_media_md_node_t *fossil_media_md_parse(const char *input) {
    if (!input) return NULL;

    fossil_media_md_node_t *root = md_new_node(FOSSIL_MEDIA_MD_PARAGRAPH, NULL, NULL);

    const char *line_ptr = input;
    while (*line_ptr) {
        char *line = extract_line(line_ptr, &line_ptr);
        if (is_blank_line(line)) {
            free(line);
            continue;
        }

        size_t level = 0;
        if (is_heading(line, &level)) {
            const char *content = line + level;
            while (*content == ' ' || *content == '\t') content++;
            fossil_media_md_node_t *heading = md_new_node(FOSSIL_MEDIA_MD_HEADING, content, NULL);
            heading->level = (int)level;
            md_add_child(root, heading);
        }
        else if (is_list_item(line)) {
            const char *content = line + 2;
            while (*content == ' ' || *content == '\t') content++;
            md_add_child(root, md_new_node(FOSSIL_MEDIA_MD_LIST_ITEM, content, NULL));
        }
        else if (is_blockquote(line)) {
            const char *content = line + 1;
            while (*content == ' ' || *content == '\t') content++;
            md_add_child(root, md_new_node(FOSSIL_MEDIA_MD_BLOCKQUOTE, content, NULL));
        }
        else if (is_code_fence(line)) {
            const char *lang = line + 3;
            while (*lang == ' ' || *lang == '\t') lang++;
            // If lang is only whitespace or empty, treat as no language
            const char *code_start = line_ptr;
            const char *code_end = strstr(code_start, "```");
            if (!code_end) {
                free(line);
                break;
            }
            // Remove trailing newline if present before closing fence
            size_t block_len = (size_t)(code_end - code_start);
            while (block_len > 0 && (code_start[block_len - 1] == '\n' || code_start[block_len - 1] == '\r')) {
                block_len--;
            }
            char *block = fossil_media_strndup(code_start, block_len);
            fossil_media_md_node_t *code_node = md_new_node(FOSSIL_MEDIA_MD_CODE_BLOCK, block, (*lang && *lang != '\n') ? lang : NULL);
            md_add_child(root, code_node);
            free(block);
            line_ptr = code_end + 3;
            // Skip possible trailing newline after closing fence
            if (*line_ptr == '\n' || *line_ptr == '\r') line_ptr++;
        }
        else {
            md_add_child(root, md_new_node(FOSSIL_MEDIA_MD_TEXT, line, NULL));
        }
        free(line);
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
        char heading_prefix[8] = {0};
        switch (n->type) {
            case FOSSIL_MEDIA_MD_HEADING:
                memset(heading_prefix, '#', n->level > 6 ? 6 : n->level);
                heading_prefix[n->level > 6 ? 6 : n->level] = ' ';
                prefix = heading_prefix;
                break;
            case FOSSIL_MEDIA_MD_LIST_ITEM: prefix = "- "; break;
            case FOSSIL_MEDIA_MD_CODE_BLOCK:
                if (n->extra && strlen(n->extra) > 0) {
                    snprintf(buf + len, buf_size - len, "```%s\n", n->extra);
                    len = strlen(buf);
                } else {
                    strcat(buf, "```\n");
                    len = strlen(buf);
                }
                if (n->content) {
                    strcat(buf, n->content);
                    len = strlen(buf);
                }
                strcat(buf, "\n```");
                len = strlen(buf);
                strcat(buf, "\n");
                continue;
            case FOSSIL_MEDIA_MD_BLOCKQUOTE: prefix = "> "; break;
            default: break;
        }
        size_t chunk_len = strlen(prefix) + (n->content ? strlen(n->content) : 0) + 5;
        if (len + chunk_len >= buf_size) {
            buf_size *= 2;
            buf = realloc(buf, buf_size);
        }
        strcat(buf, prefix);
        if (n->content) strcat(buf, n->content);
        strcat(buf, "\n");
        len = strlen(buf);
    }

    return buf;
}

void fossil_media_md_free(fossil_media_md_node_t *node) {
    if (!node) return;
    if (node->content) free(node->content);
    if (node->extra) free(node->extra);
    if (node->children) {
        for (size_t i = 0; i < node->child_count; i++) {
            fossil_media_md_free(node->children[i]);
        }
        free(node->children);
    }
    free(node);
}
