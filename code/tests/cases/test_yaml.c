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
    ASSUME_ITS_TRUE(head->next != NULL);
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
    ASSUME_ITS_TRUE(head->child != NULL);
    ASSUME_ITS_TRUE(strcmp(head->child->key, "indented") == 0);
    ASSUME_ITS_TRUE(head->child->indent > 0);
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

FOSSIL_TEST_CASE(c_test_yaml_parse_nested) {
    const char *yaml =
        "parent: root\n"
        "  child1: value1\n"
        "  child2: value2\n"
        "    grandchild: value3\n"
        "sibling: outside\n";
    fossil_media_yaml_node_t *head = fossil_media_yaml_parse(yaml);

    ASSUME_ITS_TRUE(head != NULL);
    ASSUME_ITS_TRUE(strcmp(head->key, "parent") == 0);
    ASSUME_ITS_TRUE(head->child != NULL);
    ASSUME_ITS_TRUE(strcmp(head->child->key, "child1") == 0);
    ASSUME_ITS_TRUE(strcmp(head->child->next->key, "child2") == 0);
    ASSUME_ITS_TRUE(head->child->next->child != NULL);
    ASSUME_ITS_TRUE(strcmp(head->child->next->child->key, "grandchild") == 0);
    ASSUME_ITS_TRUE(head->next != NULL);
    ASSUME_ITS_TRUE(strcmp(head->next->key, "sibling") == 0);
    fossil_media_yaml_free(head);
}

FOSSIL_TEST_CASE(c_test_yaml_parse_trailing_spaces) {
    const char *yaml = "key1: value1   \n   key2:    value2\n";
    fossil_media_yaml_node_t *head = fossil_media_yaml_parse(yaml);

    ASSUME_ITS_TRUE(head != NULL);
    ASSUME_ITS_TRUE(strcmp(head->key, "key1") == 0);
    ASSUME_ITS_TRUE(strcmp(head->value, "value1") == 0);
    ASSUME_ITS_TRUE(head->next != NULL);
    ASSUME_ITS_TRUE(strcmp(head->next->key, "key2") == 0);
    ASSUME_ITS_TRUE(strcmp(head->next->value, "value2") == 0);
    fossil_media_yaml_free(head);
}

FOSSIL_TEST_CASE(c_test_yaml_parse_multiple_indents) {
    const char *yaml =
        "a: 1\n"
        "  b: 2\n"
        "    c: 3\n"
        "      d: 4\n";
    fossil_media_yaml_node_t *head = fossil_media_yaml_parse(yaml);

    ASSUME_ITS_TRUE(head != NULL);
    ASSUME_ITS_TRUE(strcmp(head->key, "a") == 0);
    ASSUME_ITS_TRUE(head->child != NULL);
    ASSUME_ITS_TRUE(strcmp(head->child->key, "b") == 0);
    ASSUME_ITS_TRUE(head->child->child != NULL);
    ASSUME_ITS_TRUE(strcmp(head->child->child->key, "c") == 0);
    ASSUME_ITS_TRUE(head->child->child->child != NULL);
    ASSUME_ITS_TRUE(strcmp(head->child->child->child->key, "d") == 0);
    fossil_media_yaml_free(head);
}

FOSSIL_TEST_CASE(c_test_yaml_print_output) {
    const char *yaml = "x: 10\n  y: 20\n";
    fossil_media_yaml_node_t *head = fossil_media_yaml_parse(yaml);

    // Just call print to ensure no crash; output not checked here
    fossil_media_yaml_print(head);
    fossil_media_yaml_free(head);
}

FOSSIL_TEST_CASE(c_test_yaml_parse_only_spaces) {
    const char *yaml = "   \n\t\n";
    fossil_media_yaml_node_t *head = fossil_media_yaml_parse(yaml);
    ASSUME_ITS_TRUE(head == NULL);
}

FOSSIL_TEST_CASE(c_test_yaml_parse_no_colon) {
    const char *yaml = "justakey\nanotherkey\n";
    fossil_media_yaml_node_t *head = fossil_media_yaml_parse(yaml);
    ASSUME_ITS_TRUE(head == NULL);
}

FOSSIL_TEST_CASE(c_test_yaml_parse_colon_at_end) {
    const char *yaml = "key:\nother: value\n";
    fossil_media_yaml_node_t *head = fossil_media_yaml_parse(yaml);
    ASSUME_ITS_TRUE(head != NULL);
    ASSUME_ITS_TRUE(strcmp(head->key, "key") == 0);
    ASSUME_ITS_TRUE(strcmp(head->value, "") == 0);
    ASSUME_ITS_TRUE(head->next != NULL);
    ASSUME_ITS_TRUE(strcmp(head->next->key, "other") == 0);
    ASSUME_ITS_TRUE(strcmp(head->next->value, "value") == 0);
    fossil_media_yaml_free(head);
}

FOSSIL_TEST_CASE(c_test_yaml_parse_duplicate_keys) {
    const char *yaml = "dup: first\n  dup: second\n";
    fossil_media_yaml_node_t *head = fossil_media_yaml_parse(yaml);
    ASSUME_ITS_TRUE(head != NULL);
    ASSUME_ITS_TRUE(strcmp(head->key, "dup") == 0);
    ASSUME_ITS_TRUE(strcmp(head->value, "first") == 0);
    ASSUME_ITS_TRUE(head->child != NULL);
    ASSUME_ITS_TRUE(strcmp(head->child->key, "dup") == 0);
    ASSUME_ITS_TRUE(strcmp(head->child->value, "second") == 0);
    fossil_media_yaml_free(head);
}

FOSSIL_TEST_CASE(c_test_yaml_parse_long_key_and_value) {
    const char *yaml = "averyveryverylongkeyname: averyveryverylongvaluename\n";
    fossil_media_yaml_node_t *head = fossil_media_yaml_parse(yaml);
    ASSUME_ITS_TRUE(head != NULL);
    ASSUME_ITS_TRUE(strcmp(head->key, "averyveryverylongkeyname") == 0);
    ASSUME_ITS_TRUE(strcmp(head->value, "averyveryverylongvaluename") == 0);
    fossil_media_yaml_free(head);
}

FOSSIL_TEST_CASE(c_test_yaml_parse_tabs_and_spaces_indent) {
    const char *yaml = "main: root\n\tchild: tabbed\n  child2: spaced\n";
    fossil_media_yaml_node_t *head = fossil_media_yaml_parse(yaml);
    ASSUME_ITS_TRUE(head != NULL);
    ASSUME_ITS_TRUE(strcmp(head->key, "main") == 0);
    ASSUME_ITS_TRUE(head->child != NULL);
    ASSUME_ITS_TRUE(strcmp(head->child->key, "child") == 0);
    ASSUME_ITS_TRUE(head->child->next != NULL);
    ASSUME_ITS_TRUE(strcmp(head->child->next->key, "child2") == 0);
    fossil_media_yaml_free(head);
}

FOSSIL_TEST_CASE(c_test_yaml_get_nested_value) {
    const char *yaml =
        "root: base\n"
        "  nested: value\n"
        "    deep: deeper\n";
    fossil_media_yaml_node_t *head = fossil_media_yaml_parse(yaml);
    const char *val = fossil_media_yaml_get(head, "deep");
    ASSUME_ITS_TRUE(val != NULL);
    ASSUME_ITS_TRUE(strcmp(val, "deeper") == 0);
    fossil_media_yaml_free(head);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_yaml_tests) {
    FOSSIL_TEST_ADD(c_yaml_fixture, c_test_yaml_parse_simple);
    FOSSIL_TEST_ADD(c_yaml_fixture, c_test_yaml_parse_with_indent);
    FOSSIL_TEST_ADD(c_yaml_fixture, c_test_yaml_parse_empty_and_invalid);
    FOSSIL_TEST_ADD(c_yaml_fixture, c_test_yaml_get_value);
    FOSSIL_TEST_ADD(c_yaml_fixture, c_test_yaml_parse_nested);
    FOSSIL_TEST_ADD(c_yaml_fixture, c_test_yaml_parse_trailing_spaces);
    FOSSIL_TEST_ADD(c_yaml_fixture, c_test_yaml_parse_multiple_indents);
    FOSSIL_TEST_ADD(c_yaml_fixture, c_test_yaml_print_output);
    FOSSIL_TEST_ADD(c_yaml_fixture, c_test_yaml_parse_only_spaces);
    FOSSIL_TEST_ADD(c_yaml_fixture, c_test_yaml_parse_no_colon);
    FOSSIL_TEST_ADD(c_yaml_fixture, c_test_yaml_parse_colon_at_end);
    FOSSIL_TEST_ADD(c_yaml_fixture, c_test_yaml_parse_duplicate_keys);
    FOSSIL_TEST_ADD(c_yaml_fixture, c_test_yaml_parse_long_key_and_value);
    FOSSIL_TEST_ADD(c_yaml_fixture, c_test_yaml_parse_tabs_and_spaces_indent);
    FOSSIL_TEST_ADD(c_yaml_fixture, c_test_yaml_get_nested_value);

    FOSSIL_TEST_REGISTER(c_yaml_fixture);
} // end of tests
