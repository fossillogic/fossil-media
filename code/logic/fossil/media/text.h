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
#ifndef FOSSIL_MEDIA_TEXT_H
#define FOSSIL_MEDIA_TEXT_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Remove leading and trailing whitespace from a string in place.
 *
 * @param str String to trim. Modified in place.
 * @return Pointer to the trimmed string.
 */
char *fossil_media_text_trim(char *str);

/**
 * @brief Convert a string to lowercase in place.
 *
 * @param str String to modify.
 * @return Pointer to the modified string.
 */
char *fossil_media_text_tolower(char *str);

/**
 * @brief Convert a string to uppercase in place.
 *
 * @param str String to modify.
 * @return Pointer to the modified string.
 */
char *fossil_media_text_toupper(char *str);

/**
 * @brief Replace all occurrences of a substring within a string.
 *
 * @param str        The input string (modified in place if space allows).
 * @param old_sub    Substring to replace.
 * @param new_sub    Substring to insert.
 * @param buf_size   Size of the buffer pointed to by str.
 * @return Number of replacements made.
 */
size_t fossil_media_text_replace(char *str, const char *old_sub, const char *new_sub, size_t buf_size);

/**
 * @brief Find the first occurrence of a substring in a string (case-sensitive).
 *
 * @param haystack   The string to search in.
 * @param needle     The substring to search for.
 * @return Pointer to the first occurrence, or NULL if not found.
 */
char *fossil_media_text_find(const char *haystack, const char *needle);

/**
 * @brief Split a string into tokens by a delimiter.
 *
 * @param str        String to split (modified in place).
 * @param delim      Delimiter character.
 * @param tokens     Output array of string pointers.
 * @param max_tokens Maximum number of tokens to extract.
 * @return Number of tokens extracted.
 */
size_t fossil_media_text_split(char *str, char delim, char **tokens, size_t max_tokens);

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

#endif /* FOSSIL_MEDIA_TEXT_H */
