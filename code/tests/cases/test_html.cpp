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

FOSSIL_TEST_CASE(cpp_test_html_empty_string) {
    fossil::media::Html doc = fossil::media::Html::from_string("");
    ASSUME_ITS_TRUE(doc.is_valid());

    fossil_media_html_node_t *root = doc.root();
    ASSUME_NOT_CNULL(root);
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(root), FOSSIL_MEDIA_HTML_NODE_DOCUMENT);
}

FOSSIL_TEST_CASE(cpp_test_html_self_closing_tag) {
    fossil::media::Html doc = fossil::media::Html::from_string("<br/>");
    ASSUME_ITS_TRUE(doc.is_valid());

    fossil_media_html_node_t *root = doc.root();
    fossil_media_html_node_t *br = fossil_media_html_find_by_tag(root, "br");
    ASSUME_NOT_CNULL(br);
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(br), FOSSIL_MEDIA_HTML_NODE_ELEMENT);
}

FOSSIL_TEST_CASE(cpp_test_html_comment_node) {
    fossil::media::Html doc = fossil::media::Html::from_string("<!-- This is a comment --><div></div>");
    ASSUME_ITS_TRUE(doc.is_valid());

    fossil_media_html_node_t *root = doc.root();
    fossil_media_html_node_t *comment = fossil_media_html_first_child(root);
    ASSUME_NOT_CNULL(comment);
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(comment), FOSSIL_MEDIA_HTML_NODE_COMMENT);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_node_text(comment), " This is a comment ");
}

FOSSIL_TEST_CASE(cpp_test_html_cdata_node) {
    fossil::media::Html doc = fossil::media::Html::from_string("<![CDATA[Some <cdata> content]]><div></div>");
    ASSUME_ITS_TRUE(doc.is_valid());

    fossil_media_html_node_t *root = doc.root();
    fossil_media_html_node_t *cdata = fossil_media_html_first_child(root);
    ASSUME_NOT_CNULL(cdata);
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(cdata), FOSSIL_MEDIA_HTML_NODE_CDATA);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_node_text(cdata), "Some <cdata> content");
}

FOSSIL_TEST_CASE(cpp_test_html_processing_instruction) {
    fossil::media::Html doc = fossil::media::Html::from_string("<?xml version=\"1.0\"?><div></div>");
    ASSUME_ITS_TRUE(doc.is_valid());

    fossil_media_html_node_t *root = doc.root();
    fossil_media_html_node_t *pi = fossil_media_html_first_child(root);
    ASSUME_NOT_CNULL(pi);
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(pi), FOSSIL_MEDIA_HTML_NODE_PROCESSING_INSTRUCTION);
    ASSUME_ITS_TRUE(strstr(fossil_media_html_node_text(pi), "xml version=\"1.0\"") != NULL);
}

FOSSIL_TEST_CASE(cpp_test_html_multiple_attributes) {
    fossil::media::Html doc = fossil::media::Html::from_string("<input type=\"text\" name=\"username\" value=\"user1\"/>");
    ASSUME_ITS_TRUE(doc.is_valid());

    fossil_media_html_node_t *root = doc.root();
    fossil_media_html_node_t *input = fossil_media_html_find_by_tag(root, "input");
    ASSUME_NOT_CNULL(input);

    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_get_attr(input, "type"), "text");
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_get_attr(input, "name"), "username");
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_get_attr(input, "value"), "user1");
}

FOSSIL_TEST_CASE(cpp_test_html_nested_elements) {
    fossil::media::Html doc = fossil::media::Html::from_string("<ul><li>One</li><li>Two</li></ul>");
    ASSUME_ITS_TRUE(doc.is_valid());

    fossil_media_html_node_t *root = doc.root();
    fossil_media_html_node_t *ul = fossil_media_html_find_by_tag(root, "ul");
    ASSUME_NOT_CNULL(ul);

    fossil_media_html_node_t *li1 = fossil_media_html_first_child(ul);
    ASSUME_NOT_CNULL(li1);
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(li1), FOSSIL_MEDIA_HTML_NODE_ELEMENT);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_node_tag(li1), "li");

    fossil_media_html_node_t *li2 = fossil_media_html_next_sibling(li1);
    ASSUME_NOT_CNULL(li2);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_node_tag(li2), "li");
}

FOSSIL_TEST_CASE(cpp_test_html_unclosed_tag) {
    fossil::media::Html doc = fossil::media::Html::from_string("<div><span>Text");
    ASSUME_ITS_TRUE(doc.is_valid());

    fossil_media_html_node_t *root = doc.root();
    fossil_media_html_node_t *div = fossil_media_html_find_by_tag(root, "div");
    ASSUME_NOT_CNULL(div);

    fossil_media_html_node_t *span = fossil_media_html_find_by_tag(div, "span");
    ASSUME_NOT_CNULL(span);
    fossil_media_html_node_t *text = fossil_media_html_first_child(span);
    ASSUME_NOT_CNULL(text);
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(text), FOSSIL_MEDIA_HTML_NODE_TEXT);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_node_text(text), "Text");
}

FOSSIL_TEST_CASE(cpp_test_html_attribute_no_quotes) {
    fossil::media::Html doc = fossil::media::Html::from_string("<div id=main></div>");
    ASSUME_ITS_TRUE(doc.is_valid());

    fossil_media_html_node_t *root = doc.root();
    fossil_media_html_node_t *div = fossil_media_html_find_by_tag(root, "div");
    ASSUME_NOT_CNULL(div);

    const char *id_val = fossil_media_html_get_attr(div, "id");
    ASSUME_ITS_EQUAL_CSTR(id_val, "main");
}

FOSSIL_TEST_CASE(cpp_test_html_multiple_comments) {
    fossil::media::Html doc = fossil::media::Html::from_string("<!--A--><!--B--><div></div>");
    ASSUME_ITS_TRUE(doc.is_valid());

    fossil_media_html_node_t *root = doc.root();
    fossil_media_html_node_t *comment1 = fossil_media_html_first_child(root);
    ASSUME_NOT_CNULL(comment1);
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(comment1), FOSSIL_MEDIA_HTML_NODE_COMMENT);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_node_text(comment1), "A");

    fossil_media_html_node_t *comment2 = fossil_media_html_next_sibling(comment1);
    ASSUME_NOT_CNULL(comment2);
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(comment2), FOSSIL_MEDIA_HTML_NODE_COMMENT);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_node_text(comment2), "B");
}

FOSSIL_TEST_CASE(cpp_test_html_empty_tag) {
    fossil::media::Html doc = fossil::media::Html::from_string("<div></div>");
    ASSUME_ITS_TRUE(doc.is_valid());

    fossil_media_html_node_t *root = doc.root();
    fossil_media_html_node_t *div = fossil_media_html_find_by_tag(root, "div");
    ASSUME_NOT_CNULL(div);
    ASSUME_ITS_CNULL(fossil_media_html_first_child(div));
}

FOSSIL_TEST_CASE(cpp_test_html_tag_with_single_quotes) {
    fossil::media::Html doc = fossil::media::Html::from_string("<div id='main' class='container'></div>");
    ASSUME_ITS_TRUE(doc.is_valid());

    fossil_media_html_node_t *root = doc.root();
    fossil_media_html_node_t *div = fossil_media_html_find_by_tag(root, "div");
    ASSUME_NOT_CNULL(div);

    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_get_attr(div, "id"), "main");
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_get_attr(div, "class"), "container");
}

FOSSIL_TEST_CASE(cpp_test_html_text_outside_tags) {
    fossil::media::Html doc = fossil::media::Html::from_string("Hello<div>World</div>!");
    ASSUME_ITS_TRUE(doc.is_valid());

    fossil_media_html_node_t *root = doc.root();
    fossil_media_html_node_t *first = fossil_media_html_first_child(root);
    ASSUME_NOT_CNULL(first);
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(first), FOSSIL_MEDIA_HTML_NODE_TEXT);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_node_text(first), "Hello");

    fossil_media_html_node_t *div = fossil_media_html_next_sibling(first);
    ASSUME_NOT_CNULL(div);
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(div), FOSSIL_MEDIA_HTML_NODE_ELEMENT);

    fossil_media_html_node_t *exclam = fossil_media_html_next_sibling(div);
    ASSUME_NOT_CNULL(exclam);
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(exclam), FOSSIL_MEDIA_HTML_NODE_TEXT);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_node_text(exclam), "!");
}

FOSSIL_TEST_CASE(cpp_test_html_complete_document) {
    std::string html =
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<title>Test Page</title>"
        "<meta charset=\"UTF-8\">"
        "</head>"
        "<body>"
        "<h1>Header</h1>"
        "<p id=\"para\">Paragraph <b>bold</b> <i>italic</i></p>"
        "<ul><li>Item1</li><li>Item2</li></ul>"
        "<!-- Footer -->"
        "</body>"
        "</html>";

    fossil::media::Html doc = fossil::media::Html::from_string(html);
    ASSUME_ITS_TRUE(doc.is_valid());

    fossil_media_html_node_t *root = doc.root();
    ASSUME_NOT_CNULL(root);
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(root), FOSSIL_MEDIA_HTML_NODE_DOCUMENT);

    fossil_media_html_node_t *doctype = fossil_media_html_first_child(root);
    ASSUME_NOT_CNULL(doctype);
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(doctype), FOSSIL_MEDIA_HTML_NODE_DOCTYPE);

    fossil_media_html_node_t *html_node = fossil_media_html_next_sibling(doctype);
    ASSUME_NOT_CNULL(html_node);
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(html_node), FOSSIL_MEDIA_HTML_NODE_ELEMENT);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_node_tag(html_node), "html");

    fossil_media_html_node_t *head = fossil_media_html_find_by_tag(html_node, "head");
    ASSUME_NOT_CNULL(head);

    fossil_media_html_node_t *title = fossil_media_html_find_by_tag(head, "title");
    ASSUME_NOT_CNULL(title);
    fossil_media_html_node_t *title_text = fossil_media_html_first_child(title);
    ASSUME_NOT_CNULL(title_text);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_node_text(title_text), "Test Page");

    fossil_media_html_node_t *body = fossil_media_html_find_by_tag(html_node, "body");
    ASSUME_NOT_CNULL(body);

    fossil_media_html_node_t *h1 = fossil_media_html_find_by_tag(body, "h1");
    ASSUME_NOT_CNULL(h1);
    fossil_media_html_node_t *h1_text = fossil_media_html_first_child(h1);
    ASSUME_NOT_CNULL(h1_text);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_node_text(h1_text), "Header");

    fossil_media_html_node_t *p = fossil_media_html_find_by_tag(body, "p");
    ASSUME_NOT_CNULL(p);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_get_attr(p, "id"), "para");

    fossil_media_html_node_t *ul = fossil_media_html_find_by_tag(body, "ul");
    ASSUME_NOT_CNULL(ul);
    fossil_media_html_node_t *li1 = fossil_media_html_first_child(ul);
    ASSUME_NOT_CNULL(li1);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_node_tag(li1), "li");
    fossil_media_html_node_t *li2 = fossil_media_html_next_sibling(li1);
    ASSUME_NOT_CNULL(li2);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_node_tag(li2), "li");

    fossil_media_html_node_t *comment = fossil_media_html_next_sibling(ul);
    ASSUME_NOT_CNULL(comment);
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(comment), FOSSIL_MEDIA_HTML_NODE_COMMENT);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_node_text(comment), " Footer ");
}

// The timeout test is not applicable for the C++ RAII wrapper, as it throws on error.
// If you want to test timeout, you must catch the exception:
FOSSIL_TEST_CASE(cpp_test_html_large_input_timeout) {
    size_t big_size = 2000000;
    std::string big_html = "<div>" + std::string(big_size - 11, 'a') + "</div>";
    bool threw = false;
    try {
        fossil::media::Html doc = fossil::media::Html::from_string(big_html);
    } catch (const std::runtime_error&) {
        threw = true;
    }
    ASSUME_ITS_TRUE(threw);
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
    FOSSIL_TEST_ADD(cpp_html_fixture, cpp_test_html_empty_string);
    FOSSIL_TEST_ADD(cpp_html_fixture, cpp_test_html_self_closing_tag);
    FOSSIL_TEST_ADD(cpp_html_fixture, cpp_test_html_comment_node);
    FOSSIL_TEST_ADD(cpp_html_fixture, cpp_test_html_cdata_node);
    FOSSIL_TEST_ADD(cpp_html_fixture, cpp_test_html_processing_instruction);
    FOSSIL_TEST_ADD(cpp_html_fixture, cpp_test_html_multiple_attributes);
    FOSSIL_TEST_ADD(cpp_html_fixture, cpp_test_html_nested_elements);
    FOSSIL_TEST_ADD(cpp_html_fixture, cpp_test_html_unclosed_tag);
    FOSSIL_TEST_ADD(cpp_html_fixture, cpp_test_html_attribute_no_quotes);
    FOSSIL_TEST_ADD(cpp_html_fixture, cpp_test_html_multiple_comments);
    FOSSIL_TEST_ADD(cpp_html_fixture, cpp_test_html_empty_tag);
    FOSSIL_TEST_ADD(cpp_html_fixture, cpp_test_html_tag_with_single_quotes);
    FOSSIL_TEST_ADD(cpp_html_fixture, cpp_test_html_text_outside_tags);
    FOSSIL_TEST_ADD(cpp_html_fixture, cpp_test_html_complete_document);
    FOSSIL_TEST_ADD(cpp_html_fixture, cpp_test_html_large_input_timeout);

    FOSSIL_TEST_REGISTER(cpp_html_fixture);
}
