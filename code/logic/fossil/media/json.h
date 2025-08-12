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
fossil_media_json_value_t *
fossil_media_json_parse(const char *json_text, fossil_media_json_error_t *err_out);

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
        
        private:
            fossil_media_json_value_t* value_;
        };

    } // namespace media

} // namespace fossil

#endif

#endif /* FOSSIL_MEDIA_JSON_H */
