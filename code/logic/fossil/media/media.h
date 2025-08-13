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
#ifndef FOSSIL_MEDIA_CORE_H
#define FOSSIL_MEDIA_CORE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @defgroup MediaLibrary Fossil Media Core
 * @{
 */

/* ===============================
 *  Common Utility Declarations
 * =============================== */

/**
 * @brief Allocates and returns a null-terminated string containing a
 *        copy of the given input.
 *
 * @param src Null-terminated C string to copy.
 * @return Newly allocated null-terminated C string. Caller must free().
 */
char *fossil_media_strdup(const char *src);

/**
 * @brief Duplicate up to `n` characters from a string into a new heap-allocated buffer.
 *
 * This behaves like POSIX `strndup`, but is implemented here to ensure
 * portability since not all platforms provide it (especially Windows/MSVC).
 *
 * @param s   The source C-string to duplicate. Must not be NULL.
 * @param n   Maximum number of characters to copy (not counting the null terminator).
 *
 * @return Newly allocated null-terminated string containing at most `n` characters
 *         from `s`. Returns NULL if allocation fails or if `s` is NULL.
 *
 * @note Caller must free the returned buffer with `free()` when done.
 */
char *fossil_media_strndup(const char *s, size_t n);

/**
 * @brief Reads an entire file into a null-terminated buffer.
 *
 * @param path Path to the file to read.
 * @param out_size Optional pointer to store the number of bytes read.
 * @return Newly allocated buffer containing file contents, or NULL on error.
 *         Caller must free().
 */
char *fossil_media_read_file(const char *path, size_t *out_size);

/**
 * @brief Writes a string to a file.
 *
 * @param path Path to the file to write.
 * @param data Null-terminated string to write.
 * @return 0 on success, nonzero on error.
 */
int fossil_media_write_file(const char *path, const char *data);

/**
 * @brief Trims whitespace from both ends of a mutable string in place.
 *
 * @param str Mutable string buffer to trim. May be NULL (no effect).
 * @return Pointer to the trimmed string (same as input).
 */
char *fossil_media_trim(char *str);

/** @} */ // end group MediaLibrary

#ifdef __cplusplus
}
#endif

#endif /* FOSSIL_MEDIA_CORE_H */
