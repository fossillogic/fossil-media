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
#ifndef FOSSIL_MEDIA_YAML_H
#define FOSSIL_MEDIA_YAML_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief YAML key-value pair node.
 */
typedef struct fossil_media_yaml_node_t {
    char *key;                           /**< YAML key string */
    char *value;                         /**< YAML value string */
    size_t indent;                       /**< Indentation level */
    struct fossil_media_yaml_node_t *next; /**< Next node in linked list */
    struct fossil_media_yaml_node_t *child; /**< Child node (for nested maps) */
} fossil_media_yaml_node_t;

/**
 * @brief Parse YAML string into a linked list of nodes.
 * @param input YAML text (null-terminated)
 * @return Head of linked list, or NULL on failure.
 */
fossil_media_yaml_node_t *fossil_media_yaml_parse(const char *input);

/**
 * @brief Free a linked list of YAML nodes.
 * @param head Head of the linked list
 */
void fossil_media_yaml_free(fossil_media_yaml_node_t *head);

/**
 * @brief Get the value for a given key (first match).
 * @param head YAML node list
 * @param key Key string to search
 * @return Value string, or NULL if not found.
 */
const char *fossil_media_yaml_get(const fossil_media_yaml_node_t *head, const char *key);

/**
 * @brief Print the YAML node list to stdout (debug).
 * @param head YAML node list
 */
void fossil_media_yaml_print(const fossil_media_yaml_node_t *head);

#ifdef __cplusplus
}
#include <string>
#include <stdexcept>
#include <utility>

namespace fossil {

    namespace media {

        /**
         * @brief C++ wrapper for YAML parsing and manipulation.
         */
        class Yaml {
        public:
            /**
             * @brief Construct a Yaml object by parsing a YAML string.
             * @param input YAML text (null-terminated)
             * @throw std::runtime_error on parse failure
             */
            explicit Yaml(const char* input)
            : head_(::fossil_media_yaml_parse(input)) {
                if (!head_) {
                    throw std::runtime_error("Failed to parse YAML input");
                }
            }

            /**
             * @brief Destructor. Frees the linked list of YAML nodes.
             */
            ~Yaml() {
                ::fossil_media_yaml_free(head_);
            }

            // Non-copyable
            Yaml(const Yaml&) = delete;
            Yaml& operator=(const Yaml&) = delete;

            // Movable
            Yaml(Yaml&& other) noexcept : head_(other.head_) {
                other.head_ = nullptr;
                }
                Yaml& operator=(Yaml&& other) noexcept {
                if (this != &other) {
                    ::fossil_media_yaml_free(head_);
                    head_ = other.head_;
                    other.head_ = nullptr;
                }
                return *this;
            }

            /**
             * @brief Get the value for a given key (first match).
             * @param key Key string to search
             * @return Value string, or nullptr if not found.
             */
            const char* get(const char* key) const {
                return ::fossil_media_yaml_get(head_, key);
            }

            /**
             * @brief Print the YAML node list to stdout (debug).
             */
            void print() const {
                ::fossil_media_yaml_print(head_);
            }

        private:
            fossil_media_yaml_node_t* head_;
        };

    } // namespace media

} // namespace fossil

#endif

#endif /* FOSSIL_MEDIA_YAML_H */
