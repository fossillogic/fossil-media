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
    // Setup for FSON tests
}

FOSSIL_TEARDOWN(c_fson_fixture) {
    // Teardown for FSON tests
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_test_fson_parse_and_free) {
    const char *fson_text = "key:cstr : \"value\"\nnum:i32 : 42\nflag:bool : true\n";
    fossil_media_fson_error_t err;
    fossil_media_fson_value_t *v = fossil_media_fson_parse(fson_text, &err);
    ASSUME_NOT_CNULL(v);
    fossil_media_fson_free(v);
}

FOSSIL_TEST_CASE(c_test_fson_new_types_and_free) {
    fossil_media_fson_value_t *v_null = fossil_media_fson_new_null();
    fossil_media_fson_value_t *v_bool = fossil_media_fson_new_bool(1);
    fossil_media_fson_value_t *v_i32 = fossil_media_fson_new_i32(123);
    fossil_media_fson_value_t *v_str = fossil_media_fson_new_string("hello");
    fossil_media_fson_value_t *v_arr = fossil_media_fson_new_array();
    fossil_media_fson_value_t *v_obj = fossil_media_fson_new_object();
    ASSUME_NOT_CNULL(v_null);
    ASSUME_NOT_CNULL(v_bool);
    ASSUME_NOT_CNULL(v_i32);
    ASSUME_NOT_CNULL(v_str);
    ASSUME_NOT_CNULL(v_arr);
    ASSUME_NOT_CNULL(v_obj);
    fossil_media_fson_free(v_null);
    fossil_media_fson_free(v_bool);
    fossil_media_fson_free(v_i32);
    fossil_media_fson_free(v_str);
    fossil_media_fson_free(v_arr);
    fossil_media_fson_free(v_obj);
}

FOSSIL_TEST_CASE(c_test_fson_object_set_get_remove) {
    fossil_media_fson_value_t *obj = fossil_media_fson_new_object();
    fossil_media_fson_value_t *val = fossil_media_fson_new_i32(99);
    int rc = fossil_media_fson_object_set(obj, "num", val);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_MEDIA_FSON_OK);
    fossil_media_fson_value_t *got = fossil_media_fson_object_get(obj, "num");
    ASSUME_NOT_CNULL(got);
    fossil_media_fson_value_t *removed = fossil_media_fson_object_remove(obj, "num");
    ASSUME_NOT_CNULL(removed);
    fossil_media_fson_free(removed);
    fossil_media_fson_free(obj);
}

FOSSIL_TEST_CASE(c_test_fson_array_append_get_size) {
    fossil_media_fson_value_t *arr = fossil_media_fson_new_array();
    fossil_media_fson_value_t *v1 = fossil_media_fson_new_i8(1);
    fossil_media_fson_value_t *v2 = fossil_media_fson_new_i8(2);
    int rc1 = fossil_media_fson_array_append(arr, v1);
    int rc2 = fossil_media_fson_array_append(arr, v2);
    ASSUME_ITS_EQUAL_I32(rc1, FOSSIL_MEDIA_FSON_OK);
    ASSUME_ITS_EQUAL_I32(rc2, FOSSIL_MEDIA_FSON_OK);
    ASSUME_NOT_CNULL(fossil_media_fson_array_get(arr, 0));
    ASSUME_NOT_CNULL(fossil_media_fson_array_get(arr, 1));
    ASSUME_ITS_EQUAL_I32((int)fossil_media_fson_array_size(arr), 2);
    fossil_media_fson_free(arr);
}

FOSSIL_TEST_CASE(c_test_fson_stringify_roundtrip) {
    const char *fson_text = "foo:cstr : \"bar\"\nnum:i32 : 7\n";
    fossil_media_fson_error_t err;
    fossil_media_fson_value_t *v = fossil_media_fson_parse(fson_text, &err);
    ASSUME_NOT_CNULL(v);
    char *out = fossil_media_fson_stringify(v, 1, &err);
    ASSUME_NOT_CNULL(out);
    free(out);
    char *rt = fossil_media_fson_roundtrip(fson_text, 0, &err);
    ASSUME_NOT_CNULL(rt);
    free(rt);
    fossil_media_fson_free(v);
}

FOSSIL_TEST_CASE(c_test_fson_type_name_and_helpers) {
    fossil_media_fson_value_t *v_null = fossil_media_fson_new_null();
    fossil_media_fson_value_t *v_arr = fossil_media_fson_new_array();
    fossil_media_fson_value_t *v_obj = fossil_media_fson_new_object();
    ASSUME_ITS_EQUAL_I32(fossil_media_fson_is_null(v_null), 1);
    ASSUME_ITS_EQUAL_I32(fossil_media_fson_is_array(v_arr), 1);
    ASSUME_ITS_EQUAL_I32(fossil_media_fson_is_object(v_obj), 1);
    ASSUME_NOT_CNULL(fossil_media_fson_type_name(v_null->type));
    fossil_media_fson_free(v_null);
    fossil_media_fson_free(v_arr);
    fossil_media_fson_free(v_obj);
}

FOSSIL_TEST_CASE(c_test_fson_array_object_reserve) {
    fossil_media_fson_value_t *arr = fossil_media_fson_new_array();
    fossil_media_fson_value_t *obj = fossil_media_fson_new_object();
    int rc1 = fossil_media_fson_array_reserve(arr, 10);
    int rc2 = fossil_media_fson_object_reserve(obj, 10);
    ASSUME_ITS_EQUAL_I32(rc1, FOSSIL_MEDIA_FSON_OK);
    ASSUME_ITS_EQUAL_I32(rc2, FOSSIL_MEDIA_FSON_OK);
    fossil_media_fson_free(arr);
    fossil_media_fson_free(obj);
}

FOSSIL_TEST_CASE(c_test_fson_clone_and_equals) {
    fossil_media_fson_value_t *v1 = fossil_media_fson_new_i32(123);
    fossil_media_fson_value_t *v2 = fossil_media_fson_clone(v1);
    ASSUME_NOT_CNULL(v2);
    int eq = fossil_media_fson_equals(v1, v2);
    ASSUME_ITS_EQUAL_I32(eq, 1);
    fossil_media_fson_free(v1);
    fossil_media_fson_free(v2);
}

FOSSIL_TEST_CASE(c_test_fson_number_getters) {
    fossil_media_fson_value_t *vi8 = fossil_media_fson_new_i8(-8);
    fossil_media_fson_value_t *vu16 = fossil_media_fson_new_u16(65535);
    fossil_media_fson_value_t *vf32 = fossil_media_fson_new_f32(3.14f);
    int8_t i8; uint16_t u16; float f32;
    ASSUME_ITS_EQUAL_I32(fossil_media_fson_get_i8(vi8, &i8), FOSSIL_MEDIA_FSON_OK);
    ASSUME_ITS_EQUAL_I32(i8, -8);
    ASSUME_ITS_EQUAL_I32(fossil_media_fson_get_u16(vu16, &u16), FOSSIL_MEDIA_FSON_OK);
    ASSUME_ITS_EQUAL_I32(u16, 65535);
    ASSUME_ITS_EQUAL_I32(fossil_media_fson_get_f32(vf32, &f32), FOSSIL_MEDIA_FSON_OK);
    ASSUME_ITS_TRUE(f32 > 3.13f && f32 < 3.15f);
    fossil_media_fson_free(vi8);
    fossil_media_fson_free(vu16);
    fossil_media_fson_free(vf32);
}

FOSSIL_TEST_CASE(c_test_fson_debug_dump_and_validate) {
    const char *fson_text = "foo:cstr : \"bar\"\n";
    fossil_media_fson_error_t err;
    fossil_media_fson_value_t *v = fossil_media_fson_parse(fson_text, &err);
    ASSUME_NOT_CNULL(v);
    fossil_media_fson_debug_dump(v, 0);
    int rc = fossil_media_fson_validate(fson_text, &err);
    ASSUME_ITS_EQUAL_I32(rc, 0);
    fossil_media_fson_free(v);
}

FOSSIL_TEST_CASE(c_test_fson_get_path) {
    const char *fson_text =
        "user:object : {\n"
        "  name:cstr : \"Alice\"\n"
        "  age:u8 : 30\n"
        "  tags:array : [\n"
        "    tag:cstr : \"admin\"\n"
        "    tag:cstr : \"editor\"\n"
        "  ]\n"
        "}\n";
    fossil_media_fson_error_t err;
    fossil_media_fson_value_t *root = fossil_media_fson_parse(fson_text, &err);
    ASSUME_NOT_CNULL(root);
    fossil_media_fson_value_t *name = fossil_media_fson_get_path(root, "user.name");
    fossil_media_fson_value_t *tag1 = fossil_media_fson_get_path(root, "user.tags[0].tag");
    fossil_media_fson_value_t *tag2 = fossil_media_fson_get_path(root, "user.tags[1].tag");
    ASSUME_NOT_CNULL(name);
    ASSUME_NOT_CNULL(tag1);
    ASSUME_NOT_CNULL(tag2);
    fossil_media_fson_free(root);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_fson_tests) {
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_parse_and_free);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_new_types_and_free);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_object_set_get_remove);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_array_append_get_size);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_stringify_roundtrip);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_type_name_and_helpers);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_array_object_reserve);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_clone_and_equals);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_number_getters);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_debug_dump_and_validate);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_get_path);

    FOSSIL_TEST_REGISTER(c_fson_fixture);
}
