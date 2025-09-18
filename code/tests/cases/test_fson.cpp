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

FOSSIL_TEST_SUITE(cpp_fson_fixture);

FOSSIL_SETUP(cpp_fson_fixture) {
    // Setup for FSON tests
}

FOSSIL_TEARDOWN(cpp_fson_fixture) {
    // Teardown for FSON tests
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(cpp_test_fson_cpp_array_append_get_size) {
    using fossil::media::Fson;
    try {
        Fson arr = Fson::new_array();
        arr.array_append(Fson::new_i8(1));
        arr.array_append(Fson::new_i8(2));
        ASSUME_ITS_EQUAL_I32((int)arr.array_size(), 2);
        Fson v0 = arr.array_get(0);
        Fson v1 = arr.array_get(1);
        ASSUME_ITS_EQUAL_I32(v0.get_i8(), 1);
        ASSUME_ITS_EQUAL_I32(v1.get_i8(), 2);
    } catch (const fossil::media::FsonError& e) {
        ASSUME_ITS_TRUE(false);
    }
}

FOSSIL_TEST_CASE(cpp_test_fson_cpp_type_name_and_helpers) {
    using fossil::media::Fson;
    try {
        Fson v_null;
        Fson v_arr = Fson::new_array();
        Fson v_obj = Fson::new_object();
        ASSUME_ITS_EQUAL_I32(v_null.is_null(), 1);
        ASSUME_ITS_EQUAL_I32(v_arr.is_array(), 1);
        ASSUME_ITS_EQUAL_I32(v_obj.is_object(), 1);
    } catch (const fossil::media::FsonError& e) {
        ASSUME_ITS_TRUE(false);
    }
}

FOSSIL_TEST_CASE(cpp_test_fson_cpp_array_object_reserve) {
    using fossil::media::Fson;
    try {
        Fson arr = Fson::new_array();
        Fson obj = Fson::new_object();
        arr.array_reserve(10);
        obj.object_reserve(10);
        ASSUME_ITS_EQUAL_I32(arr.is_array(), 1);
        ASSUME_ITS_EQUAL_I32(obj.is_object(), 1);
    } catch (const fossil::media::FsonError& e) {
        ASSUME_ITS_TRUE(false);
    }
}

FOSSIL_TEST_CASE(cpp_test_fson_cpp_clone_and_equals) {
    using fossil::media::Fson;
    try {
        Fson v1 = Fson::new_i32(123);
        Fson v2 = v1.clone();
        ASSUME_ITS_TRUE(v1.equals(v2));
    } catch (const fossil::media::FsonError& e) {
        ASSUME_ITS_TRUE(false);
    }
}

FOSSIL_TEST_CASE(cpp_test_fson_cpp_number_getters) {
    using fossil::media::Fson;
    try {
        Fson vi8 = Fson::new_i8(-8);
        Fson vu16 = Fson::new_u16(65535);
        Fson vf32 = Fson::new_f32(3.14f);
        ASSUME_ITS_EQUAL_I32(vi8.get_i8(), -8);
        ASSUME_ITS_EQUAL_I32(vu16.get_u16(), 65535);
        float f32 = vf32.get_f32();
        ASSUME_ITS_TRUE(f32 > 3.13f && f32 < 3.15f);
    } catch (const fossil::media::FsonError& e) {
        ASSUME_ITS_TRUE(false);
    }
}

FOSSIL_TEST_CASE(cpp_test_fson_cpp_edge_cases) {
    using fossil::media::Fson;
    try {
        // Access out-of-bounds index in array
        Fson arr = Fson::new_array();
        arr.array_append(Fson::new_i8(42));
        bool caught = false;
        try {
            arr.array_get(10);
        } catch (const fossil::media::FsonError&) {
            caught = true;
        }
        ASSUME_ITS_TRUE(caught);

        // Access non-existent key in object
        Fson obj = Fson::new_object();
        obj.object_set("key", Fson::new_i8(7));
        caught = false;
        try {
            obj.object_get("missing");
        } catch (const fossil::media::FsonError&) {
            caught = true;
        }
        ASSUME_ITS_TRUE(caught);

        // Clone null value
        Fson v_null;
        Fson v_null_clone = v_null.clone();
        ASSUME_ITS_TRUE(v_null.equals(v_null_clone));

        // Compare different types
        Fson v_num = Fson::new_i8(1);
        Fson v_str = Fson::new_string("1");
        ASSUME_ITS_TRUE(!v_num.equals(v_str));
    } catch (const fossil::media::FsonError& e) {
        ASSUME_ITS_TRUE(false);
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_fson_tests) {
    FOSSIL_TEST_ADD(cpp_fson_fixture, cpp_test_fson_cpp_array_append_get_size);
    FOSSIL_TEST_ADD(cpp_fson_fixture, cpp_test_fson_cpp_type_name_and_helpers);
    FOSSIL_TEST_ADD(cpp_fson_fixture, cpp_test_fson_cpp_array_object_reserve);
    FOSSIL_TEST_ADD(cpp_fson_fixture, cpp_test_fson_cpp_clone_and_equals);
    FOSSIL_TEST_ADD(cpp_fson_fixture, cpp_test_fson_cpp_number_getters);
    FOSSIL_TEST_ADD(cpp_fson_fixture, cpp_test_fson_cpp_edge_cases);

    FOSSIL_TEST_REGISTER(cpp_fson_fixture);
}
