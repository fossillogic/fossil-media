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
#include <fossil/pizza/framework.h>
#include "fossil/media/framework.h"

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Utilities
// * * * * * * * * * * * * * * * * * * * * * * * *
// Setup steps for things like test fixtures and
// mock objects are set here.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_SUITE(c_elf_fixture);

FOSSIL_SETUP(c_elf_fixture) {
    // Setup for ELF tests
}

FOSSIL_TEARDOWN(c_elf_fixture) {
    // Teardown for ELF tests
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_test_elf_is_elf_magic) {
    unsigned char elf_magic[4] = {0x7f, 'E', 'L', 'F'};
    ASSUME_ITS_TRUE(fossil_media_elf_is_elf(elf_magic, 4));
}

FOSSIL_TEST_CASE(c_test_elf_is_elf_non_magic) {
    unsigned char not_elf[4] = {0x00, 0x01, 0x02, 0x03};
    ASSUME_ITS_FALSE(fossil_media_elf_is_elf(not_elf, 4));
}

FOSSIL_TEST_CASE(c_test_elf_is_elf_short_buffer) {
    unsigned char elf_magic[2] = {0x7f, 'E'};
    ASSUME_ITS_FALSE(fossil_media_elf_is_elf(elf_magic, 2));
}

FOSSIL_TEST_CASE(c_test_elf_load_from_memory_builtin_blob) {
    fossil_media_elf_t *elf = NULL;
    int rc = fossil_media_elf_load_from_memory(
        FOSSIL_MEDIA_ELF_BUILTIN_BLOB,
        FOSSIL_MEDIA_ELF_BUILTIN_BLOB_SIZE,
        &elf
    );
    ASSUME_ITS_TRUE(rc == FOSSIL_MEDIA_ELF_OK);
    ASSUME_ITS_TRUE(elf != NULL);

    size_t section_count = 0;
    rc = fossil_media_elf_get_section_count(elf, &section_count);
    ASSUME_ITS_TRUE(rc == FOSSIL_MEDIA_ELF_OK);
    ASSUME_ITS_TRUE(section_count == 3);

    fossil_media_elf_free(elf);
}

FOSSIL_TEST_CASE(c_test_elf_get_section_name_and_data_builtin_blob) {
    fossil_media_elf_t *elf = NULL;
    int rc = fossil_media_elf_load_from_memory(
        FOSSIL_MEDIA_ELF_BUILTIN_BLOB,
        FOSSIL_MEDIA_ELF_BUILTIN_BLOB_SIZE,
        &elf
    );
    ASSUME_ITS_TRUE(rc == FOSSIL_MEDIA_ELF_OK);

    const char *name = NULL;
    rc = fossil_media_elf_get_section_name(elf, 1, &name);
    ASSUME_ITS_TRUE(rc == FOSSIL_MEDIA_ELF_OK);
    ASSUME_ITS_TRUE(name != NULL);

    const uint8_t *ptr = NULL;
    size_t len = 0;
    rc = fossil_media_elf_get_section_data(elf, 1, &ptr, &len);
    ASSUME_ITS_TRUE(rc == FOSSIL_MEDIA_ELF_OK);

    fossil_media_elf_free(elf);
}

FOSSIL_TEST_CASE(c_test_elf_find_section_by_name_builtin_blob) {
    fossil_media_elf_t *elf = NULL;
    int rc = fossil_media_elf_load_from_memory(
        FOSSIL_MEDIA_ELF_BUILTIN_BLOB,
        FOSSIL_MEDIA_ELF_BUILTIN_BLOB_SIZE,
        &elf
    );
    ASSUME_ITS_TRUE(rc == FOSSIL_MEDIA_ELF_OK);

    size_t idx = 0;
    rc = fossil_media_elf_find_section_by_name(elf, ".text", &idx);
    ASSUME_ITS_TRUE(rc == FOSSIL_MEDIA_ELF_OK);

    rc = fossil_media_elf_find_section_by_name(elf, ".shstrtab", &idx);
    ASSUME_ITS_TRUE(rc == FOSSIL_MEDIA_ELF_OK);

    fossil_media_elf_free(elf);
}

FOSSIL_TEST_CASE(c_test_elf_strerror_known_and_unknown) {
    ASSUME_ITS_TRUE(strcmp(fossil_media_elf_strerror(FOSSIL_MEDIA_ELF_OK), "OK") == 0);
    ASSUME_ITS_TRUE(strcmp(fossil_media_elf_strerror(-9999), "Unknown error") == 0);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_elf_tests) {
    FOSSIL_TEST_ADD(c_elf_fixture, c_test_elf_is_elf_magic);
    FOSSIL_TEST_ADD(c_elf_fixture, c_test_elf_is_elf_non_magic);
    FOSSIL_TEST_ADD(c_elf_fixture, c_test_elf_is_elf_short_buffer);

    FOSSIL_TEST_ADD(c_elf_fixture, c_test_elf_load_from_memory_builtin_blob);
    FOSSIL_TEST_ADD(c_elf_fixture, c_test_elf_get_section_name_and_data_builtin_blob);
    FOSSIL_TEST_ADD(c_elf_fixture, c_test_elf_find_section_by_name_builtin_blob);
    FOSSIL_TEST_ADD(c_elf_fixture, c_test_elf_strerror_known_and_unknown);

    FOSSIL_TEST_REGISTER(c_elf_fixture);
}
