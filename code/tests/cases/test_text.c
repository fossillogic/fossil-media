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

FOSSIL_TEST_SUITE(c_text_fixture);

FOSSIL_SETUP(c_text_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(c_text_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_test_text_trim_basic) {
    char buf[] = "   hello world   ";
    char *trimmed = fossil_media_text_trim(buf);
    ASSUME_ITS_TRUE(strcmp(trimmed, "hello world") == 0);
}

FOSSIL_TEST_CASE(c_test_text_trim_all_spaces) {
    char buf[] = "     ";
    char *trimmed = fossil_media_text_trim(buf);
    ASSUME_ITS_TRUE(strcmp(trimmed, "") == 0);
}

FOSSIL_TEST_CASE(c_test_text_trim_no_spaces) {
    char buf[] = "abc";
    char *trimmed = fossil_media_text_trim(buf);
    ASSUME_ITS_TRUE(strcmp(trimmed, "abc") == 0);
}

FOSSIL_TEST_CASE(c_test_text_tolower_basic) {
    char buf[] = "HeLLo WoRLD!";
    char *lower = fossil_media_text_tolower(buf);
    ASSUME_ITS_TRUE(strcmp(lower, "hello world!") == 0);
}

FOSSIL_TEST_CASE(c_test_text_toupper_basic) {
    char buf[] = "HeLLo WoRLD!";
    char *upper = fossil_media_text_toupper(buf);
    ASSUME_ITS_TRUE(strcmp(upper, "HELLO WORLD!") == 0);
}

FOSSIL_TEST_CASE(c_test_text_replace_basic) {
    char buf[64] = "one fish two fish";
    size_t replaced = fossil_media_text_replace(buf, "fish", "cat", sizeof(buf));
    ASSUME_ITS_TRUE(replaced == 2);
    ASSUME_ITS_TRUE(strcmp(buf, "one cat two cat") == 0);
}

FOSSIL_TEST_CASE(c_test_text_replace_no_match) {
    char buf[32] = "hello world";
    size_t replaced = fossil_media_text_replace(buf, "foo", "bar", sizeof(buf));
    ASSUME_ITS_TRUE(replaced == 0);
    ASSUME_ITS_TRUE(strcmp(buf, "hello world") == 0);
}

FOSSIL_TEST_CASE(c_test_text_replace_buffer_too_small) {
    char buf[16] = "abc def abc";
    size_t replaced = fossil_media_text_replace(buf, "abc", "longerstring", sizeof(buf));
    // Should not replace due to buffer size
    ASSUME_ITS_TRUE(replaced == 0);
}

FOSSIL_TEST_CASE(c_test_text_find_basic) {
    const char *haystack = "abcdefg";
    char *found = fossil_media_text_find(haystack, "cde");
    ASSUME_ITS_TRUE(found != NULL);
    ASSUME_ITS_TRUE(strcmp(found, "cde") == 0 || strncmp(found, "cde", 3) == 0);
}

FOSSIL_TEST_CASE(c_test_text_find_not_found) {
    const char *haystack = "abcdefg";
    char *found = fossil_media_text_find(haystack, "xyz");
    ASSUME_ITS_TRUE(found == NULL);
}

FOSSIL_TEST_CASE(c_test_text_split_basic) {
    char buf[] = "a,b,c";
    char *tokens[3];
    size_t count = fossil_media_text_split(buf, ',', tokens, 3);
    ASSUME_ITS_TRUE(count == 3);
    ASSUME_ITS_TRUE(strcmp(tokens[0], "a") == 0);
    ASSUME_ITS_TRUE(strcmp(tokens[1], "b") == 0);
    ASSUME_ITS_TRUE(strcmp(tokens[2], "c") == 0);
}

FOSSIL_TEST_CASE(c_test_text_split_limit_tokens) {
    char buf[] = "a,b,c,d";
    char *tokens[2];
    size_t count = fossil_media_text_split(buf, ',', tokens, 2);
    ASSUME_ITS_TRUE(count == 2);
    ASSUME_ITS_TRUE(strcmp(tokens[0], "a") == 0);
    // The second token should contain the rest: "b"
    ASSUME_ITS_TRUE(strcmp(tokens[1], "b") == 0 || strcmp(tokens[1], "b,c,d") == 0);
}

FOSSIL_TEST_CASE(c_test_text_split_empty_string) {
    char buf[] = "";
    char *tokens[1];
    size_t count = fossil_media_text_split(buf, ',', tokens, 1);
    ASSUME_ITS_TRUE(count == 1);
    ASSUME_ITS_TRUE(strcmp(tokens[0], "") == 0);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_text_tests) {
    FOSSIL_TEST_ADD(c_text_fixture, c_test_text_trim_basic);
    FOSSIL_TEST_ADD(c_text_fixture, c_test_text_trim_all_spaces);
    FOSSIL_TEST_ADD(c_text_fixture, c_test_text_trim_no_spaces);
    FOSSIL_TEST_ADD(c_text_fixture, c_test_text_tolower_basic);
    FOSSIL_TEST_ADD(c_text_fixture, c_test_text_toupper_basic);
    FOSSIL_TEST_ADD(c_text_fixture, c_test_text_replace_basic);
    FOSSIL_TEST_ADD(c_text_fixture, c_test_text_replace_no_match);
    FOSSIL_TEST_ADD(c_text_fixture, c_test_text_replace_buffer_too_small);
    FOSSIL_TEST_ADD(c_text_fixture, c_test_text_find_basic);
    FOSSIL_TEST_ADD(c_text_fixture, c_test_text_find_not_found);
    FOSSIL_TEST_ADD(c_text_fixture, c_test_text_split_basic);
    FOSSIL_TEST_ADD(c_text_fixture, c_test_text_split_limit_tokens);
    FOSSIL_TEST_ADD(c_text_fixture, c_test_text_split_empty_string);

    FOSSIL_TEST_REGISTER(c_text_fixture);
} // end of tests
