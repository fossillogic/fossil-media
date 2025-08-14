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
#include <fossil/pizza/framework.h>
#include "fossil/media/framework.h"


// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Utilities
// * * * * * * * * * * * * * * * * * * * * * * * *
// Setup steps for things like test fixtures and
// mock objects are set here.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_SUITE(c_markdown_fixture);

FOSSIL_SETUP(c_markdown_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(c_markdown_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_test_md_parse_and_serialize) {
    const char *md_input = "# Heading\n\nThis is a **bold** text.";
    fossil_media_md_node_t *root = fossil_media_md_parse(md_input);
    ASSUME_NOT_CNULL(root);

    char *md_output = fossil_media_md_serialize(root);
    ASSUME_NOT_CNULL(md_output);

    // Optionally, check that serialization contains expected substrings
    ASSUME_ITS_TRUE(strstr(md_output, "Heading") != NULL);
    ASSUME_ITS_TRUE(strstr(md_output, "bold") != NULL);

    fossil_media_md_free(root);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_markdown_tests) {
    FOSSIL_TEST_ADD(c_markdown_fixture, c_test_md_parse_and_serialize);

    FOSSIL_TEST_REGISTER(c_markdown_fixture);
} // end of tests
