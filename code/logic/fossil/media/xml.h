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
    FOSSIL_MEDIA_XML_PI,
    FOSSIL_MEDIA_XML_CDATA
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
const char* fossil_media_xml_type_name(int type);

#ifdef __cplusplus
}
#include <string>
#include <stdexcept>
#include <utility>

namespace fossil {

    namespace media {

        /**
         * @brief C++ RAII wrapper for fossil_media_xml_node_t and related XML operations.
         *
         * Provides safe memory management and convenient methods for XML parsing,
         * node creation, tree manipulation, attribute handling, and serialization.
         */
        class Xml {
        public:
            /**
             * @brief Construct an empty Xml object (null node).
             */
            Xml() noexcept : node_(nullptr) {}

            /**
             * @brief Parse XML text into a DOM-like tree.
             * @param xml_text The XML string to parse.
             * @throws std::runtime_error on parse error.
             */
            explicit Xml(const char* xml_text) {
                fossil_media_xml_error_t err = FOSSIL_MEDIA_XML_OK;
                node_ = fossil_media_xml_parse(xml_text, &err);
                if (!node_ || err != FOSSIL_MEDIA_XML_OK) {
                    throw std::runtime_error("XML parse error");
                }
            }

            /**
             * @brief Construct from an existing node (takes ownership).
             * @param node Pointer to fossil_media_xml_node_t.
             */
            explicit Xml(fossil_media_xml_node_t* node) noexcept : node_(node) {}

            /**
             * @brief Move constructor.
             */
            Xml(Xml&& other) noexcept : node_(other.node_) {
                other.node_ = nullptr;
            }

            /**
             * @brief Move assignment.
             */
            Xml& operator=(Xml&& other) noexcept {
                if (this != &other) {
                    reset();
                    node_ = other.node_;
                    other.node_ = nullptr;
                }
                return *this;
            }

            /**
             * @brief Destructor. Frees the XML DOM tree.
             */
            ~Xml() { reset(); }

            /**
             * @brief Release ownership of the node.
             * @return Pointer to fossil_media_xml_node_t, or nullptr.
             */
            fossil_media_xml_node_t* release() noexcept {
                auto* tmp = node_;
                node_ = nullptr;
                return tmp;
            }

            /**
             * @brief Reset to a new node, freeing the old one.
             * @param node New node pointer (default nullptr).
             */
            void reset(fossil_media_xml_node_t* node = nullptr) noexcept {
                if (node_) fossil_media_xml_free(node_);
                node_ = node;
            }

            /**
             * @brief Access the underlying node pointer.
             */
            fossil_media_xml_node_t* get() noexcept { return node_; }
            const fossil_media_xml_node_t* get() const noexcept { return node_; }

            /**
             * @brief Create a new XML element node.
             * @param name Element name.
             * @return Xml object owning the new node.
             */
            static Xml new_element(const char* name) {
                return Xml(fossil_media_xml_new_element(name));
            }

            /**
             * @brief Create a new text node.
             * @param text Text content.
             * @return Xml object owning the new node.
             */
            static Xml new_text(const char* text) {
                return Xml(fossil_media_xml_new_text(text));
            }

            /**
             * @brief Create a new comment node.
             * @param text Comment text.
             * @return Xml object owning the new node.
             */
            static Xml new_comment(const char* text) {
                return Xml(fossil_media_xml_new_comment(text));
            }

            /**
             * @brief Create a new CDATA section node.
             * @param text CDATA content.
             * @return Xml object owning the new node.
             */
            static Xml new_cdata(const char* text) {
                return Xml(fossil_media_xml_new_cdata(text));
            }

            /**
             * @brief Create a new processing instruction node.
             * @param target PI target.
             * @param data PI data.
             * @return Xml object owning the new node.
             */
            static Xml new_pi(const char* target, const char* data) {
                return Xml(fossil_media_xml_new_pi(target, data));
            }

            /**
             * @brief Append a child node to this node.
             * @param child Xml object (ownership transferred).
             * @throws std::runtime_error on error.
             */
            void append_child(Xml&& child) {
                if (!node_ || !child.node_) throw std::runtime_error("Null node");
                if (fossil_media_xml_append_child(node_, child.node_) != 0)
                    throw std::runtime_error("Failed to append child");
                child.node_ = nullptr;
            }

            /**
             * @brief Get the first child node.
             * @return Xml object (non-owning, does not free).
             */
            Xml first_child() const {
                if (!node_) return Xml();
                return Xml(fossil_media_xml_first_child(node_));
            }

            /**
             * @brief Get the next sibling node.
             * @return Xml object (non-owning, does not free).
             */
            Xml next_sibling() const {
                if (!node_) return Xml();
                return Xml(const_cast<fossil_media_xml_node_t*>(
                    fossil_media_xml_next_sibling(node_)));
            }

            /**
             * @brief Set or replace an attribute on an element node.
             * @param name Attribute name.
             * @param value Attribute value.
             * @throws std::runtime_error on error.
             */
            void set_attribute(const char* name, const char* value) {
                if (!node_) throw std::runtime_error("Null node");
                if (fossil_media_xml_set_attribute(node_, name, value) != 0)
                    throw std::runtime_error("Failed to set attribute");
            }

            /**
             * @brief Get an attribute value by name.
             * @param name Attribute name.
             * @return Attribute value string, or nullptr if not found.
             */
            const char* get_attribute(const char* name) const {
                if (!node_) return nullptr;
                return fossil_media_xml_get_attribute(node_, name);
            }

            /**
             * @brief Serialize this node (and its children) to a string.
             * @param pretty Indent for human readability.
             * @return std::string with XML content.
             * @throws std::runtime_error on error.
             */
            std::string stringify(bool pretty = false) const {
                if (!node_) return {};
                fossil_media_xml_error_t err = FOSSIL_MEDIA_XML_OK;
                char* str = fossil_media_xml_stringify(node_, pretty ? 1 : 0, &err);
                if (!str || err != FOSSIL_MEDIA_XML_OK)
                    throw std::runtime_error("XML stringify error");
                std::string result(str);
                free(str);
                return result;
            }

            /**
             * @brief Get the type name of this node.
             * @return Static string representation.
             */
            const char* type_name() const {
                if (!node_) return nullptr;
                return fossil_media_xml_type_name(node_->type);
            }

            // Non-copyable
            Xml(const Xml&) = delete;
            Xml& operator=(const Xml&) = delete;

        private:
            fossil_media_xml_node_t* node_;
        };

    } // namespace media

} // namespace fossil

#endif

#endif /* FOSSIL_MEDIA_XML_H */
