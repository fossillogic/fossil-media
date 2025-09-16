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

FOSSIL_TEST_SUITE(cpp_html_fixture);

FOSSIL_SETUP(cpp_html_fixture) {
    // Setup for HTML tests
}

FOSSIL_TEARDOWN(cpp_html_fixture) {
    // Teardown for HTML tests
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(cpp_test_html_load_string_and_root) {
    // Use C++ wrapper
    fossil::media::Html doc = fossil::media::Html::from_string("<html><body><h1>Hello</h1></body></html>");
    ASSUME_ITS_TRUE(doc.is_valid());

    fossil_media_html_node_t *root = doc.root();
    ASSUME_NOT_CNULL(root);
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(root), FOSSIL_MEDIA_HTML_NODE_DOCUMENT);

    // Check that the first child is the <html> node
    fossil_media_html_node_t *html_node = fossil_media_html_first_child(root);
    ASSUME_NOT_CNULL(html_node);
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(html_node), FOSSIL_MEDIA_HTML_NODE_ELEMENT);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_node_tag(html_node), "html");
}

FOSSIL_TEST_CASE(cpp_test_html_find_by_tag) {
    fossil::media::Html doc = fossil::media::Html::from_string("<html><body><div id=\"main\">Text</div></body></html>");
    ASSUME_ITS_TRUE(doc.is_valid());

    fossil_media_html_node_t *root = doc.root();
    fossil_media_html_node_t *div = fossil_media_html_find_by_tag(root, "div");
    ASSUME_NOT_CNULL(div);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_node_tag(div), "div");
}

FOSSIL_TEST_CASE(cpp_test_html_get_and_set_attr) {
    fossil::media::Html doc = fossil::media::Html::from_string("<div id=\"main\"></div>");
    ASSUME_ITS_TRUE(doc.is_valid());

    fossil_media_html_node_t *root = doc.root();
    fossil_media_html_node_t *div = fossil_media_html_find_by_tag(root, "div");
    ASSUME_NOT_CNULL(div);

    const char *id_val = fossil_media_html_get_attr(div, "id");
    ASSUME_ITS_EQUAL_CSTR(id_val, "main");

    int rc = fossil_media_html_set_attr(div, "class", "container");
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_MEDIA_HTML_OK);
    const char *class_val = fossil_media_html_get_attr(div, "class");
    ASSUME_ITS_EQUAL_CSTR(class_val, "container");
}

FOSSIL_TEST_CASE(cpp_test_html_node_text) {
    fossil::media::Html doc = fossil::media::Html::from_string("<p>Hello <b>World</b></p>");
    ASSUME_ITS_TRUE(doc.is_valid());

    fossil_media_html_node_t *root = doc.root();
    fossil_media_html_node_t *p = fossil_media_html_find_by_tag(root, "p");
    ASSUME_NOT_CNULL(p);

    fossil_media_html_node_t *child = fossil_media_html_first_child(p);
    // First child should be a text node "Hello "
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(child), FOSSIL_MEDIA_HTML_NODE_TEXT);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_node_text(child), "Hello ");
}

FOSSIL_TEST_CASE(cpp_test_html_serialize_roundtrip) {
    fossil::media::Html doc = fossil::media::Html::from_string("<div id=\"main\">Hello</div>");
    ASSUME_ITS_TRUE(doc.is_valid());

    std::string out = doc.serialize();
    ASSUME_ITS_TRUE(out.find("div") != std::string::npos);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_html_tests) {
    FOSSIL_TEST_ADD(cpp_html_fixture, cpp_test_html_load_string_and_root);
    FOSSIL_TEST_ADD(cpp_html_fixture, cpp_test_html_find_by_tag);
    FOSSIL_TEST_ADD(cpp_html_fixture, cpp_test_html_get_and_set_attr);
    FOSSIL_TEST_ADD(cpp_html_fixture, cpp_test_html_node_text);
    FOSSIL_TEST_ADD(cpp_html_fixture, cpp_test_html_serialize_roundtrip);

    FOSSIL_TEST_REGISTER(cpp_html_fixture);
}
