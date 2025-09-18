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

FOSSIL_TEST_CASE(c_test_fson_new_types_and_free) {
    fossil_media_fson_value_t *v_null = fossil_media_fson_new_null();
    fossil_media_fson_value_t *v_bool = fossil_media_fson_new_bool(1);
    fossil_media_fson_value_t *v_i32 = fossil_media_fson_new_i32(123);
    fossil_media_fson_value_t *v_str = fossil_media_fson_new_string("hello");
    fossil_media_fson_value_t *v_arr = fossil_media_fson_new_array();
    fossil_media_fson_value_t *v_obj = fossil_media_fson_new_object();
    fossil_media_fson_value_t *v_f64 = fossil_media_fson_new_f64(3.1415926535);
    fossil_media_fson_value_t *v_u32 = fossil_media_fson_new_u32(1);
    fossil_media_fson_value_t *v_u16 = fossil_media_fson_new_u16(1024);
    fossil_media_fson_value_t *v_f32 = fossil_media_fson_new_f32(30.0f);
    fossil_media_fson_value_t *v_char = fossil_media_fson_new_char(65);
    ASSUME_NOT_CNULL(v_null);
    ASSUME_NOT_CNULL(v_bool);
    ASSUME_NOT_CNULL(v_i32);
    ASSUME_NOT_CNULL(v_str);
    ASSUME_NOT_CNULL(v_arr);
    ASSUME_NOT_CNULL(v_obj);
    ASSUME_NOT_CNULL(v_f64);
    ASSUME_NOT_CNULL(v_u32);
    ASSUME_NOT_CNULL(v_u16);
    ASSUME_NOT_CNULL(v_f32);
    ASSUME_NOT_CNULL(v_char);
    fossil_media_fson_free(v_null);
    fossil_media_fson_free(v_bool);
    fossil_media_fson_free(v_i32);
    fossil_media_fson_free(v_str);
    fossil_media_fson_free(v_arr);
    fossil_media_fson_free(v_obj);
    fossil_media_fson_free(v_f64);
    fossil_media_fson_free(v_u32);
    fossil_media_fson_free(v_u16);
    fossil_media_fson_free(v_f32);
    fossil_media_fson_free(v_char);
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

    // Test array with objects
    fossil_media_fson_value_t *colors = fossil_media_fson_new_array();
    fossil_media_fson_array_append(colors, fossil_media_fson_new_object());
    fossil_media_fson_array_append(colors, fossil_media_fson_new_object());
    fossil_media_fson_array_append(colors, fossil_media_fson_new_object());
    ASSUME_ITS_EQUAL_I32((int)fossil_media_fson_array_size(colors), 3);

    fossil_media_fson_free(arr);
    fossil_media_fson_free(colors);
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

    // Clone complex object
    fossil_media_fson_value_t *obj = fossil_media_fson_new_object();
    fossil_media_fson_object_set(obj, "name", fossil_media_fson_new_string("Nested Object"));
    fossil_media_fson_object_set(obj, "value", fossil_media_fson_new_i32(42));
    fossil_media_fson_value_t *obj_clone = fossil_media_fson_clone(obj);
    ASSUME_NOT_CNULL(obj_clone);
    ASSUME_ITS_EQUAL_I32(fossil_media_fson_equals(obj, obj_clone), 1);

    fossil_media_fson_free(v1);
    fossil_media_fson_free(v2);
    fossil_media_fson_free(obj);
    fossil_media_fson_free(obj_clone);
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

    // Test octal, hex, bin
    fossil_media_fson_value_t *voct = fossil_media_fson_new_oct(077);
    fossil_media_fson_value_t *vhex = fossil_media_fson_new_hex(0xFF);
    fossil_media_fson_value_t *vbin = fossil_media_fson_new_bin(42);
    int32_t oct, hex, bin;
    ASSUME_ITS_EQUAL_I32(fossil_media_fson_get_oct(voct, &oct), FOSSIL_MEDIA_FSON_OK);
    ASSUME_ITS_EQUAL_I32(oct, 077);
    ASSUME_ITS_EQUAL_I32(fossil_media_fson_get_hex(vhex, &hex), FOSSIL_MEDIA_FSON_OK);
    ASSUME_ITS_EQUAL_I32(hex, 0xFF);
    ASSUME_ITS_EQUAL_I32(fossil_media_fson_get_bin(vbin, &bin), FOSSIL_MEDIA_FSON_OK);
    ASSUME_ITS_EQUAL_I32(bin, 42);

    fossil_media_fson_free(vi8);
    fossil_media_fson_free(vu16);
    fossil_media_fson_free(vf32);
    fossil_media_fson_free(voct);
    fossil_media_fson_free(vhex);
    fossil_media_fson_free(vbin);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_fson_tests) {
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_new_types_and_free);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_array_append_get_size);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_type_name_and_helpers);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_array_object_reserve);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_clone_and_equals);
    FOSSIL_TEST_ADD(c_fson_fixture, c_test_fson_number_getters);

    FOSSIL_TEST_REGISTER(c_fson_fixture);
}
