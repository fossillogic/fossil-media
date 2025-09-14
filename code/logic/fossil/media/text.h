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
#include <vector>
#include <stdexcept>
#include <utility>
#include <cstring>

namespace fossil {

    namespace media {

        /**
         * @brief Text utility class providing static string manipulation methods.
         */
        class Text {
        public:
            /**
             * @brief Remove leading and trailing whitespace from a string.
             *
             * @param str String to trim.
             * @return Trimmed string.
             */
            static std::string trim(const std::string& str) {
                std::string s = str;
                char *buf = new char[s.size() + 2](); // +2 for safety and zero-initialize
                std::strncpy(buf, s.c_str(), s.size());
                buf[s.size()] = '\0'; // Ensure null-termination
                fossil_media_text_trim(buf);
                std::string result(buf);
                delete[] buf;
                return result;
            }

            /**
             * @brief Convert a string to lowercase.
             *
             * @param str String to convert.
             * @return Lowercase string.
             */
            static std::string tolower(const std::string& str) {
                std::string s = str;
                char *buf = new char[s.size() + 1];
                std::strcpy(buf, s.c_str());
                fossil_media_text_tolower(buf);
                std::string result(buf);
                delete[] buf;
                return result;
            }

            /**
             * @brief Convert a string to uppercase.
             *
             * @param str String to convert.
             * @return Uppercase string.
             */
            static std::string toupper(const std::string& str) {
                std::string s = str;
                char *buf = new char[s.size() + 1];
                std::strcpy(buf, s.c_str());
                fossil_media_text_toupper(buf);
                std::string result(buf);
                delete[] buf;
                return result;
            }

            /**
             * @brief Replace all occurrences of a substring within a string.
             *
             * @param str      The input string.
             * @param old_sub  Substring to replace.
             * @param new_sub  Substring to insert.
             * @return String with replacements.
             */
            static std::string replace(const std::string& str, const std::string& old_sub, const std::string& new_sub) {
                size_t buf_size = str.size() + (new_sub.size() > old_sub.size() ? (str.size() * (new_sub.size() - old_sub.size())) : 1) + 1;
                char *buf = new char[buf_size];
                std::strcpy(buf, str.c_str());
                fossil_media_text_replace(buf, old_sub.c_str(), new_sub.c_str(), buf_size);
                std::string result(buf);
                delete[] buf;
                return result;
            }

            /**
             * @brief Find the first occurrence of a substring in a string (case-sensitive).
             *
             * @param haystack The string to search in.
             * @param needle   The substring to search for.
             * @return Position of the first occurrence, or std::string::npos if not found.
             */
            static size_t find(const std::string& haystack, const std::string& needle) {
                char *res = fossil_media_text_find(haystack.c_str(), needle.c_str());
                if (!res) return std::string::npos;
                return static_cast<size_t>(res - haystack.c_str());
            }

            /**
             * @brief Split a string into tokens by a delimiter.
             *
             * @param str        String to split.
             * @param delim      Delimiter character.
             * @return Vector of tokens.
             */
            static std::vector<std::string> split(const std::string& str, char delim) {
                std::vector<std::string> tokens;
                std::string s = str;
                // Estimate max tokens as length + 1
                size_t max_tokens = s.size() + 1;
                std::vector<char*> c_tokens(max_tokens, nullptr);
                char *buf = new char[s.size() + 1];
                std::strcpy(buf, s.c_str());
                size_t count = fossil_media_text_split(buf, delim, c_tokens.data(), max_tokens);
                for (size_t i = 0; i < count; ++i) {
                    tokens.emplace_back(c_tokens[i]);
                }
                delete[] buf;
                return tokens;
            }
        };

    } // namespace media

} // namespace fossil

#endif

#endif /* FOSSIL_MEDIA_TEXT_H */
