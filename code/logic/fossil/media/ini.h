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
#ifndef FOSSIL_MEDIA_INI_H
#define FOSSIL_MEDIA_INI_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Represents a single key-value pair in an INI section.
 */
typedef struct fossil_media_ini_entry_t {
    char *key;
    char *value;
} fossil_media_ini_entry_t;

/**
 * @brief Represents a section in an INI file.
 */
typedef struct fossil_media_ini_section_t {
    char *name;
    fossil_media_ini_entry_t *entries;
    size_t entry_count;
} fossil_media_ini_section_t;

/**
 * @brief Represents a loaded INI file.
 */
typedef struct fossil_media_ini_t {
    fossil_media_ini_section_t *sections;
    size_t section_count;
} fossil_media_ini_t;

/**
 * @brief Load an INI file from disk.
 *
 * @param path Path to the .ini file.
 * @param ini Output structure pointer.
 * @return 0 on success, nonzero on failure.
 */
int fossil_media_ini_load_file(const char *path, fossil_media_ini_t *ini);

/**
 * @brief Load an INI file from a string buffer.
 *
 * @param data INI data as null-terminated string.
 * @param ini Output structure pointer.
 * @return 0 on success, nonzero on failure.
 */
int fossil_media_ini_load_string(const char *data, fossil_media_ini_t *ini);

/**
 * @brief Save an INI structure to disk.
 *
 * @param path Path to output file.
 * @param ini INI data structure.
 * @return 0 on success, nonzero on failure.
 */
int fossil_media_ini_save_file(const char *path, const fossil_media_ini_t *ini);

/**
 * @brief Free all memory used by an INI structure.
 */
void fossil_media_ini_free(fossil_media_ini_t *ini);

/**
 * @brief Get the value for a given section/key.
 *
 * @param ini INI data structure.
 * @param section Section name.
 * @param key Key name.
 * @return Value string or NULL if not found.
 */
const char *fossil_media_ini_get(const fossil_media_ini_t *ini, const char *section, const char *key);

/**
 * @brief Set the value for a given section/key.
 *        Creates section/key if they do not exist.
 *
 * @param ini INI data structure.
 * @param section Section name.
 * @param key Key name.
 * @param value Value string.
 * @return 0 on success, nonzero on failure.
 */
int fossil_media_ini_set(fossil_media_ini_t *ini, const char *section, const char *key, const char *value);

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

#endif /* FOSSIL_MEDIA_INI_H */