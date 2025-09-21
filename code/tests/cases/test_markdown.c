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
    free(md_output);
}

FOSSIL_TEST_CASE(c_test_md_parse_headings) {
    const char *md_input = "# H1\n## H2\n### H3";
    fossil_media_md_node_t *root = fossil_media_md_parse(md_input);
    ASSUME_NOT_CNULL(root);

    char *md_output = fossil_media_md_serialize(root);
    ASSUME_NOT_CNULL(md_output);

    ASSUME_ITS_TRUE(strstr(md_output, "# H1") != NULL);
    ASSUME_ITS_TRUE(strstr(md_output, "## H2") != NULL);
    ASSUME_ITS_TRUE(strstr(md_output, "### H3") != NULL);

    fossil_media_md_free(root);
    free(md_output);
}

FOSSIL_TEST_CASE(c_test_md_parse_list_items) {
    const char *md_input = "- Item 1\n* Item 2\n+ Item 3";
    fossil_media_md_node_t *root = fossil_media_md_parse(md_input);
    ASSUME_NOT_CNULL(root);

    char *md_output = fossil_media_md_serialize(root);
    ASSUME_NOT_CNULL(md_output);

    ASSUME_ITS_TRUE(strstr(md_output, "- Item 1") != NULL);
    ASSUME_ITS_TRUE(strstr(md_output, "- Item 2") != NULL);
    ASSUME_ITS_TRUE(strstr(md_output, "- Item 3") != NULL);

    fossil_media_md_free(root);
    free(md_output);
}

FOSSIL_TEST_CASE(c_test_md_parse_code_block) {
    const char *md_input = "```c\nint main() { return 0; }\n```";
    fossil_media_md_node_t *root = fossil_media_md_parse(md_input);
    ASSUME_NOT_CNULL(root);

    char *md_output = fossil_media_md_serialize(root);
    ASSUME_NOT_CNULL(md_output);

    ASSUME_ITS_TRUE(strstr(md_output, "```c") != NULL);
    ASSUME_ITS_TRUE(strstr(md_output, "int main()") != NULL);

    fossil_media_md_free(root);
    free(md_output);
}

FOSSIL_TEST_CASE(c_test_md_parse_blockquote) {
    const char *md_input = "> This is a quote";
    fossil_media_md_node_t *root = fossil_media_md_parse(md_input);
    ASSUME_NOT_CNULL(root);

    char *md_output = fossil_media_md_serialize(root);
    ASSUME_NOT_CNULL(md_output);

    ASSUME_ITS_TRUE(strstr(md_output, "> This is a quote") != NULL);

    fossil_media_md_free(root);
    free(md_output);
}

FOSSIL_TEST_CASE(c_test_md_parse_empty_input) {
    const char *md_input = "";
    fossil_media_md_node_t *root = fossil_media_md_parse(md_input);
    ASSUME_NOT_CNULL(root);

    char *md_output = fossil_media_md_serialize(root);
    ASSUME_NOT_CNULL(md_output);

    // Should be empty or just a newline
    ASSUME_ITS_TRUE(strlen(md_output) == 0 || strcmp(md_output, "\n") == 0);

    fossil_media_md_free(root);
    free(md_output);
}

FOSSIL_TEST_CASE(c_test_md_parse_multiple_blank_lines) {
    const char *md_input = "\n\n# Heading\n\n\nText after blanks\n\n";
    fossil_media_md_node_t *root = fossil_media_md_parse(md_input);
    ASSUME_NOT_CNULL(root);

    char *md_output = fossil_media_md_serialize(root);
    ASSUME_NOT_CNULL(md_output);

    ASSUME_ITS_TRUE(strstr(md_output, "# Heading") != NULL);
    ASSUME_ITS_TRUE(strstr(md_output, "Text after blanks") != NULL);

    fossil_media_md_free(root);
    free(md_output);
}

FOSSIL_TEST_CASE(c_test_md_parse_malformed_heading) {
    const char *md_input = "##NoSpaceHeading";
    fossil_media_md_node_t *root = fossil_media_md_parse(md_input);
    ASSUME_NOT_CNULL(root);

    char *md_output = fossil_media_md_serialize(root);
    ASSUME_NOT_CNULL(md_output);

    // Should not be parsed as heading
    ASSUME_ITS_TRUE(strstr(md_output, "##NoSpaceHeading") != NULL);

    fossil_media_md_free(root);
    free(md_output);
}

FOSSIL_TEST_CASE(c_test_md_parse_nested_blockquote) {
    const char *md_input = "> Outer quote\n> > Nested quote";
    fossil_media_md_node_t *root = fossil_media_md_parse(md_input);
    ASSUME_NOT_CNULL(root);

    char *md_output = fossil_media_md_serialize(root);
    ASSUME_NOT_CNULL(md_output);

    ASSUME_ITS_TRUE(strstr(md_output, "> Outer quote") != NULL);
    ASSUME_ITS_TRUE(strstr(md_output, "> > Nested quote") != NULL);

    fossil_media_md_free(root);
    free(md_output);
}

FOSSIL_TEST_CASE(c_test_md_parse_code_block_no_language) {
    const char *md_input = "```\ncode without language\n```";
    fossil_media_md_node_t *root = fossil_media_md_parse(md_input);
    ASSUME_NOT_CNULL(root);

    char *md_output = fossil_media_md_serialize(root);
    ASSUME_NOT_CNULL(md_output);

    ASSUME_ITS_TRUE(strstr(md_output, "```\ncode without language\n```") != NULL);

    fossil_media_md_free(root);
    free(md_output);
}

FOSSIL_TEST_CASE(c_test_md_parse_unclosed_code_block) {
    const char *md_input = "```python\nprint('Hello')";
    fossil_media_md_node_t *root = fossil_media_md_parse(md_input);
    ASSUME_NOT_CNULL(root);

    char *md_output = fossil_media_md_serialize(root);
    ASSUME_NOT_CNULL(md_output);

    // Should not crash, may not contain code block
    ASSUME_ITS_TRUE(strstr(md_output, "print('Hello')") == NULL || strstr(md_output, "```python") == NULL);

    fossil_media_md_free(root);
    free(md_output);
}

FOSSIL_TEST_CASE(c_test_md_parse_long_input) {
    char md_input[2048];
    strcpy(md_input, "# Heading\n");
    for (int i = 0; i < 100; ++i) {
        strcat(md_input, "- Item\n");
    }
    fossil_media_md_node_t *root = fossil_media_md_parse(md_input);
    ASSUME_NOT_CNULL(root);

    char *md_output = fossil_media_md_serialize(root);
    ASSUME_NOT_CNULL(md_output);

    ASSUME_ITS_TRUE(strstr(md_output, "# Heading") != NULL);

    fossil_media_md_free(root);
    free(md_output);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_markdown_tests) {
    FOSSIL_TEST_ADD(c_markdown_fixture, c_test_md_parse_and_serialize);
    FOSSIL_TEST_ADD(c_markdown_fixture, c_test_md_parse_headings);
    FOSSIL_TEST_ADD(c_markdown_fixture, c_test_md_parse_list_items);
    FOSSIL_TEST_ADD(c_markdown_fixture, c_test_md_parse_code_block);
    FOSSIL_TEST_ADD(c_markdown_fixture, c_test_md_parse_blockquote);
    FOSSIL_TEST_ADD(c_markdown_fixture, c_test_md_parse_empty_input);
    FOSSIL_TEST_ADD(c_markdown_fixture, c_test_md_parse_multiple_blank_lines);
    FOSSIL_TEST_ADD(c_markdown_fixture, c_test_md_parse_malformed_heading);
    FOSSIL_TEST_ADD(c_markdown_fixture, c_test_md_parse_nested_blockquote);
    FOSSIL_TEST_ADD(c_markdown_fixture, c_test_md_parse_code_block_no_language);
    FOSSIL_TEST_ADD(c_markdown_fixture, c_test_md_parse_unclosed_code_block);
    FOSSIL_TEST_ADD(c_markdown_fixture, c_test_md_parse_long_input);

    FOSSIL_TEST_REGISTER(c_markdown_fixture);
} // end of tests
