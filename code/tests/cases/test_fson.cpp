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

FOSSIL_TEST_CASE(cpp_test_fson_parse_and_getters) {
    using fossil::media::Fson;
    const std::string fson_text = "flag:bool : true\nnum:i32 : -42\nname:cstr : \"Bob\"\n";
    Fson v = Fson::parse(fson_text);
    Fson flag = v.get_path("flag");
    Fson num = v.get_path("num");
    Fson name = v.get_path("name");
    ASSUME_ITS_TRUE(flag.get_bool());
    ASSUME_ITS_EQUAL_I32(num.get_i32(), -42);
    ASSUME_ITS_TRUE(name.get_cstr() == "Bob");
}

FOSSIL_TEST_CASE(cpp_test_fson_new_types_and_getters) {
    using fossil::media::Fson;
    Fson v_null;
    Fson v_bool = Fson::new_bool(true);
    Fson v_i8 = Fson::new_i8(-8);
    Fson v_u16 = Fson::new_u16(65535);
    Fson v_f32 = Fson::new_f32(3.14f);
    Fson v_str = Fson::new_string("hello");
    ASSUME_ITS_TRUE(v_null.is_null());
    ASSUME_ITS_TRUE(v_bool.get_bool());
    ASSUME_ITS_EQUAL_I32(v_i8.get_i8(), -8);
    ASSUME_ITS_EQUAL_I32(v_u16.get_u16(), 65535);
    ASSUME_ITS_TRUE(v_str.get_cstr() == "hello");
    ASSUME_ITS_TRUE(v_f32.get_f32() > 3.13f && v_f32.get_f32() < 3.15f);
}

FOSSIL_TEST_CASE(cpp_test_fson_object_set_get) {
    using fossil::media::Fson;
    Fson obj = Fson::new_object();
    obj.object_set("x", Fson::new_i32(99));
    obj.object_set("y", Fson::new_string("foo"));
    Fson x = obj.object_get("x");
    Fson y = obj.object_get("y");
    ASSUME_ITS_EQUAL_I32(x.get_i32(), 99);
    ASSUME_ITS_TRUE(y.get_cstr() == "foo");
}

FOSSIL_TEST_CASE(cpp_test_fson_array_append_get_size) {
    using fossil::media::Fson;
    Fson arr = Fson::new_array();
    arr.array_append(Fson::new_i8(1));
    arr.array_append(Fson::new_i8(2));
    ASSUME_ITS_EQUAL_I32((int)arr.array_size(), 2);
    Fson v0 = arr.array_get(0);
    Fson v1 = arr.array_get(1);
    ASSUME_ITS_EQUAL_I32(v0.get_i8(), 1);
    ASSUME_ITS_EQUAL_I32(v1.get_i8(), 2);
}

FOSSIL_TEST_CASE(cpp_test_fson_stringify_roundtrip) {
    using fossil::media::Fson;
    const std::string fson_text = "foo:cstr : \"bar\"\nnum:i32 : 7\n";
    Fson v = Fson::parse(fson_text);
    std::string out = v.stringify(true);
    ASSUME_ITS_TRUE(out.find("foo") != std::string::npos);
    ASSUME_ITS_TRUE(out.find("bar") != std::string::npos);
    ASSUME_ITS_TRUE(Fson::validate(fson_text));
}

FOSSIL_TEST_CASE(cpp_test_fson_clone_and_equals) {
    using fossil::media::Fson;
    Fson v1 = Fson::new_i32(123);
    Fson v2 = v1.clone();
    ASSUME_ITS_TRUE(v1.equals(v2));
    Fson v3 = Fson::new_i32(124);
    ASSUME_ITS_TRUE(!v1.equals(v3));
}

FOSSIL_TEST_CASE(cpp_test_fson_array_object_reserve) {
    using fossil::media::Fson;
    Fson arr = Fson::new_array();
    Fson obj = Fson::new_object();
    arr.array_reserve(10);
    obj.object_reserve(10);
    arr.array_append(Fson::new_i32(1));
    obj.object_set("foo", Fson::new_string("bar"));
    ASSUME_ITS_EQUAL_I32((int)arr.array_size(), 1);
    ASSUME_ITS_TRUE(obj.object_get("foo").get_cstr() == "bar");
}

FOSSIL_TEST_CASE(cpp_test_fson_get_path) {
    using fossil::media::Fson;
    const std::string fson_text =
        "user:object : {\n"
        "  name:cstr : \"Alice\"\n"
        "  age:u8 : 30\n"
        "  tags:array : [\n"
        "    tag:cstr : \"admin\"\n"
        "    tag:cstr : \"editor\"\n"
        "  ]\n"
        "}\n";
    Fson root = Fson::parse(fson_text);
    Fson name = root.get_path("user.name");
    Fson tag1 = root.get_path("user.tags[0].tag");
    Fson tag2 = root.get_path("user.tags[1].tag");
    ASSUME_ITS_TRUE(name.get_cstr() == "Alice");
    ASSUME_ITS_TRUE(tag1.get_cstr() == "admin");
    ASSUME_ITS_TRUE(tag2.get_cstr() == "editor");
}

FOSSIL_TEST_CASE(cpp_test_fson_type_checks) {
    using fossil::media::Fson;
    Fson v_null;
    Fson v_arr = Fson::new_array();
    Fson v_obj = Fson::new_object();
    ASSUME_ITS_TRUE(v_null.is_null());
    ASSUME_ITS_TRUE(v_arr.is_array());
    ASSUME_ITS_TRUE(v_obj.is_object());
}

FOSSIL_TEST_CASE(cpp_test_fson_exceptions) {
    using fossil::media::Fson;
    bool threw = false;
    try {
        Fson arr = Fson::new_array();
        arr.array_get(99); // Out of bounds
    } catch (const fossil::media::FsonError&) {
        threw = true;
    }
    ASSUME_ITS_TRUE(threw);

    threw = false;
    try {
        Fson obj = Fson::new_object();
        obj.object_get("missing"); // Key not found
    } catch (const fossil::media::FsonError&) {
        threw = true;
    }
    ASSUME_ITS_TRUE(threw);
}

FOSSIL_TEST_CASE(cpp_test_fson_number_types) {
    using fossil::media::Fson;
    Fson vi16 = Fson::new_i16(-12345);
    Fson vu32 = Fson::new_u32(123456789);
    Fson vf64 = Fson::new_f64(2.718281828);
    Fson voct = Fson::new_oct(01234);
    Fson vhex = Fson::new_hex(0xABCD);
    Fson vbin = Fson::new_bin(0b101010);
    Fson vchar = Fson::new_char('Z');
    ASSUME_ITS_EQUAL_I32(vi16.get_i16(), -12345);
    ASSUME_ITS_EQUAL_I32((int)vu32.get_u32(), 123456789);
    ASSUME_ITS_TRUE(vf64.get_f64() > 2.71 && vf64.get_f64() < 2.72);
    ASSUME_ITS_EQUAL_I32((int)voct.get_oct(), 01234);
    ASSUME_ITS_EQUAL_I32((int)vhex.get_hex(), 0xABCD);
    ASSUME_ITS_EQUAL_I32((int)vbin.get_bin(), 42);
    ASSUME_ITS_EQUAL_I32((int)vchar.get_char(), 'Z');
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_fson_tests) {
/*
    FOSSIL_TEST_ADD(cpp_fson_fixture, cpp_test_fson_parse_and_getters);
    FOSSIL_TEST_ADD(cpp_fson_fixture, cpp_test_fson_new_types_and_getters);
    FOSSIL_TEST_ADD(cpp_fson_fixture, cpp_test_fson_object_set_get);
    FOSSIL_TEST_ADD(cpp_fson_fixture, cpp_test_fson_array_append_get_size);
    FOSSIL_TEST_ADD(cpp_fson_fixture, cpp_test_fson_stringify_roundtrip);
    FOSSIL_TEST_ADD(cpp_fson_fixture, cpp_test_fson_clone_and_equals);
    FOSSIL_TEST_ADD(cpp_fson_fixture, cpp_test_fson_array_object_reserve);
    FOSSIL_TEST_ADD(cpp_fson_fixture, cpp_test_fson_get_path);
    FOSSIL_TEST_ADD(cpp_fson_fixture, cpp_test_fson_type_checks);
    FOSSIL_TEST_ADD(cpp_fson_fixture, cpp_test_fson_exceptions);
    FOSSIL_TEST_ADD(cpp_fson_fixture, cpp_test_fson_number_types);
*/
    FOSSIL_TEST_REGISTER(cpp_fson_fixture);
}
