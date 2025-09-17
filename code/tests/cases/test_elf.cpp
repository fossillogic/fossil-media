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

FOSSIL_TEST_SUITE(cpp_elf_fixture);

FOSSIL_SETUP(cpp_elf_fixture) {
    // Setup for ELF tests
}

FOSSIL_TEARDOWN(cpp_elf_fixture) {
    // Teardown for ELF tests
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(cpp_test_elf_is_elf_magic) {
    unsigned char elf_magic[4] = {0x7f, 'E', 'L', 'F'};
    ASSUME_ITS_TRUE(fossil::media::Elf::is_elf(elf_magic, 4));
    ASSUME_ITS_TRUE(fossil_media_elf_is_elf(elf_magic, 4));
}

FOSSIL_TEST_CASE(cpp_test_elf_is_elf_non_magic) {
    unsigned char not_elf[4] = {0x00, 0x01, 0x02, 0x03};
    ASSUME_ITS_FALSE(fossil::media::Elf::is_elf(not_elf, 4));
    ASSUME_ITS_FALSE(fossil_media_elf_is_elf(not_elf, 4));
}

FOSSIL_TEST_CASE(cpp_test_elf_is_elf_short_buffer) {
    unsigned char elf_magic[2] = {0x7f, 'E'};
    ASSUME_ITS_FALSE(fossil::media::Elf::is_elf(elf_magic, 2));
    ASSUME_ITS_FALSE(fossil_media_elf_is_elf(elf_magic, 2));
}

FOSSIL_TEST_CASE(cpp_test_elf_load_from_memory_builtin_blob) {
    fossil::media::Elf elf;
    bool ok = elf.load_memory(FOSSIL_MEDIA_ELF_BUILTIN_BLOB, FOSSIL_MEDIA_ELF_BUILTIN_BLOB_SIZE);
    ASSUME_ITS_TRUE(ok);
    ASSUME_ITS_TRUE(elf.is_loaded());
    ASSUME_ITS_TRUE(elf.section_count() == 3);
}

FOSSIL_TEST_CASE(cpp_test_elf_get_section_name_and_data_builtin_blob) {
    fossil::media::Elf elf;
    bool ok = elf.load_memory(FOSSIL_MEDIA_ELF_BUILTIN_BLOB, FOSSIL_MEDIA_ELF_BUILTIN_BLOB_SIZE);
    ASSUME_ITS_TRUE(ok);

    std::string name = elf.section_name(1);
    ASSUME_ITS_TRUE(!name.empty());

    auto data = elf.section_data(1);
    ASSUME_ITS_TRUE(data.first != nullptr);
    ASSUME_ITS_TRUE(data.second > 0);
}

FOSSIL_TEST_CASE(cpp_test_elf_find_section_by_name_builtin_blob) {
    fossil::media::Elf elf;
    bool ok = elf.load_memory(FOSSIL_MEDIA_ELF_BUILTIN_BLOB, FOSSIL_MEDIA_ELF_BUILTIN_BLOB_SIZE);
    ASSUME_ITS_TRUE(ok);

    size_t idx = elf.find_section(".text");
    ASSUME_ITS_TRUE(idx != SIZE_MAX);

    idx = elf.find_section(".shstrtab");
    ASSUME_ITS_TRUE(idx != SIZE_MAX);
}

FOSSIL_TEST_CASE(cpp_test_elf_strerror_known_and_unknown) {
    ASSUME_ITS_TRUE(strcmp(fossil_media_elf_strerror(FOSSIL_MEDIA_ELF_OK), "OK") == 0);
    ASSUME_ITS_TRUE(strcmp(fossil_media_elf_strerror(-9999), "Unknown error") == 0);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_elf_tests) {
    FOSSIL_TEST_ADD(cpp_elf_fixture, cpp_test_elf_is_elf_magic);
    FOSSIL_TEST_ADD(cpp_elf_fixture, cpp_test_elf_is_elf_non_magic);
    FOSSIL_TEST_ADD(cpp_elf_fixture, cpp_test_elf_is_elf_short_buffer);

    FOSSIL_TEST_ADD(cpp_elf_fixture, cpp_test_elf_load_from_memory_builtin_blob);
    FOSSIL_TEST_ADD(cpp_elf_fixture, cpp_test_elf_get_section_name_and_data_builtin_blob);
    FOSSIL_TEST_ADD(cpp_elf_fixture, cpp_test_elf_find_section_by_name_builtin_blob);
    FOSSIL_TEST_ADD(cpp_elf_fixture, cpp_test_elf_strerror_known_and_unknown);

    FOSSIL_TEST_REGISTER(cpp_elf_fixture);
}
