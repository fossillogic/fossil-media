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

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* Error codes (negative) */
enum {
    FOSSIL_MEDIA_ELF_OK = 0,
    FOSSIL_MEDIA_ELF_ERR_IO = -1,
    FOSSIL_MEDIA_ELF_ERR_NOMEM = -2,
    FOSSIL_MEDIA_ELF_ERR_NOT_ELF = -3,
    FOSSIL_MEDIA_ELF_ERR_UNSUPPORTED = -4,
    FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT = -5,
    FOSSIL_MEDIA_ELF_ERR_RANGE = -6,
    FOSSIL_MEDIA_ELF_ERR_NO_SECTION = -7,
    FOSSIL_MEDIA_ELF_ERR_INVALID_ARG = -8

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

/**
 * @brief Check if a buffer contains an ELF magic number.
 *
 * This function checks if the first 4 bytes of the provided buffer match the ELF magic
 * sequence (0x7f 'E' 'L' 'F'). This is a fast, low-cost check and does not validate
 * the full ELF header.
 *
 * @param buf Pointer to the buffer to check.
 * @param len Length of the buffer in bytes.
 * @return Non-zero if buffer contains ELF magic, zero otherwise.
 */
int fossil_media_elf_is_elf(const void *buf, size_t len);

/**
 * @brief Load an ELF file from disk into a fossil_media_elf_t handle.
 *
 * This function reads the specified ELF file from disk and loads its contents into
 * a fossil_media_elf_t handle. The returned handle must be released using
 * fossil_media_elf_free() when no longer needed.
 *
 * @param path Path to the ELF file to load.
 * @param out Pointer to a handle that will receive the loaded ELF object.
 * @return FOSSIL_MEDIA_ELF_OK on success, or a negative error code on failure.
 */
int fossil_media_elf_load_from_file(const char *path, fossil_media_elf_t **out);

/**
 * @brief Free a fossil_media_elf_t handle.
 *
 * This function releases all resources associated with the specified ELF handle.
 * After calling this function, the handle must not be used again.
 *
 * @param elf Pointer to the ELF handle to free.
 */
void fossil_media_elf_free(fossil_media_elf_t *elf);

/**
 * @brief Get the number of sections in an ELF file.
 *
 * This function retrieves the total number of sections present in the loaded ELF file.
 *
 * @param elf Pointer to the loaded ELF handle.
 * @param out_count Pointer to a variable that will receive the section count.
 * @return FOSSIL_MEDIA_ELF_OK on success, or a negative error code on failure.
 */
int fossil_media_elf_get_section_count(const fossil_media_elf_t *elf, size_t *out_count);

/**
 * @brief Get the section header for a specific section index.
 *
 * This function retrieves the section header information for the section at the given
 * index (0-based). The header is written to the provided output structure.
 *
 * @param elf Pointer to the loaded ELF handle.
 * @param index Index of the section (0..count-1).
 * @param out_shdr Pointer to a structure that will receive the section header.
 * @return FOSSIL_MEDIA_ELF_OK on success, or a negative error code on failure.
 */
int fossil_media_elf_get_section_header(const fossil_media_elf_t *elf, size_t index, fossil_media_elf_shdr_t *out_shdr);

/**
 * @brief Get the name of a section by index.
 *
 * This function retrieves the name of the section at the specified index. The name is
 * returned as a pointer to an internal string; do not free this pointer.
 *
 * @param elf Pointer to the loaded ELF handle.
 * @param index Index of the section (0..count-1).
 * @param out_name Pointer to a variable that will receive the section name.
 * @return FOSSIL_MEDIA_ELF_OK on success, or a negative error code on failure.
 */
int fossil_media_elf_get_section_name(const fossil_media_elf_t *elf, size_t index, const char **out_name);

/**
 * @brief Get a pointer to the section data in memory.
 *
 * This function retrieves a pointer to the contents of the section at the specified
 * index. The pointer references an internal buffer; do not free it. The length of the
 * section data is written to out_len.
 *
 * @param elf Pointer to the loaded ELF handle.
 * @param index Index of the section (0..count-1).
 * @param out_ptr Pointer to a variable that will receive the data pointer.
 * @param out_len Pointer to a variable that will receive the data length.
 * @return FOSSIL_MEDIA_ELF_OK on success, or a negative error code on failure.
 */
int fossil_media_elf_get_section_data(const fossil_media_elf_t *elf, size_t index, const uint8_t **out_ptr, size_t *out_len);

/**
 * @brief Find a section by its name.
 *
 * This function searches for a section with the specified name and returns its index
 * in out_index (0-based). If the section is not found, FOSSIL_MEDIA_ELF_ERR_NO_SECTION is returned.
 *
 * @param elf Pointer to the loaded ELF handle.
 * @param name Name of the section to find.
 * @param out_index Pointer to a variable that will receive the section index.
 * @return FOSSIL_MEDIA_ELF_OK on success, FOSSIL_MEDIA_ELF_ERR_NO_SECTION if not found, or another error code.
 */
int fossil_media_elf_find_section_by_name(const fossil_media_elf_t *elf, const char *name, size_t *out_index);

/**
 * @brief Extract the contents of a section to a file on disk.
 *
 * This function writes the contents of the specified section to a file at the given
 * path. If the file already exists, it will be overwritten.
 *
 * @param elf Pointer to the loaded ELF handle.
 * @param index Index of the section to extract.
 * @param out_path Path to the output file.
 * @return FOSSIL_MEDIA_ELF_OK on success, or a negative error code on failure.
 */
int fossil_media_elf_extract_section_to_file(const fossil_media_elf_t *elf, size_t index, const char *out_path);

/**
 * @brief Get a human-readable string for a given error code.
 *
 * @param err Error code returned by other fossil_media_elf_* functions.
 * @return Static string describing the error.
 */
const char* fossil_media_elf_strerror(int err);

/**
 * @brief Load an ELF file from a memory buffer.
 *
 * @param buf Pointer to ELF data in memory.
 * @param len Length of ELF data in bytes.
 * @param out Pointer to a handle that will receive the loaded ELF object.
 * @return FOSSIL_MEDIA_ELF_OK on success, or a negative error code on failure.
 */
int fossil_media_elf_load_from_memory(const void *buf, size_t len, fossil_media_elf_t **out);

/**
 * @brief Retrieve name and data for a section by index.
 *
 * @param elf Pointer to ELF handle.
 * @param index Section index.
 * @param out_name Pointer to receive section name (optional, may be NULL).
 * @param out_ptr Pointer to receive section data pointer (optional, may be NULL).
 * @param out_len Pointer to receive section data length (optional, may be NULL).
 * @return FOSSIL_MEDIA_ELF_OK on success or negative error code.
 */
int fossil_media_elf_get_section_info(const fossil_media_elf_t *elf,
                                      size_t index,
                                      const char **out_name,
                                      const uint8_t **out_ptr,
                                      size_t *out_len);

/**
 * @brief Dump ELF section table to a FILE* for debugging.
 *
 * @param elf ELF handle to dump
 * @param out FILE stream to print to (stdout, stderr, or file)
 */
void fossil_media_elf_dump(const fossil_media_elf_t *elf, FILE *out);

/**
 * @brief Built-in minimal ELF blob for self-testing and CI.
 *
 * This is a very small, valid ELF64 object file with:
 *  - 3 sections: NULL, .shstrtab, .text
 *  - A 1-byte .text section (NOP)
 *
 * Can be used with fossil_media_elf_load_from_memory() to verify
 * that the loader works even without disk I/O.
 */
extern const uint8_t FOSSIL_MEDIA_ELF_BUILTIN_BLOB[];
extern const size_t  FOSSIL_MEDIA_ELF_BUILTIN_BLOB_SIZE;

#ifdef __cplusplus
}
#include <string>
#include <stdexcept>
#include <utility>
#include <tuple>

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
                if (fossil_media_elf_load_from_file(path.c_str(), &elf_) != FOSSIL_MEDIA_ELF_OK) {
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
                return fossil_media_elf_load_from_file(path.c_str(), &elf_) == FOSSIL_MEDIA_ELF_OK;
            }

            /**
             * @brief Load an ELF file from a memory buffer (replaces any previously loaded ELF).
             * @param buf Pointer to ELF data in memory.
             * @param len Length of ELF data in bytes.
             * @return true on success, false on failure.
             */
            bool load_memory(const void* buf, size_t len) {
                free_();
                return fossil_media_elf_load_from_memory(buf, len, &elf_) == FOSSIL_MEDIA_ELF_OK;
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
                if (fossil_media_elf_get_section_count(elf_, &count) != FOSSIL_MEDIA_ELF_OK)
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
                if (fossil_media_elf_get_section_name(elf_, index, &name) != FOSSIL_MEDIA_ELF_OK)
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
                if (rc != FOSSIL_MEDIA_ELF_OK)
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
                return fossil_media_elf_extract_section_to_file(elf_, index, path.c_str()) == FOSSIL_MEDIA_ELF_OK;
            }

            /**
             * @brief Get the section header for a specific section index.
             * @param index Section index.
             * @return fossil_media_elf_shdr_t structure.
             * @throw std::out_of_range if index is invalid.
             */
            fossil_media_elf_shdr_t section_header(size_t index) const {
                if (!elf_) throw std::runtime_error("No ELF loaded");
                fossil_media_elf_shdr_t shdr;
                if (fossil_media_elf_get_section_header(elf_, index, &shdr) != FOSSIL_MEDIA_ELF_OK)
                    throw std::out_of_range("Invalid section index");
                return shdr;
            }

            /**
             * @brief Get a pointer to the section data in memory.
             * @param index Section index.
             * @return std::pair<const uint8_t*, size_t> (pointer and length).
             * @throw std::out_of_range if index is invalid.
             */
            std::pair<const uint8_t*, size_t> section_data(size_t index) const {
                if (!elf_) throw std::runtime_error("No ELF loaded");
                const uint8_t* ptr = nullptr;
                size_t len = 0;
                if (fossil_media_elf_get_section_data(elf_, index, &ptr, &len) != FOSSIL_MEDIA_ELF_OK)
                    throw std::out_of_range("Invalid section index");
                return std::make_pair(ptr, len);
            }

            /**
             * @brief Retrieve name and data for a section by index.
             * @param index Section index.
             * @return std::tuple<std::string, const uint8_t*, size_t>
             * @throw std::out_of_range if index is invalid.
             */
            std::tuple<std::string, const uint8_t*, size_t> section_info(size_t index) const {
                if (!elf_) throw std::runtime_error("No ELF loaded");
                const char* name = nullptr;
                const uint8_t* ptr = nullptr;
                size_t len = 0;
                if (fossil_media_elf_get_section_info(elf_, index, &name, &ptr, &len) != FOSSIL_MEDIA_ELF_OK)
                    throw std::out_of_range("Invalid section index");
                return std::make_tuple(std::string(name), ptr, len);
            }

            /**
             * @brief Check if a buffer contains an ELF magic number.
             * @param data Pointer to the buffer to check.
             * @param size Length of the buffer in bytes.
             * @return true if buffer contains ELF magic, false otherwise.
             */
            static bool is_elf(const unsigned char* data, size_t size) {
                if (size < 4) return false;
                return data[0] == 0x7f && data[1] == 'E' && data[2] == 'L' && data[3] == 'F';
            }
            
            /**
             * @brief Dump ELF section table to a FILE* for debugging.
             *
             * @param elf ELF handle to dump
             * @param out FILE stream to print to (stdout, stderr, or file)
             */
            void dump(const fossil_media_elf_t *elf, FILE *out) {
                fossil_media_elf_dump(elf, out);
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
