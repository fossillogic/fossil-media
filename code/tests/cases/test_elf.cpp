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

FOSSIL_TEST_CASE(cpp_elf_is_elf_magic) {
    std::vector<unsigned char> elf_magic = {0x7f, 'E', 'L', 'F'};
    ASSUME_ITS_TRUE(fossil::media::Elf::is_elf(elf_magic.data(), elf_magic.size()));
}

FOSSIL_TEST_CASE(cpp_elf_is_elf_non_magic) {
    std::vector<unsigned char> not_elf = {0x00, 0x01, 0x02, 0x03};
    ASSUME_ITS_FALSE(fossil::media::Elf::is_elf(not_elf.data(), not_elf.size()));
}

FOSSIL_TEST_CASE(cpp_elf_is_elf_short_buffer) {
    std::vector<unsigned char> elf_magic = {0x7f, 'E'};
    ASSUME_ITS_FALSE(fossil::media::Elf::is_elf(elf_magic.data(), elf_magic.size()));
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_elf_tests) {
    FOSSIL_TEST_ADD(cpp_elf_fixture, cpp_elf_is_elf_magic);
    FOSSIL_TEST_ADD(cpp_elf_fixture, cpp_elf_is_elf_non_magic);
    FOSSIL_TEST_ADD(cpp_elf_fixture, cpp_elf_is_elf_short_buffer);

    FOSSIL_TEST_REGISTER(cpp_elf_fixture);
} // end of tests
