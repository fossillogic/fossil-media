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

FOSSIL_TEST_SUITE(c_xml_fixture);

FOSSIL_SETUP(c_xml_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(c_xml_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_test_xml_parse_simple_element) {
    const char *xml = "<root></root>";
    fossil_media_xml_error_t err = {0};
    fossil_media_xml_node_t *doc = fossil_media_xml_parse(xml, &err);
    ASSUME_ITS_TRUE(doc != NULL);
    ASSUME_ITS_TRUE(fossil_media_xml_first_child(doc) != NULL);
    fossil_media_xml_free(doc);
}

FOSSIL_TEST_CASE(c_test_create_element_and_attribute) {
    fossil_media_xml_node_t *elem = fossil_media_xml_new_element("greeting");
    ASSUME_ITS_TRUE(elem != NULL);
    int rc = fossil_media_xml_set_attribute(elem, "lang", "en");
    ASSUME_ITS_TRUE(rc == 0);
    const char *val = fossil_media_xml_get_attribute(elem, "lang");
    ASSUME_ITS_TRUE(val != NULL && strcmp(val, "en") == 0);
    fossil_media_xml_free(elem);
}

FOSSIL_TEST_CASE(c_test_append_child_and_serialize) {
    fossil_media_xml_node_t *parent = fossil_media_xml_new_element("parent");
    fossil_media_xml_node_t *child = fossil_media_xml_new_element("child");
    fossil_media_xml_append_child(parent, child);
    fossil_media_xml_error_t err = {0};
    char *xml = fossil_media_xml_stringify(parent, 0, &err);
    ASSUME_ITS_TRUE(xml != NULL);
    free(xml);
    fossil_media_xml_free(parent);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_xml_tests) {    
    FOSSIL_TEST_ADD(c_xml_fixture, c_test_xml_parse_simple_element);
    FOSSIL_TEST_ADD(c_xml_fixture, c_test_create_element_and_attribute);
    FOSSIL_TEST_ADD(c_xml_fixture, c_test_append_child_and_serialize);

    FOSSIL_TEST_REGISTER(c_xml_fixture);
} // end of tests
