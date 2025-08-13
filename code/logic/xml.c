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
#define _GNU_SOURCE
#include "fossil/media/xml.h"
#include "fossil/media/media.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* -------------------------------------------------------
 * Internal helpers
 * ------------------------------------------------------- */

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

typedef struct {
    char *buffer;
    size_t length;
    size_t capacity;
    int error;
} mem_buffer_t;

static int mem_buffer_append(mem_buffer_t *mb, const char *str, size_t len) {
    if (mb->error) return -1;
    if (mb->length + len + 1 > mb->capacity) {
        size_t new_cap = mb->capacity ? mb->capacity * 2 : 256;
        while (new_cap < mb->length + len + 1) new_cap *= 2;
        char *new_buf = (char *)realloc(mb->buffer, new_cap);
        if (!new_buf) {
            mb->error = 1;
            return -1;
        }
        mb->buffer = new_buf;
        mb->capacity = new_cap;
    }
    memcpy(mb->buffer + mb->length, str, len);
    mb->length += len;
    mb->buffer[mb->length] = '\0';
    return 0;
}

/* Modified stringify_node that writes to mem_buffer_t */
static int stringify_node_to_buffer(const fossil_media_xml_node_t *node, int pretty, int depth, mem_buffer_t *mb);

/* Helper to append indentation */
static int append_indent(mem_buffer_t *mb, int depth) {
    for (int i = 0; i < depth; i++) {
        if (mem_buffer_append(mb, "  ", 2) < 0) return -1;
    }
    return 0;
}

static int stringify_node_to_buffer(const fossil_media_xml_node_t *node, int pretty, int depth, mem_buffer_t *mb) {
    if (!node || mb->error) return -1;

    if (node->type == FOSSIL_MEDIA_XML_ELEMENT) {
        if (pretty && append_indent(mb, depth) < 0) return -1;
        if (mem_buffer_append(mb, "<", 1) < 0) return -1;
        if (node->name && mem_buffer_append(mb, node->name, strlen(node->name)) < 0) return -1;

        for (size_t i = 0; i < node->attr_count; i++) {
            if (mem_buffer_append(mb, " ", 1) < 0) return -1;
            if (mem_buffer_append(mb, node->attr_names[i], strlen(node->attr_names[i])) < 0) return -1;
            if (mem_buffer_append(mb, "=\"", 2) < 0) return -1;
            if (mem_buffer_append(mb, node->attr_values[i], strlen(node->attr_values[i])) < 0) return -1;
            if (mem_buffer_append(mb, "\"", 1) < 0) return -1;
        }

        if (node->child_count == 0) {
            if (mem_buffer_append(mb, "/>", 2) < 0) return -1;
            if (pretty && mem_buffer_append(mb, "\n", 1) < 0) return -1;
            return 0;
        }

        if (mem_buffer_append(mb, ">", 1) < 0) return -1;
        if (pretty && mem_buffer_append(mb, "\n", 1) < 0) return -1;

        for (size_t i = 0; i < node->child_count; i++) {
            if (stringify_node_to_buffer(node->children[i], pretty, depth + 1, mb) < 0) return -1;
        }

        if (pretty && append_indent(mb, depth) < 0) return -1;
        if (mem_buffer_append(mb, "</", 2) < 0) return -1;
        if (node->name && mem_buffer_append(mb, node->name, strlen(node->name)) < 0) return -1;
        if (mem_buffer_append(mb, ">", 1) < 0) return -1;
        if (pretty && mem_buffer_append(mb, "\n", 1) < 0) return -1;
    }
    else if (node->type == FOSSIL_MEDIA_XML_TEXT) {
        if (pretty && append_indent(mb, depth) < 0) return -1;
        if (node->content && mem_buffer_append(mb, node->content, strlen(node->content)) < 0) return -1;
        if (pretty && mem_buffer_append(mb, "\n", 1) < 0) return -1;
    }
    else if (node->type == FOSSIL_MEDIA_XML_COMMENT) {
        if (pretty && append_indent(mb, depth) < 0) return -1;
        if (mem_buffer_append(mb, "<!--", 4) < 0) return -1;
        if (node->content && mem_buffer_append(mb, node->content, strlen(node->content)) < 0) return -1;
        if (mem_buffer_append(mb, "-->", 3) < 0) return -1;
        if (pretty && mem_buffer_append(mb, "\n", 1) < 0) return -1;
    }

    return 0;
}

/**
 * Portable replacement for fossil_media_xml_stringify without open_memstream.
 */
char *fossil_media_xml_stringify(const fossil_media_xml_node_t *node, int pretty, fossil_media_xml_error_t *err_out) {
    mem_buffer_t mb = {0};
    if (stringify_node_to_buffer(node, pretty, 0, &mb) < 0) {
        free(mb.buffer);
        if (err_out) *err_out = FOSSIL_MEDIA_XML_ERR_MEMORY;
        return NULL;
    }
    if (err_out) *err_out = FOSSIL_MEDIA_XML_OK;
    return mb.buffer;
}

fossil_media_xml_node_t *fossil_media_xml_first_child(fossil_media_xml_node_t *node) {
    if (!node || node->child_count == 0) {
        return NULL;
    }
    return node->children[0];
}

fossil_media_xml_node_t *fossil_media_xml_new_pi(const char *target, const char *data) {
    fossil_media_xml_node_t *node = (fossil_media_xml_node_t *)calloc(1, sizeof(fossil_media_xml_node_t));
    if (!node) {
        return NULL;
    }

    node->type = FOSSIL_MEDIA_XML_PI;

    if (target) {
        node->name = fissil_media_strdup(target);
    }
    if (content) {
        node->content = fissil_media_strdup(data);
    }

    node->children = NULL;
    node->child_count = 0;
    node->attr_names = NULL;
    node->attr_values = NULL;
    node->attr_count = 0;
    node->parent = NULL;

    return node;
}
