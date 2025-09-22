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

FOSSIL_TEST_SUITE(c_ini_fixture);

FOSSIL_SETUP(c_ini_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(c_ini_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_test_ini_load_string) {
    const char *ini_data =
        "[section]\n"
        "key=value\n";
    fossil_media_ini_t ini;
    fossil_media_ini_load_string(ini_data, &ini);

    const char *val = fossil_media_ini_get(&ini, "section", "key");
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_TRUE(strcmp(val, "value") == 0);

    fossil_media_ini_free(&ini);
}

FOSSIL_TEST_CASE(c_test_ini_set_and_get) {
    fossil_media_ini_t ini = {0};
    fossil_media_ini_set(&ini, "main", "foo", "bar");

    const char *val = fossil_media_ini_get(&ini, "main", "foo");
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_TRUE(strcmp(val, "bar") == 0);

    fossil_media_ini_free(&ini);
}

FOSSIL_TEST_CASE(c_test_ini_save_and_load_file) {
    const char *test_path = "test_tmp.ini";
    fossil_media_ini_t ini = {0};
    fossil_media_ini_set(&ini, "alpha", "beta", "gamma");

    fossil_media_ini_save_file(test_path, &ini);
    
    fossil_media_ini_t loaded = {0};
    fossil_media_ini_load_file(test_path, &loaded);

    const char *val = fossil_media_ini_get(&loaded, "alpha", "beta");
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_TRUE(strcmp(val, "gamma") == 0);

    fossil_media_ini_free(&ini);
    fossil_media_ini_free(&loaded);

    remove(test_path);
}

FOSSIL_TEST_CASE(c_test_ini_multiple_sections_and_keys) {
    const char *ini_data =
        "[first]\n"
        "a=1\n"
        "b=2\n"
        "[second]\n"
        "x=foo\n"
        "y=bar\n";
    fossil_media_ini_t ini;
    fossil_media_ini_load_string(ini_data, &ini);

    ASSUME_ITS_TRUE(strcmp(fossil_media_ini_get(&ini, "first", "a"), "1") == 0);
    ASSUME_ITS_TRUE(strcmp(fossil_media_ini_get(&ini, "first", "b"), "2") == 0);
    ASSUME_ITS_TRUE(strcmp(fossil_media_ini_get(&ini, "second", "x"), "foo") == 0);
    ASSUME_ITS_TRUE(strcmp(fossil_media_ini_get(&ini, "second", "y"), "bar") == 0);

    fossil_media_ini_free(&ini);
}

FOSSIL_TEST_CASE(c_test_ini_trim_and_comments) {
    const char *ini_data =
        "   [sec]   \n"
        "key1 = value1 ; comment\n"
        "key2= value2 # another comment\n"
        "; whole line comment\n"
        "key3 = \"quoted value\" \n";
    fossil_media_ini_t ini;
    fossil_media_ini_load_string(ini_data, &ini);

    ASSUME_ITS_TRUE(strcmp(fossil_media_ini_get(&ini, "sec", "key1"), "value1") == 0);
    ASSUME_ITS_TRUE(strcmp(fossil_media_ini_get(&ini, "sec", "key2"), "value2") == 0);
    ASSUME_ITS_TRUE(strcmp(fossil_media_ini_get(&ini, "sec", "key3"), "quoted value") == 0);

    fossil_media_ini_free(&ini);
}

FOSSIL_TEST_CASE(c_test_ini_update_existing_key) {
    fossil_media_ini_t ini = {0};
    fossil_media_ini_set(&ini, "main", "foo", "bar");
    fossil_media_ini_set(&ini, "main", "foo", "baz");

    const char *val = fossil_media_ini_get(&ini, "main", "foo");
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_TRUE(strcmp(val, "baz") == 0);

    fossil_media_ini_free(&ini);
}

FOSSIL_TEST_CASE(c_test_ini_missing_section_or_key) {
    fossil_media_ini_t ini = {0};
    fossil_media_ini_set(&ini, "main", "foo", "bar");

    ASSUME_ITS_CNULL(fossil_media_ini_get(&ini, "other", "foo"));
    ASSUME_ITS_CNULL(fossil_media_ini_get(&ini, "main", "missing"));

    fossil_media_ini_free(&ini);
}

// Edge case tests for INI parser

FOSSIL_TEST_CASE(c_test_ini_empty_string) {
    fossil_media_ini_t ini = {0};
    fossil_media_ini_load_string("", &ini);
    ASSUME_ITS_CNULL(fossil_media_ini_get(&ini, "any", "key"));
    fossil_media_ini_free(&ini);
}

FOSSIL_TEST_CASE(c_test_ini_null_string) {
    fossil_media_ini_t ini = {0};
    fossil_media_ini_load_string(NULL, &ini);
    ASSUME_ITS_CNULL(fossil_media_ini_get(&ini, "any", "key"));
    fossil_media_ini_free(&ini);
}

FOSSIL_TEST_CASE(c_test_ini_no_section) {
    const char *ini_data = "key=value\n";
    fossil_media_ini_t ini;
    fossil_media_ini_load_string(ini_data, &ini);
    ASSUME_ITS_CNULL(fossil_media_ini_get(&ini, "", "key"));
    fossil_media_ini_free(&ini);
}

FOSSIL_TEST_CASE(c_test_ini_duplicate_keys) {
    const char *ini_data =
        "[dup]\n"
        "key=first\n"
        "key=second\n";
    fossil_media_ini_t ini;
    fossil_media_ini_load_string(ini_data, &ini);
    ASSUME_ITS_TRUE(strcmp(fossil_media_ini_get(&ini, "dup", "key"), "second") == 0);
    fossil_media_ini_free(&ini);
}

FOSSIL_TEST_CASE(c_test_ini_long_key_and_value) {
    char long_key[256], long_value[256];
    memset(long_key, 'k', 255); long_key[255] = '\0';
    memset(long_value, 'v', 255); long_value[255] = '\0';

    fossil_media_ini_t ini = {0};
    fossil_media_ini_set(&ini, "long", long_key, long_value);

    const char *val = fossil_media_ini_get(&ini, "long", long_key);
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_TRUE(strcmp(val, long_value) == 0);

    fossil_media_ini_free(&ini);
}

// Complex INI parser test cases

FOSSIL_TEST_CASE(c_test_ini_nested_sections_like_names) {
    const char *ini_data =
        "[main]\n"
        "key=val\n"
        "[main.sub]\n"
        "key=subval\n"
        "[main.sub.sub2]\n"
        "key=sub2val\n";
    fossil_media_ini_t ini;
    fossil_media_ini_load_string(ini_data, &ini);

    ASSUME_ITS_TRUE(strcmp(fossil_media_ini_get(&ini, "main", "key"), "val") == 0);
    ASSUME_ITS_TRUE(strcmp(fossil_media_ini_get(&ini, "main.sub", "key"), "subval") == 0);
    ASSUME_ITS_TRUE(strcmp(fossil_media_ini_get(&ini, "main.sub.sub2", "key"), "sub2val") == 0);

    fossil_media_ini_free(&ini);
}

FOSSIL_TEST_CASE(c_test_ini_multiline_value) {
    const char *ini_data =
        "[multi]\n"
        "desc=\"This is a\n"
        "multiline\n"
        "value\"\n";
    fossil_media_ini_t ini;
    fossil_media_ini_load_string(ini_data, &ini);

    const char *val = fossil_media_ini_get(&ini, "multi", "desc");
    ASSUME_NOT_CNULL(val);
    // Multiline values may be handled as a single string with embedded newlines
    ASSUME_ITS_TRUE(strstr(val, "multiline") != NULL);

    fossil_media_ini_free(&ini);
}

FOSSIL_TEST_CASE(c_test_ini_special_characters) {
    const char *ini_data =
        "[specialchars]\n"
        "path=C:\\Program Files\\App\n"
        "unicode=✓\n"
        "escaped=Line\\nBreak\n";
    fossil_media_ini_t ini;
    fossil_media_ini_load_string(ini_data, &ini);

    ASSUME_ITS_TRUE(strcmp(fossil_media_ini_get(&ini, "specialchars", "path"), "C:\\Program Files\\App") == 0);
    ASSUME_ITS_TRUE(strcmp(fossil_media_ini_get(&ini, "specialchars", "unicode"), "✓") == 0);
    ASSUME_ITS_TRUE(strcmp(fossil_media_ini_get(&ini, "specialchars", "escaped"), "Line\\nBreak") == 0);

    fossil_media_ini_free(&ini);
}

FOSSIL_TEST_CASE(c_test_ini_section_and_key_whitespace) {
    const char *ini_data =
        "[  spaced section  ]\n"
        "   spaced key   =   spaced value   \n";
    fossil_media_ini_t ini;
    fossil_media_ini_load_string(ini_data, &ini);

    ASSUME_ITS_TRUE(strcmp(fossil_media_ini_get(&ini, "spaced section", "spaced key"), "spaced value") == 0);

    fossil_media_ini_free(&ini);
}

FOSSIL_TEST_CASE(c_test_ini_key_without_value) {
    const char *ini_data =
        "[empty]\n"
        "novalue=\n"
        "justkey\n";
    fossil_media_ini_t ini;
    fossil_media_ini_load_string(ini_data, &ini);

    ASSUME_ITS_TRUE(strcmp(fossil_media_ini_get(&ini, "empty", "novalue"), "") == 0);
    ASSUME_ITS_CNULL(fossil_media_ini_get(&ini, "empty", "justkey"));

    fossil_media_ini_free(&ini);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_ini_tests) {    
    FOSSIL_TEST_ADD(c_ini_fixture, c_test_ini_load_string);
    FOSSIL_TEST_ADD(c_ini_fixture, c_test_ini_set_and_get);
    FOSSIL_TEST_ADD(c_ini_fixture, c_test_ini_save_and_load_file);
    FOSSIL_TEST_ADD(c_ini_fixture, c_test_ini_multiple_sections_and_keys);
    FOSSIL_TEST_ADD(c_ini_fixture, c_test_ini_trim_and_comments);
    FOSSIL_TEST_ADD(c_ini_fixture, c_test_ini_update_existing_key);
    FOSSIL_TEST_ADD(c_ini_fixture, c_test_ini_missing_section_or_key);
    FOSSIL_TEST_ADD(c_ini_fixture, c_test_ini_empty_string);
    FOSSIL_TEST_ADD(c_ini_fixture, c_test_ini_null_string);
    FOSSIL_TEST_ADD(c_ini_fixture, c_test_ini_no_section);
    FOSSIL_TEST_ADD(c_ini_fixture, c_test_ini_duplicate_keys);
    FOSSIL_TEST_ADD(c_ini_fixture, c_test_ini_long_key_and_value);
    FOSSIL_TEST_ADD(c_ini_fixture, c_test_ini_nested_sections_like_names);
    FOSSIL_TEST_ADD(c_ini_fixture, c_test_ini_multiline_value);
    FOSSIL_TEST_ADD(c_ini_fixture, c_test_ini_special_characters);
    FOSSIL_TEST_ADD(c_ini_fixture, c_test_ini_section_and_key_whitespace);
    FOSSIL_TEST_ADD(c_ini_fixture, c_test_ini_key_without_value);

    FOSSIL_TEST_REGISTER(c_ini_fixture);
} // end of tests
