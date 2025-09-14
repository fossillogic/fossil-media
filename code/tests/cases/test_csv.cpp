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

FOSSIL_TEST_SUITE(cpp_csv_fixture);

FOSSIL_SETUP(cpp_csv_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(cpp_csv_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

using fossil::media::Csv;

FOSSIL_TEST_CASE(cpp_test_parse_simple_csv) {
    Csv csv("a,b,c\n1,2,3\n");
    ASSUME_ITS_TRUE(csv.row_count() == 2);
    ASSUME_ITS_TRUE(csv.field_count(0) == 3);
    ASSUME_ITS_TRUE(csv.field(0, 0) == "a");
    ASSUME_ITS_TRUE(csv.field(1, 2) == "3");
}

FOSSIL_TEST_CASE(cpp_test_parse_quoted_fields) {
    Csv csv("\"a\",\"b\",\"c\"\n\"1,2\",\"3\",\"4\"\n");
    ASSUME_ITS_TRUE(csv.row_count() == 2);
    ASSUME_ITS_TRUE(csv.field(1, 0) == "1,2");
}

FOSSIL_TEST_CASE(cpp_test_parse_empty_fields) {
    Csv csv("a,,c\n,,\n");
    ASSUME_ITS_TRUE(csv.row_count() == 2);
    ASSUME_ITS_TRUE(csv.field(0, 1) == "");
    ASSUME_ITS_TRUE(csv.field(1, 2) == "");
}

FOSSIL_TEST_CASE(cpp_test_stringify_roundtrip) {
    Csv csv("a,b,\"c,d\"\n1,2,3\n");
    std::string out = csv.to_string();
    ASSUME_ITS_TRUE(out.find("c,d") != std::string::npos);
}

FOSSIL_TEST_CASE(cpp_test_append_row) {
    Csv csv("a,b\n");
    csv.append_row({"1", "2"});
    ASSUME_ITS_TRUE(csv.row_count() == 2);
    ASSUME_ITS_TRUE(csv.field(1, 1) == "2");
}

FOSSIL_TEST_CASE(cpp_test_parse_invalid_input) {
    Csv csv(std::string(), ',');
    ASSUME_ITS_TRUE(csv.row_count() == 0);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_csv_tests) {
    FOSSIL_TEST_ADD(cpp_csv_fixture, cpp_test_parse_simple_csv);
    FOSSIL_TEST_ADD(cpp_csv_fixture, cpp_test_parse_quoted_fields);
    FOSSIL_TEST_ADD(cpp_csv_fixture, cpp_test_parse_empty_fields);
    FOSSIL_TEST_ADD(cpp_csv_fixture, cpp_test_stringify_roundtrip);
    FOSSIL_TEST_ADD(cpp_csv_fixture, cpp_test_append_row);
    FOSSIL_TEST_ADD(cpp_csv_fixture, cpp_test_parse_invalid_input);

    FOSSIL_TEST_REGISTER(cpp_csv_fixture);
} // end of tests
