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
#ifndef FOSSIL_MEDIA_XML_H
#define FOSSIL_MEDIA_XML_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Node type in the XML DOM.
 */
typedef enum fossil_media_xml_type_t {
    FOSSIL_MEDIA_XML_ELEMENT, /**< Standard XML element */
    FOSSIL_MEDIA_XML_TEXT,    /**< Text node */
    FOSSIL_MEDIA_XML_COMMENT, /**< Comment node */
    FOSSIL_MEDIA_XML_PI
} fossil_media_xml_type_t;

/**
 * @brief Error codes for XML operations.
 */
typedef enum fossil_media_xml_error_t {
    FOSSIL_MEDIA_XML_OK = 0,
    FOSSIL_MEDIA_XML_ERR_MEMORY,
    FOSSIL_MEDIA_XML_ERR_PARSE
} fossil_media_xml_error_t;

/**
 * @brief XML DOM Node structure.
 *
 * Stores element names, attributes, text content, and hierarchical children.
 * Children and attributes are stored in dynamically allocated arrays.
 */
typedef struct fossil_media_xml_node_t {
    fossil_media_xml_type_t type;  /**< Node type (element, text, comment) */
    char *name;                    /**< Element name (NULL for text/comment nodes) */
    char *content;                  /**< Text or comment content (NULL for elements without direct text) */

    struct fossil_media_xml_node_t **children; /**< Array of pointers to child nodes */
    size_t child_count;                        /**< Number of child nodes */

    char **attr_names;   /**< Array of attribute names */
    char **attr_values;  /**< Array of attribute values */
    size_t attr_count;   /**< Number of attributes */

    struct fossil_media_xml_node_t *parent; /**< Parent node (NULL for root) */
} fossil_media_xml_node_t;

/* ----------------------------------------------------------------------
 * Parsing and memory management
 * ---------------------------------------------------------------------- */

/**
 * Parse XML text into a DOM-like tree.
 * - xml_text must be NUL-terminated.
 * - The returned tree must be freed with fossil_media_xml_free().
 * @param xml_text The XML string to parse.
 * @param err_out Pointer to an error struct for reporting errors (may be NULL).
 * @return Pointer to the root node of the parsed document, or NULL on failure.
 */
fossil_media_xml_node_t *
fossil_media_xml_parse(const char *xml_text, fossil_media_xml_error_t *err_out);

/**
 * Free an XML DOM tree.
 * @param node Pointer to the root node to free.
 */
void fossil_media_xml_free(fossil_media_xml_node_t *node);

/* ----------------------------------------------------------------------
 * Node creation helpers
 * ---------------------------------------------------------------------- */

/**
 * Create a new XML element node.
 * @param name The element name (copied internally).
 * @return Pointer to new node, or NULL on allocation failure.
 */
fossil_media_xml_node_t *fossil_media_xml_new_element(const char *name);

/**
 * Create a new text node.
 * @param text The text content (copied internally).
 */
fossil_media_xml_node_t *fossil_media_xml_new_text(const char *text);

/**
 * Create a new comment node.
 * @param text The comment text (copied internally).
 */
fossil_media_xml_node_t *fossil_media_xml_new_comment(const char *text);

/**
 * Create a new CDATA section node.
 * @param text The CDATA content (copied internally).
 */
fossil_media_xml_node_t *fossil_media_xml_new_cdata(const char *text);

/**
 * @brief Create a new processing instruction (PI) XML node.
 *
 * @param target The PI target (e.g., "xml-stylesheet").
 * @param data The PI data string.
 * @return Pointer to the new PI node, or NULL on allocation failure.
 */
fossil_media_xml_node_t *fossil_media_xml_new_pi(const char *target, const char *data);

/* ----------------------------------------------------------------------
 * Tree manipulation
 * ---------------------------------------------------------------------- */

/**
 * Append a child node to a parent.
 * @param parent Parent node (must be element or document).
 * @param child Child node to append (ownership transferred).
 * @return 0 on success, nonzero on error.
 */
int fossil_media_xml_append_child(fossil_media_xml_node_t *parent, fossil_media_xml_node_t *child);

/**
 * Get the first child node of a parent.
 * @param node Parent node.
 * @return Pointer to first child, or NULL if none.
 */
fossil_media_xml_node_t *fossil_media_xml_first_child(fossil_media_xml_node_t *node);

/**
 * Get the next sibling node.
 * @param node Current node.
 * @return Pointer to next sibling, or NULL if none.
 */
fossil_media_xml_node_t *fossil_media_xml_next_sibling(const fossil_media_xml_node_t *node);

/* ----------------------------------------------------------------------
 * Attributes
 * ---------------------------------------------------------------------- */

/**
 * Set or replace an attribute on an element node.
 * @param elem Element node.
 * @param name Attribute name.
 * @param value Attribute value.
 * @return 0 on success, nonzero on error.
 */
int fossil_media_xml_set_attribute(fossil_media_xml_node_t *elem, const char *name, const char *value);

/**
 * Get an attribute value by name.
 * @param elem Element node.
 * @param name Attribute name.
 * @return Attribute value string, or NULL if not found.
 */
const char *fossil_media_xml_get_attribute(const fossil_media_xml_node_t *elem, const char *name);

/* ----------------------------------------------------------------------
 * Serialization
 * ---------------------------------------------------------------------- */

/**
 * Serialize an XML node (and its children) to a string.
 * - Caller must free returned string with free().
 * @param node Node to serialize.
 * @param pretty If nonzero, indent for human readability.
 * @param err_out Optional error output.
 * @return Heap-allocated XML string, or NULL on failure.
 */
char *fossil_media_xml_stringify(const fossil_media_xml_node_t *node, int pretty, fossil_media_xml_error_t *err_out);

/* ----------------------------------------------------------------------
 * Utility
 * ---------------------------------------------------------------------- */

/**
 * Get the type name of an XML node type.
 * @param t Node type enum value.
 * @return String representation (static).
 */
const char *fossil_media_xml_type_name(fossil_media_xml_type_t t);

#ifdef __cplusplus
}
#include <string>
#include <stdexcept>
#include <utility>

namespace fossil {

    namespace media {



    } // namespace media

} // namespace fossil

#endif

#endif /* FOSSIL_MEDIA_XML_H */
