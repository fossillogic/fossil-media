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

FOSSIL_TEST_SUITE(c_toml_fixture);

FOSSIL_SETUP(c_toml_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(c_toml_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_test_parse_simple_toml) {
    const char *toml = "title = \"TOML Example\"\n[owner]\nname = \"Tom\"\n";
    fossil_media_toml_t doc;
    int rc = fossil_media_toml_parse(toml, &doc);
    ASSUME_ITS_TRUE(rc == 0);
    const char *title = fossil_media_toml_get(&doc, NULL, "title");
    ASSUME_ITS_TRUE(title != NULL);
    if (title != NULL) {
        ASSUME_ITS_TRUE(strcmp(title, "TOML Example") == 0);
    }
    const char *owner_name = fossil_media_toml_get(&doc, "owner", "name");
    ASSUME_ITS_TRUE(owner_name != NULL);
    if (owner_name != NULL) {
        ASSUME_ITS_TRUE(strcmp(owner_name, "Tom") == 0);
    }
    fossil_media_toml_free(&doc);
}

FOSSIL_TEST_CASE(c_test_parse_toml_missing_key) {
    const char *toml = "foo = \"bar\"\n";
    fossil_media_toml_t doc;
    int rc = fossil_media_toml_parse(toml, &doc);
    ASSUME_ITS_TRUE(rc == 0);
    const char *val = fossil_media_toml_get(&doc, NULL, "baz");
    ASSUME_ITS_TRUE(val == NULL);
    fossil_media_toml_free(&doc);
}

FOSSIL_TEST_CASE(c_test_parse_toml_invalid_input) {
    fossil_media_toml_t doc;
    int rc = fossil_media_toml_parse(NULL, &doc);
    ASSUME_ITS_TRUE(rc != 0);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_toml_tests) {
    FOSSIL_TEST_ADD(c_toml_fixture, c_test_parse_simple_toml);
    FOSSIL_TEST_ADD(c_toml_fixture, c_test_parse_toml_missing_key);
    FOSSIL_TEST_ADD(c_toml_fixture, c_test_parse_toml_invalid_input);

    FOSSIL_TEST_REGISTER(c_toml_fixture);
} // end of tests
