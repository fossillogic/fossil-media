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
    int result = fossil_media_ini_load_string(ini_data, &ini);

    const char *val = fossil_media_ini_get(&ini, "section", "key");
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_TRUE(strcmp(val, "value") == 0);

    fossil_media_ini_free(&ini);
}

FOSSIL_TEST_CASE(c_test_ini_set_and_get) {
    fossil_media_ini_t ini = {0};
    int result = fossil_media_ini_set(&ini, "main", "foo", "bar");

    const char *val = fossil_media_ini_get(&ini, "main", "foo");
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_TRUE(strcmp(val, "bar") == 0);

    fossil_media_ini_free(&ini);
}

FOSSIL_TEST_CASE(c_test_ini_save_and_load_file) {
    const char *test_path = "test_tmp.ini";
    fossil_media_ini_t ini = {0};
    fossil_media_ini_set(&ini, "alpha", "beta", "gamma");

    int save_result = fossil_media_ini_save_file(test_path, &ini);
    
    fossil_media_ini_t loaded = {0};
    int load_result = fossil_media_ini_load_file(test_path, &loaded);

    const char *val = fossil_media_ini_get(&loaded, "alpha", "beta");
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_TRUE(strcmp(val, "gamma") == 0);

    fossil_media_ini_free(&ini);
    fossil_media_ini_free(&loaded);

    remove(test_path);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_ini_tests) {    
    FOSSIL_TEST_ADD(c_ini_fixture, c_test_ini_load_string);
    FOSSIL_TEST_ADD(c_ini_fixture, c_test_ini_set_and_get);
    FOSSIL_TEST_ADD(c_ini_fixture, c_test_ini_save_and_load_file);

    FOSSIL_TEST_REGISTER(c_ini_fixture);
} // end of tests
