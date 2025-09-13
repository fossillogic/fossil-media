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
    // Setup for XML tests
}

FOSSIL_TEARDOWN(c_xml_fixture) {
    // Teardown for XML tests
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_test_xml_new_element) {
    fossil_media_xml_node_t *node = fossil_media_xml_new_element("foo");
    ASSUME_NOT_CNULL(node);
    ASSUME_ITS_EQUAL_CSTR(node->name, "foo");
    ASSUME_ITS_EQUAL_CSTR(fossil_media_xml_type_name(node->type), "element");
    fossil_media_xml_free(node);
}

FOSSIL_TEST_CASE(c_test_xml_new_text) {
    fossil_media_xml_node_t *node = fossil_media_xml_new_text("bar");
    ASSUME_NOT_CNULL(node);
    ASSUME_ITS_EQUAL_CSTR(node->content, "bar");
    ASSUME_ITS_EQUAL_CSTR(fossil_media_xml_type_name(node->type), "text");
    fossil_media_xml_free(node);
}

FOSSIL_TEST_CASE(c_test_xml_new_comment) {
    fossil_media_xml_node_t *node = fossil_media_xml_new_comment("baz");
    ASSUME_NOT_CNULL(node);
    ASSUME_ITS_EQUAL_CSTR(node->content, "baz");
    ASSUME_ITS_EQUAL_CSTR(fossil_media_xml_type_name(node->type), "comment");
    fossil_media_xml_free(node);
}

FOSSIL_TEST_CASE(c_test_xml_new_cdata) {
    fossil_media_xml_node_t *node = fossil_media_xml_new_cdata("cdata content");
    ASSUME_NOT_CNULL(node);
    ASSUME_ITS_EQUAL_CSTR(node->content, "cdata content");
    ASSUME_ITS_EQUAL_CSTR(fossil_media_xml_type_name(node->type), "cdata");
    fossil_media_xml_free(node);
}

FOSSIL_TEST_CASE(c_test_xml_new_pi) {
    fossil_media_xml_node_t *node = fossil_media_xml_new_pi("xml-stylesheet", "type=\"text/xsl\" href=\"style.xsl\"");
    ASSUME_NOT_CNULL(node);
    ASSUME_ITS_EQUAL_CSTR(node->name, "xml-stylesheet");
    ASSUME_ITS_EQUAL_CSTR(node->content, "type=\"text/xsl\" href=\"style.xsl\"");
    ASSUME_ITS_EQUAL_CSTR(fossil_media_xml_type_name(node->type), "pi");
    fossil_media_xml_free(node);
}

FOSSIL_TEST_CASE(c_test_xml_append_child_and_first_child) {
    fossil_media_xml_node_t *parent = fossil_media_xml_new_element("parent");
    fossil_media_xml_node_t *child = fossil_media_xml_new_element("child");
    int rc = fossil_media_xml_append_child(parent, child);
    ASSUME_ITS_EQUAL_I32(rc, 0);
    fossil_media_xml_node_t *first = fossil_media_xml_first_child(parent);
    ASSUME_NOT_CNULL(first);
    ASSUME_ITS_EQUAL_CSTR(first->name, "child");
    fossil_media_xml_free(parent);
}

FOSSIL_TEST_CASE(c_test_xml_set_and_get_attribute) {
    fossil_media_xml_node_t *node = fossil_media_xml_new_element("item");
    int rc = fossil_media_xml_set_attribute(node, "id", "42");
    ASSUME_ITS_EQUAL_I32(rc, 0);
    const char *val = fossil_media_xml_get_attribute(node, "id");
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(val, "42");
    fossil_media_xml_free(node);
}

FOSSIL_TEST_CASE(c_test_xml_set_attribute_replace) {
    fossil_media_xml_node_t *node = fossil_media_xml_new_element("item");
    fossil_media_xml_set_attribute(node, "id", "42");
    fossil_media_xml_set_attribute(node, "id", "43");
    const char *val = fossil_media_xml_get_attribute(node, "id");
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(val, "43");
    fossil_media_xml_free(node);
}

FOSSIL_TEST_CASE(c_test_xml_stringify_simple) {
    fossil_media_xml_error_t err = FOSSIL_MEDIA_XML_OK;
    fossil_media_xml_node_t *node = fossil_media_xml_new_element("root");
    fossil_media_xml_set_attribute(node, "foo", "bar");
    char *xml = fossil_media_xml_stringify(node, 0, &err);
    ASSUME_NOT_CNULL(xml);
    ASSUME_ITS_EQUAL_I32(err, FOSSIL_MEDIA_XML_OK);
    ASSUME_ITS_TRUE(strstr(xml, "<root foo=\"bar\"/>") != NULL);
    free(xml);
    fossil_media_xml_free(node);
}

FOSSIL_TEST_CASE(c_test_xml_stringify_pretty) {
    fossil_media_xml_error_t err = FOSSIL_MEDIA_XML_OK;
    fossil_media_xml_node_t *root = fossil_media_xml_new_element("root");
    fossil_media_xml_node_t *child = fossil_media_xml_new_element("child");
    fossil_media_xml_append_child(root, child);
    char *xml = fossil_media_xml_stringify(root, 1, &err);
    ASSUME_NOT_CNULL(xml);
    ASSUME_ITS_EQUAL_I32(err, FOSSIL_MEDIA_XML_OK);
    ASSUME_ITS_TRUE(strstr(xml, "<root>") != NULL);
    ASSUME_ITS_TRUE(strstr(xml, "<child/>") != NULL);
    free(xml);
    fossil_media_xml_free(root);
}

FOSSIL_TEST_CASE(c_test_xml_parse_dummy) {
    fossil_media_xml_error_t err = FOSSIL_MEDIA_XML_OK;
    const char *xml = "<foo>bar</foo>";
    fossil_media_xml_node_t *node = fossil_media_xml_parse(xml, &err);
    ASSUME_NOT_CNULL(node);
    ASSUME_ITS_EQUAL_I32(err, FOSSIL_MEDIA_XML_OK);
    ASSUME_ITS_EQUAL_CSTR(node->name, "root"); // Dummy parser returns "root"
    fossil_media_xml_free(node);
}

FOSSIL_TEST_CASE(c_test_xml_type_name) {
    ASSUME_ITS_EQUAL_CSTR(fossil_media_xml_type_name(FOSSIL_MEDIA_XML_ELEMENT), "element");
    ASSUME_ITS_EQUAL_CSTR(fossil_media_xml_type_name(FOSSIL_MEDIA_XML_TEXT), "text");
    ASSUME_ITS_EQUAL_CSTR(fossil_media_xml_type_name(FOSSIL_MEDIA_XML_COMMENT), "comment");
    ASSUME_ITS_EQUAL_CSTR(fossil_media_xml_type_name(FOSSIL_MEDIA_XML_PI), "pi");
    ASSUME_ITS_EQUAL_CSTR(fossil_media_xml_type_name(FOSSIL_MEDIA_XML_CDATA), "cdata");
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_xml_tests) {
    FOSSIL_TEST_ADD(c_xml_fixture, c_test_xml_new_element);
    FOSSIL_TEST_ADD(c_xml_fixture, c_test_xml_new_text);
    FOSSIL_TEST_ADD(c_xml_fixture, c_test_xml_new_comment);
    FOSSIL_TEST_ADD(c_xml_fixture, c_test_xml_new_cdata);
    FOSSIL_TEST_ADD(c_xml_fixture, c_test_xml_new_pi);
    FOSSIL_TEST_ADD(c_xml_fixture, c_test_xml_append_child_and_first_child);
    FOSSIL_TEST_ADD(c_xml_fixture, c_test_xml_set_and_get_attribute);
    FOSSIL_TEST_ADD(c_xml_fixture, c_test_xml_set_attribute_replace);
    FOSSIL_TEST_ADD(c_xml_fixture, c_test_xml_stringify_simple);
    FOSSIL_TEST_ADD(c_xml_fixture, c_test_xml_stringify_pretty);
    FOSSIL_TEST_ADD(c_xml_fixture, c_test_xml_parse_dummy);
    FOSSIL_TEST_ADD(c_xml_fixture, c_test_xml_type_name);

    FOSSIL_TEST_REGISTER(c_xml_fixture);
} // end of tests
