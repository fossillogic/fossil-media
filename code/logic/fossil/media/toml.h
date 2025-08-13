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
#ifndef FOSSIL_MEDIA_TOML_H
#define FOSSIL_MEDIA_TOML_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Representation of a TOML key-value pair.
 */
typedef struct fossil_media_toml_entry_t {
    char *key;      /**< The key name */
    char *value;    /**< The string value (numbers are stored as strings and can be converted) */
} fossil_media_toml_entry_t;

/**
 * @brief Representation of a parsed TOML table (section).
 */
typedef struct fossil_media_toml_table_t {
    char *name;                             /**< Table name */
    fossil_media_toml_entry_t *entries;     /**< Array of key-value pairs */
    size_t entry_count;                     /**< Number of entries */
} fossil_media_toml_table_t;

/**
 * @brief Main parsed TOML document.
 */
typedef struct fossil_media_toml_t {
    fossil_media_toml_table_t *tables;  /**< Array of tables */
    size_t table_count;                 /**< Number of tables */
} fossil_media_toml_t;

/**
 * @brief Parse TOML data from a string.
 *
 * @param input The TOML string to parse.
 * @param out_toml Pointer to TOML document struct to populate.
 * @return 0 on success, nonzero on error.
 */
int fossil_media_toml_parse(const char *input, fossil_media_toml_t *out_toml);

/**
 * @brief Retrieve a value from a specific table by key.
 *
 * @param toml Pointer to parsed TOML document.
 * @param table_name Table name to search (NULL for root).
 * @param key Key name to search for.
 * @return Value string, or NULL if not found.
 */
const char *fossil_media_toml_get(const fossil_media_toml_t *toml, const char *table_name, const char *key);

/**
 * @brief Free a parsed TOML document.
 *
 * @param toml Pointer to TOML document to free.
 */
void fossil_media_toml_free(fossil_media_toml_t *toml);

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

#endif /* FOSSIL_MEDIA_TOML_H */
