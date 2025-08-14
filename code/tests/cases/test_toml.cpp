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

FOSSIL_TEST_SUITE(cpp_toml_fixture);

FOSSIL_SETUP(cpp_toml_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(cpp_toml_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(cpp_test_parse_simple_toml) {
    using fossil::media::Toml;
    std::string toml = "title = \"TOML Example\"\n[owner]\nname = \"Tom\"\n";
    Toml doc(toml);
    std::string title = doc.get("", "title");
    ASSUME_ITS_TRUE(title == "TOML Example");
    std::string owner_name = doc.get("owner", "name");
    ASSUME_ITS_TRUE(owner_name == "Tom");
}

FOSSIL_TEST_CASE(cpp_test_parse_toml_missing_key) {
    using fossil::media::Toml;
    std::string toml = "foo = \"bar\"\n";
    Toml doc(toml);
    std::string val = doc.get("", "baz");
    ASSUME_ITS_TRUE(val.empty());
}

FOSSIL_TEST_CASE(cpp_test_parse_toml_invalid_input) {
    using fossil::media::Toml;
    bool threw = false;
    try {
        Toml doc("");
    } catch (const std::exception&) {
        threw = true;
    }
    ASSUME_ITS_TRUE(threw);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_toml_tests) {
    FOSSIL_TEST_ADD(cpp_toml_fixture, cpp_test_parse_simple_toml);
    FOSSIL_TEST_ADD(cpp_toml_fixture, cpp_test_parse_toml_missing_key);
    FOSSIL_TEST_ADD(cpp_toml_fixture, cpp_test_parse_toml_invalid_input);

    FOSSIL_TEST_REGISTER(cpp_toml_fixture);
} // end of tests
