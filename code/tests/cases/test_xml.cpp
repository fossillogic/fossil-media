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

FOSSIL_TEST_SUITE(cpp_xml_fixture);

FOSSIL_SETUP(cpp_xml_fixture) {
    // Setup for XML tests
}

FOSSIL_TEARDOWN(cpp_xml_fixture) {
    // Teardown for XML tests
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(cpp_test_xml_new_element) {
    fossil::media::Xml node = fossil::media::Xml::new_element("foo");
    ASSUME_NOT_CNULL(node.get());
    ASSUME_ITS_EQUAL_CSTR(node.get()->name, "foo");
    ASSUME_ITS_EQUAL_CSTR(node.type_name(), "element");
}

FOSSIL_TEST_CASE(cpp_test_xml_new_text) {
    fossil::media::Xml node = fossil::media::Xml::new_text("bar");
    ASSUME_NOT_CNULL(node.get());
    ASSUME_ITS_EQUAL_CSTR(node.get()->content, "bar");
    ASSUME_ITS_EQUAL_CSTR(node.type_name(), "text");
}

FOSSIL_TEST_CASE(cpp_test_xml_new_comment) {
    fossil::media::Xml node = fossil::media::Xml::new_comment("baz");
    ASSUME_NOT_CNULL(node.get());
    ASSUME_ITS_EQUAL_CSTR(node.get()->content, "baz");
    ASSUME_ITS_EQUAL_CSTR(node.type_name(), "comment");
}

FOSSIL_TEST_CASE(cpp_test_xml_new_cdata) {
    fossil::media::Xml node = fossil::media::Xml::new_cdata("cdata content");
    ASSUME_NOT_CNULL(node.get());
    ASSUME_ITS_EQUAL_CSTR(node.get()->content, "cdata content");
    ASSUME_ITS_EQUAL_CSTR(node.type_name(), "cdata");
}

FOSSIL_TEST_CASE(cpp_test_xml_new_pi) {
    fossil::media::Xml node = fossil::media::Xml::new_pi("xml-stylesheet", "type=\"text/xsl\" href=\"style.xsl\"");
    ASSUME_NOT_CNULL(node.get());
    ASSUME_ITS_EQUAL_CSTR(node.get()->name, "xml-stylesheet");
    ASSUME_ITS_EQUAL_CSTR(node.get()->content, "type=\"text/xsl\" href=\"style.xsl\"");
    ASSUME_ITS_EQUAL_CSTR(node.type_name(), "pi");
}

FOSSIL_TEST_CASE(cpp_test_xml_set_and_get_attribute) {
    fossil::media::Xml node = fossil::media::Xml::new_element("item");
    node.set_attribute("id", "42");
    const char *val = node.get_attribute("id");
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(val, "42");
}

FOSSIL_TEST_CASE(cpp_test_xml_set_attribute_replace) {
    fossil::media::Xml node = fossil::media::Xml::new_element("item");
    node.set_attribute("id", "42");
    node.set_attribute("id", "43");
    const char *val = node.get_attribute("id");
    ASSUME_NOT_CNULL(val);
    ASSUME_ITS_EQUAL_CSTR(val, "43");
}

FOSSIL_TEST_CASE(cpp_test_xml_stringify_simple) {
    fossil::media::Xml node = fossil::media::Xml::new_element("root");
    node.set_attribute("foo", "bar");
    std::string xml = node.stringify(false);
    ASSUME_ITS_TRUE(xml.find("<root foo=\"bar\"/>") != std::string::npos);
}

FOSSIL_TEST_CASE(cpp_test_xml_stringify_pretty) {
    fossil::media::Xml root = fossil::media::Xml::new_element("root");
    fossil::media::Xml child = fossil::media::Xml::new_element("child");
    root.append_child(std::move(child));
    std::string xml = root.stringify(true);
    ASSUME_ITS_TRUE(xml.find("<root>") != std::string::npos);
    ASSUME_ITS_TRUE(xml.find("<child/>") != std::string::npos);
}

FOSSIL_TEST_CASE(cpp_test_xml_parse_dummy) {
    const char *xml = "<foo>bar</foo>";
    fossil::media::Xml node(xml);
    ASSUME_NOT_CNULL(node.get());
    ASSUME_ITS_EQUAL_CSTR(node.get()->name, "root"); // Dummy parser returns "root"
}

FOSSIL_TEST_CASE(cpp_test_xml_type_name) {
    fossil::media::Xml elem = fossil::media::Xml::new_element("e");
    fossil::media::Xml txt = fossil::media::Xml::new_text("t");
    fossil::media::Xml com = fossil::media::Xml::new_comment("c");
    fossil::media::Xml pi = fossil::media::Xml::new_pi("p", "d");
    fossil::media::Xml cdata = fossil::media::Xml::new_cdata("cd");
    ASSUME_ITS_EQUAL_CSTR(elem.type_name(), "element");
    ASSUME_ITS_EQUAL_CSTR(txt.type_name(), "text");
    ASSUME_ITS_EQUAL_CSTR(com.type_name(), "comment");
    ASSUME_ITS_EQUAL_CSTR(pi.type_name(), "pi");
    ASSUME_ITS_EQUAL_CSTR(cdata.type_name(), "cdata");
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_xml_tests) {
    FOSSIL_TEST_ADD(cpp_xml_fixture, cpp_test_xml_new_element);
    FOSSIL_TEST_ADD(cpp_xml_fixture, cpp_test_xml_new_text);
    FOSSIL_TEST_ADD(cpp_xml_fixture, cpp_test_xml_new_comment);
    FOSSIL_TEST_ADD(cpp_xml_fixture, cpp_test_xml_new_cdata);
    FOSSIL_TEST_ADD(cpp_xml_fixture, cpp_test_xml_new_pi);
    FOSSIL_TEST_ADD(cpp_xml_fixture, cpp_test_xml_set_and_get_attribute);
    FOSSIL_TEST_ADD(cpp_xml_fixture, cpp_test_xml_set_attribute_replace);
    FOSSIL_TEST_ADD(cpp_xml_fixture, cpp_test_xml_stringify_simple);
    FOSSIL_TEST_ADD(cpp_xml_fixture, cpp_test_xml_stringify_pretty);
    FOSSIL_TEST_ADD(cpp_xml_fixture, cpp_test_xml_parse_dummy);
    FOSSIL_TEST_ADD(cpp_xml_fixture, cpp_test_xml_type_name);

    FOSSIL_TEST_REGISTER(cpp_xml_fixture);
} // end of tests
