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

FOSSIL_TEST_SUITE(cpp_ini_fixture);

FOSSIL_SETUP(cpp_ini_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(cpp_ini_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

using fossil::media::Ini;

FOSSIL_TEST_CASE(cpp_ini_default_ctor) {
    Ini ini;
    ASSUME_ITS_TRUE(ini.get("section", "key").empty());
}

FOSSIL_TEST_CASE(cpp_ini_load_string_ctor) {
    const char *data = "[s]\nk=v\n";
    Ini ini(data);
    ASSUME_ITS_TRUE(ini.get("s", "k") == "v");
}

FOSSIL_TEST_CASE(cpp_ini_move_ctor) {
    Ini ini1;
    ini1.set("x", "y", "z");
    Ini ini2(std::move(ini1));
    ASSUME_ITS_TRUE(ini2.get("x", "y") == "z");
}

FOSSIL_TEST_CASE(cpp_ini_move_assign) {
    Ini ini1;
    ini1.set("a", "b", "c");
    Ini ini2;
    ini2 = std::move(ini1);
    ASSUME_ITS_TRUE(ini2.get("a", "b") == "c");
}

FOSSIL_TEST_CASE(cpp_ini_set_and_get) {
    Ini ini;
    bool ok = ini.set("main", "foo", "bar");
    ASSUME_ITS_TRUE(ok);
    ASSUME_ITS_TRUE(ini.get("main", "foo") == "bar");
}

FOSSIL_TEST_CASE(cpp_ini_save_and_load_file) {
    Ini ini;
    ini.set("alpha", "beta", "gamma");
    std::string path = "cpp_ini_save_load.ini";
    bool saved = ini.save_file(path);
    ASSUME_ITS_TRUE(saved);

    Ini loaded;
    bool loaded_ok = loaded.load_file(path);
    ASSUME_ITS_TRUE(loaded_ok);
    ASSUME_ITS_TRUE(loaded.get("alpha", "beta") == "gamma");
    remove(path.c_str());
}

FOSSIL_TEST_CASE(cpp_ini_load_string) {
    Ini ini;
    bool ok = ini.load_string("[sec]\nk1=v1\n");
    ASSUME_ITS_TRUE(ok);
    ASSUME_ITS_TRUE(ini.get("sec", "k1") == "v1");
}

FOSSIL_TEST_CASE(cpp_ini_get_missing) {
    Ini ini;
    ini.set("s", "k", "v");
    ASSUME_ITS_TRUE(ini.get("s", "missing").empty());
    ASSUME_ITS_TRUE(ini.get("missing", "k").empty());
}

FOSSIL_TEST_CASE(cpp_ini_throw_on_bad_file) {
    Ini ini;
    bool loaded = ini.load_file("no_such_file_123456789.ini");
    ASSUME_ITS_TRUE(!loaded);
}

FOSSIL_TEST_CASE(cpp_ini_multiple_sections_and_keys) {
    const char *ini_data =
        "[first]\n"
        "a=1\n"
        "b=2\n"
        "[second]\n"
        "x=foo\n"
        "y=bar\n";
    Ini ini;
    bool ok = ini.load_string(ini_data);
    ASSUME_ITS_TRUE(ok);

    ASSUME_ITS_TRUE(ini.get("first", "a") == "1");
    ASSUME_ITS_TRUE(ini.get("first", "b") == "2");
    ASSUME_ITS_TRUE(ini.get("second", "x") == "foo");
    ASSUME_ITS_TRUE(ini.get("second", "y") == "bar");
}

FOSSIL_TEST_CASE(cpp_ini_trim_and_comments) {
    const char *ini_data =
        "   [sec]   \n"
        "key1 = value1 ; comment\n"
        "key2= value2 # another comment\n"
        "; whole line comment\n"
        "key3 = \"quoted value\" \n";
    Ini ini;
    bool ok = ini.load_string(ini_data);
    ASSUME_ITS_TRUE(ok);

    ASSUME_ITS_TRUE(ini.get("sec", "key1") == "value1");
    ASSUME_ITS_TRUE(ini.get("sec", "key2") == "value2");
    ASSUME_ITS_TRUE(ini.get("sec", "key3") == "\"quoted value\"");
}

FOSSIL_TEST_CASE(cpp_ini_update_existing_key) {
    Ini ini;
    bool ok1 = ini.set("main", "foo", "bar");
    bool ok2 = ini.set("main", "foo", "baz");
    ASSUME_ITS_TRUE(ok1 && ok2);

    ASSUME_ITS_TRUE(ini.get("main", "foo") == "baz");
}

FOSSIL_TEST_CASE(cpp_ini_missing_section_or_key) {
    Ini ini;
    ini.set("main", "foo", "bar");

    ASSUME_ITS_TRUE(ini.get("other", "foo").empty());
    ASSUME_ITS_TRUE(ini.get("main", "missing").empty());
}

// Edge case tests for INI parser

FOSSIL_TEST_CASE(cpp_ini_empty_string) {
    Ini ini;
    bool ok = ini.load_string("");
    ASSUME_ITS_TRUE(ok);
    ASSUME_ITS_TRUE(ini.get("any", "key").empty());
}

FOSSIL_TEST_CASE(cpp_ini_null_string) {
    Ini ini;
    bool ok = ini.load_string(nullptr);
    ASSUME_ITS_TRUE(ok);
    ASSUME_ITS_TRUE(ini.get("any", "key").empty());
}

FOSSIL_TEST_CASE(cpp_ini_no_section) {
    const char *ini_data = "key=value\n";
    Ini ini;
    bool ok = ini.load_string(ini_data);
    ASSUME_ITS_TRUE(ok);
    ASSUME_ITS_TRUE(ini.get("", "key").empty());
}

FOSSIL_TEST_CASE(cpp_ini_duplicate_keys) {
    const char *ini_data =
        "[dup]\n"
        "key=first\n"
        "key=second\n";
    Ini ini;
    bool ok = ini.load_string(ini_data);
    ASSUME_ITS_TRUE(ok);
    ASSUME_ITS_TRUE(ini.get("dup", "key") == "second");
}

FOSSIL_TEST_CASE(cpp_ini_long_key_and_value) {
    std::string long_key(255, 'k');
    std::string long_value(255, 'v');

    Ini ini;
    bool ok = ini.set("long", long_key, long_value);
    ASSUME_ITS_TRUE(ok);

    ASSUME_ITS_TRUE(ini.get("long", long_key) == long_value);
}

// Complex INI parser test cases

FOSSIL_TEST_CASE(cpp_ini_nested_sections_like_names) {
    const char *ini_data =
        "[main]\n"
        "key=val\n"
        "[main.sub]\n"
        "key=subval\n"
        "[main.sub.sub2]\n"
        "key=sub2val\n";
    Ini ini;
    bool ok = ini.load_string(ini_data);
    ASSUME_ITS_TRUE(ok);

    ASSUME_ITS_TRUE(ini.get("main", "key") == "val");
    ASSUME_ITS_TRUE(ini.get("main.sub", "key") == "subval");
    ASSUME_ITS_TRUE(ini.get("main.sub.sub2", "key") == "sub2val");
}

FOSSIL_TEST_CASE(cpp_ini_multiline_value) {
    const char *ini_data =
        "[multi]\n"
        "desc=\"This is a\n"
        "multiline\n"
        "value\"\n";
    Ini ini;
    bool ok = ini.load_string(ini_data);
    ASSUME_ITS_TRUE(ok);

    std::string val = ini.get("multi", "desc");
    ASSUME_ITS_TRUE(!val.empty());
    ASSUME_ITS_TRUE(val.find("multiline") != std::string::npos);
}

FOSSIL_TEST_CASE(cpp_ini_special_characters) {
    const char *ini_data =
        "[specialchars]\n"
        "path=C:\\Program Files\\App\n"
        "unicode=✓\n"
        "escaped=Line\\nBreak\n";
    Ini ini;
    bool ok = ini.load_string(ini_data);
    ASSUME_ITS_TRUE(ok);

    ASSUME_ITS_TRUE(ini.get("specialchars", "path") == "C:\\Program Files\\App");
    ASSUME_ITS_TRUE(ini.get("specialchars", "unicode") == "✓");
    ASSUME_ITS_TRUE(ini.get("specialchars", "escaped") == "Line\\nBreak");
}

FOSSIL_TEST_CASE(cpp_ini_section_and_key_whitespace) {
    const char *ini_data =
        "[  spaced section  ]\n"
        "   spaced key   =   spaced value   \n";
    Ini ini;
    bool ok = ini.load_string(ini_data);
    ASSUME_ITS_TRUE(ok);

    ASSUME_ITS_TRUE(ini.get("spaced section", "spaced key") == "spaced value");
}

FOSSIL_TEST_CASE(cpp_ini_key_without_value) {
    const char *ini_data =
        "[empty]\n"
        "novalue=\n"
        "justkey\n";
    Ini ini;
    bool ok = ini.load_string(ini_data);
    ASSUME_ITS_TRUE(ok);

    ASSUME_ITS_TRUE(ini.get("empty", "novalue") == "");
    ASSUME_ITS_TRUE(ini.get("empty", "justkey").empty());
}

FOSSIL_TEST_GROUP(cpp_ini_tests) {
    FOSSIL_TEST_ADD(cpp_ini_fixture, cpp_ini_default_ctor);
    FOSSIL_TEST_ADD(cpp_ini_fixture, cpp_ini_load_string_ctor);
    FOSSIL_TEST_ADD(cpp_ini_fixture, cpp_ini_move_ctor);
    FOSSIL_TEST_ADD(cpp_ini_fixture, cpp_ini_move_assign);
    FOSSIL_TEST_ADD(cpp_ini_fixture, cpp_ini_set_and_get);
    FOSSIL_TEST_ADD(cpp_ini_fixture, cpp_ini_save_and_load_file);
    FOSSIL_TEST_ADD(cpp_ini_fixture, cpp_ini_load_string);
    FOSSIL_TEST_ADD(cpp_ini_fixture, cpp_ini_get_missing);
    FOSSIL_TEST_ADD(cpp_ini_fixture, cpp_ini_throw_on_bad_file);
    FOSSIL_TEST_ADD(cpp_ini_fixture, cpp_ini_multiple_sections_and_keys);
    FOSSIL_TEST_ADD(cpp_ini_fixture, cpp_ini_trim_and_comments);
    FOSSIL_TEST_ADD(cpp_ini_fixture, cpp_ini_update_existing_key);
    FOSSIL_TEST_ADD(cpp_ini_fixture, cpp_ini_missing_section_or_key);
    FOSSIL_TEST_ADD(cpp_ini_fixture, cpp_ini_empty_string);
    FOSSIL_TEST_ADD(cpp_ini_fixture, cpp_ini_null_string);
    FOSSIL_TEST_ADD(cpp_ini_fixture, cpp_ini_no_section);
    FOSSIL_TEST_ADD(cpp_ini_fixture, cpp_ini_duplicate_keys);
    FOSSIL_TEST_ADD(cpp_ini_fixture, cpp_ini_long_key_and_value);
    FOSSIL_TEST_ADD(cpp_ini_fixture, cpp_ini_nested_sections_like_names);
    FOSSIL_TEST_ADD(cpp_ini_fixture, cpp_ini_multiline_value);
    FOSSIL_TEST_ADD(cpp_ini_fixture, cpp_ini_special_characters);
    FOSSIL_TEST_ADD(cpp_ini_fixture, cpp_ini_section_and_key_whitespace);
    FOSSIL_TEST_ADD(cpp_ini_fixture, cpp_ini_key_without_value);

    FOSSIL_TEST_REGISTER(cpp_ini_fixture);
} // end of tests
