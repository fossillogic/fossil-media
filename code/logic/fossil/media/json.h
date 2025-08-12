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
#include <stdexcept>
#include <vector>
#include <string>

namespace fossil {

namespace media {



} // namespace media

} // namespace fossil

#endif

#endif /* FOSSIL_MEDIA_JSON_H */
