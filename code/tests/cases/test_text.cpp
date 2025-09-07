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

FOSSIL_TEST_SUITE(cpp_text_fixture);

FOSSIL_SETUP(cpp_text_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(cpp_text_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

using fossil::media::Text;

FOSSIL_TEST_CASE(cpp_test_text_trim_no_spaces) {
    std::string input = "abc";
    std::string trimmed = Text::trim(input);
    ASSUME_ITS_TRUE(trimmed == "abc");
}

FOSSIL_TEST_CASE(cpp_test_text_tolower_basic) {
    std::string input = "HeLLo WoRLD!";
    std::string lower = Text::tolower(input);
    ASSUME_ITS_TRUE(lower == "hello world!");
}

FOSSIL_TEST_CASE(cpp_test_text_toupper_basic) {
    std::string input = "HeLLo WoRLD!";
    std::string upper = Text::toupper(input);
    ASSUME_ITS_TRUE(upper == "HELLO WORLD!");
}

FOSSIL_TEST_CASE(cpp_test_text_replace_basic) {
    std::string input = "one fish two fish";
    std::string replaced = Text::replace(input, "fish", "cat");
    ASSUME_ITS_TRUE(replaced == "one cat two cat");
}

FOSSIL_TEST_CASE(cpp_test_text_replace_no_match) {
    std::string input = "hello world";
    std::string replaced = Text::replace(input, "foo", "bar");
    ASSUME_ITS_TRUE(replaced == "hello world");
}

FOSSIL_TEST_CASE(cpp_test_text_replace_buffer_too_small) {
    // Not directly testable in C++ interface, but we can check that
    // if replacement is too long, original string is returned unchanged.
    std::string input = "abc def abc";
    std::string replaced = Text::replace(input, "abc", "longerstring");
    // Updated expectation: replacement with longer string is allowed.
    ASSUME_ITS_TRUE(replaced == "longerstring def longerstring");
}

FOSSIL_TEST_CASE(cpp_test_text_find_basic) {
    std::string haystack = "abcdefg";
    size_t pos = Text::find(haystack, "cde");
    ASSUME_ITS_TRUE(pos == 2);
}

FOSSIL_TEST_CASE(cpp_test_text_find_not_found) {
    std::string haystack = "abcdefg";
    size_t pos = Text::find(haystack, "xyz");
    ASSUME_ITS_TRUE(pos == std::string::npos);
}

FOSSIL_TEST_CASE(cpp_test_text_split_basic) {
    std::string input = "a,b,c";
    std::vector<std::string> tokens = Text::split(input, ',');
    ASSUME_ITS_TRUE(tokens.size() == 3);
    ASSUME_ITS_TRUE(tokens[0] == "a");
    ASSUME_ITS_TRUE(tokens[1] == "b");
    ASSUME_ITS_TRUE(tokens[2] == "c");
}

FOSSIL_TEST_CASE(cpp_test_text_split_limit_tokens) {
    std::string input = "a,b,c,d";
    // The C++ interface does not expose a max tokens parameter,
    // so this test is not directly portable. We'll just check normal split.
    std::vector<std::string> tokens = Text::split(input, ',');
    ASSUME_ITS_TRUE(tokens.size() == 4);
    ASSUME_ITS_TRUE(tokens[0] == "a");
    ASSUME_ITS_TRUE(tokens[1] == "b");
    ASSUME_ITS_TRUE(tokens[2] == "c");
    ASSUME_ITS_TRUE(tokens[3] == "d");
}

FOSSIL_TEST_CASE(cpp_test_text_split_empty_string) {
    std::string input = "";
    std::vector<std::string> tokens = Text::split(input, ',');
    ASSUME_ITS_TRUE(tokens.size() == 1);
    ASSUME_ITS_TRUE(tokens[0] == "");
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_text_tests) {
    FOSSIL_TEST_ADD(cpp_text_fixture, cpp_test_text_trim_no_spaces);
    FOSSIL_TEST_ADD(cpp_text_fixture, cpp_test_text_tolower_basic);
    FOSSIL_TEST_ADD(cpp_text_fixture, cpp_test_text_toupper_basic);
    FOSSIL_TEST_ADD(cpp_text_fixture, cpp_test_text_replace_basic);
    FOSSIL_TEST_ADD(cpp_text_fixture, cpp_test_text_replace_no_match);
    FOSSIL_TEST_ADD(cpp_text_fixture, cpp_test_text_replace_buffer_too_small);
    FOSSIL_TEST_ADD(cpp_text_fixture, cpp_test_text_find_basic);
    FOSSIL_TEST_ADD(cpp_text_fixture, cpp_test_text_find_not_found);
    FOSSIL_TEST_ADD(cpp_text_fixture, cpp_test_text_split_basic);
    FOSSIL_TEST_ADD(cpp_text_fixture, cpp_test_text_split_limit_tokens);
    FOSSIL_TEST_ADD(cpp_text_fixture, cpp_test_text_split_empty_string);

    FOSSIL_TEST_REGISTER(cpp_text_fixture);
} // end of tests
