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


FOSSIL_TEST_CASE(c_test_elf_load_builtin_blob) {
    fossil_media_elf_t *elf = NULL;
    int rc = fossil_media_elf_load_from_memory(
        FOSSIL_MEDIA_ELF_BUILTIN_BLOB,
        FOSSIL_MEDIA_ELF_BUILTIN_BLOB_SIZE,
        &elf);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_MEDIA_ELF_OK);
    ASSUME_NOT_CNULL(elf);

    fossil_media_elf_free(elf);
}

FOSSIL_TEST_CASE(c_test_elf_section_lookup) {
    fossil_media_elf_t *elf = NULL;
    int rc = fossil_media_elf_load_from_memory(
        FOSSIL_MEDIA_ELF_BUILTIN_BLOB,
        FOSSIL_MEDIA_ELF_BUILTIN_BLOB_SIZE,
        &elf);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_MEDIA_ELF_OK);

    const char *name = NULL;
    rc = fossil_media_elf_get_section_name(elf, 2, &name);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_MEDIA_ELF_OK);
    ASSUME_NOT_CNULL(name);
    ASSUME_ITS_EQUAL_CSTR(name, ".text");

    fossil_media_elf_free(elf);
}

FOSSIL_TEST_CASE(c_test_elf_section_data) {
    fossil_media_elf_t *elf = NULL;
    int rc = fossil_media_elf_load_from_memory(
        FOSSIL_MEDIA_ELF_BUILTIN_BLOB,
        FOSSIL_MEDIA_ELF_BUILTIN_BLOB_SIZE,
        &elf);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_MEDIA_ELF_OK);

    const void *sec_data = NULL;
    size_t sec_size = 0;
    rc = fossil_media_elf_get_section_data(elf, 2, &sec_data, &sec_size);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_MEDIA_ELF_OK);
    ASSUME_ITS_EQUAL_U32(sec_size, 1U);
    ASSUME_NOT_CNULL(sec_data);

    const uint8_t *bytes = (const uint8_t *)sec_data;
    ASSUME_ITS_EQUAL_O32(bytes[0], 0x90);  // single NOP

    fossil_media_elf_free(elf);
}

FOSSIL_TEST_CASE(c_test_elf_dump_does_not_crash) {
    fossil_media_elf_t *elf = NULL;
    int rc = fossil_media_elf_load_from_memory(
        FOSSIL_MEDIA_ELF_BUILTIN_BLOB,
        FOSSIL_MEDIA_ELF_BUILTIN_BLOB_SIZE,
        &elf);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_MEDIA_ELF_OK);

    // This should just print and not crash
    fossil_media_elf_dump(elf, stdout);

    fossil_media_elf_free(elf);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_elf_tests) {
    FOSSIL_TEST_ADD(c_elf_fixture, c_test_elf_is_elf_magic);
    FOSSIL_TEST_ADD(c_elf_fixture, c_test_elf_is_elf_non_magic);
    FOSSIL_TEST_ADD(c_elf_fixture, c_test_elf_is_elf_short_buffer);

    FOSSIL_TEST_ADD(c_elf_fixture, c_test_elf_load_builtin_blob);
    FOSSIL_TEST_ADD(c_elf_fixture, c_test_elf_section_lookup);
    FOSSIL_TEST_ADD(c_elf_fixture, c_test_elf_section_data);
    FOSSIL_TEST_ADD(c_elf_fixture, c_test_elf_dump_does_not_crash);

    FOSSIL_TEST_REGISTER(c_elf_fixture);
}
