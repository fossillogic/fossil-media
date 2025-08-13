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

FOSSIL_TEST_SUITE(c_json_fixture);

FOSSIL_SETUP(c_json_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(c_json_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_test_json_parse_null) {
    fossil_media_json_error_t err = {0};
    const char *json = "null";
    fossil_media_json_value_t *val = fossil_media_json_parse(json, &err);
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_json_type_name(val->type), "null");
    fossil_media_json_free(val);
}

FOSSIL_TEST_CASE(c_test_json_parse_bool) {
    fossil_media_json_error_t err = {0};
    const char *json_true = "true";
    const char *json_false = "false";
    fossil_media_json_value_t *val_true = fossil_media_json_parse(json_true, &err);
    fossil_media_json_value_t *val_false = fossil_media_json_parse(json_false, &err);
    ASSUME_NOT_CNULL(val_true);
    ASSUME_NOT_CNULL(val_false);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_json_type_name(val_true->type), "bool");
    ASSUME_ITS_EQUAL_CSTR(fossil_media_json_type_name(val_false->type), "bool");
    fossil_media_json_free(val_true);
    fossil_media_json_free(val_false);
}

FOSSIL_TEST_CASE(c_test_json_parse_number) {
    fossil_media_json_error_t err = {0};
    const char *json = "42.5";
    fossil_media_json_value_t *val = fossil_media_json_parse(json, &err);
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_json_type_name(val->type), "number");
    fossil_media_json_free(val);
}

FOSSIL_TEST_CASE(c_test_json_parse_string) {
    fossil_media_json_error_t err = {0};
    const char *json = "\"hello\"";
    fossil_media_json_value_t *val = fossil_media_json_parse(json, &err);
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_json_type_name(val->type), "string");
    fossil_media_json_free(val);
}

FOSSIL_TEST_CASE(c_test_json_parse_array) {
    fossil_media_json_error_t err = {0};
    const char *json = "[1, 2, 3]";
    fossil_media_json_value_t *val = fossil_media_json_parse(json, &err);
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_json_type_name(val->type), "array");
    ASSUME_ITS_EQUAL_CSTR(fossil_media_json_array_size(val), 3);
    fossil_media_json_free(val);
}

FOSSIL_TEST_CASE(c_test_json_parse_object) {
    fossil_media_json_error_t err = {0};
    const char *json = "{\"a\":1,\"b\":2}";
    fossil_media_json_value_t *val = fossil_media_json_parse(json, &err);
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_json_type_name(val->type), "object");
    fossil_media_json_free(val);
}

FOSSIL_TEST_CASE(c_test_json_stringify_roundtrip) {
    fossil_media_json_error_t err = {0};
    const char *json = "{\"foo\":[1,true,null]}";
    char *out = fossil_media_json_roundtrip(json, 0, &err);
    ASSUME_NOT_CNULL(out);
    free(out);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_json_tests) {
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_parse_null);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_parse_bool);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_parse_number);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_parse_string);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_parse_array);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_parse_object);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_stringify_roundtrip);

    FOSSIL_TEST_REGISTER(c_json_fixture);
} // end of tests
