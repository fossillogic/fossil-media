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

FOSSIL_TEST_SUITE(c_yaml_fixture);

FOSSIL_SETUP(c_yaml_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(c_yaml_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_test_yaml_parse_simple) {
    const char *yaml = "foo: bar\nbaz: qux\n";
    fossil_media_yaml_node_t *head = fossil_media_yaml_parse(yaml);
    ASSUME_ITS_TRUE(head != NULL);
    ASSUME_ITS_TRUE(strcmp(head->key, "foo") == 0);
    ASSUME_ITS_TRUE(strcmp(head->value, "bar") == 0);
    ASSUME_ITS_TRUE(strcmp(head->next->key, "baz") == 0);
    ASSUME_ITS_TRUE(strcmp(head->next->value, "qux") == 0);
    fossil_media_yaml_free(head);
}

FOSSIL_TEST_CASE(c_test_yaml_parse_with_indent) {
    const char *yaml = "foo: bar\n  indented: value\n";
    fossil_media_yaml_node_t *head = fossil_media_yaml_parse(yaml);
    ASSUME_ITS_TRUE(head != NULL);
    ASSUME_ITS_TRUE(strcmp(head->key, "foo") == 0);
    ASSUME_ITS_TRUE(head->indent == 0);
    ASSUME_ITS_TRUE(strcmp(head->next->key, "indented") == 0);
    ASSUME_ITS_TRUE(head->next->indent > 0);
    fossil_media_yaml_free(head);
}

FOSSIL_TEST_CASE(c_test_yaml_parse_empty_and_invalid) {
    fossil_media_yaml_node_t *head = fossil_media_yaml_parse(NULL);
    ASSUME_ITS_TRUE(head == NULL);

    head = fossil_media_yaml_parse("");
    ASSUME_ITS_TRUE(head == NULL);

    head = fossil_media_yaml_parse("no_colon_line\nfoo: bar\n");
    ASSUME_ITS_TRUE(head != NULL);
    ASSUME_ITS_TRUE(strcmp(head->key, "foo") == 0);
    fossil_media_yaml_free(head);
}

FOSSIL_TEST_CASE(c_test_yaml_get_value) {
    const char *yaml = "alpha: 1\nbeta: 2\n";
    fossil_media_yaml_node_t *head = fossil_media_yaml_parse(yaml);
    const char *val = fossil_media_yaml_get(head, "beta");
    ASSUME_ITS_TRUE(val != NULL);
    ASSUME_ITS_TRUE(strcmp(val, "2") == 0);
    val = fossil_media_yaml_get(head, "gamma");
    ASSUME_ITS_TRUE(val == NULL);
    fossil_media_yaml_free(head);
}

FOSSIL_TEST_CASE(c_test_yaml_free_null) {
    fossil_media_yaml_free(NULL); // Should not crash
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_yaml_tests) {
    FOSSIL_TEST_ADD(c_yaml_fixture, c_test_yaml_parse_simple);
    FOSSIL_TEST_ADD(c_yaml_fixture, c_test_yaml_parse_with_indent);
    FOSSIL_TEST_ADD(c_yaml_fixture, c_test_yaml_parse_empty_and_invalid);
    FOSSIL_TEST_ADD(c_yaml_fixture, c_test_yaml_get_value);
    FOSSIL_TEST_ADD(c_yaml_fixture, c_test_yaml_free_null);

    FOSSIL_TEST_REGISTER(c_yaml_fixture);
} // end of tests
