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
    ASSUME_ITS_EQUAL_SIZE(fossil_media_json_array_size(val), 3);
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

FOSSIL_TEST_CASE(c_test_json_clone_and_equals) {
    fossil_media_json_error_t err = {0};
    const char *json = "{\"a\":1,\"b\":[true,null]}";
    fossil_media_json_value_t *val = fossil_media_json_parse(json, &err);
    ASSUME_NOT_CNULL(val);

    fossil_media_json_value_t *clone = fossil_media_json_clone(val);
    ASSUME_NOT_CNULL(clone);

    int eq = fossil_media_json_equals(val, clone);
    ASSUME_ITS_EQUAL_I32(eq, 1);

    fossil_media_json_free(val);
    fossil_media_json_free(clone);
}

FOSSIL_TEST_CASE(c_test_json_equals_not_equal) {
    fossil_media_json_error_t err = {0};
    const char *json1 = "{\"x\":42}";
    const char *json2 = "{\"x\":43}";
    fossil_media_json_value_t *val1 = fossil_media_json_parse(json1, &err);
    fossil_media_json_value_t *val2 = fossil_media_json_parse(json2, &err);
    ASSUME_NOT_CNULL(val1);
    ASSUME_NOT_CNULL(val2);

    int eq = fossil_media_json_equals(val1, val2);
    ASSUME_ITS_EQUAL_I32(eq, 0);

    fossil_media_json_free(val1);
    fossil_media_json_free(val2);
}

FOSSIL_TEST_CASE(c_test_json_new_null) {
    fossil_media_json_value_t *val = fossil_media_json_new_null();
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_json_type_name(val->type), "null");
    fossil_media_json_free(val);
}

FOSSIL_TEST_CASE(c_test_json_new_bool) {
    fossil_media_json_value_t *val_true = fossil_media_json_new_bool(1);
    fossil_media_json_value_t *val_false = fossil_media_json_new_bool(0);
    ASSUME_NOT_CNULL(val_true);
    ASSUME_NOT_CNULL(val_false);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_json_type_name(val_true->type), "bool");
    ASSUME_ITS_EQUAL_CSTR(fossil_media_json_type_name(val_false->type), "bool");
    fossil_media_json_free(val_true);
    fossil_media_json_free(val_false);
}

FOSSIL_TEST_CASE(c_test_json_new_number) {
    fossil_media_json_value_t *val = fossil_media_json_new_number(3.14);
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_json_type_name(val->type), "number");
    fossil_media_json_free(val);
}

FOSSIL_TEST_CASE(c_test_json_new_string) {
    fossil_media_json_value_t *val = fossil_media_json_new_string("test");
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_json_type_name(val->type), "string");
    fossil_media_json_free(val);
}

FOSSIL_TEST_CASE(c_test_json_new_array_and_append) {
    fossil_media_json_value_t *arr = fossil_media_json_new_array();
    ASSUME_NOT_CNULL(arr);
    fossil_media_json_array_append(arr, fossil_media_json_new_number(1));
    fossil_media_json_array_append(arr, fossil_media_json_new_bool(1));
    ASSUME_ITS_EQUAL_SIZE(fossil_media_json_array_size(arr), 2);
    fossil_media_json_free(arr);
}

FOSSIL_TEST_CASE(c_test_json_new_object_and_set_get_remove) {
    fossil_media_json_value_t *obj = fossil_media_json_new_object();
    ASSUME_NOT_CNULL(obj);
    fossil_media_json_object_set(obj, "foo", fossil_media_json_new_string("bar"));
    fossil_media_json_value_t *got = fossil_media_json_object_get(obj, "foo");
    ASSUME_NOT_CNULL(got);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_json_type_name(got->type), "string");
    fossil_media_json_value_t *removed = fossil_media_json_object_remove(obj, "foo");
    ASSUME_NOT_CNULL(removed);
    fossil_media_json_free(removed);
    fossil_media_json_free(obj);
}

FOSSIL_TEST_CASE(c_test_json_array_reserve) {
    fossil_media_json_value_t *arr = fossil_media_json_new_array();
    ASSUME_NOT_CNULL(arr);
    int res = fossil_media_json_array_reserve(arr, 10);
    ASSUME_ITS_EQUAL_I32(res, 0);
    fossil_media_json_free(arr);
}

FOSSIL_TEST_CASE(c_test_json_object_reserve) {
    fossil_media_json_value_t *obj = fossil_media_json_new_object();
    ASSUME_NOT_CNULL(obj);
    int res = fossil_media_json_object_reserve(obj, 10);
    ASSUME_ITS_EQUAL_I32(res, 0);
    fossil_media_json_free(obj);
}

FOSSIL_TEST_CASE(c_test_json_new_int_and_get_int) {
    fossil_media_json_value_t *val = fossil_media_json_new_int(12345);
    ASSUME_NOT_CNULL(val);
    long long out = 0;
    int res = fossil_media_json_get_int(val, &out);
    ASSUME_ITS_EQUAL_I32(res, 0);
    ASSUME_ITS_EQUAL_I32(out, 12345);
    fossil_media_json_free(val);
}

FOSSIL_TEST_CASE(c_test_json_validate) {
    fossil_media_json_error_t err = {0};
    int valid = fossil_media_json_validate("{\"a\":1}", &err);
    ASSUME_ITS_EQUAL_I32(valid, 0);
    int invalid = fossil_media_json_validate("{a:1}", &err);
    ASSUME_ITS_EQUAL_I32(invalid, 1);
}

FOSSIL_TEST_CASE(c_test_json_get_path) {
    fossil_media_json_error_t err = {0};
    const char *json = "{\"user\":{\"name\":\"alice\",\"items\":[10,20],\"complex.key\":{\"foo\":42}}}";
    fossil_media_json_value_t *val = fossil_media_json_parse(json, &err);
    ASSUME_NOT_CNULL(val);

    // Simple dot notation
    fossil_media_json_value_t *name = fossil_media_json_get_path(val, "user.name");
    ASSUME_NOT_CNULL(name);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_json_type_name(name->type), "string");
    fossil_media_json_free(name);

    // Array index
    fossil_media_json_value_t *item1 = fossil_media_json_get_path(val, "user.items[1]");
    ASSUME_NOT_CNULL(item1);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_json_type_name(item1->type), "number");
    fossil_media_json_free(item1);

    // Quoted key with dot in key
    fossil_media_json_value_t *complex = fossil_media_json_get_path(val, "user.\"complex.key\".foo");
    ASSUME_NOT_CNULL(complex);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_json_type_name(complex->type), "number");
    fossil_media_json_free(complex);

    // Chained array indices
    const char *json2 = "{\"arr\":[[1,2],[3,4]]}";
    fossil_media_json_value_t *val2 = fossil_media_json_parse(json2, &err);
    ASSUME_NOT_CNULL(val2);
    fossil_media_json_value_t *nested = fossil_media_json_get_path(val2, "arr[1][0]");
    ASSUME_NOT_CNULL(nested);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_json_type_name(nested->type), "number");
    fossil_media_json_free(nested);
    fossil_media_json_free(val2);

    fossil_media_json_free(val);
}

FOSSIL_TEST_CASE(c_test_json_parse_empty_array) {
    fossil_media_json_error_t err = {0};
    const char *json = "[]";
    fossil_media_json_value_t *val = fossil_media_json_parse(json, &err);
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_json_type_name(val->type), "array");
    ASSUME_ITS_EQUAL_SIZE(fossil_media_json_array_size(val), 0);
    fossil_media_json_free(val);
}

FOSSIL_TEST_CASE(c_test_json_parse_empty_object) {
    fossil_media_json_error_t err = {0};
    const char *json = "{}";
    fossil_media_json_value_t *val = fossil_media_json_parse(json, &err);
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_json_type_name(val->type), "object");
    fossil_media_json_free(val);
}

FOSSIL_TEST_CASE(c_test_json_parse_invalid_trailing_comma_array) {
    fossil_media_json_error_t err = {0};
    const char *json = "[1,2,]";
    fossil_media_json_value_t *val = fossil_media_json_parse(json, &err);
    ASSUME_ITS_CNULL(val);
}

FOSSIL_TEST_CASE(c_test_json_parse_invalid_trailing_comma_object) {
    fossil_media_json_error_t err = {0};
    const char *json = "{\"a\":1,}";
    fossil_media_json_value_t *val = fossil_media_json_parse(json, &err);
    ASSUME_ITS_CNULL(val);
}

FOSSIL_TEST_CASE(c_test_json_parse_unterminated_string) {
    fossil_media_json_error_t err = {0};
    const char *json = "\"unterminated";
    fossil_media_json_value_t *val = fossil_media_json_parse(json, &err);
    ASSUME_ITS_CNULL(val);
}

FOSSIL_TEST_CASE(c_test_json_parse_large_number) {
    fossil_media_json_error_t err = {0};
    const char *json = "1e308";
    fossil_media_json_value_t *val = fossil_media_json_parse(json, &err);
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_json_type_name(val->type), "number");
    fossil_media_json_free(val);
}

FOSSIL_TEST_CASE(c_test_json_stringify_escape_chars) {
    fossil_media_json_value_t *val = fossil_media_json_new_string("line\nbreak\t\"quote\"");
    fossil_media_json_error_t err = {0};
    char *out = fossil_media_json_stringify(val, 0, &err);
    ASSUME_NOT_CNULL(out);
    ASSUME_ITS_TRUE(strstr(out, "\\n") != NULL);
    ASSUME_ITS_TRUE(strstr(out, "\\t") != NULL);
    ASSUME_ITS_TRUE(strstr(out, "\\\"") != NULL);
    fossil_media_json_free(val);
    free(out);
}

FOSSIL_TEST_CASE(c_test_json_object_remove_nonexistent) {
    fossil_media_json_value_t *obj = fossil_media_json_new_object();
    ASSUME_NOT_CNULL(obj);
    fossil_media_json_value_t *removed = fossil_media_json_object_remove(obj, "nope");
    ASSUME_ITS_CNULL(removed);
    fossil_media_json_free(obj);
}

FOSSIL_TEST_CASE(c_test_json_array_get_out_of_bounds) {
    fossil_media_json_value_t *arr = fossil_media_json_new_array();
    ASSUME_NOT_CNULL(arr);
    fossil_media_json_array_append(arr, fossil_media_json_new_number(1));
    fossil_media_json_value_t *val = fossil_media_json_array_get(arr, 5);
    ASSUME_ITS_CNULL(val);
    fossil_media_json_free(arr);
}

FOSSIL_TEST_CASE(c_test_json_object_get_nonexistent) {
    fossil_media_json_value_t *obj = fossil_media_json_new_object();
    ASSUME_NOT_CNULL(obj);
    fossil_media_json_object_set(obj, "foo", fossil_media_json_new_string("bar"));
    fossil_media_json_value_t *got = fossil_media_json_object_get(obj, "baz");
    ASSUME_ITS_CNULL(got);
    fossil_media_json_free(obj);
}

FOSSIL_TEST_CASE(c_test_json_parse_multiple_values) {
    fossil_media_json_error_t err = {0};
    const char *json = "true false";
    fossil_media_json_value_t *val = fossil_media_json_parse(json, &err);
    ASSUME_ITS_CNULL(val);
}

FOSSIL_TEST_CASE(c_test_json_parse_null_input) {
    fossil_media_json_error_t err = {0};
    fossil_media_json_value_t *val = fossil_media_json_parse(NULL, &err);
    ASSUME_ITS_CNULL(val);
}

FOSSIL_TEST_CASE(c_test_json_stringify_null_value) {
    fossil_media_json_error_t err = {0};
    char *out = fossil_media_json_stringify(NULL, 0, &err);
    ASSUME_ITS_CNULL(out);
}

FOSSIL_TEST_CASE(c_test_json_clone_null) {
    fossil_media_json_value_t *clone = fossil_media_json_clone(NULL);
    ASSUME_ITS_CNULL(clone);
}

FOSSIL_TEST_CASE(c_test_json_equals_nulls) {
    int eq = fossil_media_json_equals(NULL, NULL);
    ASSUME_ITS_EQUAL_I32(eq, -1);
}

FOSSIL_TEST_CASE(c_test_json_parse_nested_object_array) {
    fossil_media_json_error_t err = {0};
    const char *json = "{\"users\":[{\"id\":1,\"name\":\"Alice\"},{\"id\":2,\"name\":\"Bob\"}]}";
    fossil_media_json_value_t *val = fossil_media_json_parse(json, &err);
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_json_type_name(val->type), "object");
    fossil_media_json_value_t *users = fossil_media_json_object_get(val, "users");
    ASSUME_NOT_CNULL(users);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_json_type_name(users->type), "array");
    ASSUME_ITS_EQUAL_SIZE(fossil_media_json_array_size(users), 2);
    fossil_media_json_free(val);
}

FOSSIL_TEST_CASE(c_test_json_parse_deeply_nested) {
    fossil_media_json_error_t err = {0};
    const char *json = "{\"a\":{\"b\":{\"c\":{\"d\":[1,2,{\"e\":\"f\"}]}}}}";
    fossil_media_json_value_t *val = fossil_media_json_parse(json, &err);
    ASSUME_NOT_CNULL(val);

    // Test path: a.b.c.d[2].e
    fossil_media_json_value_t *e_val = fossil_media_json_get_path(val, "a.b.c.d[2].e");
    ASSUME_NOT_CNULL(e_val);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_json_type_name(e_val->type), "string");
    fossil_media_json_free(e_val);

    // Also test chained array indices: a.b.c.d[2]
    fossil_media_json_value_t *d2 = fossil_media_json_get_path(val, "a.b.c.d[2]");
    ASSUME_NOT_CNULL(d2);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_json_type_name(d2->type), "object");
    fossil_media_json_free(d2);

    // Also test invalid path: a.b.c.d[5]
    fossil_media_json_value_t *invalid = fossil_media_json_get_path(val, "a.b.c.d[5]");
    ASSUME_ITS_CNULL(invalid);

    fossil_media_json_free(val);
}

FOSSIL_TEST_CASE(c_test_json_parse_mixed_types_array) {
    fossil_media_json_error_t err = {0};
    const char *json = "[1, \"two\", null, true, {\"x\":3}]";
    fossil_media_json_value_t *val = fossil_media_json_parse(json, &err);
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_json_type_name(val->type), "array");
    ASSUME_ITS_EQUAL_SIZE(fossil_media_json_array_size(val), 5);
    fossil_media_json_free(val);
}

FOSSIL_TEST_CASE(c_test_json_parse_object_with_array_values) {
    fossil_media_json_error_t err = {0};
    const char *json = "{\"nums\":[1,2,3],\"letters\":[\"a\",\"b\",\"c\"]}";
    fossil_media_json_value_t *val = fossil_media_json_parse(json, &err);
    ASSUME_NOT_CNULL(val);
    fossil_media_json_value_t *nums = fossil_media_json_object_get(val, "nums");
    fossil_media_json_value_t *letters = fossil_media_json_object_get(val, "letters");
    ASSUME_NOT_CNULL(nums);
    ASSUME_NOT_CNULL(letters);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_json_type_name(nums->type), "array");
    ASSUME_ITS_EQUAL_CSTR(fossil_media_json_type_name(letters->type), "array");
    fossil_media_json_free(val);
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
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_clone_and_equals);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_equals_not_equal);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_new_null);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_new_bool);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_new_number);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_new_string);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_new_array_and_append);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_new_object_and_set_get_remove);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_array_reserve);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_object_reserve);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_new_int_and_get_int);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_validate);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_get_path);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_parse_empty_array);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_parse_empty_object);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_parse_invalid_trailing_comma_array);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_parse_invalid_trailing_comma_object);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_parse_unterminated_string);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_parse_large_number);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_stringify_escape_chars);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_object_remove_nonexistent);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_array_get_out_of_bounds);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_object_get_nonexistent);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_parse_multiple_values);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_parse_null_input);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_stringify_null_value);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_clone_null);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_equals_nulls);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_parse_nested_object_array);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_parse_deeply_nested);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_parse_mixed_types_array);
    FOSSIL_TEST_ADD(c_json_fixture, c_test_json_parse_object_with_array_values);

    FOSSIL_TEST_REGISTER(c_json_fixture);
} // end of tests
