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
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
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

using namespace fossil::media;

FOSSIL_TEST_SUITE(cpp_elf_fixture);

FOSSIL_SETUP(cpp_elf_fixture) {
    // Setup for C++ ELF tests (no-op)
}

FOSSIL_TEARDOWN(cpp_elf_fixture) {
    // Cleanup (no-op)
}

// ------------------------------------------------------------
// Basic magic number detection
// ------------------------------------------------------------
FOSSIL_TEST_CASE(cpp_test_elf_is_elf_magic) {
    unsigned char elf_magic[4] = {0x7f, 'E', 'L', 'F'};
    ASSUME_ITS_TRUE(Elf::is_elf(elf_magic, 4));
}

FOSSIL_TEST_CASE(cpp_test_elf_is_elf_non_magic) {
    unsigned char not_elf[4] = {0x00, 0x01, 0x02, 0x03};
    ASSUME_ITS_FALSE(Elf::is_elf(not_elf, 4));
}

FOSSIL_TEST_CASE(cpp_test_elf_is_elf_short_buffer) {
    unsigned char elf_magic[2] = {0x7f, 'E'};
    ASSUME_ITS_FALSE(Elf::is_elf(elf_magic, 2));
}

FOSSIL_TEST_CASE(cpp_test_elf_load_builtin_blob) {
    Elf elf;
    ASSUME_ITS_TRUE(elf.load_memory(FOSSIL_MEDIA_ELF_BUILTIN_BLOB,
                                    FOSSIL_MEDIA_ELF_BUILTIN_BLOB_SIZE));
    ASSUME_ITS_TRUE(elf.is_loaded());
}

FOSSIL_TEST_CASE(cpp_test_elf_section_lookup) {
    Elf elf;
    ASSUME_ITS_TRUE(elf.load_memory(FOSSIL_MEDIA_ELF_BUILTIN_BLOB,
                                    FOSSIL_MEDIA_ELF_BUILTIN_BLOB_SIZE));
    ASSUME_ITS_TRUE(elf.is_loaded());
    ASSUME_ITS_TRUE(elf.section_count() >= 3);

    std::string name = elf.section_name(2);
    ASSUME_ITS_EQUAL_CSTR(name.c_str(), ".text");
}

FOSSIL_TEST_CASE(cpp_test_elf_section_data) {
    Elf elf;
    ASSUME_ITS_TRUE(elf.load_memory(FOSSIL_MEDIA_ELF_BUILTIN_BLOB,
                                    FOSSIL_MEDIA_ELF_BUILTIN_BLOB_SIZE));
    auto section = elf.section_data(2);
    ASSUME_ITS_EQUAL_U32(section.second, 1U);
    ASSUME_ITS_EQUAL_O32(section.first[0], 0x90);  // NOP
}

FOSSIL_TEST_CASE(cpp_test_elf_dump_does_not_crash) {
    Elf elf;
    ASSUME_ITS_TRUE(elf.load_memory(FOSSIL_MEDIA_ELF_BUILTIN_BLOB,
                                    FOSSIL_MEDIA_ELF_BUILTIN_BLOB_SIZE));
    // Should print to stdout safely
    elf.dump(elf.c_struct(), stdout);
}

// ------------------------------------------------------------
// Test Group Registration
// ------------------------------------------------------------
FOSSIL_TEST_GROUP(cpp_elf_tests) {
    FOSSIL_TEST_ADD(cpp_elf_fixture, cpp_test_elf_is_elf_magic);
    FOSSIL_TEST_ADD(cpp_elf_fixture, cpp_test_elf_is_elf_non_magic);
    FOSSIL_TEST_ADD(cpp_elf_fixture, cpp_test_elf_is_elf_short_buffer);

    FOSSIL_TEST_ADD(cpp_elf_fixture, cpp_test_elf_load_builtin_blob);
    FOSSIL_TEST_ADD(cpp_elf_fixture, cpp_test_elf_section_lookup);
    FOSSIL_TEST_ADD(cpp_elf_fixture, cpp_test_elf_section_data);
    FOSSIL_TEST_ADD(cpp_elf_fixture, cpp_test_elf_dump_does_not_crash);

    FOSSIL_TEST_REGISTER(cpp_elf_fixture);
}