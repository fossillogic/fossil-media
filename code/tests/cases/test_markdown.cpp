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

FOSSIL_TEST_CASE(cpp_test_md_parse_headings) {
    const std::string md_input = "# H1\n## H2\n### H3";
    fossil_media_md_node_t* root = fossil::media::Markdown::parse(md_input);
    ASSUME_NOT_CNULL(root);

    std::string md_output = fossil::media::Markdown::serialize(root);
    ASSUME_ITS_TRUE(md_output.find("# H1") != std::string::npos);
    ASSUME_ITS_TRUE(md_output.find("## H2") != std::string::npos);
    ASSUME_ITS_TRUE(md_output.find("### H3") != std::string::npos);

    fossil::media::Markdown::free(root);
}

FOSSIL_TEST_CASE(cpp_test_md_parse_list_items) {
    const std::string md_input = "- Item 1\n* Item 2\n+ Item 3";
    fossil_media_md_node_t* root = fossil::media::Markdown::parse(md_input);
    ASSUME_NOT_CNULL(root);

    std::string md_output = fossil::media::Markdown::serialize(root);
    ASSUME_ITS_TRUE(md_output.find("- Item 1") != std::string::npos);
    ASSUME_ITS_TRUE(md_output.find("- Item 2") != std::string::npos);
    ASSUME_ITS_TRUE(md_output.find("- Item 3") != std::string::npos);

    fossil::media::Markdown::free(root);
}

FOSSIL_TEST_CASE(cpp_test_md_parse_code_block) {
    const std::string md_input = "```c\nint main() { return 0; }\n```";
    fossil_media_md_node_t* root = fossil::media::Markdown::parse(md_input);
    ASSUME_NOT_CNULL(root);

    std::string md_output = fossil::media::Markdown::serialize(root);
    ASSUME_ITS_TRUE(md_output.find("```c") != std::string::npos);
    ASSUME_ITS_TRUE(md_output.find("int main()") != std::string::npos);

    fossil::media::Markdown::free(root);
}

FOSSIL_TEST_CASE(cpp_test_md_parse_blockquote) {
    const std::string md_input = "> This is a quote";
    fossil_media_md_node_t* root = fossil::media::Markdown::parse(md_input);
    ASSUME_NOT_CNULL(root);

    std::string md_output = fossil::media::Markdown::serialize(root);
    ASSUME_ITS_TRUE(md_output.find("> This is a quote") != std::string::npos);

    fossil::media::Markdown::free(root);
}

FOSSIL_TEST_CASE(cpp_test_md_parse_empty_input) {
    const std::string md_input = "";
    fossil_media_md_node_t* root = fossil::media::Markdown::parse(md_input);
    ASSUME_NOT_CNULL(root);

    std::string md_output = fossil::media::Markdown::serialize(root);
    ASSUME_ITS_TRUE(md_output.empty() || md_output == "\n");

    fossil::media::Markdown::free(root);
}

FOSSIL_TEST_CASE(cpp_test_md_parse_multiple_blank_lines) {
    const std::string md_input = "\n\n# Heading\n\n\nText after blanks\n\n";
    fossil_media_md_node_t* root = fossil::media::Markdown::parse(md_input);
    ASSUME_NOT_CNULL(root);

    std::string md_output = fossil::media::Markdown::serialize(root);
    ASSUME_ITS_TRUE(md_output.find("# Heading") != std::string::npos);
    ASSUME_ITS_TRUE(md_output.find("Text after blanks") != std::string::npos);

    fossil::media::Markdown::free(root);
}

FOSSIL_TEST_CASE(cpp_test_md_parse_malformed_heading) {
    const std::string md_input = "##NoSpaceHeading";
    fossil_media_md_node_t* root = fossil::media::Markdown::parse(md_input);
    ASSUME_NOT_CNULL(root);

    std::string md_output = fossil::media::Markdown::serialize(root);
    ASSUME_ITS_TRUE(md_output.find("##NoSpaceHeading") != std::string::npos);

    fossil::media::Markdown::free(root);
}

FOSSIL_TEST_CASE(cpp_test_md_parse_nested_blockquote) {
    const std::string md_input = "> Outer quote\n> > Nested quote";
    fossil_media_md_node_t* root = fossil::media::Markdown::parse(md_input);
    ASSUME_NOT_CNULL(root);

    std::string md_output = fossil::media::Markdown::serialize(root);
    ASSUME_ITS_TRUE(md_output.find("> Outer quote") != std::string::npos);
    ASSUME_ITS_TRUE(md_output.find("> > Nested quote") != std::string::npos);

    fossil::media::Markdown::free(root);
}

FOSSIL_TEST_CASE(cpp_test_md_parse_code_block_no_language) {
    const std::string md_input = "```\ncode without language\n```";
    fossil_media_md_node_t* root = fossil::media::Markdown::parse(md_input);
    ASSUME_NOT_CNULL(root);

    std::string md_output = fossil::media::Markdown::serialize(root);
    ASSUME_ITS_TRUE(md_output.find("```\ncode without language\n```") != std::string::npos);

    fossil::media::Markdown::free(root);
}

FOSSIL_TEST_CASE(cpp_test_md_parse_unclosed_code_block) {
    const std::string md_input = "```python\nprint('Hello')";
    fossil_media_md_node_t* root = fossil::media::Markdown::parse(md_input);
    ASSUME_NOT_CNULL(root);

    std::string md_output = fossil::media::Markdown::serialize(root);
    ASSUME_ITS_TRUE(md_output.find("print('Hello')") == std::string::npos || md_output.find("```python") == std::string::npos);

    fossil::media::Markdown::free(root);
}

FOSSIL_TEST_CASE(cpp_test_md_parse_long_input) {
    std::string md_input = "# Heading\n";
    for (int i = 0; i < 100; ++i) {
        md_input += "- Item\n";
    }
    fossil_media_md_node_t* root = fossil::media::Markdown::parse(md_input);
    ASSUME_NOT_CNULL(root);

    std::string md_output = fossil::media::Markdown::serialize(root);
    ASSUME_ITS_TRUE(md_output.find("# Heading") != std::string::npos);

    fossil::media::Markdown::free(root);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_markdown_tests) {
    FOSSIL_TEST_ADD(cpp_markdown_fixture, cpp_test_md_parse_and_serialize);
    FOSSIL_TEST_ADD(cpp_markdown_fixture, cpp_test_md_parse_headings);
    FOSSIL_TEST_ADD(cpp_markdown_fixture, cpp_test_md_parse_list_items);
    FOSSIL_TEST_ADD(cpp_markdown_fixture, cpp_test_md_parse_code_block);
    FOSSIL_TEST_ADD(cpp_markdown_fixture, cpp_test_md_parse_blockquote);
    FOSSIL_TEST_ADD(cpp_markdown_fixture, cpp_test_md_parse_empty_input);
    FOSSIL_TEST_ADD(cpp_markdown_fixture, cpp_test_md_parse_multiple_blank_lines);
    FOSSIL_TEST_ADD(cpp_markdown_fixture, cpp_test_md_parse_malformed_heading);
    FOSSIL_TEST_ADD(cpp_markdown_fixture, cpp_test_md_parse_nested_blockquote);
    FOSSIL_TEST_ADD(cpp_markdown_fixture, cpp_test_md_parse_code_block_no_language);
    FOSSIL_TEST_ADD(cpp_markdown_fixture, cpp_test_md_parse_unclosed_code_block);
    FOSSIL_TEST_ADD(cpp_markdown_fixture, cpp_test_md_parse_long_input);

    FOSSIL_TEST_REGISTER(cpp_markdown_fixture);
} // end of tests
