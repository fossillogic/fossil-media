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

/* -------------------------------------------------------------
 * FSON v2: Error Codes
 * ------------------------------------------------------------- */
enum {
    FOSSIL_MEDIA_FSON_OK = 0,
    FOSSIL_MEDIA_FSON_ERR_IO = -1,           /* File/stream error */
    FOSSIL_MEDIA_FSON_ERR_NOMEM = -2,        /* Out of memory */
    FOSSIL_MEDIA_FSON_ERR_PARSE = -3,        /* Syntax or structure error */
    FOSSIL_MEDIA_FSON_ERR_TYPE = -4,         /* Type mismatch */
    FOSSIL_MEDIA_FSON_ERR_RANGE = -5,        /* Value out of range */
    FOSSIL_MEDIA_FSON_ERR_NOT_FOUND = -6,    /* Key or index not found */
    FOSSIL_MEDIA_FSON_ERR_INVALID_ARG = -7,  /* Bad API argument */
    FOSSIL_MEDIA_FSON_ERR_SCHEMA = -8,       /* Schema validation failed */
    FOSSIL_MEDIA_FSON_ERR_INCLUDE = -9       /* Include could not be resolved */
};

/* -------------------------------------------------------------
 * FSON v2: Value Types
 * ------------------------------------------------------------- */
typedef enum {
    FSON_TYPE_NULL = 0,
    FSON_TYPE_BOOL,

    /* Explicit scalar types */
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

    /* Literal number bases */
    FSON_TYPE_OCT,
    FSON_TYPE_HEX,
    FSON_TYPE_BIN,

    /* Strings and chars */
    FSON_TYPE_CHAR,
    FSON_TYPE_CSTR,

    /* Composite containers */
    FSON_TYPE_ARRAY,
    FSON_TYPE_OBJECT,

    /* New in FSON v2 */
    FSON_TYPE_ENUM,       /* Symbol from a fixed set */
    FSON_TYPE_FLAGS,      /* Bitmask of symbolic flags */
    FSON_TYPE_DATETIME,   /* ISO 8601 datetime */
    FSON_TYPE_DURATION,   /* Time span (e.g. "30s", "5m", "1h") */

    /* Virtual/meta nodes */
    FSON_TYPE_INCLUDE,    /* $include directive */
    FSON_TYPE_SCHEMA      /* $schema declaration */
} fossil_media_fson_type_t;

/* -------------------------------------------------------------
 * FSON v2: Error Struct
 * ------------------------------------------------------------- */
typedef struct {
    int code;             /* 0 = OK, negative = error */
    size_t position;      /* char offset in input (if available) */
    char message[256];    /* detailed error message (UTF-8) */
} fossil_media_fson_error_t;

/* -------------------------------------------------------------
 * FSON v2: Forward Declarations
 * ------------------------------------------------------------- */
typedef struct fossil_media_fson_value fossil_media_fson_value_t;

/* -------------------------------------------------------------
 * FSON v2: Value Representation
 * ------------------------------------------------------------- */
struct fossil_media_fson_value {
    fossil_media_fson_type_t type;
    union {
        /* Scalars */
        int boolean;
        int8_t i8;
        int16_t i16;
        int32_t i32;
        int64_t i64;
        uint8_t u8;
        uint16_t u16;
        uint32_t u32;
        uint64_t u64;
        float f32;
        double f64;

        /* Encoded numbers */
        uint64_t oct;
        uint64_t hex;
        uint64_t bin;

        /* Characters and strings */
        char character;
        char *cstr;       /* NUL-terminated, heap-allocated */

        /* Enums and Flags */
        struct {
            char *symbol;     /* e.g. "warn" */
            const char **allowed; /* optional schema-backed allowed values */
            size_t allowed_count;
        } enum_val;

        struct {
            uint64_t bitmask;   /* resolved numeric value */
            char **symbols;     /* symbolic flag names */
            size_t count;
        } flags_val;

        /* Date/time and duration */
        struct {
            int64_t epoch_ns;  /* nanoseconds since Unix epoch */
        } datetime;

        struct {
            int64_t ns;        /* duration in nanoseconds */
        } duration;

        /* Arrays */
        struct {
            fossil_media_fson_value_t **items;
            size_t count;
            size_t capacity;
        } array;

        /* Objects */
        struct {
            char **keys;
            fossil_media_fson_value_t **values;
            size_t count;
            size_t capacity;
        } object;

        /* Meta-directives */
        char *include_path;    /* $include: cstr */
        struct {
            /* schema data is itself an object of constraints */
            struct fossil_media_fson_value *schema_root;
        } schema;
    } u;
};

// *****************************************************************************
// Function prototypes
// *****************************************************************************

/**
 * @brief Parse FSON text into a DOM tree.
 *
 * Parses a NUL-terminated UTF-8 FSON string into an internal DOM structure.
 * The returned value can be queried and manipulated using the API functions.
 *
 * @param json_text  Input FSON text (must be valid UTF-8 and NUL-terminated).
 * @param err_out    Optional pointer to a fossil_media_fson_error_t to store error details.
 * @return Pointer to the parsed FSON value on success, or NULL on failure.
 *
 * @note The returned value must be freed with fossil_media_fson_free().
 */
fossil_media_fson_value_t *fossil_media_fson_parse(const char *json_text, fossil_media_fson_error_t *err_out);

/**
 * @brief Free a FSON DOM tree.
 *
 * Recursively frees a FSON value and all its children. Safe to call with NULL.
 *
 * @param v  Pointer to the FSON value to free.
 */
void fossil_media_fson_free(fossil_media_fson_value_t *v);

/** @name Creation Functions
 *  @{
 */

/**
 * @brief Create a FSON null value.
 *
 * @return Newly allocated FSON null value, or NULL if allocation fails.
 */
fossil_media_fson_value_t *fossil_media_fson_new_null(void);

/**
 * @brief Create a FSON boolean value.
 *
 * @param b  Boolean value (0 for false, nonzero for true).
 * @return Newly allocated FSON boolean value, or NULL if allocation fails.
 */
fossil_media_fson_value_t *fossil_media_fson_new_bool(int b);

/**
 * @brief Create a FSON int8 value.
 * @param value int8_t value.
 * @return Newly allocated FSON int8 value, or NULL if allocation fails.
 */
fossil_media_fson_value_t *fossil_media_fson_new_i8(int8_t value);

/**
 * @brief Create a FSON int16 value.
 * @param value int16_t value.
 * @return Newly allocated FSON int16 value, or NULL if allocation fails.
 */
fossil_media_fson_value_t *fossil_media_fson_new_i16(int16_t value);

/**
 * @brief Create a FSON int32 value.
 * @param value int32_t value.
 * @return Newly allocated FSON int32 value, or NULL if allocation fails.
 */
fossil_media_fson_value_t *fossil_media_fson_new_i32(int32_t value);

/**
 * @brief Create a FSON int64 value.
 * @param value int64_t value.
 * @return Newly allocated FSON int64 value, or NULL if allocation fails.
 */
fossil_media_fson_value_t *fossil_media_fson_new_i64(int64_t value);

/**
 * @brief Create a FSON uint8 value.
 * @param value uint8_t value.
 * @return Newly allocated FSON uint8 value, or NULL if allocation fails.
 */
fossil_media_fson_value_t *fossil_media_fson_new_u8(uint8_t value);

/**
 * @brief Create a FSON uint16 value.
 * @param value uint16_t value.
 * @return Newly allocated FSON uint16 value, or NULL if allocation fails.
 */
fossil_media_fson_value_t *fossil_media_fson_new_u16(uint16_t value);

/**
 * @brief Create a FSON uint32 value.
 * @param value uint32_t value.
 * @return Newly allocated FSON uint32 value, or NULL if allocation fails.
 */
fossil_media_fson_value_t *fossil_media_fson_new_u32(uint32_t value);

/**
 * @brief Create a FSON uint64 value.
 * @param value uint64_t value.
 * @return Newly allocated FSON uint64 value, or NULL if allocation fails.
 */
fossil_media_fson_value_t *fossil_media_fson_new_u64(uint64_t value);

/**
 * @brief Create a FSON float32 value.
 * @param value float value.
 * @return Newly allocated FSON float32 value, or NULL if allocation fails.
 */
fossil_media_fson_value_t *fossil_media_fson_new_f32(float value);

/**
 * @brief Create a FSON float64 value.
 * @param value double value.
 * @return Newly allocated FSON float64 value, or NULL if allocation fails.
 */
fossil_media_fson_value_t *fossil_media_fson_new_f64(double value);

/**
 * @brief Create a FSON octal value.
 * @param value uint64_t value.
 * @return Newly allocated FSON octal value, or NULL if allocation fails.
 */
fossil_media_fson_value_t *fossil_media_fson_new_oct(uint64_t value);

/**
 * @brief Create a FSON hexadecimal value.
 * @param value uint64_t value.
 * @return Newly allocated FSON hexadecimal value, or NULL if allocation fails.
 */
fossil_media_fson_value_t *fossil_media_fson_new_hex(uint64_t value);

/**
 * @brief Create a FSON binary value.
 * @param value uint64_t value.
 * @return Newly allocated FSON binary value, or NULL if allocation fails.
 */
fossil_media_fson_value_t *fossil_media_fson_new_bin(uint64_t value);

/**
 * @brief Create a FSON char value.
 * @param value char value.
 * @return Newly allocated FSON char value, or NULL if allocation fails.
 */
fossil_media_fson_value_t *fossil_media_fson_new_char(char value);

/**
 * @brief Create a FSON string value.
 *
 * Copies the provided string into the new FSON value.
 *
 * @param s  NUL-terminated string (UTF-8). Cannot be NULL.
 * @return Newly allocated FSON string value, or NULL if allocation fails.
 */
fossil_media_fson_value_t *fossil_media_fson_new_string(const char *s);

/**
 * @brief Create a FSON array.
 *
 * @return Newly allocated FSON array value, or NULL if allocation fails.
 */
fossil_media_fson_value_t *fossil_media_fson_new_array(void);

/**
 * @brief Create a FSON object.
 *
 * @return Newly allocated FSON object value, or NULL if allocation fails.
 */
fossil_media_fson_value_t *fossil_media_fson_new_object(void);

/**
 * @brief Create a FSON enum value.
 *
 * @param symbol       Enum symbol string (UTF-8, cannot be NULL).
 * @param allowed      Optional array of allowed symbols (can be NULL).
 * @param allowed_count Number of allowed symbols (0 if none).
 * @return Newly allocated FSON enum value, or NULL if allocation fails.
 */
fossil_media_fson_value_t *fossil_media_fson_new_enum(const char *symbol, const char **allowed, size_t allowed_count);

/**
 * @brief Create a FSON flags value.
 *
 * @param bitmask  Numeric bitmask value.
 * @param symbols  Array of symbolic flag names (UTF-8, cannot be NULL).
 * @param count    Number of symbols in the array.
 * @return Newly allocated FSON flags value, or NULL if allocation fails.
 */
fossil_media_fson_value_t *fossil_media_fson_new_flags(uint64_t bitmask, const char **symbols, size_t count);

/** @} */

/** @name Object Helpers
 *  @{
 */

/**
 * @brief Set a key/value pair in a FSON object.
 *
 * If the key already exists, its value is replaced (the old value is freed).
 *
 * @param obj  FSON object value (must be of type OBJECT).
 * @param key  Key string (UTF-8, cannot be NULL).
 * @param val  FSON value to insert (ownership is transferred).
 * @return 0 on success, nonzero on error.
 */
int fossil_media_fson_object_set(fossil_media_fson_value_t *obj, const char *key, fossil_media_fson_value_t *val);

/**
 * @brief Get a value from a FSON object by key.
 *
 * @param obj  FSON object value (must be of type OBJECT).
 * @param key  Key string (UTF-8).
 * @return Pointer to the FSON value, or NULL if not found.
 */
fossil_media_fson_value_t *fossil_media_fson_object_get(const fossil_media_fson_value_t *obj, const char *key);

/**
 * @brief Remove a key from a FSON object.
 *
 * @param obj  FSON object value (must be of type OBJECT).
 * @param key  Key string (UTF-8).
 * @return Pointer to the removed FSON value (caller must free), or NULL if not found.
 */
fossil_media_fson_value_t *fossil_media_fson_object_remove(fossil_media_fson_value_t *obj, const char *key);

/** @} */

/** @name Array Helpers
 *  @{
 */

/**
 * @brief Append a value to a FSON array.
 *
 * @param arr  FSON array value (must be of type ARRAY).
 * @param val  FSON value to append (ownership is transferred).
 * @return 0 on success, nonzero on error.
 */
int fossil_media_fson_array_append(fossil_media_fson_value_t *arr, fossil_media_fson_value_t *val);

/**
 * @brief Get an element from a FSON array by index.
 *
 * @param arr    FSON array value (must be of type ARRAY).
 * @param index  Zero-based index.
 * @return Pointer to the FSON value, or NULL if index is out of range.
 */
fossil_media_fson_value_t *fossil_media_fson_array_get(const fossil_media_fson_value_t *arr, size_t index);

/**
 * @brief Get the number of elements in a FSON array.
 *
 * @param arr  FSON array value (must be of type ARRAY).
 * @return Number of elements.
 */
size_t fossil_media_fson_array_size(const fossil_media_fson_value_t *arr);

/** @} */

/** @name Stringification
 *  @{
 */

/**
 * @brief Convert a FSON value to a string.
 *
 * @param v        FSON value to stringify.
 * @param pretty   Nonzero for human-readable output with indentation.
 * @param err_out  Optional pointer to store error details.
 * @return Newly allocated NUL-terminated string on success, or NULL on failure.
 *
 * @note Caller must free the returned string using free().
 */
char *fossil_media_fson_stringify(const fossil_media_fson_value_t *v, int pretty, fossil_media_fson_error_t *err_out);

/**
 * @brief Parse FSON text and then stringify it back.
 *
 * Useful for testing parser correctness (round-trip validation).
 *
 * @param json_text  Input FSON text (NUL-terminated).
 * @param pretty     Nonzero for pretty-print output.
 * @param err_out    Optional pointer to store error details.
 * @return Newly allocated FSON string, or NULL on error.
 */
char *fossil_media_fson_roundtrip(const char *json_text, int pretty, fossil_media_fson_error_t *err_out);

/** @} */

/**
 * @brief Get the type name for a FSON value type.
 *
 * @param t  FSON value type.
 * @return NUL-terminated string representing the type (e.g., "null", "string").
 */
const char *fossil_media_fson_type_name(fossil_media_fson_type_t t);

/** @name Clone & Equality
 *  @{
 */

/**
 * @brief Deep-copy a FSON value.
 *
 * Recursively clones the entire FSON value and its children.
 *
 * @param src  Source FSON value (must not be NULL).
 * @return Newly allocated FSON value on success, or NULL on failure.
 */
fossil_media_fson_value_t *
fossil_media_fson_clone(const fossil_media_fson_value_t *src);

/**
 * @brief Compare two FSON values for equality.
 *
 * Performs a deep structural and value comparison.
 *
 * @param a  First FSON value.
 * @param b  Second FSON value.
 * @return 1 if equal, 0 if not equal, -1 on error.
 */
int fossil_media_fson_equals(const fossil_media_fson_value_t *a,
                             const fossil_media_fson_value_t *b);

/** @} */

/** @name Type Helpers
 *  @{
 */

/**
 * @brief Check if a FSON value is null.
 *
 * @param v  FSON value to check.
 * @return 1 if null, 0 otherwise.
 */
int fossil_media_fson_is_null(const fossil_media_fson_value_t *v);

/**
 * @brief Check if a FSON value is an array.
 *
 * @param v  FSON value to check.
 * @return 1 if array, 0 otherwise.
 */
int fossil_media_fson_is_array(const fossil_media_fson_value_t *v);

/**
 * @brief Check if a FSON value is an object.
 *
 * @param v  FSON value to check.
 * @return 1 if object, 0 otherwise.
 */
int fossil_media_fson_is_object(const fossil_media_fson_value_t *v);

/** @} */

/** @name Memory & Capacity
 *  @{
 */

/**
 * @brief Reserve capacity for a FSON array.
 *
 * Ensures that the array can hold at least `capacity` items without resizing.
 *
 * @param arr      FSON array value (must be of type ARRAY).
 * @param capacity Desired capacity.
 * @return 0 on success, nonzero on error.
 */
int fossil_media_fson_array_reserve(fossil_media_fson_value_t *arr, size_t capacity);

/**
 * @brief Reserve capacity for a FSON object.
 *
 * Ensures that the object can hold at least `capacity` key/value pairs.
 *
 * @param obj      FSON object value (must be of type OBJECT).
 * @param capacity Desired capacity.
 * @return 0 on success, nonzero on error.
 */
int fossil_media_fson_object_reserve(fossil_media_fson_value_t *obj, size_t capacity);

/** @} */

/** @name File I/O
 *  @{
 */

/**
 * @brief Parse a FSON file into a DOM tree.
 *
 * Reads the entire file and parses it into an internal DOM structure.
 *
 * @param filename Path to FSON file.
 * @param err_out  Optional pointer to error details.
 * @return Pointer to the parsed FSON value, or NULL on failure.
 */
fossil_media_fson_value_t *
fossil_media_fson_parse_file(const char *filename, fossil_media_fson_error_t *err_out);

/**
 * @brief Write a FSON value to a file.
 *
 * Serializes the FSON value and writes it to the given file.
 *
 * @param v        FSON value to write.
 * @param filename Path to output file.
 * @param pretty   Nonzero for human-readable indentation.
 * @param err_out  Optional pointer to error details.
 * @return 0 on success, nonzero on error.
 */
int fossil_media_fson_write_file(const fossil_media_fson_value_t *v,
                                 const char *filename,
                                 int pretty,
                                 fossil_media_fson_error_t *err_out);

/** @} */

/** @name Number Handling
 *  @{
 */

/**
 * @brief Get int8 value from a FSON value.
 * @param v FSON value.
 * @param out Pointer to output int8_t.
 * @return 0 on success, nonzero on error.
 */
int fossil_media_fson_get_i8(const fossil_media_fson_value_t *v, int8_t *out);

/**
 * @brief Get int16 value from a FSON value.
 * @param v FSON value.
 * @param out Pointer to output int16_t.
 * @return 0 on success, nonzero on error.
 */
int fossil_media_fson_get_i16(const fossil_media_fson_value_t *v, int16_t *out);

/**
 * @brief Get int32 value from a FSON value.
 * @param v FSON value.
 * @param out Pointer to output int32_t.
 * @return 0 on success, nonzero on error.
 */
int fossil_media_fson_get_i32(const fossil_media_fson_value_t *v, int32_t *out);

/**
 * @brief Get int64 value from a FSON value.
 * @param v FSON value.
 * @param out Pointer to output int64_t.
 * @return 0 on success, nonzero on error.
 */
int fossil_media_fson_get_i64(const fossil_media_fson_value_t *v, int64_t *out);

/**
 * @brief Get uint8 value from a FSON value.
 * @param v FSON value.
 * @param out Pointer to output uint8_t.
 * @return 0 on success, nonzero on error.
 */
int fossil_media_fson_get_u8(const fossil_media_fson_value_t *v, uint8_t *out);

/**
 * @brief Get uint16 value from a FSON value.
 * @param v FSON value.
 * @param out Pointer to output uint16_t.
 * @return 0 on success, nonzero on error.
 */
int fossil_media_fson_get_u16(const fossil_media_fson_value_t *v, uint16_t *out);

/**
 * @brief Get uint32 value from a FSON value.
 * @param v FSON value.
 * @param out Pointer to output uint32_t.
 * @return 0 on success, nonzero on error.
 */
int fossil_media_fson_get_u32(const fossil_media_fson_value_t *v, uint32_t *out);

/**
 * @brief Get uint64 value from a FSON value.
 * @param v FSON value.
 * @param out Pointer to output uint64_t.
 * @return 0 on success, nonzero on error.
 */
int fossil_media_fson_get_u64(const fossil_media_fson_value_t *v, uint64_t *out);

/**
 * @brief Get float32 value from a FSON value.
 * @param v FSON value.
 * @param out Pointer to output float.
 * @return 0 on success, nonzero on error.
 */
int fossil_media_fson_get_f32(const fossil_media_fson_value_t *v, float *out);

/**
 * @brief Get float64 value from a FSON value.
 * @param v FSON value.
 * @param out Pointer to output double.
 * @return 0 on success, nonzero on error.
 */
int fossil_media_fson_get_f64(const fossil_media_fson_value_t *v, double *out);

/**
 * @brief Get octal value from a FSON value.
 * @param v FSON value.
 * @param out Pointer to output uint64_t.
 * @return 0 on success, nonzero on error.
 */
int fossil_media_fson_get_oct(const fossil_media_fson_value_t *v, uint64_t *out);

/**
 * @brief Get hexadecimal value from a FSON value.
 * @param v FSON value.
 * @param out Pointer to output uint64_t.
 * @return 0 on success, nonzero on error.
 */
int fossil_media_fson_get_hex(const fossil_media_fson_value_t *v, uint64_t *out);

/**
 * @brief Get binary value from a FSON value.
 * @param v FSON value.
 * @param out Pointer to output uint64_t.
 * @return 0 on success, nonzero on error.
 */
int fossil_media_fson_get_bin(const fossil_media_fson_value_t *v, uint64_t *out);

/**
 * @brief Get char value from a FSON value.
 * @param v FSON value.
 * @param out Pointer to output char.
 * @return 0 on success, nonzero on error.
 */
int fossil_media_fson_get_char(const fossil_media_fson_value_t *v, char *out);

/**
 * @brief Get boolean value from a FSON value.
 * @param v FSON value.
 * @param out Pointer to output int (0 or 1).
 * @return 0 on success, nonzero on error.
 */
int fossil_media_fson_get_bool(const fossil_media_fson_value_t *v, int *out);

/**
 * @brief Get C string value from a FSON value.
 * @param v FSON value.
 * @param out Pointer to output char* (NUL-terminated string).
 * @return 0 on success, nonzero on error.
 */
int fossil_media_fson_get_cstr(const fossil_media_fson_value_t *v, char **out);

/**
 * @brief Get enum symbol from a FSON enum value.
 * @param v FSON enum value.
 * @param out Pointer to output char* (NUL-terminated string).
 * @return 0 on success, nonzero on error.
 */
int fossil_media_fson_get_enum(const fossil_media_fson_value_t *v, const char **out);

/**
 * @brief Get flags bitmask from a FSON flags value.
 * @param v FSON flags value.
 * @param out Pointer to output uint64_t bitmask.
 * @return 0 on success, nonzero on error.
 */
int fossil_media_fson_get_flags(const fossil_media_fson_value_t *v, uint64_t *out);

/** @} */

/** @name Debug & Validation
 *  @{
 */

/**
 * @brief Print a debug dump of a FSON value.
 *
 * Dumps the FSON tree in a human-readable indented format for debugging.
 *
 * @param v      FSON value to dump.
 * @param indent Starting indentation level.
 */
void fossil_media_fson_debug_dump(const fossil_media_fson_value_t *v, int indent);

/**
 * @brief Validate FSON text without building a DOM.
 *
 * Parses the text and discards the result, returning only validity status.
 *
 * @param json_text  Input FSON text (NUL-terminated).
 * @param err_out    Optional pointer to error details.
 * @return 0 if valid, nonzero if invalid.
 */
int fossil_media_fson_validate(const char *json_text, fossil_media_fson_error_t *err_out);

/** @} */

/** @name Path Access
 *  @{
 */

/**
 * @brief Get a FSON value using a dotted path expression.
 *
 * Supports object keys and array indices, e.g. "user.name" or "items[2].id".
 *
 * @param root  Root FSON value.
 * @param path  Path string (UTF-8, cannot be NULL).
 * @return Pointer to the FSON value, or NULL if not found.
 */
fossil_media_fson_value_t * fossil_media_fson_get_path(const fossil_media_fson_value_t *root, const char *path);

#ifdef __cplusplus
}

#include <string>
#include <stdexcept>
#include <utility>

namespace fossil {

    namespace media {

        /**
         * @brief Exception type thrown when FSON operations fail in FossilMediaFson.
         */
        class FsonError : public std::runtime_error {
        public:
            /**
             * @brief Construct a new FsonError with the given message.
             * @param msg Error message text.
             */
            explicit FsonError(const std::string& msg)
                : std::runtime_error(msg) {}
        };

        /**
         * @brief C++ RAII wrapper around fossil_media_fson_value_t from the C API.
         *
         * This class manages the lifetime of the underlying FSON value automatically
         * and provides type-safe, snake_case access methods for manipulating FSON data.
         *
         * It is designed for convenience in C++ applications while still relying on
         * the lightweight, portable C backend provided by Fossil Media.
         */
        class Fson {
        public:
            /**
             * @brief Construct an empty null FSON value.
             */
            Fson() : value_(fossil_media_fson_new_null()) {}

            /**
             * @brief Construct from a C API value pointer (takes ownership).
             * @param val Pointer to a fossil_media_fson_value_t.
             * @throws std::invalid_argument if val is nullptr.
             */
            explicit Fson(fossil_media_fson_value_t* val) : value_(val) {
                if (!val) {
                    throw std::invalid_argument("Null FSON value pointer passed to Fson");
                }
            }

            /**
             * @brief Destructor: frees the underlying C API FSON value.
             */
            ~Fson() {
                fossil_media_fson_free(value_);
            }

            // Non-copyable
            Fson(const Fson&) = delete;
            Fson& operator=(const Fson&) = delete;

            // Movable
            Fson(Fson&& other) noexcept : value_(std::exchange(other.value_, nullptr)) {}

            Fson& operator=(Fson&& other) noexcept {
                if (this != &other) {
                    fossil_media_fson_free(value_);
                    value_ = std::exchange(other.value_, nullptr);
                }
                return *this;
            }

            /**
             * @brief Parse FSON text into a Fson object.
             * @param text NUL-terminated FSON string.
             * @return Parsed Fson object.
             * @throws FsonError if parsing fails.
             */
            static Fson parse(const std::string& text) {
                fossil_media_fson_error_t err{};
                fossil_media_fson_value_t* val = fossil_media_fson_parse(text.c_str(), &err);
                if (!val) {
                    throw FsonError(std::string("Parse error: ") + err.message);
                }
                return Fson(val);
            }

            /**
             * @brief Create a FSON boolean value.
             * @param b Boolean value.
             * @return Fson object holding a boolean.
             */
            static Fson new_bool(bool b) {
                return Fson(fossil_media_fson_new_bool(b ? 1 : 0));
            }

            /**
             * @brief Create a FSON int8 value.
             * @param value int8_t value.
             * @return Fson object holding an int8.
             */
            static Fson new_i8(int8_t value) {
                return Fson(fossil_media_fson_new_i8(value));
            }

            /**
             * @brief Create a FSON int16 value.
             * @param value int16_t value.
             * @return Fson object holding an int16.
             */
            static Fson new_i16(int16_t value) {
                return Fson(fossil_media_fson_new_i16(value));
            }

            /**
             * @brief Create a FSON int32 value.
             * @param value int32_t value.
             * @return Fson object holding an int32.
             */
            static Fson new_i32(int32_t value) {
                return Fson(fossil_media_fson_new_i32(value));
            }

            /**
             * @brief Create a FSON int64 value.
             * @param value int64_t value.
             * @return Fson object holding an int64.
             */
            static Fson new_i64(int64_t value) {
                return Fson(fossil_media_fson_new_i64(value));
            }

            /**
             * @brief Create a FSON uint8 value.
             * @param value uint8_t value.
             * @return Fson object holding a uint8.
             */
            static Fson new_u8(uint8_t value) {
                return Fson(fossil_media_fson_new_u8(value));
            }

            /**
             * @brief Create a FSON uint16 value.
             * @param value uint16_t value.
             * @return Fson object holding a uint16.
             */
            static Fson new_u16(uint16_t value) {
                return Fson(fossil_media_fson_new_u16(value));
            }

            /**
             * @brief Create a FSON uint32 value.
             * @param value uint32_t value.
             * @return Fson object holding a uint32.
             */
            static Fson new_u32(uint32_t value) {
                return Fson(fossil_media_fson_new_u32(value));
            }

            /**
             * @brief Create a FSON uint64 value.
             * @param value uint64_t value.
             * @return Fson object holding a uint64.
             */
            static Fson new_u64(uint64_t value) {
                return Fson(fossil_media_fson_new_u64(value));
            }

            /**
             * @brief Create a FSON float32 value.
             * @param value float value.
             * @return Fson object holding a float32.
             */
            static Fson new_f32(float value) {
                return Fson(fossil_media_fson_new_f32(value));
            }

            /**
             * @brief Create a FSON float64 value.
             * @param value double value.
             * @return Fson object holding a float64.
             */
            static Fson new_f64(double value) {
                return Fson(fossil_media_fson_new_f64(value));
            }

            /**
             * @brief Create a FSON octal value.
             * @param value uint64_t value.
             * @return Fson object holding an octal value.
             */
            static Fson new_oct(uint64_t value) {
                return Fson(fossil_media_fson_new_oct(value));
            }

            /**
             * @brief Create a FSON hexadecimal value.
             * @param value uint64_t value.
             * @return Fson object holding a hexadecimal value.
             */
            static Fson new_hex(uint64_t value) {
                return Fson(fossil_media_fson_new_hex(value));
            }

            /**
             * @brief Create a FSON binary value.
             * @param value uint64_t value.
             * @return Fson object holding a binary value.
             */
            static Fson new_bin(uint64_t value) {
                return Fson(fossil_media_fson_new_bin(value));
            }

            /**
             * @brief Create a FSON char value.
             * @param value char value.
             * @return Fson object holding a char.
             */
            static Fson new_char(char value) {
                return Fson(fossil_media_fson_new_char(value));
            }

            /**
             * @brief Create a FSON string value.
             * @param s String value (copied internally).
             * @return Fson object holding a string.
             */
            static Fson new_string(const std::string& s) {
                return Fson(fossil_media_fson_new_string(s.c_str()));
            }

            /**
             * @brief Create a FSON array.
             * @return Fson object holding an empty array.
             */
            static Fson new_array() {
                return Fson(fossil_media_fson_new_array());
            }

            /**
             * @brief Create a FSON object.
             * @return Fson object holding an empty object.
             */
            static Fson new_object() {
                return Fson(fossil_media_fson_new_object());
            }

            /**
             * @brief Create a FSON enum value.
             * @param symbol Enum symbol string.
             * @param allowed Optional array of allowed symbols.
             * @param allowed_count Number of allowed symbols.
             * @return Fson object holding an enum value.
             */
            static Fson new_enum(const std::string& symbol, const std::vector<std::string>& allowed = {}) {
                std::vector<const char*> allowed_cstrs;
                for (const auto& s : allowed) {
                    allowed_cstrs.push_back(s.c_str());
                }
                return Fson(fossil_media_fson_new_enum(
                    symbol.c_str(),
                    allowed.empty() ? nullptr : allowed_cstrs.data(),
                    allowed_cstrs.size()
                ));
            }

            /**
             * @brief Create a FSON flags value.
             * @param bitmask Numeric bitmask value.
             * @param symbols Array of symbolic flag names.
             * @return Fson object holding a flags value.
             */
            static Fson new_flags(uint64_t bitmask, const std::vector<std::string>& symbols) {
                std::vector<const char*> symbol_cstrs;
                for (const auto& s : symbols) {
                    symbol_cstrs.push_back(s.c_str());
                }
                return Fson(fossil_media_fson_new_flags(
                    bitmask,
                    symbol_cstrs.empty() ? nullptr : symbol_cstrs.data(),
                    symbol_cstrs.size()
                ));
            }

            /**
             * @brief Append a value to a FSON array.
             * @param val Fson value to append (moved).
             * @throws FsonError if not an array or append fails.
             */
            void array_append(Fson&& val) {
                if (fossil_media_fson_array_append(value_, val.value_) != 0) {
                    throw FsonError("Failed to append to array");
                }
                val.value_ = nullptr; // Ownership transferred
            }

            /**
             * @brief Get element at index in FSON array.
             * @param index Zero-based index.
             * @return Fson element at index (shared, no ownership transfer).
             */
            Fson array_get(size_t index) const {
                fossil_media_fson_value_t* v = fossil_media_fson_array_get(value_, index);
                if (!v) {
                    throw FsonError("Array index out of range");
                }
                // Deep copy for safe ownership
                fossil_media_fson_value_t* copy = fossil_media_fson_clone(v);
                if (!copy) {
                    throw FsonError("Failed to clone array element");
                }
                return Fson(copy);
            }

            /**
             * @brief Get the number of elements in a FSON array.
             * @return Number of elements.
             * @throws FsonError if not an array.
             */
            size_t array_size() const {
                return fossil_media_fson_array_size(value_);
            }

            /**
             * @brief Set key-value in FSON object.
             * @param key String key.
             * @param val Fson value to set (moved).
             * @throws FsonError if not an object or set fails.
             */
            void object_set(const std::string& key, Fson&& val) {
                if (fossil_media_fson_object_set(value_, key.c_str(), val.value_) != 0) {
                    throw FsonError("Failed to set key in object");
                }
                val.value_ = nullptr; // Ownership transferred
            }

            /**
             * @brief Get value from FSON object by key.
             * @param key String key.
             * @return Fson value at key.
             * @throws FsonError if not found.
             */
            Fson object_get(const std::string& key) const {
                fossil_media_fson_value_t* v = fossil_media_fson_object_get(value_, key.c_str());
                if (!v) {
                    throw FsonError("Key not found in object");
                }
                fossil_media_fson_value_t* copy = fossil_media_fson_clone(v);
                if (!copy) {
                    throw FsonError("Failed to clone object value");
                }
                return Fson(copy);
            }

            /**
             * @brief Serialize FSON to string.
             * @param pretty If true, output with indentation.
             * @return Serialized FSON string.
             * @throws FsonError if stringify fails.
             */
            std::string stringify(bool pretty = false) const {
                fossil_media_fson_error_t err{};
                char* s = fossil_media_fson_stringify(value_, pretty ? 1 : 0, &err);
                if (!s) {
                    throw FsonError(std::string("Stringify error: ") + err.message);
                }
                std::string result(s);
                free(s);
                return result;
            }

            /**
             * @brief Deep copy this FSON value.
             * @return A new Fson object that is a clone of this value.
             * @throws FsonError if cloning fails.
             */
            Fson clone() const {
                fossil_media_fson_value_t* v = fossil_media_fson_clone(value_);
                if (!v) {
                    throw FsonError("Failed to clone FSON value");
                }
                return Fson(v);
            }

            /**
             * @brief Compare this FSON value to another for equality.
             * @param other The other Fson object to compare.
             * @return true if equal, false otherwise.
             * @throws FsonError if comparison fails.
             */
            bool equals(const Fson& other) const {
                int result = fossil_media_fson_equals(value_, other.value_);
                if (result == -1) {
                    throw FsonError("Failed to compare FSON values");
                }
                return result == 1;
            }

            /**
             * @brief Check if this value is null.
             * @return true if null, false otherwise.
             */
            bool is_null() const {
                return fossil_media_fson_is_null(value_) == 1;
            }

            /**
             * @brief Check if this value is an array.
             * @return true if array, false otherwise.
             */
            bool is_array() const {
                return fossil_media_fson_is_array(value_) == 1;
            }

            /**
             * @brief Check if this value is an object.
             * @return true if object, false otherwise.
             */
            bool is_object() const {
                return fossil_media_fson_is_object(value_) == 1;
            }

            /**
             * @brief Reserve capacity for a FSON array.
             * @param capacity Desired capacity.
             * @throws FsonError if not an array or reserve fails.
             */
            void array_reserve(size_t capacity) {
                if (fossil_media_fson_array_reserve(value_, capacity) != 0) {
                    throw FsonError("Failed to reserve array capacity");
                }
            }

            /**
             * @brief Reserve capacity for a FSON object.
             * @param capacity Desired capacity.
             * @throws FsonError if not an object or reserve fails.
             */
            void object_reserve(size_t capacity) {
                if (fossil_media_fson_object_reserve(value_, capacity) != 0) {
                    throw FsonError("Failed to reserve object capacity");
                }
            }

            /**
             * @brief Parse a FSON file into a Fson object.
             * @param filename Path to FSON file.
             * @return Parsed Fson object.
             * @throws FsonError if parsing fails.
             */
            static Fson parse_file(const std::string& filename) {
                fossil_media_fson_error_t err{};
                fossil_media_fson_value_t* val = fossil_media_fson_parse_file(filename.c_str(), &err);
                if (!val) {
                    throw FsonError(std::string("Parse file error: ") + err.message);
                }
                return Fson(val);
            }

            /**
             * @brief Write this FSON value to a file.
             * @param filename Path to output file.
             * @param pretty If true, output with indentation.
             * @throws FsonError if writing fails.
             */
            void write_file(const std::string& filename, bool pretty = false) const {
                fossil_media_fson_error_t err{};
                int rc = fossil_media_fson_write_file(value_, filename.c_str(), pretty ? 1 : 0, &err);
                if (rc != 0) {
                    throw FsonError(std::string("Write file error: ") + err.message);
                }
            }

            /**
             * @brief Get int8 value from this FSON value.
             * @return int8_t value.
             * @throws FsonError if type mismatch or error.
             */
            int8_t get_i8() const {
                int8_t out;
                if (fossil_media_fson_get_i8(value_, &out) != 0)
                    throw FsonError("Failed to get int8 value");
                return out;
            }

            /**
             * @brief Get int16 value from this FSON value.
             * @return int16_t value.
             * @throws FsonError if type mismatch or error.
             */
            int16_t get_i16() const {
                int16_t out;
                if (fossil_media_fson_get_i16(value_, &out) != 0)
                    throw FsonError("Failed to get int16 value");
                return out;
            }

            /**
             * @brief Get int32 value from this FSON value.
             * @return int32_t value.
             * @throws FsonError if type mismatch or error.
             */
            int32_t get_i32() const {
                int32_t out;
                if (fossil_media_fson_get_i32(value_, &out) != 0)
                    throw FsonError("Failed to get int32 value");
                return out;
            }

            /**
             * @brief Get int64 value from this FSON value.
             * @return int64_t value.
             * @throws FsonError if type mismatch or error.
             */
            int64_t get_i64() const {
                int64_t out;
                if (fossil_media_fson_get_i64(value_, &out) != 0)
                    throw FsonError("Failed to get int64 value");
                return out;
            }

            /**
             * @brief Get uint8 value from this FSON value.
             * @return uint8_t value.
             * @throws FsonError if type mismatch or error.
             */
            uint8_t get_u8() const {
                uint8_t out;
                if (fossil_media_fson_get_u8(value_, &out) != 0)
                    throw FsonError("Failed to get uint8 value");
                return out;
            }

            /**
             * @brief Get uint16 value from this FSON value.
             * @return uint16_t value.
             * @throws FsonError if type mismatch or error.
             */
            uint16_t get_u16() const {
                uint16_t out;
                if (fossil_media_fson_get_u16(value_, &out) != 0)
                    throw FsonError("Failed to get uint16 value");
                return out;
            }

            /**
             * @brief Get uint32 value from this FSON value.
             * @return uint32_t value.
             * @throws FsonError if type mismatch or error.
             */
            uint32_t get_u32() const {
                uint32_t out;
                if (fossil_media_fson_get_u32(value_, &out) != 0)
                    throw FsonError("Failed to get uint32 value");
                return out;
            }

            /**
             * @brief Get uint64 value from this FSON value.
             * @return uint64_t value.
             * @throws FsonError if type mismatch or error.
             */
            uint64_t get_u64() const {
                uint64_t out;
                if (fossil_media_fson_get_u64(value_, &out) != 0)
                    throw FsonError("Failed to get uint64 value");
                return out;
            }

            /**
             * @brief Get float32 value from this FSON value.
             * @return float value.
             * @throws FsonError if type mismatch or error.
             */
            float get_f32() const {
                float out;
                if (fossil_media_fson_get_f32(value_, &out) != 0)
                    throw FsonError("Failed to get float32 value");
                return out;
            }

            /**
             * @brief Get float64 value from this FSON value.
             * @return double value.
             * @throws FsonError if type mismatch or error.
             */
            double get_f64() const {
                double out;
                if (fossil_media_fson_get_f64(value_, &out) != 0)
                    throw FsonError("Failed to get float64 value");
                return out;
            }

            /**
             * @brief Get octal value from this FSON value.
             * @return uint64_t value.
             * @throws FsonError if type mismatch or error.
             */
            uint64_t get_oct() const {
                uint64_t out;
                if (fossil_media_fson_get_oct(value_, &out) != 0)
                    throw FsonError("Failed to get octal value");
                return out;
            }

            /**
             * @brief Get hexadecimal value from this FSON value.
             * @return uint64_t value.
             * @throws FsonError if type mismatch or error.
             */
            uint64_t get_hex() const {
                uint64_t out;
                if (fossil_media_fson_get_hex(value_, &out) != 0)
                    throw FsonError("Failed to get hexadecimal value");
                return out;
            }

            /**
             * @brief Get binary value from this FSON value.
             * @return uint64_t value.
             * @throws FsonError if type mismatch or error.
             */
            uint64_t get_bin() const {
                uint64_t out;
                if (fossil_media_fson_get_bin(value_, &out) != 0)
                    throw FsonError("Failed to get binary value");
                return out;
            }

            /**
             * @brief Get char value from this FSON value.
             * @return char value.
             * @throws FsonError if type mismatch or error.
             */
            char get_char() const {
                char out;
                if (fossil_media_fson_get_char(value_, &out) != 0)
                    throw FsonError("Failed to get char value");
                return out;
            }

            /**
             * @brief Get boolean value from this FSON value.
             * @return bool value.
             * @throws FsonError if type mismatch or error.
             */
            bool get_bool() const {
                int out;
                if (fossil_media_fson_get_bool(value_, &out) != 0)
                    throw FsonError("Failed to get bool value");
                return out != 0;
            }

            /**
             * @brief Get C string value from this FSON value.
             * @return std::string value.
             * @throws FsonError if type mismatch or error.
             */
            std::string get_cstr() const {
                char* out = nullptr;
                if (fossil_media_fson_get_cstr(value_, &out) != 0 || !out)
                    throw FsonError("Failed to get string value");
                std::string result(out);
                free(out);
                return result;
            }

            /**
             * @brief Get enum symbol from this FSON enum value.
             * @return std::string symbol.
             * @throws FsonError if type mismatch or error.
             */
            std::string get_enum() const {
                const char* out = nullptr;
                if (fossil_media_fson_get_enum(value_, &out) != 0 || !out)
                    throw FsonError("Failed to get enum symbol");
                std::string result(out);
                // No need to free 'out' as it's not heap-allocated by API
                return result;
            }

            /**
             * @brief Get flags bitmask from this FSON flags value.
             * @return uint64_t bitmask.
             * @throws FsonError if type mismatch or error.
             */
            uint64_t get_flags() const {
                uint64_t out;
                if (fossil_media_fson_get_flags(value_, &out) != 0)
                    throw FsonError("Failed to get flags bitmask");
                return out;
            }

            /**
             * @brief Print a debug dump of this FSON value.
             * @param indent Starting indentation level.
             */
            void debug_dump(int indent = 0) const {
                fossil_media_fson_debug_dump(value_, indent);
            }

            /**
             * @brief Validate FSON text without building a DOM.
             * @param text Input FSON text.
             * @return true if valid, false otherwise.
             */
            static bool validate(const std::string& text) {
                fossil_media_fson_error_t err{};
                return fossil_media_fson_validate(text.c_str(), &err) == 0;
            }

            /**
             * @brief Get a FSON value using a dotted path expression.
             * @param path Path string (e.g., "user.name" or "items[2].id").
             * @return Fson object at the path.
             * @throws FsonError if not found.
             */
            Fson get_path(const std::string& path) const {
                fossil_media_fson_value_t* v = fossil_media_fson_get_path(value_, path.c_str());
                if (!v) {
                    // Return a null Fson object if path not found
                    return Fson(fossil_media_fson_new_null());
                }
                fossil_media_fson_value_t* copy = fossil_media_fson_clone(v);
                if (!copy) {
                    throw FsonError("Failed to clone path value");
                }
                return Fson(copy);
            }

        private:
            fossil_media_fson_value_t* value_;
        };

    } // namespace media

} // namespace fossil

#endif

#endif /* FOSSIL_MEDIA_FSON_H */
