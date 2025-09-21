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

FOSSIL_TEST_SUITE(cpp_yaml_fixture);

FOSSIL_SETUP(cpp_yaml_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(cpp_yaml_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(cpp_test_yaml_construct_and_get) {
    using fossil::media::Yaml;
    const char *yaml = "foo: bar\nbaz: qux\n";
    Yaml doc(yaml);
    ASSUME_ITS_TRUE(strcmp(doc.get("foo"), "bar") == 0);
    ASSUME_ITS_TRUE(strcmp(doc.get("baz"), "qux") == 0);
    ASSUME_ITS_TRUE(doc.get("notfound") == nullptr);
}

FOSSIL_TEST_CASE(cpp_test_yaml_construct_invalid) {
    using fossil::media::Yaml;
    bool threw = false;
    try {
        Yaml doc(nullptr);
    } catch (const std::runtime_error&) {
        threw = true;
    }
    ASSUME_ITS_TRUE(threw);

    threw = false;
    try {
        Yaml doc("");
    } catch (const std::runtime_error&) {
        threw = true;
    }
    ASSUME_ITS_TRUE(threw);
}

FOSSIL_TEST_CASE(cpp_test_yaml_move_constructor) {
    using fossil::media::Yaml;
    const char *yaml = "alpha: 1\nbeta: 2\n";
    Yaml doc1(yaml);
    Yaml doc2(std::move(doc1));
    ASSUME_ITS_TRUE(strcmp(doc2.get("alpha"), "1") == 0);
    ASSUME_ITS_TRUE(doc1.get("alpha") == nullptr); // doc1 should be empty after move
}

FOSSIL_TEST_CASE(cpp_test_yaml_move_assignment) {
    using fossil::media::Yaml;
    const char *yaml1 = "x: 10\ny: 20\n";
    const char *yaml2 = "a: b\n";
    Yaml doc1(yaml1);
    Yaml doc2(yaml2);
    doc2 = std::move(doc1);
    ASSUME_ITS_TRUE(strcmp(doc2.get("x"), "10") == 0);
    ASSUME_ITS_TRUE(doc1.get("x") == nullptr); // doc1 should be empty after move
}

FOSSIL_TEST_CASE(cpp_test_yaml_parse_nested) {
    using fossil::media::Yaml;
    const char *yaml =
        "parent: root\n"
        "  child1: value1\n"
        "  child2: value2\n"
        "    grandchild: value3\n"
        "sibling: outside\n";
    Yaml doc(yaml);

    ASSUME_ITS_TRUE(strcmp(doc.get("parent"), "root") == 0);
    ASSUME_ITS_TRUE(strcmp(doc.get("child1"), "value1") == 0);
    ASSUME_ITS_TRUE(strcmp(doc.get("child2"), "value2") == 0);
    ASSUME_ITS_TRUE(strcmp(doc.get("grandchild"), "value3") == 0);
    ASSUME_ITS_TRUE(strcmp(doc.get("sibling"), "outside") == 0);
}

FOSSIL_TEST_CASE(cpp_test_yaml_parse_trailing_spaces) {
    using fossil::media::Yaml;
    const char *yaml = "key1: value1   \n   key2:    value2\n";
    Yaml doc(yaml);

    ASSUME_ITS_TRUE(strcmp(doc.get("key1"), "value1") == 0);
    ASSUME_ITS_TRUE(strcmp(doc.get("key2"), "value2") == 0);
}

FOSSIL_TEST_CASE(cpp_test_yaml_parse_multiple_indents) {
    using fossil::media::Yaml;
    const char *yaml =
        "a: 1\n"
        "  b: 2\n"
        "    c: 3\n"
        "      d: 4\n";
    Yaml doc(yaml);

    ASSUME_ITS_TRUE(strcmp(doc.get("a"), "1") == 0);
    ASSUME_ITS_TRUE(strcmp(doc.get("b"), "2") == 0);
    ASSUME_ITS_TRUE(strcmp(doc.get("c"), "3") == 0);
    ASSUME_ITS_TRUE(strcmp(doc.get("d"), "4") == 0);
}

FOSSIL_TEST_CASE(cpp_test_yaml_print_output) {
    using fossil::media::Yaml;
    const char *yaml = "x: 10\n  y: 20\n";
    Yaml doc(yaml);

    // Just call print to ensure no crash; output not checked here
    doc.print();
}

FOSSIL_TEST_CASE(cpp_test_yaml_parse_only_spaces) {
    using fossil::media::Yaml;
    const char *yaml = "   \n\t\n";
    bool threw = false;
    try {
        Yaml doc(yaml);
    } catch (const std::runtime_error&) {
        threw = true;
    }
    ASSUME_ITS_TRUE(threw);
}

FOSSIL_TEST_CASE(cpp_test_yaml_parse_no_colon) {
    using fossil::media::Yaml;
    const char *yaml = "justakey\nanotherkey\n";
    bool threw = false;
    try {
        Yaml doc(yaml);
    } catch (const std::runtime_error&) {
        threw = true;
    }
    ASSUME_ITS_TRUE(threw);
}

FOSSIL_TEST_CASE(cpp_test_yaml_parse_colon_at_end) {
    using fossil::media::Yaml;
    const char *yaml = "key:\nother: value\n";
    Yaml doc(yaml);

    ASSUME_ITS_TRUE(strcmp(doc.get("key"), "") == 0);
    ASSUME_ITS_TRUE(strcmp(doc.get("other"), "value") == 0);
}

FOSSIL_TEST_CASE(cpp_test_yaml_parse_duplicate_keys) {
    using fossil::media::Yaml;
    const char *yaml = "dup: first\n  dup: second\n";
    Yaml doc(yaml);

    // Only first match is returned by get
    ASSUME_ITS_TRUE(strcmp(doc.get("dup"), "first") == 0);
}

FOSSIL_TEST_CASE(cpp_test_yaml_parse_long_key_and_value) {
    using fossil::media::Yaml;
    const char *yaml = "averyveryverylongkeyname: averyveryverylongvaluename\n";
    Yaml doc(yaml);

    ASSUME_ITS_TRUE(strcmp(doc.get("averyveryverylongkeyname"), "averyveryverylongvaluename") == 0);
}

FOSSIL_TEST_CASE(cpp_test_yaml_parse_tabs_and_spaces_indent) {
    using fossil::media::Yaml;
    const char *yaml = "main: root\n\tchild: tabbed\n  child2: spaced\n";
    Yaml doc(yaml);

    ASSUME_ITS_TRUE(strcmp(doc.get("main"), "root") == 0);
    ASSUME_ITS_TRUE(strcmp(doc.get("child"), "tabbed") == 0);
    ASSUME_ITS_TRUE(strcmp(doc.get("child2"), "spaced") == 0);
}

FOSSIL_TEST_CASE(cpp_test_yaml_get_nested_value) {
    using fossil::media::Yaml;
    const char *yaml =
        "root: base\n"
        "  nested: value\n"
        "    deep: deeper\n";
    Yaml doc(yaml);

    ASSUME_ITS_TRUE(strcmp(doc.get("deep"), "deeper") == 0);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_yaml_tests) {
    FOSSIL_TEST_ADD(cpp_yaml_fixture, cpp_test_yaml_construct_and_get);
    FOSSIL_TEST_ADD(cpp_yaml_fixture, cpp_test_yaml_construct_invalid);
    FOSSIL_TEST_ADD(cpp_yaml_fixture, cpp_test_yaml_move_constructor);
    FOSSIL_TEST_ADD(cpp_yaml_fixture, cpp_test_yaml_move_assignment);
    FOSSIL_TEST_ADD(cpp_yaml_fixture, cpp_test_yaml_parse_nested);
    FOSSIL_TEST_ADD(cpp_yaml_fixture, cpp_test_yaml_parse_trailing_spaces);
    FOSSIL_TEST_ADD(cpp_yaml_fixture, cpp_test_yaml_parse_multiple_indents);
    FOSSIL_TEST_ADD(cpp_yaml_fixture, cpp_test_yaml_print_output);
    FOSSIL_TEST_ADD(cpp_yaml_fixture, cpp_test_yaml_parse_only_spaces);
    FOSSIL_TEST_ADD(cpp_yaml_fixture, cpp_test_yaml_parse_no_colon);
    FOSSIL_TEST_ADD(cpp_yaml_fixture, cpp_test_yaml_parse_colon_at_end);
    FOSSIL_TEST_ADD(cpp_yaml_fixture, cpp_test_yaml_parse_duplicate_keys);
    FOSSIL_TEST_ADD(cpp_yaml_fixture, cpp_test_yaml_parse_long_key_and_value);
    FOSSIL_TEST_ADD(cpp_yaml_fixture, cpp_test_yaml_parse_tabs_and_spaces_indent);
    FOSSIL_TEST_ADD(cpp_yaml_fixture, cpp_test_yaml_get_nested_value);

    FOSSIL_TEST_REGISTER(cpp_yaml_fixture);
} // end of tests
