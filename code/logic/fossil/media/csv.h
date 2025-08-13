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
#ifndef FOSSIL_MEDIA_CSV_H
#define FOSSIL_MEDIA_CSV_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @file fossil_media_csv.h
 * @brief Fossil Media CSV parsing and writing library (pure C, no dependencies).
 *
 * Supports basic CSV parsing with configurable delimiter and quoting rules.
 * Handles RFC4180-compatible CSV by default but allows customization.
 */

/* Error codes for CSV parsing/writing */
typedef enum fossil_media_csv_error_t {
    FOSSIL_MEDIA_CSV_OK = 0,          /**< No error */
    FOSSIL_MEDIA_CSV_ERR_MEMORY,      /**< Memory allocation failed */
    FOSSIL_MEDIA_CSV_ERR_SYNTAX,      /**< Syntax error in CSV input */
    FOSSIL_MEDIA_CSV_ERR_IO,          /**< I/O error */
    FOSSIL_MEDIA_CSV_ERR_INVALID_ARG  /**< Invalid argument */
} fossil_media_csv_error_t;

/* CSV row structure: array of strings (fields) */
typedef struct fossil_media_csv_row_t {
    char **fields;       /**< Array of NUL-terminated strings */
    size_t field_count;  /**< Number of fields */
} fossil_media_csv_row_t;

/* CSV document structure: array of rows */
typedef struct fossil_media_csv_doc_t {
    fossil_media_csv_row_t *rows; /**< Array of CSV rows */
    size_t row_count;             /**< Number of rows */
} fossil_media_csv_doc_t;

/**
 * @brief Parse a CSV-formatted string into a document.
 *
 * @param csv_text   NUL-terminated CSV text.
 * @param delimiter  Field delimiter (usually ',' or ';').
 * @param err_out    Optional pointer to error code.
 * @return Pointer to a parsed CSV document (caller must free with fossil_media_csv_free()).
 */
fossil_media_csv_doc_t *
fossil_media_csv_parse(const char *csv_text, char delimiter, fossil_media_csv_error_t *err_out);

/**
 * @brief Free a CSV document and all associated memory.
 *
 * @param doc  Pointer to document (can be NULL).
 */
void fossil_media_csv_free(fossil_media_csv_doc_t *doc);

/**
 * @brief Convert a CSV document back to a string.
 *
 * @param doc        CSV document to stringify.
 * @param delimiter  Field delimiter to use.
 * @param err_out    Optional pointer to error code.
 * @return Heap-allocated CSV string (caller frees with free()).
 */
char *
fossil_media_csv_stringify(const fossil_media_csv_doc_t *doc, char delimiter, fossil_media_csv_error_t *err_out);

/**
 * @brief Append a row to the CSV document.
 *
 * @param doc        CSV document.
 * @param fields     Array of field strings.
 * @param field_cnt  Number of fields.
 * @return 0 on success, non-zero on error.
 */
int fossil_media_csv_append_row(fossil_media_csv_doc_t *doc, const char **fields, size_t field_cnt);

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

#endif /* FOSSIL_MEDIA_CSV_H */
