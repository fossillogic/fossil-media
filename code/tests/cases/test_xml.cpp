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

FOSSIL_TEST_SUITE(cpp_xml_fixture);

FOSSIL_SETUP(cpp_xml_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(cpp_xml_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

using fossil::media::Xml;

FOSSIL_TEST_CASE(cpp_test_xml_parse_simple_element) {
    Xml doc("<root></root>");
    ASSUME_ITS_TRUE(doc.get() != nullptr);
    Xml child = doc.first_child();
    ASSUME_ITS_TRUE(child.get() != nullptr);
}

FOSSIL_TEST_CASE(cpp_test_create_element_and_attribute) {
    Xml elem = Xml::new_element("greeting");
    ASSUME_ITS_TRUE(elem.get() != nullptr);
    elem.set_attribute("lang", "en");
    const char* val = elem.get_attribute("lang");
    ASSUME_ITS_TRUE(val != nullptr && std::string(val) == "en");
}

FOSSIL_TEST_CASE(cpp_test_append_child_and_serialize) {
    Xml parent = Xml::new_element("parent");
    Xml child = Xml::new_element("child");
    parent.append_child(std::move(child));
    std::string xml = parent.stringify();
    ASSUME_ITS_TRUE(!xml.empty());
}

FOSSIL_TEST_CASE(cpp_test_text_and_comment_nodes) {
    Xml elem = Xml::new_element("data");
    Xml text = Xml::new_text("Hello");
    Xml comment = Xml::new_comment("A comment");
    elem.append_child(std::move(text));
    elem.append_child(std::move(comment));
    std::string xml = elem.stringify();
    ASSUME_ITS_TRUE(!xml.empty());
}

FOSSIL_TEST_CASE(cpp_test_cdata_and_pi_nodes) {
    Xml elem = Xml::new_element("script");
    Xml cdata = Xml::new_cdata("x < y && y > z");
    Xml pi = Xml::new_pi("xml-stylesheet", "type=\"text/xsl\" href=\"style.xsl\"");
    elem.append_child(std::move(cdata));
    elem.append_child(std::move(pi));
    std::string xml = elem.stringify();
    ASSUME_ITS_TRUE(!xml.empty());
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(cpp_xml_tests) {
    FOSSIL_TEST_ADD(cpp_xml_fixture, cpp_test_xml_parse_simple_element);
    FOSSIL_TEST_ADD(cpp_xml_fixture, cpp_test_create_element_and_attribute);
    FOSSIL_TEST_ADD(cpp_xml_fixture, cpp_test_append_child_and_serialize);
    FOSSIL_TEST_ADD(cpp_xml_fixture, cpp_test_text_and_comment_nodes);
    FOSSIL_TEST_ADD(cpp_xml_fixture, cpp_test_cdata_and_pi_nodes);

    FOSSIL_TEST_REGISTER(cpp_xml_fixture);
} // end of tests
