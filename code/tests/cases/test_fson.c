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

FOSSIL_TEST_SUITE(c_fson_fixture);

FOSSIL_SETUP(c_fson_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(c_fson_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_test_fson_parse_null) {
    fossil_media_fson_error_t err = {0};
    const char *json =
        "{\n"
        "    null: null\n"
        "}";
    fossil_media_fson_value_t *val = fossil_media_fson_parse(json, &err);
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_fson_type_name(val->type), "null");
    fossil_media_fson_free(val);
}

FOSSIL_TEST_CASE(c_test_fson_parse_bool) {
    fossil_media_fson_error_t err = {0};
    const char *json_true =
        "{\n"
        "    flag: bool: true\n"
        "}";
    const char *json_false =
        "{\n"
        "    flag: bool: false\n"
        "}";
    fossil_media_fson_value_t *val_true = fossil_media_fson_parse(json_true, &err);
    fossil_media_fson_value_t *val_false = fossil_media_fson_parse(json_false, &err);
    ASSUME_NOT_CNULL(val_true);
    ASSUME_NOT_CNULL(val_false);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_fson_type_name(val_true->type), "bool");
    ASSUME_ITS_EQUAL_CSTR(fossil_media_fson_type_name(val_false->type), "bool");
    fossil_media_fson_free(val_true);
    fossil_media_fson_free(val_false);
}

FOSSIL_TEST_CASE(c_test_fson_parse_number) {
    fossil_media_fson_error_t err = {0};
    const char *json =
        "{\n"
        "    num: f64: 42.5\n"
        "}";
    fossil_media_fson_value_t *val = fossil_media_fson_parse(json, &err);
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_fson_type_name(val->type), "f64");
    fossil_media_fson_free(val);
}

FOSSIL_TEST_CASE(c_test_fson_parse_string) {
    fossil_media_fson_error_t err = {0};
    const char *json =
        "{\n"
        "    msg: cstr: \"hello\"\n"
        "}";
    fossil_media_fson_value_t *val = fossil_media_fson_parse(json, &err);
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_fson_type_name(val->type), "cstr");
    fossil_media_fson_free(val);
}

FOSSIL_TEST_CASE(c_test_fson_parse_array) {
    fossil_media_fson_error_t err = {0};
    const char *json =
        "{\n"
        "    arr: array: [\n"
        "        1: i32: 1,\n"
        "        2: i32: 2,\n"
        "        3: i32: 3\n"
        "    ]\n"
        "}";
    fossil_media_fson_value_t *val = fossil_media_fson_parse(json, &err);
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_fson_type_name(val->type), "array");
    ASSUME_ITS_EQUAL_SIZE(fossil_media_fson_array_size(val), 3);
    fossil_media_fson_free(val);
}

FOSSIL_TEST_CASE(c_test_fson_parse_object) {
    fossil_media_fson_error_t err = {0};
    const char *json =
        "{\n"
        "    obj: object: {\n"
        "        a: i32: 1,\n"
        "        b: i32: 2\n"
        "    }\n"
        "}";
    fossil_media_fson_value_t *val = fossil_media_fson_parse(json, &err);
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_fson_type_name(val->type), "object");
    fossil_media_fson_free(val);
}

FOSSIL_TEST_CASE(c_test_fson_stringify_roundtrip) {
    fossil_media_fson_error_t err = {0};
    const char *json =
        "{\n"
        "    foo: array: [\n"
        "        1: i32: 1,\n"
        "        true: bool: true,\n"
        "        null: null: null\n"
        "    ]\n"
        "}";
    char *out = fossil_media_fson_roundtrip(json, 0, &err);
    ASSUME_NOT_CNULL(out);
    free(out);
}

FOSSIL_TEST_CASE(c_test_fson_clone_and_equals) {
    fossil_media_fson_error_t err = {0};
    const char *json =
        "{\n"
        "    obj: object: {\n"
        "        a: i32: 1,\n"
        "        b: array: [\n"
        "            true: bool: true,\n"
        "            null: null: null\n"
        "        ]\n"
        "    }\n"
        "}";
    fossil_media_fson_value_t *val = fossil_media_fson_parse(json, &err);
    ASSUME_NOT_CNULL(val);

    fossil_media_fson_value_t *clone = fossil_media_fson_clone(val);
    ASSUME_NOT_CNULL(clone);

    int eq = fossil_media_fson_equals(val, clone);
    ASSUME_ITS_EQUAL_I32(eq, 1);

    fossil_media_fson_free(val);
    fossil_media_fson_free(clone);
}

FOSSIL_TEST_CASE(c_test_fson_equals_not_equal) {
    fossil_media_fson_error_t err = {0};
    const char *json1 =
        "{\n"
        "    x: i32: 42\n"
        "}";
    const char *json2 =
        "{\n"
        "    x: i32: 43\n"
        "}";
    fossil_media_fson_value_t *val1 = fossil_media_fson_parse(json1, &err);
    fossil_media_fson_value_t *val2 = fossil_media_fson_parse(json2, &err);
    ASSUME_NOT_CNULL(val1);
    ASSUME_NOT_CNULL(val2);

    int eq = fossil_media_fson_equals(val1, val2);
    ASSUME_ITS_EQUAL_I32(eq, 0);

    fossil_media_fson_free(val1);
    fossil_media_fson_free(val2);
}

FOSSIL_TEST_CASE(c_test_fson_parse_empty_array) {
    fossil_media_fson_error_t err = {0};
    const char *json =
        "{\n"
        "    arr: array: []\n"
        "}";
    fossil_media_fson_value_t *val = fossil_media_fson_parse(json, &err);
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_fson_type_name(val->type), "array");
    ASSUME_ITS_EQUAL_SIZE(fossil_media_fson_array_size(val), 0);
    fossil_media_fson_free(val);
}

FOSSIL_TEST_CASE(c_test_fson_parse_invalid_json) {
    fossil_media_fson_error_t err = {0};
    const char *json = "{ invalid json ";
    fossil_media_fson_value_t *val = fossil_media_fson_parse(json, &err);
    ASSUME_ITS_CNULL(val);
}

FOSSIL_TEST_CASE(c_test_fson_parse_large_number) {
    fossil_media_fson_error_t err = {0};
    const char *json =
        "{\n"
        "    big: i64: 9223372036854775807\n"
        "}";
    fossil_media_fson_value_t *val = fossil_media_fson_parse(json, &err);
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_fson_type_name(val->type), "i64");
    fossil_media_fson_free(val);
}

FOSSIL_TEST_CASE(c_test_fson_parse_special_char_string) {
    fossil_media_fson_error_t err = {0};
    const char *json =
        "{\n"
        "    msg: cstr: \"hello\\nworld\\t!\"\n"
        "}";
    fossil_media_fson_value_t *val = fossil_media_fson_parse(json, &err);
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_fson_type_name(val->type), "cstr");
    fossil_media_fson_free(val);
}

FOSSIL_TEST_CASE(c_test_fson_parse_enum) {
    fossil_media_fson_error_t err = {0};
    const char *json =
        "{\n"
        "    log_level: enum: \"warn\"\n"
        "}";
    fossil_media_fson_value_t *val = fossil_media_fson_parse(json, &err);
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_fson_type_name(val->type), "cstr");
    fossil_media_fson_free(val);
}

FOSSIL_TEST_CASE(c_test_fson_parse_flags) {
    fossil_media_fson_error_t err = {0};
    const char *json =
        "{\n"
        "    features: flags:u16: [ \"debug\", \"trace\" ]\n"
        "}";
    fossil_media_fson_value_t *val = fossil_media_fson_parse(json, &err);
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_fson_type_name(val->type), "array");
    fossil_media_fson_free(val);
}

FOSSIL_TEST_CASE(c_test_fson_parse_datetime) {
    fossil_media_fson_error_t err = {0};
    const char *json =
        "{\n"
        "    timestamp: datetime: \"2025-09-18T23:59:59Z\"\n"
        "}";
    fossil_media_fson_value_t *val = fossil_media_fson_parse(json, &err);
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_fson_type_name(val->type), "datetime");
    fossil_media_fson_free(val);
}

FOSSIL_TEST_CASE(c_test_fson_parse_duration) {
    fossil_media_fson_error_t err = {0};
    const char *json =
        "{\n"
        "    timeout: duration: \"5m30s\"\n"
        "}";
    fossil_media_fson_value_t *val = fossil_media_fson_parse(json, &err);
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_fson_type_name(val->type), "duration");
    fossil_media_fson_free(val);
}

FOSSIL_TEST_CASE(c_test_fson_parse_invalid_enum) {
    fossil_media_fson_error_t err = {0};
    const char *json =
        "{\n"
        "    log_level: enum: \"invalid-level\"\n"
        "}";
    fossil_media_fson_value_t *val = fossil_media_fson_parse(json, &err);
    ASSUME_ITS_CNULL(val);
    ASSUME_ITS_EQUAL_I32(err.code, FOSSIL_MEDIA_FSON_ERR_RANGE);
}

FOSSIL_TEST_CASE(c_test_fson_parse_invalid_flags_type) {
    fossil_media_fson_error_t err = {0};
    const char *json =
        "{\n"
        "    features: flags:u16: \"debug\"  // Must be an array, not a string\n"
        "}";
    fossil_media_fson_value_t *val = fossil_media_fson_parse(json, &err);
    ASSUME_ITS_CNULL(val);
    ASSUME_ITS_EQUAL_I32(err.code, FOSSIL_MEDIA_FSON_ERR_TYPE);
}

FOSSIL_TEST_CASE(c_test_fson_parse_flags_out_of_range) {
    fossil_media_fson_error_t err = {0};
    const char *json =
        "{\n"
        "    features: flags:u8: [ \"debug\", \"trace\", \"extra\", \"overflow\" ]\n"
        "}";
    fossil_media_fson_value_t *val = fossil_media_fson_parse(json, &err);
    ASSUME_ITS_CNULL(val);
    ASSUME_ITS_EQUAL_I32(err.code, FOSSIL_MEDIA_FSON_ERR_TYPE);
}

FOSSIL_TEST_CASE(c_test_fson_parse_invalid_datetime) {
    fossil_media_fson_error_t err = {0};
    const char *json =
        "{\n"
        "    timestamp: datetime: \"2025-99-99T99:99:99Z\"\n"
        "}";
    fossil_media_fson_value_t *val = fossil_media_fson_parse(json, &err);
    ASSUME_ITS_CNULL(val);
    ASSUME_ITS_EQUAL_I32(err.code, FOSSIL_MEDIA_FSON_ERR_PARSE);
}

FOSSIL_TEST_CASE(c_test_fson_parse_invalid_duration) {
    fossil_media_fson_error_t err = {0};
    const char *json =
        "{\n"
        "    timeout: duration: \"5minutes30seconds\"  // not ISO 8601 or simplified format\n"
        "}";
    fossil_media_fson_value_t *val = fossil_media_fson_parse(json, &err);
    ASSUME_ITS_CNULL(val);
    ASSUME_ITS_EQUAL_I32(err.code, FOSSIL_MEDIA_FSON_ERR_PARSE);
}

// Complex object with nested arrays and objects
FOSSIL_TEST_CASE(c_test_fson_complex_nested) {
    fossil_media_fson_error_t err = {0};
    const char *json =
        "{\n"
        "    app: object: {\n"
        "        name: cstr: \"Fossil App\",\n"
        "        version: u32: 101,\n"
        "        debug: bool: true,\n"
        "        log: object: {\n"
        "            level: enum: \"info\",\n"
        "            output: cstr: \"/tmp/fossil.log\"\n"
        "        },\n"
        "        features: array: [\n"
        "            feature1: bool: true,\n"
        "            feature2: bool: false,\n"
        "            max_threads: u8: 16\n"
        "        ]\n"
        "    },\n"
        "    crypto: object: {\n"
        "        enabled: bool: true,\n"
        "        key: hex: \"DEADBEEFCAFEBABE\"\n"
        "    },\n"
        "    build_flags: flags:u16: [ \"debug\", \"trace\" ]\n"
        "}";
    fossil_media_fson_value_t *val = fossil_media_fson_parse(json, &err);
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_fson_type_name(val->type), "object");

    // Validate nested object
    fossil_media_fson_value_t *app_obj = fossil_media_fson_object_get(val, "app");
    ASSUME_NOT_CNULL(app_obj);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_fson_type_name(app_obj->type), "object");

    // Validate nested array
    fossil_media_fson_value_t *features_arr = fossil_media_fson_object_get(app_obj, "features");
    ASSUME_NOT_CNULL(features_arr);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_fson_type_name(features_arr->type), "array");
    ASSUME_ITS_EQUAL_SIZE(fossil_media_fson_array_size(features_arr), 3);

    fossil_media_fson_free(val);
}

// Complex object with enums and flags
FOSSIL_TEST_CASE(c_test_fson_complex_flags_enum) {
    fossil_media_fson_error_t err = {0};
    const char *json =
        "{\n"
        "    build: object: {\n"
        "        type: enum: \"release\",\n"
        "        flags: flags:u16: [ \"optimize\", \"log\" ]\n"
        "    }\n"
        "}";
    fossil_media_fson_value_t *val = fossil_media_fson_parse(json, &err);
    ASSUME_NOT_CNULL(val);

    fossil_media_fson_value_t *build_obj = fossil_media_fson_object_get(val, "build");
    ASSUME_NOT_CNULL(build_obj);

    fossil_media_fson_value_t *type_val = fossil_media_fson_object_get(build_obj, "type");
    ASSUME_NOT_CNULL(type_val);
    // With this parser, enums are parsed as cstr
    ASSUME_ITS_EQUAL_CSTR(fossil_media_fson_type_name(type_val->type), "cstr");

    fossil_media_fson_value_t *flags_val = fossil_media_fson_object_get(build_obj, "flags");
    ASSUME_NOT_CNULL(flags_val);
    // With this parser, flags are parsed as array of cstr
    ASSUME_ITS_EQUAL_CSTR(fossil_media_fson_type_name(flags_val->type), "array");

    fossil_media_fson_free(val);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_fson_tests) {
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_parse_null);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_parse_bool);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_parse_number);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_parse_string);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_parse_array);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_parse_object);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_stringify_roundtrip);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_clone_and_equals);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_equals_not_equal);

    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_parse_empty_array);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_parse_invalid_json);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_parse_large_number);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_parse_special_char_string);

    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_parse_enum);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_parse_flags);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_parse_datetime);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_parse_duration);

    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_parse_invalid_enum);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_parse_invalid_flags_type);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_parse_flags_out_of_range);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_parse_invalid_datetime);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_parse_invalid_duration);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_complex_nested);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_complex_flags_enum);

    FOSSIL_TEST_REGISTER(c_fson_fixture);
} // end of tests
