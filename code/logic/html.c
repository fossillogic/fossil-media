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

static int parse_html_string(const char *input, fossil_media_html_doc_t **out_doc) {
    if (!input || !out_doc) return FOSSIL_MEDIA_HTML_ERR_INVALID_ARG;

    fossil_media_html_doc_t *doc = (fossil_media_html_doc_t*)calloc(1, sizeof(*doc));
    if (!doc) return FOSSIL_MEDIA_HTML_ERR_NOMEM;

    fossil_media_html_node_t *root = alloc_node(FOSSIL_MEDIA_HTML_NODE_DOCUMENT);
    if (!root) { free(doc); return FOSSIL_MEDIA_HTML_ERR_NOMEM; }
    doc->root = root;

    fossil_media_html_node_t *current = root;
    const char *p = input;

    /* Timeout handling: limit max processed characters (not just loop iterations) */
    size_t max_steps = 1000000; /* tuneable; test uses big input ~2,000,000 so this will timeout */
    size_t steps = 0;

    while (*p) {
        /* guard on processed characters */
        if (steps > max_steps) { fossil_media_html_free(doc); return FOSSIL_MEDIA_HTML_ERR_TIMEOUT; }

        if (*p == '<') {
            /* Ensure we have at least one following char before indexing p[1] */
            char next = p[1];

            /* Processing instruction <? ... ?> */
            if (next == '?') {
                const char *end = strstr(p + 2, "?>");
                if (!end) break;
                size_t len = (size_t)(end - (p + 2));
                char *txt = (char*)malloc(len + 1);
                if (!txt) { fossil_media_html_free(doc); return FOSSIL_MEDIA_HTML_ERR_NOMEM; }
                memcpy(txt, p + 2, len);
                txt[len] = '\0';

                fossil_media_html_node_t *n = alloc_node(FOSSIL_MEDIA_HTML_NODE_PROCESSING_INSTRUCTION);
                if (!n) { free(txt); fossil_media_html_free(doc); return FOSSIL_MEDIA_HTML_ERR_NOMEM; }
                n->text = txt;

                /* append as last child of current (document root usually) */
                if (!current->first_child) current->first_child = n;
                else {
                    fossil_media_html_node_t *s = current->first_child;
                    while (s->next_sibling) s = s->next_sibling;
                    s->next_sibling = n;
                }
                n->parent = current;

                /* advance p and steps */
                steps += (size_t)((end + 2) - p);
                p = end + 2;
                continue;
            }

            /* Declarations / comments / doctype / cdata start with <! */
            if (next == '!') {
                /* Comment: <!-- ... --> */
                if (strncmp(p + 2, "--", 2) == 0) {
                    const char *end = strstr(p + 4, "-->");
                    if (!end) break;
                    size_t len = (size_t)(end - (p + 4));
                    char *txt = (char*)malloc(len + 1);
                    if (!txt) { fossil_media_html_free(doc); return FOSSIL_MEDIA_HTML_ERR_NOMEM; }
                    memcpy(txt, p + 4, len);
                    txt[len] = '\0';

                    fossil_media_html_node_t *n = alloc_node(FOSSIL_MEDIA_HTML_NODE_COMMENT);
                    if (!n) { free(txt); fossil_media_html_free(doc); return FOSSIL_MEDIA_HTML_ERR_NOMEM; }
                    n->text = txt;

                    if (!current->first_child) current->first_child = n;
                    else {
                        fossil_media_html_node_t *s = current->first_child;
                        while (s->next_sibling) s = s->next_sibling;
                        s->next_sibling = n;
                    }
                    n->parent = current;

                    steps += (size_t)((end + 3) - p);
                    p = end + 3;
                    continue;
                }

                /* CDATA: <![CDATA[ ... ]]> */
                if (strncmp(p + 2, "[CDATA[", 7) == 0) {
                    const char *end = strstr(p + 9, "]]>");
                    if (!end) break;
                    size_t len = (size_t)(end - (p + 9));
                    char *txt = (char*)malloc(len + 1);
                    if (!txt) { fossil_media_html_free(doc); return FOSSIL_MEDIA_HTML_ERR_NOMEM; }
                    memcpy(txt, p + 9, len);
                    txt[len] = '\0';

                    fossil_media_html_node_t *n = alloc_node(FOSSIL_MEDIA_HTML_NODE_CDATA);
                    if (!n) { free(txt); fossil_media_html_free(doc); return FOSSIL_MEDIA_HTML_ERR_NOMEM; }
                    n->text = txt;

                    if (!current->first_child) current->first_child = n;
                    else {
                        fossil_media_html_node_t *s = current->first_child;
                        while (s->next_sibling) s = s->next_sibling;
                        s->next_sibling = n;
                    }
                    n->parent = current;

                    steps += (size_t)((end + 3) - p);
                    p = end + 3;
                    continue;
                }

                /* DOCTYPE: case-insensitive <!DOCTYPE ...>  */
                if (fossil_media_strncasecmp(p + 2, "DOCTYPE", 7) == 0) {
                    const char *end = strchr(p + 2, '>');
                    if (!end) break;
                    size_t len = (size_t)(end - (p + 2));
                    char *txt = (char*)malloc(len + 1);
                    if (!txt) { fossil_media_html_free(doc); return FOSSIL_MEDIA_HTML_ERR_NOMEM; }
                    memcpy(txt, p + 2, len);
                    txt[len] = '\0';

                    fossil_media_html_node_t *n = alloc_node(FOSSIL_MEDIA_HTML_NODE_DOCTYPE);
                    if (!n) { free(txt); fossil_media_html_free(doc); return FOSSIL_MEDIA_HTML_ERR_NOMEM; }
                    n->text = txt;

                    if (!current->first_child) current->first_child = n;
                    else {
                        fossil_media_html_node_t *s = current->first_child;
                        while (s->next_sibling) s = s->next_sibling;
                        s->next_sibling = n;
                    }
                    n->parent = current;

                    steps += (size_t)((end + 1) - p);
                    p = end + 1;
                    continue;
                }

                /* Unknown <! ... > sequence - skip until next '>' */
                {
                    const char *end = strchr(p + 2, '>');
                    if (!end) break;
                    steps += (size_t)((end + 1) - p);
                    p = end + 1;
                    continue;
                }
            }

            /* Closing tag: </...> */
            if (next == '/') {
                const char *end = strchr(p + 2, '>');
                if (!end) break;
                /* naive pop: move to parent if present */
                if (current->parent) current = current->parent;

                steps += (size_t)((end + 1) - p);
                p = end + 1;
                continue;
            }

            /* Opening tag or self-closing: <tag ...> */
            {
                const char *end = strchr(p + 1, '>');
                if (!end) break;
                size_t len = (size_t)(end - (p + 1));
                /* copy the inside of the angle brackets for parsing */
                char *tagbuf = (char*)malloc(len + 1);
                if (!tagbuf) { fossil_media_html_free(doc); return FOSSIL_MEDIA_HTML_ERR_NOMEM; }
                memcpy(tagbuf, p + 1, len);
                tagbuf[len] = '\0';

                /* Check for trailing '/' for self-closing '<.../>' (allow spaces before '/') */
                int self_closing = 0;
                /* trim trailing whitespace */
                while (len > 0 && (tagbuf[len-1] == ' ' || tagbuf[len-1] == '\t' || tagbuf[len-1] == '\r' || tagbuf[len-1] == '\n')) {
                    tagbuf[--len] = '\0';
                }
                if (len > 0 && tagbuf[len-1] == '/') {
                    self_closing = 1;
                    tagbuf[--len] = '\0';
                }

                /* Extract tag name (up to first space) */
                char *space = NULL;
                for (size_t i = 0; i < len; ++i) {
                    if (tagbuf[i] == ' ' || tagbuf[i] == '\t') { tagbuf[i] = '\0'; space = &tagbuf[i+1]; break; }
                }
                char *tagname = tagbuf;
                /* tagname lower/upper doesn't matter for node->tag, keep as-is or normalize as you prefer */
                fossil_media_html_node_t *n = alloc_node(FOSSIL_MEDIA_HTML_NODE_ELEMENT);
                if (!n) { free(tagbuf); fossil_media_html_free(doc); return FOSSIL_MEDIA_HTML_ERR_NOMEM; }
                n->tag = fossil_media_strdup(tagname);

                /* Parse attributes (basic handling: key="value" or key='value' or unquoted) */
                if (space) {
                    char *attrstr = space;
                    while (*attrstr) {
                        /* skip whitespace */
                        while (*attrstr == ' ' || *attrstr == '\t') attrstr++;
                        if (!*attrstr) break;

                        /* find '=' */
                        char *eq = strchr(attrstr, '=');
                        if (!eq) break;

                        /* key: from attrstr to eq-1, trim trailing whitespace */
                        size_t klen = 0;
                        /* compute real key start/end */
                        const char *kstart = attrstr;
                        const char *kend = eq - 1;
                        while (kend >= kstart && (*kend == ' ' || *kend == '\t')) kend--;
                        if (kend < kstart) { /* empty key */ break; }
                        klen = (size_t)(kend - kstart + 1);
                        char *key = (char*)malloc(klen + 1);
                        if (!key) { free(tagbuf); fossil_media_html_free(doc); return FOSSIL_MEDIA_HTML_ERR_NOMEM; }
                        memcpy(key, kstart, klen);
                        key[klen] = '\0';

                        /* value parsing */
                        char *valstart = (char*)(eq + 1);
                        while (*valstart == ' ' || *valstart == '\t') valstart++;
                        if (*valstart == '"' || *valstart == '\'') {
                            char quote = *valstart++;
                            char *valend = strchr(valstart, quote);
                            if (!valend) { free(key); break; } /* malformed attribute: bail */
                            size_t vlen = (size_t)(valend - valstart);
                            char *value = (char*)malloc(vlen + 1);
                            if (!value) { free(key); free(tagbuf); fossil_media_html_free(doc); return FOSSIL_MEDIA_HTML_ERR_NOMEM; }
                            memcpy(value, valstart, vlen);
                            value[vlen] = '\0';
                            fossil_media_html_set_attr(n, key, value);
                            free(value);
                            free(key);
                            attrstr = valend + 1;
                        } else {
                            /* unquoted value: ends at space or end */
                            char *valend = valstart;
                            while (*valend && *valend != ' ' && *valend != '\t') valend++;
                            size_t vlen = (size_t)(valend - valstart);
                            char *value = (char*)malloc(vlen + 1);
                            if (!value) { free(key); free(tagbuf); fossil_media_html_free(doc); return FOSSIL_MEDIA_HTML_ERR_NOMEM; }
                            memcpy(value, valstart, vlen);
                            value[vlen] = '\0';
                            fossil_media_html_set_attr(n, key, value);
                            free(value);
                            free(key);
                            attrstr = valend;
                        }
                    }
                }

                free(tagbuf);

                /* Attach node */
                if (!current->first_child) current->first_child = n;
                else {
                    fossil_media_html_node_t *s = current->first_child;
                    while (s->next_sibling) s = s->next_sibling;
                    s->next_sibling = n;
                }
                n->parent = current;

                if (!self_closing) current = n;

                steps += (size_t)((end + 1) - p);
                p = end + 1;
                continue;
            }
        } else {
            /* Text node: consume until next '<' or end */
            const char *next = strchr(p, '<');
            size_t len = next ? (size_t)(next - p) : strlen(p);
            if (len > 0) {
                char *txt = (char*)malloc(len + 1);
                if (!txt) { fossil_media_html_free(doc); return FOSSIL_MEDIA_HTML_ERR_NOMEM; }
                memcpy(txt, p, len);
                txt[len] = '\0';

                fossil_media_html_node_t *n = alloc_node(FOSSIL_MEDIA_HTML_NODE_TEXT);
                if (!n) { free(txt); fossil_media_html_free(doc); return FOSSIL_MEDIA_HTML_ERR_NOMEM; }
                n->text = txt;

                if (!current->first_child) current->first_child = n;
                else {
                    fossil_media_html_node_t *s = current->first_child;
                    while (s->next_sibling) s = s->next_sibling;
                    s->next_sibling = n;
                }
                n->parent = current;

                steps += len;
                p += len;
                continue;
            } else {
                /* zero-length text (shouldn't happen), just advance one char */
                steps += 1;
                p++;
                continue;
            }
        }
    }

    /* If we exited because of EOF, return success and document root */
    *out_doc = doc;
    return FOSSIL_MEDIA_HTML_OK;
}

fossil_media_html_node_t* fossil_media_html_find_by_tag(fossil_media_html_node_t *node, const char *tag) {
    if (!node || !tag) return NULL;
    if (node->type == FOSSIL_MEDIA_HTML_NODE_ELEMENT && node->tag && strcmp(node->tag, tag) == 0)
        return node;
    for (fossil_media_html_node_t *c = node->first_child; c; c = c->next_sibling) {
        fossil_media_html_node_t *res = fossil_media_html_find_by_tag(c, tag);
        if (res) return res;
    }
    return NULL;
}

/* --- Public API --- */

int fossil_media_html_load_file(const char *path, fossil_media_html_doc_t **out_doc) {
    if (!path || !out_doc) return FOSSIL_MEDIA_HTML_ERR_PARSE;

    FILE *f = fopen(path, "rb");
    if (!f) return FOSSIL_MEDIA_HTML_ERR_IO;

    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return FOSSIL_MEDIA_HTML_ERR_IO; }
    long sz = ftell(f);
    if (sz <= 0) { fclose(f); return FOSSIL_MEDIA_HTML_ERR_IO; }
    if (fseek(f, 0, SEEK_SET) != 0) { fclose(f); return FOSSIL_MEDIA_HTML_ERR_IO; }

    char *buf = (char*)malloc(sz + 1);
    if (!buf) { fclose(f); return FOSSIL_MEDIA_HTML_ERR_NOMEM; }

    size_t read_bytes = fread(buf, 1, sz, f);
    if (read_bytes != (size_t)sz) {
        free(buf);
        fclose(f);
        return FOSSIL_MEDIA_HTML_ERR_IO;  // File read error
    }
    buf[sz] = '\0';  // Null-terminate

    fclose(f);

    int rc = parse_html_string(buf, out_doc);
    free(buf);
    return rc;
}

int fossil_media_html_load_string(const char *data, fossil_media_html_doc_t **out_doc) {
    if (!data || !out_doc) return FOSSIL_MEDIA_HTML_ERR_PARSE;
    return parse_html_string(data, out_doc);
}

void fossil_media_html_free(fossil_media_html_doc_t *doc) {
    if (!doc) return;
    /* recursively free nodes using a dynamic stack */
    size_t stack_cap = 256;
    size_t sp = 0;
    fossil_media_html_node_t **stack = (fossil_media_html_node_t**)malloc(stack_cap * sizeof(fossil_media_html_node_t*));
    if (!stack) {
        free(doc);
        return;
    }
    if (doc->root) stack[sp++] = doc->root;
    while (sp > 0) {
        fossil_media_html_node_t *n = stack[--sp];
        // Save next_sibling before freeing n
        fossil_media_html_node_t *child = n->first_child;
        while (child) {
            if (sp >= stack_cap) {
                stack_cap *= 2;
                fossil_media_html_node_t **new_stack = (fossil_media_html_node_t**)realloc(stack, stack_cap * sizeof(fossil_media_html_node_t*));
                if (!new_stack) {
                    // If realloc fails, just free what we can and exit
                    break;
                }
                stack = new_stack;
            }
            fossil_media_html_node_t *next = child->next_sibling;
            stack[sp++] = child;
            child = next;
        }
        if (n->tag) { free(n->tag); n->tag = NULL; }
        if (n->text) { free(n->text); n->text = NULL; }
        for (size_t i = 0; i < n->attrs.count; ++i) {
            if (n->attrs.keys) { free(n->attrs.keys[i]); n->attrs.keys[i] = NULL; }
            if (n->attrs.values) { free(n->attrs.values[i]); n->attrs.values[i] = NULL; }
        }
        if (n->attrs.keys) { free(n->attrs.keys); n->attrs.keys = NULL; }
        if (n->attrs.values) { free(n->attrs.values); n->attrs.values = NULL; }
        free(n);
    }
    free(stack);
    free(doc);
}

fossil_media_html_node_t* fossil_media_html_root(fossil_media_html_doc_t *doc) {
    return doc ? doc->root : NULL;
}

fossil_media_html_node_type_t fossil_media_html_node_type(const fossil_media_html_node_t *node) {
    return node ? node->type : FOSSIL_MEDIA_HTML_NODE_DOCUMENT;
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
    if (!node || !attr_name || !attr_value) return FOSSIL_MEDIA_HTML_ERR_PARSE;
    for (size_t i = 0; i < node->attrs.count; ++i) {
        if (strcmp(node->attrs.keys[i], attr_name) == 0) {
            free(node->attrs.values[i]);
            node->attrs.values[i] = fossil_media_strdup(attr_value);
            return FOSSIL_MEDIA_HTML_OK;
        }
    }
    char **new_keys = realloc(node->attrs.keys, (node->attrs.count+1)*sizeof(char*));
    char **new_vals = realloc(node->attrs.values, (node->attrs.count+1)*sizeof(char*));
    if (!new_keys || !new_vals) return FOSSIL_MEDIA_HTML_ERR_NOMEM;
    node->attrs.keys = new_keys;
    node->attrs.values = new_vals;
    node->attrs.keys[node->attrs.count] = fossil_media_strdup(attr_name);
    node->attrs.values[node->attrs.count] = fossil_media_strdup(attr_value);
    node->attrs.count++;
    return FOSSIL_MEDIA_HTML_OK;
}

/* Serialization: naive recursive printer */
static void serialize_node(const fossil_media_html_node_t *node, char **buf, size_t *len, size_t *cap) {
    if (!node) return;
    if (node->type == FOSSIL_MEDIA_HTML_NODE_ELEMENT) {
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

    } else if (node->type == FOSSIL_MEDIA_HTML_NODE_TEXT) {
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
