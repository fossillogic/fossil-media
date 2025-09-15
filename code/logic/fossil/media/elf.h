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
#ifndef FOSSIL_MEDIA_ELF_H
#define FOSSIL_MEDIA_ELF_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* Error codes (negative) */
enum {
    FMELF_OK = 0,
    FMELF_ERR_IO = -1,
    FMELF_ERR_NOMEM = -2,
    FMELF_ERR_NOT_ELF = -3,
    FMELF_ERR_UNSUPPORTED = -4,
    FMELF_ERR_BAD_FORMAT = -5,
    FMELF_ERR_RANGE = -6,
    FMELF_ERR_NO_SECTION = -7
};

/* Opaque handle for loaded ELF */
typedef struct fossil_media_elf fossil_media_elf_t;

/* Lightweight descriptor for a section */
typedef struct {
    uint32_t sh_name;    /* index into section name string table */
    uint32_t sh_type;
    uint64_t sh_flags;
    uint64_t sh_addr;
    uint64_t sh_offset;
    uint64_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint64_t sh_addralign;
    uint64_t sh_entsize;
} fossil_media_elf_shdr_t;

/* Detect if buffer contains an ELF magic (first 4 bytes = 0x7f 'E' 'L' 'F').
   This is a low-cost check and doesn't validate full header. */
int fossil_media_elf_is_elf(const void *buf, size_t len);

/* Load ELF file from disk into a fossil_media_elf_t handle.
   The handle must be released with fossil_media_elf_free(). */
int fossil_media_elf_load_from_file(const char *path, fossil_media_elf_t **out);

/* Free a handle from fossil_media_elf_load_from_file */
void fossil_media_elf_free(fossil_media_elf_t *elf);

/* Count of sections */
int fossil_media_elf_get_section_count(const fossil_media_elf_t *elf, size_t *out_count);

/* Get section header by index (0..count-1); returns FMELF_OK on success */
int fossil_media_elf_get_section_header(const fossil_media_elf_t *elf, size_t index, fossil_media_elf_shdr_t *out_shdr);

/* Get section name (writes pointer to internal string, do not free) */
int fossil_media_elf_get_section_name(const fossil_media_elf_t *elf, size_t index, const char **out_name);

/* Get pointer to section data in memory (internal buffer), set length in out_len */
int fossil_media_elf_get_section_data(const fossil_media_elf_t *elf, size_t index, const uint8_t **out_ptr, size_t *out_len);

/* Find a section by name; returns index in out_index (0-based) or FMELF_ERR_NO_SECTION */
int fossil_media_elf_find_section_by_name(const fossil_media_elf_t *elf, const char *name, size_t *out_index);

/* Extract section contents to a file on disk (path). Overwrites if exists. */
int fossil_media_elf_extract_section_to_file(const fossil_media_elf_t *elf, size_t index, const char *out_path);

#ifdef __cplusplus
}
#include <string>
#include <stdexcept>
#include <utility>

namespace fossil {

    namespace media {

        /**
         * @brief C++ RAII wrapper for handling ELF files.
         *
         * This class wraps the C-based fossil_media_elf_* API,
         * managing memory automatically and providing exception-safe usage.
         */
        class Elf {
        public:
            /**
             * @brief Construct an empty ELF object (no file loaded).
             */
            Elf() noexcept
            : elf_(nullptr) {}
        
            /**
             * @brief Construct and load an ELF file from disk.
             * @param path Path to the ELF file.
             * @throw std::runtime_error on failure.
             */
            explicit Elf(const std::string& path)
            : elf_(nullptr) {
                if (fossil_media_elf_load_from_file(path.c_str(), &elf_) != FMELF_OK) {
                    throw std::runtime_error("Failed to load ELF file: " + path);
                }
            }
        
            /**
             * @brief Move constructor.
             */
            Elf(Elf&& other) noexcept
            : elf_(other.elf_) {
                other.elf_ = nullptr;
            }
        
            /**
             * @brief Move assignment operator.
             */
            Elf& operator=(Elf&& other) noexcept {
                if (this != &other) {
                    free_();
                    elf_ = other.elf_;
                    other.elf_ = nullptr;
                }
                return *this;
            }
        
            /**
             * @brief Destructor. Frees ELF handle if loaded.
             */
            ~Elf() {
                free_();
            }
        
            // Non-copyable
            Elf(const Elf&) = delete;
            Elf& operator=(const Elf&) = delete;
        
            /**
             * @brief Load an ELF file from disk (replaces any previously loaded ELF).
             * @param path Path to the ELF file.
             * @return true on success, false on failure.
             */
            bool load_file(const std::string& path) {
                free_();
                return fossil_media_elf_load_from_file(path.c_str(), &elf_) == FMELF_OK;
            }
        
            /**
             * @brief Check if an ELF file is currently loaded.
             */
            bool is_loaded() const noexcept {
                return elf_ != nullptr;
            }
        
            /**
             * @brief Get the number of sections in the ELF file.
             * @return Number of sections, or 0 if no ELF is loaded.
             */
            size_t section_count() const {
                if (!elf_) return 0;
                size_t count = 0;
                if (fossil_media_elf_get_section_count(elf_, &count) != FMELF_OK)
                    return 0;
                return count;
            }
        
            /**
             * @brief Get the name of a section by index.
             * @param index Section index.
             * @return Section name as std::string.
             * @throw std::out_of_range if index is invalid.
             */
            std::string section_name(size_t index) const {
                if (!elf_) throw std::runtime_error("No ELF loaded");
                const char* name = nullptr;
                if (fossil_media_elf_get_section_name(elf_, index, &name) != FMELF_OK)
                    throw std::out_of_range("Invalid section index");
                return std::string(name);
            }
        
            /**
             * @brief Find a section by name.
             * @param name Section name.
             * @return Section index.
             * @throw std::runtime_error if section is not found.
             */
            size_t find_section(const std::string& name) const {
                if (!elf_) throw std::runtime_error("No ELF loaded");
                size_t idx = 0;
                int rc = fossil_media_elf_find_section_by_name(elf_, name.c_str(), &idx);
                if (rc != FMELF_OK)
                    throw std::runtime_error("Section not found: " + name);
                return idx;
            }
        
            /**
             * @brief Extract a section to a file on disk.
             * @param index Section index.
             * @param path Output file path.
             * @return true on success, false on failure.
             */
            bool extract_section_to_file(size_t index, const std::string& path) const {
                if (!elf_) return false;
                return fossil_media_elf_extract_section_to_file(elf_, index, path.c_str()) == FMELF_OK;
            }
        
            /**
             * @brief Get the underlying C handle (const).
             */
            const fossil_media_elf_t* c_struct() const noexcept { return elf_; }
        
            /**
             * @brief Get the underlying C handle (non-const).
             */
            fossil_media_elf_t* c_struct() noexcept { return elf_; }
        
        private:
            fossil_media_elf_t* elf_;
        
            void free_() noexcept {
                if (elf_) {
                    fossil_media_elf_free(elf_);
                    elf_ = nullptr;
                }
            }
        };

    } // namespace media

} // namespace fossil

#endif

#endif /* FOSSIL_MEDIA_ELF_H */
