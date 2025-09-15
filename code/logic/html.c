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
#include "fossil/media/html.h"
#include "fossil/media/media.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Basic tree node structure */
struct fossil_media_html_node {
    fossil_media_html_node_type_t type;
    char *tag;        /* only for element nodes */
    char *text;       /* only for text/comment nodes */
    struct fossil_media_html_node *parent;
    struct fossil_media_html_node *first_child;
    struct fossil_media_html_node *next_sibling;
    /* attributes (array of key-value pairs) */
    struct {
        char **keys;
        char **values;
        size_t count;
    } attrs;
};

struct fossil_media_html_doc {
    fossil_media_html_node_t *root;
};

/* --- Minimal helpers --- */

static fossil_media_html_node_t* alloc_node(fossil_media_html_node_type_t type) {
    fossil_media_html_node_t *n = (fossil_media_html_node_t*)calloc(1, sizeof(*n));
    if (n) n->type = type;
    return n;
}

/* Very naive parser: handles <tag>, </tag>, text.
   (Does NOT support full HTML5 spec — good enough for small documents.) */
static int parse_html_string(const char *input, fossil_media_html_doc_t **out_doc) {
    fossil_media_html_doc_t *doc = (fossil_media_html_doc_t*)calloc(1, sizeof(*doc));
    if (!doc) return FMHTML_ERR_NOMEM;

    fossil_media_html_node_t *root = alloc_node(FMHTML_NODE_DOCUMENT);
    if (!root) { free(doc); return FMHTML_ERR_NOMEM; }
    doc->root = root;

    fossil_media_html_node_t *current = root;
    const char *p = input;

    while (*p) {
        if (*p == '<') {
            if (p[1] == '/') {
                /* closing tag: pop stack */
                p = strchr(p, '>');
                if (!p) break;
                if (current->parent) current = current->parent;
                p++;
                continue;
            } else {
                /* opening tag */
                const char *end = strchr(p, '>');
                if (!end) break;
                size_t len = (size_t)(end - (p+1));
                char *tag = (char*)malloc(len+1);
                strncpy(tag, p+1, len);
                tag[len] = '\0';

                fossil_media_html_node_t *n = alloc_node(FMHTML_NODE_ELEMENT);
                if (!n) { fossil_media_html_free(doc); return FMHTML_ERR_NOMEM; }
                n->tag = tag;

                /* attach to tree */
                if (!current->first_child)
                    current->first_child = n;
                else {
                    fossil_media_html_node_t *s = current->first_child;
                    while (s->next_sibling) s = s->next_sibling;
                    s->next_sibling = n;
                }
                n->parent = current;
                current = n;
                p = end + 1;
                continue;
            }
        } else {
            /* text node */
            const char *end = strchr(p, '<');
            size_t len = end ? (size_t)(end - p) : strlen(p);
            char *txt = (char*)malloc(len+1);
            strncpy(txt, p, len);
            txt[len] = '\0';

            fossil_media_html_node_t *n = alloc_node(FMHTML_NODE_TEXT);
            if (!n) { fossil_media_html_free(doc); return FMHTML_ERR_NOMEM; }
            n->text = txt;

            if (!current->first_child)
                current->first_child = n;
            else {
                fossil_media_html_node_t *s = current->first_child;
                while (s->next_sibling) s = s->next_sibling;
                s->next_sibling = n;
            }
            n->parent = current;
            p += len;
        }
    }

    *out_doc = doc;
    return FMHTML_OK;
}

/* --- Public API --- */

int fossil_media_html_load_file(const char *path, fossil_media_html_doc_t **out_doc) {
    FILE *f = fopen(path, "rb");
    if (!f) return FMHTML_ERR_IO;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = (char*)malloc(sz+1);
    if (!buf) { fclose(f); return FMHTML_ERR_NOMEM; }
    fread(buf, 1, sz, f);
    buf[sz] = '\0';
    fclose(f);
    int rc = parse_html_string(buf, out_doc);
    free(buf);
    return rc;
}

int fossil_media_html_load_string(const char *data, fossil_media_html_doc_t **out_doc) {
    return parse_html_string(data, out_doc);
}

void fossil_media_html_free(fossil_media_html_doc_t *doc) {
    if (!doc) return;
    /* recursively free nodes */
    fossil_media_html_node_t *stack[256]; /* simple stack */
    size_t sp = 0;
    if (doc->root) stack[sp++] = doc->root;
    while (sp > 0) {
        fossil_media_html_node_t *n = stack[--sp];
        if (n->tag) free(n->tag);
        if (n->text) free(n->text);
        for (size_t i = 0; i < n->attrs.count; ++i) {
            free(n->attrs.keys[i]);
            free(n->attrs.values[i]);
        }
        free(n->attrs.keys);
        free(n->attrs.values);
        for (fossil_media_html_node_t *c = n->first_child; c; c = c->next_sibling)
            stack[sp++] = c;
        free(n);
    }
    free(doc);
}

fossil_media_html_node_t* fossil_media_html_root(fossil_media_html_doc_t *doc) {
    return doc ? doc->root : NULL;
}

fossil_media_html_node_type_t fossil_media_html_node_type(const fossil_media_html_node_t *node) {
    return node ? node->type : FMHTML_NODE_DOCUMENT;
}

const char* fossil_media_html_node_tag(const fossil_media_html_node_t *node) {
    return node ? node->tag : NULL;
}

const char* fossil_media_html_node_text(const fossil_media_html_node_t *node) {
    return node ? node->text : NULL;
}

fossil_media_html_node_t* fossil_media_html_first_child(fossil_media_html_node_t *node) {
    return node ? node->first_child : NULL;
}

fossil_media_html_node_t* fossil_media_html_next_sibling(fossil_media_html_node_t *node) {
    return node ? node->next_sibling : NULL;
}

/* Attributes: not fully parsed in this minimal version, left as future work */
const char* fossil_media_html_get_attr(const fossil_media_html_node_t *node, const char *attr_name) {
    if (!node || !attr_name) return NULL;
    for (size_t i = 0; i < node->attrs.count; ++i) {
        if (strcmp(node->attrs.keys[i], attr_name) == 0)
            return node->attrs.values[i];
    }
    return NULL;
}

int fossil_media_html_set_attr(fossil_media_html_node_t *node, const char *attr_name, const char *attr_value) {
    if (!node || !attr_name || !attr_value) return FMHTML_ERR_PARSE;
    for (size_t i = 0; i < node->attrs.count; ++i) {
        if (strcmp(node->attrs.keys[i], attr_name) == 0) {
            free(node->attrs.values[i]);
            node->attrs.values[i] = fossil_media_strdup(attr_value);
            return FMHTML_OK;
        }
    }
    char **new_keys = realloc(node->attrs.keys, (node->attrs.count+1)*sizeof(char*));
    char **new_vals = realloc(node->attrs.values, (node->attrs.count+1)*sizeof(char*));
    if (!new_keys || !new_vals) return FMHTML_ERR_NOMEM;
    node->attrs.keys = new_keys;
    node->attrs.values = new_vals;
    node->attrs.keys[node->attrs.count] = fossil_media_strdup(attr_name);
    node->attrs.values[node->attrs.count] = fossil_media_strdup(attr_value);
    node->attrs.count++;
    return FMHTML_OK;
}

/* Serialization: naive recursive printer */
static void serialize_node(const fossil_media_html_node_t *node, char **buf, size_t *len, size_t *cap) {
    if (!node) return;
    if (node->type == FMHTML_NODE_ELEMENT) {
        size_t needed = strlen(node->tag) + 4;
        if (*len + needed >= *cap) {
            while (*len + needed >= *cap) *cap *= 2;
            *buf = (char*)realloc(*buf, *cap);
        }
        *len += snprintf(*buf + *len, *cap - *len, "<%s>", node->tag);

        for (fossil_media_html_node_t *c = node->first_child; c; c = c->next_sibling)
            serialize_node(c, buf, len, cap);

        if (*len + strlen(node->tag) + 4 >= *cap) {
            *cap += strlen(node->tag) + 4;
            *buf = (char*)realloc(*buf, *cap);
        }
        *len += snprintf(*buf + *len, *cap - *len, "</%s>", node->tag);

    } else if (node->type == FMHTML_NODE_TEXT) {
        size_t tlen = strlen(node->text);
        if (*len + tlen + 1 >= *cap) {
            while (*len + tlen + 1 >= *cap) *cap *= 2;
            *buf = (char*)realloc(*buf, *cap);
        }
        memcpy(*buf + *len, node->text, tlen);
        *len += tlen;
        (*buf)[*len] = '\0';
    }
}

char* fossil_media_html_serialize(const fossil_media_html_doc_t *doc) {
    if (!doc || !doc->root) return NULL;

    size_t cap = 1024, len = 0;
    char *buf = (char*)malloc(cap);
    if (!buf) return NULL;
    buf[0] = '\0';

    for (fossil_media_html_node_t *c = doc->root->first_child; c; c = c->next_sibling)
        serialize_node(c, &buf, &len, &cap);

    char *final_buf = (char*)realloc(buf, len + 1);
    return final_buf ? final_buf : buf;
}
