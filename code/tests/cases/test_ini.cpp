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
    bool threw = false;
    try {
        Ini ini("no_such_file_123456789.ini");
    } catch (const std::runtime_error&) {
        threw = true;
    }
    ASSUME_ITS_TRUE(threw);
}

FOSSIL_TEST_CASE(cpp_ini_throw_on_bad_string) {
    bool threw = false;
    try {
        Ini ini("[broken");
    } catch (const std::runtime_error&) {
        threw = true;
    }
    ASSUME_ITS_TRUE(threw);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
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
    FOSSIL_TEST_ADD(cpp_ini_fixture, cpp_ini_throw_on_bad_string);

    FOSSIL_TEST_REGISTER(cpp_ini_fixture);
} // end of tests
