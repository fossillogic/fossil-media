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
#ifndef FOSSIL_MEDIA_JSON_H
#define FOSSIL_MEDIA_JSON_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* JSON value types */
typedef enum {
    FOSSIL_MEDIA_JSON_NULL = 0,
    FOSSIL_MEDIA_JSON_BOOL,
    FOSSIL_MEDIA_JSON_NUMBER,
    FOSSIL_MEDIA_JSON_STRING,
    FOSSIL_MEDIA_JSON_ARRAY,
    FOSSIL_MEDIA_JSON_OBJECT
} fossil_media_json_type_t;

/* Error struct */
typedef struct {
    int code;             /* 0 = OK, non-zero = error */
    size_t position;      /* char offset in input (if applicable) */
    char message[128];    /* short error message (truncated) */
} fossil_media_json_error_t;

/* Forward declaration */
typedef struct fossil_media_json_value fossil_media_json_value_t;

/* JSON value */
struct fossil_media_json_value {
    fossil_media_json_type_t type;
    union {
        double number;
        int boolean;            /* 0 or 1 */
        char *string;           /* NUL-terminated, heap allocated */
        struct {
            fossil_media_json_value_t **items;
            size_t count;
            size_t capacity;
        } array;
        struct {
            char **keys;                         /* keys[i] -> values[i] */
            fossil_media_json_value_t **values;
            size_t count;
            size_t capacity;
        } object;
    } u;
};

// *****************************************************************************
// Function prototypes
// *****************************************************************************

/**
 * @brief Parse JSON text into a DOM tree.
 *
 * Parses a NUL-terminated UTF-8 JSON string into an internal DOM structure.
 * The returned value can be queried and manipulated using the API functions.
 *
 * @param json_text  Input JSON text (must be valid UTF-8 and NUL-terminated).
 * @param err_out    Optional pointer to a fossil_media_json_error_t to store error details.
 * @return Pointer to the parsed JSON value on success, or NULL on failure.
 *
 * @note The returned value must be freed with fossil_media_json_free().
 */
fossil_media_json_value_t *fossil_media_json_parse(const char *json_text, fossil_media_json_error_t *err_out);

/**
 * @brief Free a JSON DOM tree.
 *
 * Recursively frees a JSON value and all its children. Safe to call with NULL.
 *
 * @param v  Pointer to the JSON value to free.
 */
void fossil_media_json_free(fossil_media_json_value_t *v);

/** @name Creation Functions
 *  @{
 */

/**
 * @brief Create a JSON null value.
 *
 * @return Newly allocated JSON null value, or NULL if allocation fails.
 */
fossil_media_json_value_t *fossil_media_json_new_null(void);

/**
 * @brief Create a JSON boolean value.
 *
 * @param b  Boolean value (0 for false, nonzero for true).
 * @return Newly allocated JSON boolean value, or NULL if allocation fails.
 */
fossil_media_json_value_t *fossil_media_json_new_bool(int b);

/**
 * @brief Create a JSON number value.
 *
 * @param n  Numeric value (double precision).
 * @return Newly allocated JSON number value, or NULL if allocation fails.
 */
fossil_media_json_value_t *fossil_media_json_new_number(double n);

/**
 * @brief Create a JSON string value.
 *
 * Copies the provided string into the new JSON value.
 *
 * @param s  NUL-terminated string (UTF-8). Cannot be NULL.
 * @return Newly allocated JSON string value, or NULL if allocation fails.
 */
fossil_media_json_value_t *fossil_media_json_new_string(const char *s);

/**
 * @brief Create a JSON array.
 *
 * @return Newly allocated JSON array value, or NULL if allocation fails.
 */
fossil_media_json_value_t *fossil_media_json_new_array(void);

/**
 * @brief Create a JSON object.
 *
 * @return Newly allocated JSON object value, or NULL if allocation fails.
 */
fossil_media_json_value_t *fossil_media_json_new_object(void);

/** @} */

/** @name Object Helpers
 *  @{
 */

/**
 * @brief Set a key/value pair in a JSON object.
 *
 * If the key already exists, its value is replaced (the old value is freed).
 *
 * @param obj  JSON object value (must be of type OBJECT).
 * @param key  Key string (UTF-8, cannot be NULL).
 * @param val  JSON value to insert (ownership is transferred).
 * @return 0 on success, nonzero on error.
 */
int fossil_media_json_object_set(fossil_media_json_value_t *obj, const char *key, fossil_media_json_value_t *val);

/**
 * @brief Get a value from a JSON object by key.
 *
 * @param obj  JSON object value (must be of type OBJECT).
 * @param key  Key string (UTF-8).
 * @return Pointer to the JSON value, or NULL if not found.
 */
fossil_media_json_value_t *fossil_media_json_object_get(const fossil_media_json_value_t *obj, const char *key);

/**
 * @brief Remove a key from a JSON object.
 *
 * @param obj  JSON object value (must be of type OBJECT).
 * @param key  Key string (UTF-8).
 * @return Pointer to the removed JSON value (caller must free), or NULL if not found.
 */
fossil_media_json_value_t *fossil_media_json_object_remove(fossil_media_json_value_t *obj, const char *key);

/** @} */

/** @name Array Helpers
 *  @{
 */

/**
 * @brief Append a value to a JSON array.
 *
 * @param arr  JSON array value (must be of type ARRAY).
 * @param val  JSON value to append (ownership is transferred).
 * @return 0 on success, nonzero on error.
 */
int fossil_media_json_array_append(fossil_media_json_value_t *arr, fossil_media_json_value_t *val);

/**
 * @brief Get an element from a JSON array by index.
 *
 * @param arr    JSON array value (must be of type ARRAY).
 * @param index  Zero-based index.
 * @return Pointer to the JSON value, or NULL if index is out of range.
 */
fossil_media_json_value_t *fossil_media_json_array_get(const fossil_media_json_value_t *arr, size_t index);

/**
 * @brief Get the number of elements in a JSON array.
 *
 * @param arr  JSON array value (must be of type ARRAY).
 * @return Number of elements.
 */
size_t fossil_media_json_array_size(const fossil_media_json_value_t *arr);

/** @} */

/** @name Stringification
 *  @{
 */

/**
 * @brief Convert a JSON value to a string.
 *
 * @param v        JSON value to stringify.
 * @param pretty   Nonzero for human-readable output with indentation.
 * @param err_out  Optional pointer to store error details.
 * @return Newly allocated NUL-terminated string on success, or NULL on failure.
 *
 * @note Caller must free the returned string using free().
 */
char *fossil_media_json_stringify(const fossil_media_json_value_t *v, int pretty, fossil_media_json_error_t *err_out);

/**
 * @brief Parse JSON text and then stringify it back.
 *
 * Useful for testing parser correctness (round-trip validation).
 *
 * @param json_text  Input JSON text (NUL-terminated).
 * @param pretty     Nonzero for pretty-print output.
 * @param err_out    Optional pointer to store error details.
 * @return Newly allocated JSON string, or NULL on error.
 */
char *fossil_media_json_roundtrip(const char *json_text, int pretty, fossil_media_json_error_t *err_out);

/** @} */

/**
 * @brief Get the type name for a JSON value type.
 *
 * @param t  JSON value type.
 * @return NUL-terminated string representing the type (e.g., "null", "string").
 */
const char *fossil_media_json_type_name(fossil_media_json_type_t t);

/** @name Clone & Equality
 *  @{
 */

/**
 * @brief Deep-copy a JSON value.
 *
 * Recursively clones the entire JSON value and its children.
 *
 * @param src  Source JSON value (must not be NULL).
 * @return Newly allocated JSON value on success, or NULL on failure.
 */
fossil_media_json_value_t *
fossil_media_json_clone(const fossil_media_json_value_t *src);

/**
 * @brief Compare two JSON values for equality.
 *
 * Performs a deep structural and value comparison.
 *
 * @param a  First JSON value.
 * @param b  Second JSON value.
 * @return 1 if equal, 0 if not equal, -1 on error.
 */
int fossil_media_json_equals(const fossil_media_json_value_t *a,
                             const fossil_media_json_value_t *b);

/** @} */

/** @name Type Helpers
 *  @{
 */

/**
 * @brief Check if a JSON value is null.
 *
 * @param v  JSON value to check.
 * @return 1 if null, 0 otherwise.
 */
int fossil_media_json_is_null(const fossil_media_json_value_t *v);

/**
 * @brief Check if a JSON value is an array.
 *
 * @param v  JSON value to check.
 * @return 1 if array, 0 otherwise.
 */
int fossil_media_json_is_array(const fossil_media_json_value_t *v);

/**
 * @brief Check if a JSON value is an object.
 *
 * @param v  JSON value to check.
 * @return 1 if object, 0 otherwise.
 */
int fossil_media_json_is_object(const fossil_media_json_value_t *v);

/** @} */

/** @name Memory & Capacity
 *  @{
 */

/**
 * @brief Reserve capacity for a JSON array.
 *
 * Ensures that the array can hold at least `capacity` items without resizing.
 *
 * @param arr      JSON array value (must be of type ARRAY).
 * @param capacity Desired capacity.
 * @return 0 on success, nonzero on error.
 */
int fossil_media_json_array_reserve(fossil_media_json_value_t *arr, size_t capacity);

/**
 * @brief Reserve capacity for a JSON object.
 *
 * Ensures that the object can hold at least `capacity` key/value pairs.
 *
 * @param obj      JSON object value (must be of type OBJECT).
 * @param capacity Desired capacity.
 * @return 0 on success, nonzero on error.
 */
int fossil_media_json_object_reserve(fossil_media_json_value_t *obj, size_t capacity);

/** @} */

/** @name File I/O
 *  @{
 */

/**
 * @brief Parse a JSON file into a DOM tree.
 *
 * Reads the entire file and parses it into an internal DOM structure.
 *
 * @param filename Path to JSON file.
 * @param err_out  Optional pointer to error details.
 * @return Pointer to the parsed JSON value, or NULL on failure.
 */
fossil_media_json_value_t *
fossil_media_json_parse_file(const char *filename, fossil_media_json_error_t *err_out);

/**
 * @brief Write a JSON value to a file.
 *
 * Serializes the JSON value and writes it to the given file.
 *
 * @param v        JSON value to write.
 * @param filename Path to output file.
 * @param pretty   Nonzero for human-readable indentation.
 * @param err_out  Optional pointer to error details.
 * @return 0 on success, nonzero on error.
 */
int fossil_media_json_write_file(const fossil_media_json_value_t *v,
                                 const char *filename,
                                 int pretty,
                                 fossil_media_json_error_t *err_out);

/** @} */

/** @name Number Handling
 *  @{
 */

/**
 * @brief Create a JSON integer value.
 *
 * Stores an integer in a JSON number node (lossless if within range).
 *
 * @param i  Integer value.
 * @return Newly allocated JSON number value, or NULL if allocation fails.
 */
fossil_media_json_value_t *fossil_media_json_new_int(long long i);

/**
 * @brief Get an integer from a JSON number.
 *
 * @param v    JSON number value.
 * @param out  Output pointer to receive integer value.
 * @return 0 on success, nonzero if not a number or out of range.
 */
int fossil_media_json_get_int(const fossil_media_json_value_t *v, long long *out);

/** @} */

/** @name Debug & Validation
 *  @{
 */

/**
 * @brief Print a debug dump of a JSON value.
 *
 * Dumps the JSON tree in a human-readable indented format for debugging.
 *
 * @param v      JSON value to dump.
 * @param indent Starting indentation level.
 */
void fossil_media_json_debug_dump(const fossil_media_json_value_t *v, int indent);

/**
 * @brief Validate JSON text without building a DOM.
 *
 * Parses the text and discards the result, returning only validity status.
 *
 * @param json_text  Input JSON text (NUL-terminated).
 * @param err_out    Optional pointer to error details.
 * @return 0 if valid, nonzero if invalid.
 */
int fossil_media_json_validate(const char *json_text, fossil_media_json_error_t *err_out);

/** @} */

/** @name Path Access
 *  @{
 */

/**
 * @brief Get a JSON value using a dotted path expression.
 *
 * Supports object keys and array indices, e.g. "user.name" or "items[2].id".
 *
 * @param root  Root JSON value.
 * @param path  Path string (UTF-8, cannot be NULL).
 * @return Pointer to the JSON value, or NULL if not found.
 */
fossil_media_json_value_t *
fossil_media_json_get_path(const fossil_media_json_value_t *root, const char *path);

/** @} */


#ifdef __cplusplus
}
#include <string>
#include <stdexcept>
#include <utility>

namespace fossil {

    namespace media {

        /**
         * @brief Exception type thrown when JSON operations fail in FossilMediaJson.
         */
        class JsonError : public std::runtime_error {
        public:
            /**
             * @brief Construct a new JsonError with the given message.
             * @param msg Error message text.
             */
            explicit JsonError(const std::string& msg)
                : std::runtime_error(msg) {}
        };
        
        /**
         * @brief C++ RAII wrapper around fossil_media_json_value_t from the C API.
         * 
         * This class manages the lifetime of the underlying JSON value automatically
         * and provides type-safe, snake_case access methods for manipulating JSON data.
         * 
         * It is designed for convenience in C++ applications while still relying on
         * the lightweight, portable C backend provided by Fossil Media.
         */
        class Json {
        public:
            /**
             * @brief Construct an empty null JSON value.
             */
            Json() : value_(fossil_media_json_new_null()) {}
        
            /**
             * @brief Construct from a C API value pointer (takes ownership).
             * @param val Pointer to a fossil_media_json_value_t.
             * @throws std::invalid_argument if val is nullptr.
             */
            explicit Json(fossil_media_json_value_t* val) : value_(val) {
                if (!val) {
                    throw std::invalid_argument("Null JSON value pointer passed to Json");
                }
            }
        
            /**
             * @brief Destructor: frees the underlying C API JSON value.
             */
            ~Json() {
                fossil_media_json_free(value_);
            }
        
            // Non-copyable
            Json(const Json&) = delete;
            Json& operator=(const Json&) = delete;
        
            // Movable
            Json(Json&& other) noexcept : value_(other.value_) {
                other.value_ = nullptr;
            }
        
            Json& operator=(Json&& other) noexcept {
                if (this != &other) {
                    fossil_media_json_free(value_);
                    value_ = other.value_;
                    other.value_ = nullptr;
                }
                return *this;
            }
        
            /**
             * @brief Parse JSON text into a Json object.
             * @param text NUL-terminated JSON string.
             * @return Parsed Json object.
             * @throws JsonError if parsing fails.
             */
            static Json parse(const std::string& text) {
                fossil_media_json_error_t err{};
                fossil_media_json_value_t* val = fossil_media_json_parse(text.c_str(), &err);
                if (!val) {
                    throw JsonError(std::string("Parse error: ") + err.message);
                }
                return Json(val);
            }
        
            /**
             * @brief Create a JSON boolean value.
             * @param b Boolean value.
             * @return Json object holding a boolean.
             */
            static Json new_bool(bool b) {
                return Json(fossil_media_json_new_bool(b ? 1 : 0));
            }
        
            /**
             * @brief Create a JSON number value.
             * @param n Number value (double).
             * @return Json object holding a number.
             */
            static Json new_number(double n) {
                return Json(fossil_media_json_new_number(n));
            }
        
            /**
             * @brief Create a JSON string value.
             * @param s String value (copied internally).
             * @return Json object holding a string.
             */
            static Json new_string(const std::string& s) {
                return Json(fossil_media_json_new_string(s.c_str()));
            }
        
            /**
             * @brief Create a JSON array.
             * @return Json object holding an empty array.
             */
            static Json new_array() {
                return Json(fossil_media_json_new_array());
            }
        
            /**
             * @brief Create a JSON object.
             * @return Json object holding an empty object.
             */
            static Json new_object() {
                return Json(fossil_media_json_new_object());
            }
        
            /**
             * @brief Append a value to a JSON array.
             * @param val Json value to append (moved).
             * @throws JsonError if not an array or append fails.
             */
            void array_append(Json&& val) {
                if (fossil_media_json_array_append(value_, val.value_) != 0) {
                    throw JsonError("Failed to append to array");
                }
                val.value_ = nullptr; // Ownership transferred
            }
        
            /**
             * @brief Get element at index in JSON array.
             * @param index Zero-based index.
             * @return Json element at index (shared, no ownership transfer).
             */
            Json array_get(size_t index) const {
                fossil_media_json_value_t* v = fossil_media_json_array_get(value_, index);
                if (!v) {
                    throw JsonError("Array index out of range");
                }
                return Json(fossil_media_json_new_string(fossil_media_json_stringify(v, 0, nullptr))); // Copy via stringify
            }
        
            /**
             * @brief Set key-value in JSON object.
             * @param key String key.
             * @param val Json value to set (moved).
             * @throws JsonError if not an object or set fails.
             */
            void object_set(const std::string& key, Json&& val) {
                if (fossil_media_json_object_set(value_, key.c_str(), val.value_) != 0) {
                    throw JsonError("Failed to set key in object");
                }
                val.value_ = nullptr; // Ownership transferred
            }
        
            /**
             * @brief Serialize JSON to string.
             * @param pretty If true, output with indentation.
             * @return Serialized JSON string.
             * @throws JsonError if stringify fails.
             */
            std::string stringify(bool pretty = false) const {
                fossil_media_json_error_t err{};
                char* s = fossil_media_json_stringify(value_, pretty ? 1 : 0, &err);
                if (!s) {
                    throw JsonError(std::string("Stringify error: ") + err.message);
                }
                std::string result(s);
                free(s);
                return result;
            }

            /**
             * @brief Deep copy this JSON value.
             * @return A new Json object that is a clone of this value.
             * @throws JsonError if cloning fails.
             */
            Json clone() const {
                fossil_media_json_value_t* v = fossil_media_json_clone(value_);
                if (!v) {
                    throw JsonError("Failed to clone JSON value");
                }
                return Json(v);
            }

            /**
             * @brief Compare this JSON value to another for equality.
             * @param other The other Json object to compare.
             * @return true if equal, false otherwise.
             * @throws JsonError if comparison fails.
             */
            bool equals(const Json& other) const {
                int result = fossil_media_json_equals(value_, other.value_);
                if (result == -1) {
                    throw JsonError("Failed to compare JSON values");
                }
                return result == 1;
            }

            /**
             * @brief Check if this value is null.
             * @return true if null, false otherwise.
             */
            bool is_null() const {
                return fossil_media_json_is_null(value_) == 1;
            }

            /**
             * @brief Check if this value is an array.
             * @return true if array, false otherwise.
             */
            bool is_array() const {
                return fossil_media_json_is_array(value_) == 1;
            }

            /**
             * @brief Check if this value is an object.
             * @return true if object, false otherwise.
             */
            bool is_object() const {
                return fossil_media_json_is_object(value_) == 1;
            }

            /**
             * @brief Reserve capacity for a JSON array.
             * @param capacity Desired capacity.
             * @throws JsonError if not an array or reserve fails.
             */
            void array_reserve(size_t capacity) {
                if (fossil_media_json_array_reserve(value_, capacity) != 0) {
                    throw JsonError("Failed to reserve array capacity");
                }
            }

            /**
             * @brief Reserve capacity for a JSON object.
             * @param capacity Desired capacity.
             * @throws JsonError if not an object or reserve fails.
             */
            void object_reserve(size_t capacity) {
                if (fossil_media_json_object_reserve(value_, capacity) != 0) {
                    throw JsonError("Failed to reserve object capacity");
                }
            }

            /**
             * @brief Parse a JSON file into a Json object.
             * @param filename Path to JSON file.
             * @return Parsed Json object.
             * @throws JsonError if parsing fails.
             */
            static Json parse_file(const std::string& filename) {
                fossil_media_json_error_t err{};
                fossil_media_json_value_t* val = fossil_media_json_parse_file(filename.c_str(), &err);
                if (!val) {
                    throw JsonError(std::string("Parse file error: ") + err.message);
                }
                return Json(val);
            }

            /**
             * @brief Write this JSON value to a file.
             * @param filename Path to output file.
             * @param pretty If true, output with indentation.
             * @throws JsonError if writing fails.
             */
            void write_file(const std::string& filename, bool pretty = false) const {
                fossil_media_json_error_t err{};
                int rc = fossil_media_json_write_file(value_, filename.c_str(), pretty ? 1 : 0, &err);
                if (rc != 0) {
                    throw JsonError(std::string("Write file error: ") + err.message);
                }
            }

            /**
             * @brief Create a JSON integer value.
             * @param i Integer value.
             * @return Json object holding an integer.
             */
            static Json new_int(long long i) {
                return Json(fossil_media_json_new_int(i));
            }

            /**
             * @brief Get integer value from this JSON number.
             * @return Integer value.
             * @throws JsonError if not a number or out of range.
             */
            long long get_int() const {
                long long out = 0;
                if (fossil_media_json_get_int(value_, &out) != 0) {
                    throw JsonError("Failed to get integer from JSON value");
                }
                return out;
            }

            /**
             * @brief Print a debug dump of this JSON value.
             * @param indent Starting indentation level.
             */
            void debug_dump(int indent = 0) const {
                fossil_media_json_debug_dump(value_, indent);
            }

            /**
             * @brief Validate JSON text without building a DOM.
             * @param text Input JSON text.
             * @return true if valid, false otherwise.
             */
            static bool validate(const std::string& text) {
                fossil_media_json_error_t err{};
                return fossil_media_json_validate(text.c_str(), &err) == 0;
            }

            /**
             * @brief Get a JSON value using a dotted path expression.
             * @param path Path string (e.g., "user.name" or "items[2].id").
             * @return Json object at the path.
             * @throws JsonError if not found.
             */
            Json get_path(const std::string& path) const {
                fossil_media_json_value_t* v = fossil_media_json_get_path(value_, path.c_str());
                if (!v) {
                    // Return a null Json object if path not found
                    return Json(fossil_media_json_new_null());
                }
                return Json(v);
            }
        
        private:
            fossil_media_json_value_t* value_;
        };

    } // namespace media

} // namespace fossil

#endif

#endif /* FOSSIL_MEDIA_JSON_H */
