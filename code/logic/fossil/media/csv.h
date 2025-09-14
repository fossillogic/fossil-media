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
#include <vector>
#include <utility>

namespace fossil {

    namespace media {

        /**
         * @class Csv
         * @brief C++ RAII wrapper for fossil_media_csv_doc_t.
         *
         * Provides convenient C++ interface for parsing, manipulating,
         * and serializing CSV data using the underlying C library.
         */
        class Csv {
        public:
            /**
             * @brief Construct from CSV string.
             * @param csv_text   CSV input as std::string.
             * @param delimiter  Field delimiter (default: ',').
             * @throws std::runtime_error on parse error.
             */
            Csv(const std::string& csv_text, char delimiter = ',') {
                fossil_media_csv_error_t err = FOSSIL_MEDIA_CSV_OK;
                doc_ = fossil_media_csv_parse(csv_text.c_str(), delimiter, &err);
                if (!doc_ || err != FOSSIL_MEDIA_CSV_OK) {
                    throw std::runtime_error("CSV parse error");
                }
                delimiter_ = delimiter;
            }

            /**
             * @brief Destructor. Frees all resources.
             */
            ~Csv() {
                if (doc_) {
                    fossil_media_csv_free(doc_);
                }
            }

            // Non-copyable
            Csv(const Csv&) = delete;
            Csv& operator=(const Csv&) = delete;

            /**
             * @brief Move constructor.
             */
            Csv(Csv&& other) noexcept : doc_(other.doc_), delimiter_(other.delimiter_) {
                other.doc_ = nullptr;
            }

            /**
             * @brief Move assignment.
             */
            Csv& operator=(Csv&& other) noexcept {
                if (this != &other) {
                    if (doc_) fossil_media_csv_free(doc_);
                    doc_ = other.doc_;
                    delimiter_ = other.delimiter_;
                    other.doc_ = nullptr;
                }
                return *this;
            }

            /**
             * @brief Get number of rows in the CSV document.
             * @return Row count.
             */
            size_t row_count() const {
                return doc_ ? doc_->row_count : 0;
            }

            /**
             * @brief Get number of fields in a given row.
             * @param row Row index.
             * @return Number of fields, or 0 if out of bounds.
             */
            size_t field_count(size_t row) const {
                if (!doc_ || row >= doc_->row_count) return 0;
                return doc_->rows[row].field_count;
            }

            /**
             * @brief Get field value as string.
             * @param row Row index.
             * @param col Column index.
             * @return Field value, or empty string if out of bounds.
             */
            std::string field(size_t row, size_t col) const {
                if (!doc_ || row >= doc_->row_count) return {};
                const fossil_media_csv_row_t& r = doc_->rows[row];
                if (col >= r.field_count) return {};
                return r.fields[col] ? r.fields[col] : "";
            }

            /**
             * @brief Append a row to the CSV document.
             * @param fields Vector of field strings.
             * @throws std::runtime_error on error.
             */
            void append_row(const std::vector<std::string>& fields) {
                std::vector<const char*> cfields;
                for (const auto& f : fields) cfields.push_back(f.c_str());
                if (fossil_media_csv_append_row(doc_, cfields.data(), cfields.size()) != 0) {
                    throw std::runtime_error("CSV append_row error");
                }
            }

            /**
             * @brief Convert the CSV document back to a CSV-formatted string.
             * @return CSV string.
             * @throws std::runtime_error on error.
             */
            std::string to_string() const {
                fossil_media_csv_error_t err = FOSSIL_MEDIA_CSV_OK;
                char* cstr = fossil_media_csv_stringify(doc_, delimiter_, &err);
                if (!cstr || err != FOSSIL_MEDIA_CSV_OK) {
                    throw std::runtime_error("CSV stringify error");
                }
                std::string result(cstr);
                free(cstr);
                return result;
            }

        private:
            fossil_media_csv_doc_t* doc_ = nullptr; /**< Underlying CSV document pointer */
            char delimiter_ = ',';                  /**< Field delimiter */
        };

    } // namespace media

} // namespace fossil

#endif

#endif /* FOSSIL_MEDIA_CSV_H */
