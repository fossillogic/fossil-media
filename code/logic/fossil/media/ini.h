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

        /**
         * @brief C++ wrapper class for handling INI files.
         */
        class Ini {
        public:
            /**
             * @brief Construct an empty INI object.
             */
            Ini() {
                ini_.sections = nullptr;
                ini_.section_count = 0;
            }

            /**
             * @brief Construct and load an INI file from disk.
             * @param path Path to the .ini file.
             * @throw std::runtime_error on failure.
             */
            explicit Ini(const std::string& path) {
                ini_.sections = nullptr;
                ini_.section_count = 0;
                if (fossil_media_ini_load_file(path.c_str(), &ini_) != 0)
                    throw std::runtime_error("Failed to load INI file: " + path);
            }

            /**
             * @brief Construct and load an INI file from a string buffer.
             * @param data INI data as null-terminated string.
             * @throw std::runtime_error on failure.
             */
            Ini(const char* data) {
                ini_.sections = nullptr;
                ini_.section_count = 0;
                if (fossil_media_ini_load_string(data, &ini_) != 0)
                    throw std::runtime_error("Failed to load INI from string");
            }

            /**
             * @brief Move constructor.
             */
            Ini(Ini&& other) noexcept
            : ini_{other.ini_} {
                other.ini_.sections = nullptr;
                other.ini_.section_count = 0;
            }

            /**
             * @brief Move assignment operator.
             */
            Ini& operator=(Ini&& other) noexcept {
                if (this != &other) {
                    fossil_media_ini_free(&ini_);
                    ini_ = other.ini_;
                    other.ini_.sections = nullptr;
                    other.ini_.section_count = 0;
                }
                return *this;
            }

            /**
             * @brief Destructor. Frees all memory used by the INI structure.
             */
            ~Ini() {
                fossil_media_ini_free(&ini_);
            }

            // Non-copyable
            Ini(const Ini&) = delete;
            Ini& operator=(const Ini&) = delete;

            /**
             * @brief Load an INI file from disk.
             * @param path Path to the .ini file.
             * @return true on success, false on failure.
             */
            bool load_file(const std::string& path) {
                fossil_media_ini_free(&ini_);
                ini_.sections = nullptr;
                ini_.section_count = 0;
                return fossil_media_ini_load_file(path.c_str(), &ini_) == 0;
            }

            /**
             * @brief Load an INI file from a string buffer.
             * @param data INI data as null-terminated string.
             * @return true on success, false on failure.
             */
            bool load_string(const char* data) {
                fossil_media_ini_free(&ini_);
                ini_.sections = nullptr;
                ini_.section_count = 0;
                return fossil_media_ini_load_string(data, &ini_) == 0;
            }

            /**
             * @brief Save the INI structure to disk.
             * @param path Path to output file.
             * @return true on success, false on failure.
             */
            bool save_file(const std::string& path) const {
                return fossil_media_ini_save_file(path.c_str(), &ini_) == 0;
            }

            /**
             * @brief Get the value for a given section/key.
             * @param section Section name.
             * @param key Key name.
             * @return Value string or empty string if not found.
             */
            std::string get(const std::string& section, const std::string& key) const {
                const char* val = fossil_media_ini_get(&ini_, section.c_str(), key.c_str());
                return val ? std::string(val) : std::string();
            }

            /**
             * @brief Set the value for a given section/key. Creates section/key if they do not exist.
             * @param section Section name.
             * @param key Key name.
             * @param value Value string.
             * @return true on success, false on failure.
             */
            bool set(const std::string& section, const std::string& key, const std::string& value) {
                return fossil_media_ini_set(&ini_, section.c_str(), key.c_str(), value.c_str()) == 0;
            }

            /**
             * @brief Get the underlying C structure (const).
             */
            const fossil_media_ini_t* c_struct() const { return &ini_; }

            /**
             * @brief Get the underlying C structure (non-const).
             */
            fossil_media_ini_t* c_struct() { return &ini_; }

        private:
            fossil_media_ini_t ini_;
        };

    } // namespace media

} // namespace fossil

#endif

#endif /* FOSSIL_MEDIA_INI_H */
