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
    FMHTML_OK = 0,
    FMHTML_ERR_IO = -1,
    FMHTML_ERR_NOMEM = -2,
    FMHTML_ERR_PARSE = -3,
    FMHTML_ERR_NOT_FOUND = -4,
};

/* Node types */
typedef enum {
    FMHTML_NODE_ELEMENT,
    FMHTML_NODE_TEXT,
    FMHTML_NODE_COMMENT,
    FMHTML_NODE_DOCUMENT
} fossil_media_html_node_type_t;

/* Forward-declared opaque types */
typedef struct fossil_media_html_doc fossil_media_html_doc_t;
typedef struct fossil_media_html_node fossil_media_html_node_t;

/**
 * @brief Load HTML from a file into memory and parse into a document tree.
 * Must be freed with fossil_media_html_free().
 */
int fossil_media_html_load_file(const char *path, fossil_media_html_doc_t **out_doc);

/**
 * @brief Load HTML from a string buffer (null-terminated).
 */
int fossil_media_html_load_string(const char *data, fossil_media_html_doc_t **out_doc);

/**
 * @brief Free an HTML document and all associated nodes.
 */
void fossil_media_html_free(fossil_media_html_doc_t *doc);

/**
 * @brief Get the root (document) node.
 */
fossil_media_html_node_t* fossil_media_html_root(fossil_media_html_doc_t *doc);

/**
 * @brief Get node type (element, text, etc.)
 */
fossil_media_html_node_type_t fossil_media_html_node_type(const fossil_media_html_node_t *node);

/**
 * @brief Get the tag name of an element node. Returns NULL for text/comment nodes.
 */
const char* fossil_media_html_node_tag(const fossil_media_html_node_t *node);

/**
 * @brief Get text content of a text node. Returns NULL for non-text nodes.
 */
const char* fossil_media_html_node_text(const fossil_media_html_node_t *node);

/**
 * @brief Get first child node (or NULL if none).
 */
fossil_media_html_node_t* fossil_media_html_first_child(fossil_media_html_node_t *node);

/**
 * @brief Get next sibling node (or NULL if none).
 */
fossil_media_html_node_t* fossil_media_html_next_sibling(fossil_media_html_node_t *node);

/**
 * @brief Find first element with given tag name under the subtree.
 */
fossil_media_html_node_t* fossil_media_html_find_by_tag(fossil_media_html_node_t *node, const char *tag);

/**
 * @brief Get attribute value by name (or NULL if not present).
 */
const char* fossil_media_html_get_attr(const fossil_media_html_node_t *node, const char *attr_name);

/**
 * @brief Set (or add) an attribute. Replaces existing attribute value.
 */
int fossil_media_html_set_attr(fossil_media_html_node_t *node, const char *attr_name, const char *attr_value);

/**
 * @brief Serialize HTML document back to a string (caller frees).
 */
char* fossil_media_html_serialize(const fossil_media_html_doc_t *doc);

#ifdef __cplusplus
}
#include <stdexcept>
#include <string>

namespace fossil {

    namespace media {

        class Html {
        public:
            /// Construct an empty HTML document.
            Html() : doc_(nullptr) {}
        
            /// Construct from file path (throws on error)
            explicit Html(const std::string &path) : doc_(nullptr) {
                if (fossil_media_html_load_file(path.c_str(), &doc_) != FMHTML_OK)
                    throw std::runtime_error("Html: failed to load file");
            }
        
            /// Construct from string (throws on error)
            static Html from_string(const std::string &html) {
                Html h;
                if (fossil_media_html_load_string(html.c_str(), &h.doc_) != FMHTML_OK)
                    throw std::runtime_error("Html: failed to parse string");
                return h;
            }
        
            /// Destructor frees underlying document
            ~Html() {
                if (doc_) fossil_media_html_free(doc_);
            }
        
            /// Move constructor
            Html(Html &&other) noexcept : doc_(other.doc_) {
                other.doc_ = nullptr;
            }
        
            /// Move assignment
            Html& operator=(Html &&other) noexcept {
                if (this != &other) {
                    if (doc_) fossil_media_html_free(doc_);
                    doc_ = other.doc_;
                    other.doc_ = nullptr;
                }
                return *this;
            }
        
            /// Deleted copy constructor/assignment
            Html(const Html&) = delete;
            Html& operator=(const Html&) = delete;
        
            /// Access the root node
            fossil_media_html_node_t* root() {
                return fossil_media_html_root(doc_);
            }
        
            /// Serialize to string
            std::string serialize() const {
                char *buf = fossil_media_html_serialize(doc_);
                if (!buf) return {};
                std::string out(buf);
                free(buf);
                return out;
            }
        
            /// Check if document is valid
            bool is_valid() const noexcept {
                return doc_ != nullptr;
            }
        
            /// Reload from file
            void load_file(const std::string &path) {
                if (doc_) fossil_media_html_free(doc_);
                if (fossil_media_html_load_file(path.c_str(), &doc_) != FMHTML_OK)
                    throw std::runtime_error("Html: failed to reload file");
            }
        
            /// Reload from string
            void load_string(const std::string &html) {
                if (doc_) fossil_media_html_free(doc_);
                if (fossil_media_html_load_string(html.c_str(), &doc_) != FMHTML_OK)
                    throw std::runtime_error("Html: failed to reload string");
            }
        
        private:
            fossil_media_html_doc_t *doc_;
        };

    } // namespace media

} // namespace fossil

#endif

#endif /* FOSSIL_MEDIA_HTML_H */
