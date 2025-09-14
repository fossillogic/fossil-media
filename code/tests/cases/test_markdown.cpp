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

FOSSIL_TEST_SUITE(cpp_markdown_fixture);

FOSSIL_SETUP(cpp_markdown_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(cpp_markdown_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(cpp_test_md_parse_and_serialize) {
    const std::string md_input = "# Heading\n\nThis is a **bold** text.";
    fossil_media_md_node_t* root = fossil::media::Markdown::parse(md_input);
    ASSUME_NOT_CNULL(root);

    // Defensive: Check root before further use
    if (root) {
        std::string md_output = fossil::media::Markdown::serialize(root);
        ASSUME_ITS_TRUE(md_output.find("Heading") != std::string::npos);
        ASSUME_ITS_TRUE(md_output.find("bold") != std::string::npos);

        fossil::media::Markdown::free(root);
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_markdown_tests) {
    FOSSIL_TEST_ADD(cpp_markdown_fixture, cpp_test_md_parse_and_serialize);

    FOSSIL_TEST_REGISTER(cpp_markdown_fixture);
} // end of tests
