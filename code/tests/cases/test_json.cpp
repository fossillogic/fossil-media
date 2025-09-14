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

FOSSIL_TEST_SUITE(cpp_json_fixture);

FOSSIL_SETUP(cpp_json_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(cpp_json_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

using fossil::media::Json;
using fossil::media::JsonError;

FOSSIL_TEST_CASE(cpp_test_json_parse_null) {
    Json j = Json::parse("null");
    ASSUME_ITS_EQUAL_CSTR(j.stringify().c_str(), "null");
}

FOSSIL_TEST_CASE(cpp_test_json_parse_bool) {
    Json jt = Json::parse("true");
    Json jf = Json::parse("false");
    ASSUME_ITS_EQUAL_CSTR(jt.stringify().c_str(), "true");
    ASSUME_ITS_EQUAL_CSTR(jf.stringify().c_str(), "false");
}

FOSSIL_TEST_CASE(cpp_test_json_parse_number) {
    Json j = Json::parse("42.5");
    ASSUME_ITS_EQUAL_CSTR(j.stringify().c_str(), "42.5");
}

FOSSIL_TEST_CASE(cpp_test_json_parse_string) {
    Json j = Json::parse("\"hello\"");
    ASSUME_ITS_EQUAL_CSTR(j.stringify().c_str(), "\"hello\"");
}

FOSSIL_TEST_CASE(cpp_test_json_parse_array) {
    Json j = Json::parse("[1, 2, 3]");
    ASSUME_ITS_EQUAL_CSTR(j.stringify().c_str(), "[1,2,3]");
}

FOSSIL_TEST_CASE(cpp_test_json_parse_object) {
    Json j = Json::parse("{\"a\":1,\"b\":2}");
    // Accept either order due to unordered object keys
    std::string s = j.stringify();
    bool ok = (s == "{\"a\":1,\"b\":2}" || s == "{\"b\":2,\"a\":1}");
    ASSUME_ITS_TRUE(ok);
}

FOSSIL_TEST_CASE(cpp_test_json_stringify_roundtrip) {
    std::string src = "{\"foo\":[1,true,null]}";
    Json j = Json::parse(src);
    std::string out = j.stringify();
    ASSUME_ITS_TRUE(out == src || out == "{\"foo\":[1,true,null]}");
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_json_tests) {    
    // C++ Wrapper Tests
    FOSSIL_TEST_ADD(cpp_json_fixture, cpp_test_json_parse_null);
    FOSSIL_TEST_ADD(cpp_json_fixture, cpp_test_json_parse_bool);
    FOSSIL_TEST_ADD(cpp_json_fixture, cpp_test_json_parse_number);
    FOSSIL_TEST_ADD(cpp_json_fixture, cpp_test_json_parse_string);
    FOSSIL_TEST_ADD(cpp_json_fixture, cpp_test_json_parse_array);
    FOSSIL_TEST_ADD(cpp_json_fixture, cpp_test_json_parse_object);
    FOSSIL_TEST_ADD(cpp_json_fixture, cpp_test_json_stringify_roundtrip);

    FOSSIL_TEST_REGISTER(cpp_json_fixture);
} // end of tests
