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

FOSSIL_TEST_CASE(cpp_test_fson_load_memory_and_root) {
    const char *fson_text =
        "{"
        " \"app_name\": \"Fossil Media\","
        " \"version\": 1u32,"
        " \"pi\": 3.1415926535f64,"
        " \"enabled\": true,"
        " \"max_connections\": 1024u16,"
        " \"timeout\": 30.0f32,"
        " \"initial_char\": 65c,"
        " \"null_field\": null,"
        " \"colors\": ["
        "     { \"red\": \"Red\" },"
        "     { \"green\": \"Green\" },"
        "     { \"blue\": \"Blue\" }"
        " ],"
        " \"config\": {"
        "     \"debug\": false,"
        "     \"retries\": 5u8,"
        "     \"oct_value\": 077oct,"
        "     \"hex_value\": 0xFFhex,"
        "     \"bin_value\": 101010bin,"
        "     \"nested\": {"
        "         \"name\": \"Nested Object\","
        "         \"value\": 42i32"
        "     }"
        " },"
        " \"numbers\": ["
        "     { \"one\": 1i32 },"
        "     { \"two\": 2i32 },"
        "     { \"three\": 3i32 }"
        " ],"
        " \"empty_object\": {},"
        " \"empty_array\": []"
        "}";

    fossil::media::FSON fson(fson_text, strlen(fson_text));
    ASSUME_NOT_CNULL(fson.root());
}

FOSSIL_TEST_CASE(cpp_test_fson_get_by_key) {
    const char *fson_text =
        "{"
        " \"app_name\": \"Fossil Media\","
        " \"version\": 1u32,"
        " \"pi\": 3.1415926535f64,"
        " \"enabled\": true,"
        " \"max_connections\": 1024u16,"
        " \"timeout\": 30.0f32,"
        " \"initial_char\": 65c,"
        " \"null_field\": null,"
        " \"colors\": ["
        "     { \"red\": \"Red\" },"
        "     { \"green\": \"Green\" },"
        "     { \"blue\": \"Blue\" }"
        " ],"
        " \"config\": {"
        "     \"debug\": false,"
        "     \"retries\": 5u8,"
        "     \"oct_value\": 077oct,"
        "     \"hex_value\": 0xFFhex,"
        "     \"bin_value\": 101010bin,"
        "     \"nested\": {"
        "         \"name\": \"Nested Object\","
        "         \"value\": 42i32"
        "     }"
        " },"
        " \"numbers\": ["
        "     { \"one\": 1i32 },"
        "     { \"two\": 2i32 },"
        "     { \"three\": 3i32 }"
        " ],"
        " \"empty_object\": {},"
        " \"empty_array\": []"
        "}";

    fossil::media::FSON fson(fson_text, strlen(fson_text));
    const fossil_media_fson_node_t *root = fson.root();
    ASSUME_NOT_CNULL(root);

    ASSUME_NOT_CNULL(fossil::media::FSON::get(root, "app_name"));
    ASSUME_NOT_CNULL(fossil::media::FSON::get(root, "version"));
    ASSUME_NOT_CNULL(fossil::media::FSON::get(root, "pi"));
    ASSUME_NOT_CNULL(fossil::media::FSON::get(root, "enabled"));
    ASSUME_NOT_CNULL(fossil::media::FSON::get(root, "max_connections"));
    ASSUME_NOT_CNULL(fossil::media::FSON::get(root, "timeout"));
    ASSUME_NOT_CNULL(fossil::media::FSON::get(root, "initial_char"));
    ASSUME_NOT_CNULL(fossil::media::FSON::get(root, "null_field"));
    ASSUME_NOT_CNULL(fossil::media::FSON::get(root, "colors"));
    ASSUME_NOT_CNULL(fossil::media::FSON::get(root, "config"));
    ASSUME_NOT_CNULL(fossil::media::FSON::get(root, "numbers"));
    ASSUME_NOT_CNULL(fossil::media::FSON::get(root, "empty_object"));
    ASSUME_NOT_CNULL(fossil::media::FSON::get(root, "empty_array"));
}

FOSSIL_TEST_CASE(cpp_test_fson_array_access) {
    const char *fson_text =
        "{"
        " \"colors\": ["
        "     { \"red\": \"Red\" },"
        "     { \"green\": \"Green\" },"
        "     { \"blue\": \"Blue\" }"
        " ],"
        " \"numbers\": ["
        "     { \"one\": 1i32 },"
        "     { \"two\": 2i32 },"
        "     { \"three\": 3i32 }"
        " ]"
        "}";

    fossil::media::FSON fson(fson_text, strlen(fson_text));
    const fossil_media_fson_node_t *root = fson.root();

    const fossil_media_fson_node_t *colors = fossil::media::FSON::get(root, "colors");
    ASSUME_NOT_CNULL(colors);
    ASSUME_NOT_CNULL(fossil::media::FSON::at(colors, 0));
    ASSUME_NOT_CNULL(fossil::media::FSON::at(colors, 1));
    ASSUME_NOT_CNULL(fossil::media::FSON::at(colors, 2));

    const fossil_media_fson_node_t *numbers = fossil::media::FSON::get(root, "numbers");
    ASSUME_NOT_CNULL(numbers);
    ASSUME_NOT_CNULL(fossil::media::FSON::at(numbers, 0));
    ASSUME_NOT_CNULL(fossil::media::FSON::at(numbers, 1));
    ASSUME_NOT_CNULL(fossil::media::FSON::at(numbers, 2));
}

FOSSIL_TEST_CASE(cpp_test_fson_serialize_roundtrip) {
    const char *fson_text =
        "{"
        " \"app_name\": \"Fossil Media\","
        " \"version\": 1u32,"
        " \"pi\": 3.1415926535f64,"
        " \"enabled\": true,"
        " \"max_connections\": 1024u16,"
        " \"timeout\": 30.0f32,"
        " \"initial_char\": 65c,"
        " \"null_field\": null,"
        " \"colors\": ["
        "     { \"red\": \"Red\" },"
        "     { \"green\": \"Green\" },"
        "     { \"blue\": \"Blue\" }"
        " ],"
        " \"config\": {"
        "     \"debug\": false,"
        "     \"retries\": 5u8,"
        "     \"oct_value\": 077oct,"
        "     \"hex_value\": 0xFFhex,"
        "     \"bin_value\": 101010bin,"
        "     \"nested\": {"
        "         \"name\": \"Nested Object\","
        "         \"value\": 42i32"
        "     }"
        " },"
        " \"numbers\": ["
        "     { \"one\": 1i32 },"
        "     { \"two\": 2i32 },"
        "     { \"three\": 3i32 }"
        " ],"
        " \"empty_object\": {},"
        " \"empty_array\": []"
        "}";

    fossil::media::FSON fson(fson_text, strlen(fson_text));
    std::string out_text = fson.serialize();
    ASSUME_ITS_TRUE(!out_text.empty());
}

FOSSIL_TEST_CASE(cpp_test_fson_validate) {
    const char *fson_text =
        "{"
        " \"app_name\": \"Fossil Media\","
        " \"version\": 1u32,"
        " \"pi\": 3.1415926535f64,"
        " \"enabled\": true,"
        " \"max_connections\": 1024u16,"
        " \"timeout\": 30.0f32,"
        " \"initial_char\": 65c,"
        " \"null_field\": null,"
        " \"colors\": ["
        "     { \"red\": \"Red\" },"
        "     { \"green\": \"Green\" },"
        "     { \"blue\": \"Blue\" }"
        " ],"
        " \"config\": {"
        "     \"debug\": false,"
        "     \"retries\": 5u8,"
        "     \"oct_value\": 077oct,"
        "     \"hex_value\": 0xFFhex,"
        "     \"bin_value\": 101010bin,"
        "     \"nested\": {"
        "         \"name\": \"Nested Object\","
        "         \"value\": 42i32"
        "     }"
        " },"
        " \"numbers\": ["
        "     { \"one\": 1i32 },"
        "     { \"two\": 2i32 },"
        "     { \"three\": 3i32 }"
        " ],"
        " \"empty_object\": {},"
        " \"empty_array\": []"
        "}";

    fossil::media::FSON fson(fson_text, strlen(fson_text));
    fson.validate();
}


// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_fson_tests) {
    FOSSIL_TEST_ADD(cpp_fson_fixture, cpp_test_fson_load_memory_and_root);
    FOSSIL_TEST_ADD(cpp_fson_fixture, cpp_test_fson_get_by_key);
    FOSSIL_TEST_ADD(cpp_fson_fixture, cpp_test_fson_array_access);
    FOSSIL_TEST_ADD(cpp_fson_fixture, cpp_test_fson_serialize_roundtrip);
    FOSSIL_TEST_ADD(cpp_fson_fixture, cpp_test_fson_validate);

    FOSSIL_TEST_REGISTER(cpp_fson_fixture);
}
