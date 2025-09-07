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

FOSSIL_TEST_CASE(cpp_test_json_clone_deep_copy) {
    Json orig = Json::parse("{\"x\":42,\"y\":[1,2]}");
    Json copy = orig.clone();
    ASSUME_ITS_TRUE(orig.equals(copy));
    // Mutate copy, orig should not change
    Json arr = copy.get_path("y");
    arr = Json::parse("[99]");
    ASSUME_ITS_FALSE(orig.equals(copy));
}

FOSSIL_TEST_CASE(cpp_test_json_equals_true_and_false) {
    Json a = Json::parse("[1,2,3]");
    Json b = Json::parse("[1,2,3]");
    Json c = Json::parse("[1,2,4]");
    ASSUME_ITS_TRUE(a.equals(b));
    ASSUME_ITS_FALSE(a.equals(c));
}

FOSSIL_TEST_CASE(cpp_test_json_is_null) {
    Json j = Json::parse("null");
    ASSUME_ITS_TRUE(j.is_null());
    Json k = Json::parse("42");
    ASSUME_ITS_FALSE(k.is_null());
}

FOSSIL_TEST_CASE(cpp_test_json_is_array_and_object) {
    Json arr = Json::parse("[1,2]");
    Json obj = Json::parse("{\"a\":1}");
    ASSUME_ITS_TRUE(arr.is_array());
    ASSUME_ITS_FALSE(arr.is_object());
    ASSUME_ITS_TRUE(obj.is_object());
    ASSUME_ITS_FALSE(obj.is_array());
}

FOSSIL_TEST_CASE(cpp_test_json_array_reserve_and_object_reserve) {
    Json arr = Json::parse("[1]");
    arr.array_reserve(10); // Should not throw
    Json obj = Json::parse("{\"a\":1}");
    obj.object_reserve(5); // Should not throw
}

FOSSIL_TEST_CASE(cpp_test_json_new_int_and_get_int) {
    Json j = Json::new_int(123456789);
    ASSUME_ITS_EQUAL(j.get_int(), 123456789);
}

FOSSIL_TEST_CASE(cpp_test_json_validate_valid_and_invalid) {
    ASSUME_ITS_TRUE(Json::validate("{\"a\":1}"));
    ASSUME_ITS_FALSE(Json::validate("{a:1}")); // Invalid JSON
}

FOSSIL_TEST_CASE(cpp_test_json_get_path_simple_and_nested) {
    Json j = Json::parse("{\"user\":{\"name\":\"Bob\"},\"items\":[{\"id\":1},{\"id\":2}]}");
    Json name = j.get_path("user.name");
    ASSUME_ITS_EQUAL_CSTR(name.stringify().c_str(), "\"Bob\"");
    Json id2 = j.get_path("items[1].id");
    ASSUME_ITS_EQUAL(id2.get_int(), 2);
}

// Note: parse_file and write_file require filesystem access, so only pseudo-tested here.
FOSSIL_TEST_CASE(cpp_test_json_parse_file_and_write_file) {
    // Setup: Write a temp file, then parse it
    const std::string fname = "test_tmp.json";
    Json j = Json::parse("{\"foo\":123}");
    j.write_file(fname, false);
    Json k = Json::parse_file(fname);
    ASSUME_ITS_TRUE(j.equals(k));
    // Cleanup: remove file (if possible)
    std::remove(fname.c_str());
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
    FOSSIL_TEST_ADD(cpp_json_fixture, cpp_test_json_clone_deep_copy);
    FOSSIL_TEST_ADD(cpp_json_fixture, cpp_test_json_equals_true_and_false);
    FOSSIL_TEST_ADD(cpp_json_fixture, cpp_test_json_is_null);
    FOSSIL_TEST_ADD(cpp_json_fixture, cpp_test_json_is_array_and_object);
    FOSSIL_TEST_ADD(cpp_json_fixture, cpp_test_json_array_reserve_and_object_reserve);
    FOSSIL_TEST_ADD(cpp_json_fixture, cpp_test_json_new_int_and_get_int);
    FOSSIL_TEST_ADD(cpp_json_fixture, cpp_test_json_validate_valid_and_invalid);
    FOSSIL_TEST_ADD(cpp_json_fixture, cpp_test_json_get_path_simple_and_nested);
    FOSSIL_TEST_ADD(cpp_json_fixture, cpp_test_json_parse_file_and_write_file);

    FOSSIL_TEST_REGISTER(cpp_json_fixture);
} // end of tests
