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
#ifndef FOSSIL_MEDIA_HTML_H
#define FOSSIL_MEDIA_HTML_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* Error codes (negative) */
enum {
    FOSSIL_MEDIA_HTML_OK = 0,
    FOSSIL_MEDIA_HTML_ERR_IO = -1,
    FOSSIL_MEDIA_HTML_ERR_NOMEM = -2,
    FOSSIL_MEDIA_HTML_ERR_PARSE = -3,
    FOSSIL_MEDIA_HTML_ERR_NOT_FOUND = -4,
    FOSSIL_MEDIA_HTML_ERR_TIMEOUT = -5,
    FOSSIL_MEDIA_HTML_ERR_INVALID_ARG = -6
};

/* Node types */
typedef enum {
    FOSSIL_MEDIA_HTML_NODE_ELEMENT,
    FOSSIL_MEDIA_HTML_NODE_TEXT,
    FOSSIL_MEDIA_HTML_NODE_COMMENT,
    FOSSIL_MEDIA_HTML_NODE_DOCUMENT,
    FOSSIL_MEDIA_HTML_NODE_DOCTYPE,
    FOSSIL_MEDIA_HTML_NODE_CDATA,
    FOSSIL_MEDIA_HTML_NODE_PROCESSING_INSTRUCTION
} fossil_media_html_node_type_t;

/* Forward-declared opaque types */
typedef struct fossil_media_html_doc fossil_media_html_doc_t;
typedef struct fossil_media_html_node fossil_media_html_node_t;

/**
 * @brief Load HTML from a file into memory and parse into a document tree.
 * 
 * This function reads the contents of the file specified by `path`, parses the HTML,
 * and constructs a document tree in memory. The resulting document pointer is stored
 * in `out_doc`. The caller is responsible for freeing the document using
 * fossil_media_html_free() when done.
 * 
 * @param path Path to the HTML file to load.
 * @param out_doc Pointer to receive the allocated document tree.
 * @return FOSSIL_MEDIA_HTML_OK on success, negative error code on failure.
 */
int fossil_media_html_load_file(const char *path, fossil_media_html_doc_t **out_doc);

/**
 * @brief Load HTML from a string buffer (null-terminated).
 * 
 * Parses the HTML content provided in the null-terminated string `data` and builds
 * a document tree in memory. The resulting document pointer is stored in `out_doc`.
 * The caller must free the document using fossil_media_html_free().
 * 
 * @param data Null-terminated string containing HTML data.
 * @param out_doc Pointer to receive the allocated document tree.
 * @return FOSSIL_MEDIA_HTML_OK on success, negative error code on failure.
 */
int fossil_media_html_load_string(const char *data, fossil_media_html_doc_t **out_doc);

/**
 * @brief Free an HTML document and all associated nodes.
 * 
 * Releases all memory associated with the document tree pointed to by `doc`,
 * including all nodes and attributes. After calling this function, the document
 * pointer is no longer valid.
 * 
 * @param doc Pointer to the document tree to free.
 */
void fossil_media_html_free(fossil_media_html_doc_t *doc);

/**
 * @brief Get the root (document) node.
 * 
 * Returns a pointer to the root node of the HTML document tree. This node
 * represents the entire document and is typically of type FOSSIL_MEDIA_HTML_NODE_DOCUMENT.
 * 
 * @param doc Pointer to the HTML document.
 * @return Pointer to the root node, or NULL if the document is invalid.
 */
fossil_media_html_node_t* fossil_media_html_root(fossil_media_html_doc_t *doc);

/**
 * @brief Get node type (element, text, etc.)
 * 
 * Returns the type of the specified node, indicating whether it is an element,
 * text, comment, or document node.
 * 
 * @param node Pointer to the node to query.
 * @return Node type as fossil_media_html_node_type_t.
 */
fossil_media_html_node_type_t fossil_media_html_node_type(const fossil_media_html_node_t *node);

/**
 * @brief Get the tag name of an element node. Returns NULL for text/comment nodes.
 * 
 * If the node is an element, returns its tag name (e.g., "div", "span"). For
 * text or comment nodes, returns NULL.
 * 
 * @param node Pointer to the node to query.
 * @return Tag name string, or NULL if not an element node.
 */
const char* fossil_media_html_node_tag(const fossil_media_html_node_t *node);

/**
 * @brief Get text content of a text node. Returns NULL for non-text nodes.
 * 
 * If the node is a text node, returns its text content. For other node types,
 * returns NULL.
 * 
 * @param node Pointer to the node to query.
 * @return Text content string, or NULL if not a text node.
 */
const char* fossil_media_html_node_text(const fossil_media_html_node_t *node);

/**
 * @brief Get first child node (or NULL if none).
 * 
 * Returns a pointer to the first child node of the specified node. If the node
 * has no children, returns NULL.
 * 
 * @param node Pointer to the parent node.
 * @return Pointer to the first child node, or NULL if none.
 */
fossil_media_html_node_t* fossil_media_html_first_child(fossil_media_html_node_t *node);

/**
 * @brief Get next sibling node (or NULL if none).
 * 
 * Returns a pointer to the next sibling node following the specified node. If
 * there are no more siblings, returns NULL.
 * 
 * @param node Pointer to the current node.
 * @return Pointer to the next sibling node, or NULL if none.
 */
fossil_media_html_node_t* fossil_media_html_next_sibling(fossil_media_html_node_t *node);

/**
 * @brief Find first element with given tag name under the subtree.
 * 
 * Searches the subtree rooted at the specified node for the first element node
 * with the given tag name. Returns a pointer to the matching node, or NULL if
 * not found.
 * 
 * @param node Pointer to the root of the subtree to search.
 * @param tag Tag name to search for (case-sensitive).
 * @return Pointer to the matching element node, or NULL if not found.
 */
fossil_media_html_node_t* fossil_media_html_find_by_tag(fossil_media_html_node_t *node, const char *tag);

/**
 * @brief Get attribute value by name (or NULL if not present).
 * 
 * Returns the value of the attribute with the specified name for the given node.
 * If the attribute is not present, returns NULL.
 * 
 * @param node Pointer to the node to query.
 * @param attr_name Name of the attribute to retrieve.
 * @return Attribute value string, or NULL if not present.
 */
const char* fossil_media_html_get_attr(const fossil_media_html_node_t *node, const char *attr_name);

/**
 * @brief Set (or add) an attribute. Replaces existing attribute value.
 * 
 * Sets the value of the attribute with the specified name for the given node.
 * If the attribute already exists, its value is replaced. If not, the attribute
 * is added.
 * 
 * @param node Pointer to the node to modify.
 * @param attr_name Name of the attribute to set.
 * @param attr_value Value to assign to the attribute.
 * @return FOSSIL_MEDIA_HTML_OK on success, negative error code on failure.
 */
int fossil_media_html_set_attr(fossil_media_html_node_t *node, const char *attr_name, const char *attr_value);

/**
 * @brief Serialize HTML document back to a string (caller frees).
 * 
 * Converts the HTML document tree into a string representation. The returned
 * string is dynamically allocated and must be freed by the caller using free().
 * 
 * @param doc Pointer to the HTML document to serialize.
 * @return Pointer to the serialized HTML string, or NULL on failure.
 */
char* fossil_media_html_serialize(const fossil_media_html_doc_t *doc);

#ifdef __cplusplus
}
#include <stdexcept>
#include <string>

namespace fossil {

    namespace media {

        /**
         * @brief C++ wrapper for Fossil HTML document.
         *
         * This class provides RAII management and convenient methods for working
         * with HTML documents using the Fossil Logic C API. It supports loading
         * documents from files or strings, serialization, and move semantics.
         */
        class Html {
        public:
            /**
             * @brief Construct an empty HTML document.
             *
             * Initializes the internal document pointer to nullptr.
             */
            Html() : doc_(nullptr) {}

            /**
             * @brief Construct from file path (throws on error).
             *
             * Loads and parses an HTML file from the given path. Throws
             * std::runtime_error if loading fails.
             *
             * @param path Path to the HTML file.
             */
            explicit Html(const std::string &path) : doc_(nullptr) {
                if (fossil_media_html_load_file(path.c_str(), &doc_) != FOSSIL_MEDIA_HTML_OK)
                    throw std::runtime_error("Html: failed to load file");
            }

            /**
             * @brief Construct from string (throws on error).
             *
             * Parses HTML content from the provided string and returns an Html
             * object. Throws std::runtime_error if parsing fails.
             *
             * @param html HTML content string.
             * @return Html object containing the parsed document.
             */
            static Html from_string(const std::string &html) {
                Html h;
                if (fossil_media_html_load_string(html.c_str(), &h.doc_) != FOSSIL_MEDIA_HTML_OK)
                    throw std::runtime_error("Html: failed to parse string");
                return h;
            }

            /**
             * @brief Destructor frees underlying document.
             *
             * Releases all memory associated with the HTML document.
             */
            ~Html() {
                if (doc_) fossil_media_html_free(doc_);
            }

            /**
             * @brief Move constructor.
             *
             * Transfers ownership of the document pointer from another Html object.
             *
             * @param other Html object to move from.
             */
            Html(Html &&other) noexcept : doc_(other.doc_) {
                other.doc_ = nullptr;
            }

            /**
             * @brief Move assignment operator.
             *
             * Frees any existing document and transfers ownership from another Html object.
             *
             * @param other Html object to move from.
             * @return Reference to this object.
             */
            Html& operator=(Html &&other) noexcept {
                if (this != &other) {
                    if (doc_) fossil_media_html_free(doc_);
                    doc_ = other.doc_;
                    other.doc_ = nullptr;
                }
                return *this;
            }

            /**
             * @brief Deleted copy constructor.
             *
             * Prevents copying of Html objects.
             */
            Html(const Html&) = delete;

            /**
             * @brief Deleted copy assignment operator.
             *
             * Prevents copying of Html objects.
             */
            Html& operator=(const Html&) = delete;

            /**
             * @brief Access the root node of the document.
             *
             * Returns a pointer to the root node of the HTML document tree.
             *
             * @return Pointer to the root node, or nullptr if invalid.
             */
            fossil_media_html_node_t* root() {
                return fossil_media_html_root(doc_);
            }

            /**
             * @brief Serialize document to string.
             *
             * Converts the HTML document tree into a string representation.
             *
             * @return Serialized HTML string.
             */
            std::string serialize() const {
                char *buf = fossil_media_html_serialize(doc_);
                if (!buf) return {};
                std::string out(buf);
                free(buf);
                return out;
            }

            /**
             * @brief Check if document is valid.
             *
             * Indicates whether the document pointer is non-null.
             *
             * @return True if valid, false otherwise.
             */
            bool is_valid() const noexcept {
                return doc_ != nullptr;
            }

            /**
             * @brief Reload document from file.
             *
             * Frees any existing document and loads a new one from the specified file.
             * Throws std::runtime_error on failure.
             *
             * @param path Path to the HTML file.
             */
            void load_file(const std::string &path) {
                if (doc_) fossil_media_html_free(doc_);
                if (fossil_media_html_load_file(path.c_str(), &doc_) != FOSSIL_MEDIA_HTML_OK)
                    throw std::runtime_error("Html: failed to reload file");
            }

            /**
             * @brief Reload document from string.
             *
             * Frees any existing document and parses a new one from the provided string.
             * Throws std::runtime_error on failure.
             *
             * @param html HTML content string.
             */
            void load_string(const std::string &html) {
                if (doc_) fossil_media_html_free(doc_);
                if (fossil_media_html_load_string(html.c_str(), &doc_) != FOSSIL_MEDIA_HTML_OK)
                    throw std::runtime_error("Html: failed to reload string");
            }

        private:
            /**
             * @brief Pointer to the underlying Fossil HTML document.
             */
            fossil_media_html_doc_t *doc_;
        };

    } // namespace media

} // namespace fossil

#endif

#endif /* FOSSIL_MEDIA_HTML_H */
