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
#ifndef FOSSIL_MEDIA_FSON_H
#define FOSSIL_MEDIA_FSON_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Error codes */
enum {
    FOSSIL_MEDIA_FSON_OK = 0,
    FOSSIL_MEDIA_FSON_ERR_IO = -1,
    FOSSIL_MEDIA_FSON_ERR_NOMEM = -2,
    FOSSIL_MEDIA_FSON_ERR_PARSE = -3,
    FOSSIL_MEDIA_FSON_ERR_TYPE = -4,
    FOSSIL_MEDIA_FSON_ERR_RANGE = -5,
    FOSSIL_MEDIA_FSON_ERR_NOT_FOUND = -6,
    FOSSIL_MEDIA_FSON_ERR_INVALID_ARG = -7
};

/* Value types (explicit, matches FSON spec) */
typedef enum {
    FSON_TYPE_NULL = 0,
    FSON_TYPE_BOOL,
    FSON_TYPE_I8,
    FSON_TYPE_I16,
    FSON_TYPE_I32,
    FSON_TYPE_I64,
    FSON_TYPE_U8,
    FSON_TYPE_U16,
    FSON_TYPE_U32,
    FSON_TYPE_U64,
    FSON_TYPE_F32,
    FSON_TYPE_F64,
    FSON_TYPE_OCT,
    FSON_TYPE_HEX,
    FSON_TYPE_BIN,
    FSON_TYPE_CHAR,
    FSON_TYPE_CSTR,
    FSON_TYPE_ARRAY,
    FSON_TYPE_OBJECT
} fossil_media_fson_type_t;

/* Forward declaration for opaque handle */
typedef struct fossil_media_fson fossil_media_fson_t;

/* Node representation (lightweight view, not owned) */
typedef struct {
    const char *key;
    fossil_media_fson_type_t type;
    union {
        int64_t i64;
        uint64_t u64;
        float f32;
        double f64;
        char c;
        const char *str;
    } value;
    size_t child_count;
    const struct fossil_media_fson *children;
} fossil_media_fson_node_t;

/**
 * @brief Load FSON from a file.
 *
 * @param path Path to the FSON file.
 * @param out Pointer to receive handle.
 * @return FOSSIL_MEDIA_FSON_OK or negative error code.
 */
int fossil_media_fson_load_file(const char *path, fossil_media_fson_t **out);

/**
 * @brief Load FSON from memory buffer.
 *
 * @param buf Pointer to FSON text.
 * @param len Length of buffer in bytes.
 * @param out Pointer to receive handle.
 * @return FOSSIL_MEDIA_FSON_OK or negative error code.
 */
int fossil_media_fson_load_memory(const void *buf, size_t len, fossil_media_fson_t **out);

/**
 * @brief Free a FSON handle.
 *
 * @param fson Handle to free.
 */
void fossil_media_fson_free(fossil_media_fson_t *fson);

/**
 * @brief Get root node of the parsed FSON document.
 *
 * @param fson FSON handle.
 * @return Pointer to root node or NULL on error.
 */
const fossil_media_fson_node_t *fossil_media_fson_root(const fossil_media_fson_t *fson);

/**
 * @brief Get a child node by key from an object node.
 *
 * @param node Parent node (must be FSON_TYPE_OBJECT).
 * @param key Key to search for.
 * @return Pointer to matching child node or NULL if not found.
 */
const fossil_media_fson_node_t *fossil_media_fson_get(const fossil_media_fson_node_t *node, const char *key);

/**
 * @brief Get an element by index from an array node.
 *
 * @param node Parent node (must be FSON_TYPE_ARRAY).
 * @param index Zero-based index.
 * @return Pointer to element node or NULL if out of range.
 */
const fossil_media_fson_node_t *fossil_media_fson_at(const fossil_media_fson_node_t *node, size_t index);

/**
 * @brief Serialize a FSON tree back to text.
 *
 * @param fson Handle to serialize.
 * @param out_text Pointer to receive allocated string (null-terminated).
 * @param out_len Pointer to receive string length.
 * @return FOSSIL_MEDIA_FSON_OK or negative error code.
 *
 * @note Caller must free out_text using free().
 */
int fossil_media_fson_serialize(const fossil_media_fson_t *fson, char **out_text, size_t *out_len);

/**
 * @brief Validate a FSON handle for type correctness.
 *
 * @param fson Handle to validate.
 * @return FOSSIL_MEDIA_FSON_OK if valid, error code otherwise.
 */
int fossil_media_fson_validate(const fossil_media_fson_t *fson);

#ifdef __cplusplus
}

#include <string>
#include <stdexcept>

namespace fossil {

    namespace media {

        /**
         * @brief C++ RAII wrapper for fossil_media_fson_t handle.
         *
         * Provides convenient methods for loading, accessing, serializing,
         * and validating FSON documents using the C API.
         */
        class FSON {
        public:
            /**
             * @brief Default constructor. Creates an empty FSON handle.
             */
            FSON() : handle_(nullptr) {}

            /**
             * @brief Load FSON from file.
             * @param path Path to FSON file.
             * @throws std::runtime_error on failure.
             */
            explicit FSON(const std::string& path) : handle_(nullptr) {
                int rc = fossil_media_fson_load_file(path.c_str(), &handle_);
                if (rc != FOSSIL_MEDIA_FSON_OK) {
                    throw std::runtime_error("Failed to load FSON file");
                }
            }

            /**
             * @brief Load FSON from memory buffer.
             * @param buf Pointer to FSON text.
             * @param len Length of buffer in bytes.
             * @throws std::runtime_error on failure.
             */
            FSON(const void* buf, size_t len) : handle_(nullptr) {
                int rc = fossil_media_fson_load_memory(buf, len, &handle_);
                if (rc != FOSSIL_MEDIA_FSON_OK) {
                    throw std::runtime_error("Failed to load FSON from memory");
                }
            }

            /**
             * @brief Destructor. Frees the FSON handle.
             */
            ~FSON() {
                if (handle_) {
                    fossil_media_fson_free(handle_);
                }
            }

            /** Deleted copy constructor. */
            FSON(const FSON&) = delete;
            /** Deleted copy assignment. */
            FSON& operator=(const FSON&) = delete;

            /**
             * @brief Move constructor.
             */
            FSON(FSON&& other) noexcept : handle_(other.handle_) {
                other.handle_ = nullptr;
            }

            /**
             * @brief Move assignment operator.
             */
            FSON& operator=(FSON&& other) noexcept {
                if (this != &other) {
                    if (handle_) {
                    fossil_media_fson_free(handle_);
                    }
                    handle_ = other.handle_;
                    other.handle_ = nullptr;
                }
                return *this;
            }

            /**
             * @brief Get root node of the FSON document.
             * @return Pointer to root node.
             */
            const fossil_media_fson_node_t* root() const {
                return fossil_media_fson_root(handle_);
            }

            /**
             * @brief Serialize FSON tree to text.
             * @return Serialized string.
             * @throws std::runtime_error on failure.
             */
            std::string serialize() const {
                char* out_text = nullptr;
                size_t out_len = 0;
                int rc = fossil_media_fson_serialize(handle_, &out_text, &out_len);
                if (rc != FOSSIL_MEDIA_FSON_OK || !out_text) {
                    throw std::runtime_error("Failed to serialize FSON");
                }
                std::string result(out_text, out_len);
                free(out_text);
                return result;
            }

            /**
             * @brief Validate the FSON handle.
             * @throws std::runtime_error if validation fails.
             */
            void validate() const {
                int rc = fossil_media_fson_validate(handle_);
                if (rc != FOSSIL_MEDIA_FSON_OK) {
                    throw std::runtime_error("FSON validation failed");
                }
            }

            /**
             * @brief Get native handle (mutable).
             * @return fossil_media_fson_t* handle.
             */
            fossil_media_fson_t* native_handle() { return handle_; }

            /**
             * @brief Get native handle (const).
             * @return const fossil_media_fson_t* handle.
             */
            const fossil_media_fson_t* native_handle() const { return handle_; }

        private:
            fossil_media_fson_t* handle_;
        };

    } // namespace media

} // namespace fossil

#endif

#endif /* FOSSIL_MEDIA_FSON_H */
