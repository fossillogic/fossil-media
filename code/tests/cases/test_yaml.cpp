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

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_yaml_tests) {
    FOSSIL_TEST_ADD(cpp_yaml_fixture, cpp_test_yaml_construct_and_get);
    FOSSIL_TEST_ADD(cpp_yaml_fixture, cpp_test_yaml_construct_invalid);
    FOSSIL_TEST_ADD(cpp_yaml_fixture, cpp_test_yaml_move_constructor);
    FOSSIL_TEST_ADD(cpp_yaml_fixture, cpp_test_yaml_move_assignment);

    FOSSIL_TEST_REGISTER(cpp_yaml_fixture);
} // end of tests
