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
#include "fossil/media/xml.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* -------------------------------------------------------
 * Internal helpers
 * ------------------------------------------------------- */

static char *fossil_media_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *out = (char *)malloc(len + 1);
    if (!out) return NULL;
    memcpy(out, s, len + 1);
    return out;
}

static fossil_media_xml_node_t *alloc_node(fossil_media_xml_type_t type) {
    fossil_media_xml_node_t *n = (fossil_media_xml_node_t *)calloc(1, sizeof(fossil_media_xml_node_t));
    if (!n) return NULL;
    n->type = type;
    return n;
}

/* -------------------------------------------------------
 * Public API: Creation
 * ------------------------------------------------------- */

fossil_media_xml_node_t *fossil_media_xml_new_element(const char *name) {
    fossil_media_xml_node_t *n = alloc_node(FOSSIL_MEDIA_XML_ELEMENT);
    if (!n) return NULL;
    n->name = fossil_media_strdup(name);
    return n;
}

fossil_media_xml_node_t *fossil_media_xml_new_text(const char *text) {
    fossil_media_xml_node_t *n = alloc_node(FOSSIL_MEDIA_XML_TEXT);
    if (!n) return NULL;
    n->content = fossil_media_strdup(text);
    return n;
}

fossil_media_xml_node_t *fossil_media_xml_new_comment(const char *comment) {
    fossil_media_xml_node_t *n = alloc_node(FOSSIL_MEDIA_XML_COMMENT);
    if (!n) return NULL;
    n->content = fossil_media_strdup(comment);
    return n;
}

/* -------------------------------------------------------
 * Public API: Freeing
 * ------------------------------------------------------- */

void fossil_media_xml_free(fossil_media_xml_node_t *node) {
    if (!node) return;
    free(node->name);
    free(node->content);

    for (size_t i = 0; i < node->child_count; i++) {
        fossil_media_xml_free(node->children[i]);
    }
    free(node->children);

    for (size_t i = 0; i < node->attr_count; i++) {
        free(node->attr_names[i]);
        free(node->attr_values[i]);
    }
    free(node->attr_names);
    free(node->attr_values);

    free(node);
}

/* -------------------------------------------------------
 * Public API: Tree operations
 * ------------------------------------------------------- */

int fossil_media_xml_append_child(fossil_media_xml_node_t *parent, fossil_media_xml_node_t *child) {
    if (!parent || parent->type != FOSSIL_MEDIA_XML_ELEMENT) return -1;
    fossil_media_xml_node_t **tmp = realloc(parent->children, (parent->child_count + 1) * sizeof(*tmp));
    if (!tmp) return -1;
    parent->children = tmp;
    parent->children[parent->child_count++] = child;
    child->parent = parent;
    return 0;
}

int fossil_media_xml_set_attribute(fossil_media_xml_node_t *node, const char *name, const char *value) {
    if (!node || node->type != FOSSIL_MEDIA_XML_ELEMENT) return -1;
    for (size_t i = 0; i < node->attr_count; i++) {
        if (strcmp(node->attr_names[i], name) == 0) {
            free(node->attr_values[i]);
            node->attr_values[i] = fossil_media_strdup(value);
            return 0;
        }
    }
    char **new_names = realloc(node->attr_names, (node->attr_count + 1) * sizeof(char *));
    char **new_values = realloc(node->attr_values, (node->attr_count + 1) * sizeof(char *));
    if (!new_names || !new_values) return -1;
    node->attr_names = new_names;
    node->attr_values = new_values;
    node->attr_names[node->attr_count] = fossil_media_strdup(name);
    node->attr_values[node->attr_count] = fossil_media_strdup(value);
    node->attr_count++;
    return 0;
}

const char *fossil_media_xml_get_attribute(const fossil_media_xml_node_t *node, const char *name) {
    if (!node || node->type != FOSSIL_MEDIA_XML_ELEMENT) return NULL;
    for (size_t i = 0; i < node->attr_count; i++) {
        if (strcmp(node->attr_names[i], name) == 0)
            return node->attr_values[i];
    }
    return NULL;
}

/* -------------------------------------------------------
 * Public API: Parsing
 * ------------------------------------------------------- */

/**
 * @note This parser is minimal — it only supports elements, attributes, and text.
 *       No namespace handling, CDATA, or processing instructions yet.
 */
fossil_media_xml_node_t *
fossil_media_xml_parse(const char *xml_text, fossil_media_xml_error_t *err_out) {
    /* For now, return a dummy root node with text child (placeholder parser) */
    fossil_media_xml_node_t *root = fossil_media_xml_new_element("root");
    if (!root) {
        if (err_out) *err_out = FOSSIL_MEDIA_XML_ERR_MEMORY;
        return NULL;
    }
    fossil_media_xml_node_t *txt = fossil_media_xml_new_text(xml_text);
    if (!txt) {
        fossil_media_xml_free(root);
        if (err_out) *err_out = FOSSIL_MEDIA_XML_ERR_MEMORY;
        return NULL;
    }
    fossil_media_xml_append_child(root, txt);
    if (err_out) *err_out = FOSSIL_MEDIA_XML_OK;
    return root;
}

/* -------------------------------------------------------
 * Public API: Stringify
 * ------------------------------------------------------- */

static void stringify_node(const fossil_media_xml_node_t *node, int pretty, int depth, FILE *out) {
    if (!node) return;
    if (node->type == FOSSIL_MEDIA_XML_ELEMENT) {
        if (pretty) for (int i = 0; i < depth; i++) fputs("  ", out);
        fprintf(out, "<%s", node->name ? node->name : "");
        for (size_t i = 0; i < node->attr_count; i++) {
            fprintf(out, " %s=\"%s\"", node->attr_names[i], node->attr_values[i]);
        }
        if (node->child_count == 0) {
            fputs("/>", out);
            if (pretty) fputc('\n', out);
            return;
        }
        fputc('>', out);
        if (pretty) fputc('\n', out);
        for (size_t i = 0; i < node->child_count; i++) {
            stringify_node(node->children[i], pretty, depth + 1, out);
        }
        if (pretty) for (int i = 0; i < depth; i++) fputs("  ", out);
        fprintf(out, "</%s>", node->name ? node->name : "");
        if (pretty) fputc('\n', out);
    }
    else if (node->type == FOSSIL_MEDIA_XML_TEXT) {
        if (pretty) for (int i = 0; i < depth; i++) fputs("  ", out);
        fputs(node->content ? node->content : "", out);
        if (pretty) fputc('\n', out);
    }
    else if (node->type == FOSSIL_MEDIA_XML_COMMENT) {
        if (pretty) for (int i = 0; i < depth; i++) fputs("  ", out);
        fprintf(out, "<!--%s-->", node->content ? node->content : "");
        if (pretty) fputc('\n', out);
    }
}

char *fossil_media_xml_stringify(const fossil_media_xml_node_t *node, int pretty, fossil_media_xml_error_t *err_out) {
    char *buf = NULL;
    size_t bufsize = 0;
    FILE *mem = open_memstream(&buf, &bufsize);
    if (!mem) {
        if (err_out) *err_out = FOSSIL_MEDIA_XML_ERR_MEMORY;
        return NULL;
    }
    stringify_node(node, pretty, 0, mem);
    fclose(mem);
    if (err_out) *err_out = FOSSIL_MEDIA_XML_OK;
    return buf;
}
