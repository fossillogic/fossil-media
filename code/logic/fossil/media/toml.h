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

        /**
         * @brief C++ wrapper for parsed TOML documents.
         *
         * Provides RAII management and convenient access to TOML data.
         */
        class Toml {
        public:
            /**
             * @brief Parse TOML data from a string.
             *
             * @param input TOML string to parse.
             * @throws std::runtime_error on parse error.
             */
            explicit Toml(const std::string& input) {
                if (fossil_media_toml_parse(input.c_str(), &doc_) != 0) {
                    throw std::runtime_error("Failed to parse TOML data");
                }
            }

            /**
             * @brief Move constructor.
             */
            Toml(Toml&& other) noexcept : doc_{other.doc_} {
                other.doc_.tables = nullptr;
                other.doc_.table_count = 0;
            }

            /**
             * @brief Move assignment operator.
             */
            Toml& operator=(Toml&& other) noexcept {
                if (this != &other) {
                    fossil_media_toml_free(&doc_);
                    doc_ = other.doc_;
                    other.doc_.tables = nullptr;
                    other.doc_.table_count = 0;
                }
                return *this;
            }

            /**
             * @brief Deleted copy constructor.
             */
            Toml(const Toml&) = delete;

            /**
             * @brief Deleted copy assignment operator.
             */
            Toml& operator=(const Toml&) = delete;

            /**
             * @brief Destructor. Frees the TOML document.
             */
            ~Toml() {
                fossil_media_toml_free(&doc_);
            }

            /**
             * @brief Retrieve a value from a specific table by key.
             *
             * @param table_name Table name to search (empty for root).
             * @param key Key name to search for.
             * @return Value string, or empty string if not found.
             */
            std::string get(const std::string& table_name, const std::string& key) const {
                const char* val = fossil_media_toml_get(
                    &doc_,
                    table_name.empty() ? nullptr : table_name.c_str(),
                    key.c_str()
                );
                return val ? std::string(val) : std::string();
            }

        private:
            fossil_media_toml_t doc_{};
        };

    } // namespace media

} // namespace fossil

#endif

#endif /* FOSSIL_MEDIA_TOML_H */
