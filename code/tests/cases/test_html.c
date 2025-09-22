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

FOSSIL_TEST_SUITE(c_html_fixture);

FOSSIL_SETUP(c_html_fixture) {
    // Setup for HTML tests
}

FOSSIL_TEARDOWN(c_html_fixture) {
    // Teardown for HTML tests
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_test_html_load_string_and_root) {
    const char *html = "<!DOCTYPE html><html><body><h1>Hello</h1></body></html>";
    fossil_media_html_doc_t *doc = NULL;
    int rc = fossil_media_html_load_string(html, &doc);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_MEDIA_HTML_OK);
    ASSUME_NOT_CNULL(doc);

    fossil_media_html_node_t *root = fossil_media_html_root(doc);
    ASSUME_NOT_CNULL(root);
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(root), FOSSIL_MEDIA_HTML_NODE_DOCUMENT);

    // Check that the first child is a comment node for DOCTYPE
    fossil_media_html_node_t *doctype = fossil_media_html_first_child(root);
    ASSUME_NOT_CNULL(doctype);
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(doctype), FOSSIL_MEDIA_HTML_NODE_DOCTYPE);

    fossil_media_html_free(doc);
}

FOSSIL_TEST_CASE(c_test_html_find_by_tag) {
    const char *html = "<html><body><div id=\"main\">Text</div></body></html>";
    fossil_media_html_doc_t *doc = NULL;
    int rc = fossil_media_html_load_string(html, &doc);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_MEDIA_HTML_OK);
    ASSUME_NOT_CNULL(doc);

    fossil_media_html_node_t *root = fossil_media_html_root(doc);
    fossil_media_html_node_t *div = fossil_media_html_find_by_tag(root, "div");
    ASSUME_NOT_CNULL(div);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_node_tag(div), "div");

    fossil_media_html_free(doc);
}

FOSSIL_TEST_CASE(c_test_html_get_and_set_attr) {
    const char *html = "<div id=\"main\"></div>";
    fossil_media_html_doc_t *doc = NULL;
    int rc = fossil_media_html_load_string(html, &doc);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_MEDIA_HTML_OK);
    ASSUME_NOT_CNULL(doc);

    fossil_media_html_node_t *root = fossil_media_html_root(doc);
    fossil_media_html_node_t *div = fossil_media_html_find_by_tag(root, "div");
    ASSUME_NOT_CNULL(div);

    const char *id_val = fossil_media_html_get_attr(div, "id");
    ASSUME_ITS_EQUAL_CSTR(id_val, "main");

    rc = fossil_media_html_set_attr(div, "class", "container");
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_MEDIA_HTML_OK);
    const char *class_val = fossil_media_html_get_attr(div, "class");
    ASSUME_ITS_EQUAL_CSTR(class_val, "container");

    fossil_media_html_free(doc);
}

FOSSIL_TEST_CASE(c_test_html_node_text) {
    const char *html = "<p>Hello <b>World</b></p>";
    fossil_media_html_doc_t *doc = NULL;
    int rc = fossil_media_html_load_string(html, &doc);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_MEDIA_HTML_OK);
    ASSUME_NOT_CNULL(doc);

    fossil_media_html_node_t *root = fossil_media_html_root(doc);
    fossil_media_html_node_t *p = fossil_media_html_find_by_tag(root, "p");
    ASSUME_NOT_CNULL(p);

    fossil_media_html_node_t *child = fossil_media_html_first_child(p);
    // First child should be a text node "Hello "
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(child), FOSSIL_MEDIA_HTML_NODE_TEXT);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_node_text(child), "Hello ");

    fossil_media_html_free(doc);
}

FOSSIL_TEST_CASE(c_test_html_serialize_roundtrip) {
    const char *html = "<div id=\"main\">Hello</div>";
    fossil_media_html_doc_t *doc = NULL;
    int rc = fossil_media_html_load_string(html, &doc);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_MEDIA_HTML_OK);
    ASSUME_NOT_CNULL(doc);

    char *out = fossil_media_html_serialize(doc);
    ASSUME_NOT_CNULL(out);
    // Optionally check substring presence
    ASSUME_ITS_TRUE(strstr(out, "div") != NULL);
    free(out);

    fossil_media_html_free(doc);
}


FOSSIL_TEST_CASE(c_test_html_empty_string) {
    fossil_media_html_doc_t *doc = NULL;
    int rc = fossil_media_html_load_string("", &doc);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_MEDIA_HTML_OK);
    ASSUME_NOT_CNULL(doc);

    fossil_media_html_node_t *root = fossil_media_html_root(doc);
    ASSUME_NOT_CNULL(root);
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(root), FOSSIL_MEDIA_HTML_NODE_DOCUMENT);

    fossil_media_html_free(doc);
}

FOSSIL_TEST_CASE(c_test_html_self_closing_tag) {
    const char *html = "<br/>";
    fossil_media_html_doc_t *doc = NULL;
    int rc = fossil_media_html_load_string(html, &doc);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_MEDIA_HTML_OK);
    ASSUME_NOT_CNULL(doc);

    fossil_media_html_node_t *root = fossil_media_html_root(doc);
    fossil_media_html_node_t *br = fossil_media_html_find_by_tag(root, "br");
    ASSUME_NOT_CNULL(br);
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(br), FOSSIL_MEDIA_HTML_NODE_ELEMENT);

    fossil_media_html_free(doc);
}

FOSSIL_TEST_CASE(c_test_html_comment_node) {
    const char *html = "<!-- This is a comment --><div></div>";
    fossil_media_html_doc_t *doc = NULL;
    int rc = fossil_media_html_load_string(html, &doc);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_MEDIA_HTML_OK);
    ASSUME_NOT_CNULL(doc);

    fossil_media_html_node_t *root = fossil_media_html_root(doc);
    fossil_media_html_node_t *comment = fossil_media_html_first_child(root);
    ASSUME_NOT_CNULL(comment);
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(comment), FOSSIL_MEDIA_HTML_NODE_COMMENT);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_node_text(comment), " This is a comment ");

    fossil_media_html_free(doc);
}

FOSSIL_TEST_CASE(c_test_html_cdata_node) {
    const char *html = "<![CDATA[Some <cdata> content]]><div></div>";
    fossil_media_html_doc_t *doc = NULL;
    int rc = fossil_media_html_load_string(html, &doc);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_MEDIA_HTML_OK);
    ASSUME_NOT_CNULL(doc);

    fossil_media_html_node_t *root = fossil_media_html_root(doc);
    fossil_media_html_node_t *cdata = fossil_media_html_first_child(root);
    ASSUME_NOT_CNULL(cdata);
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(cdata), FOSSIL_MEDIA_HTML_NODE_CDATA);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_node_text(cdata), "Some <cdata> content");

    fossil_media_html_free(doc);
}

FOSSIL_TEST_CASE(c_test_html_processing_instruction) {
    const char *html = "<?xml version=\"1.0\"?><div></div>";
    fossil_media_html_doc_t *doc = NULL;
    int rc = fossil_media_html_load_string(html, &doc);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_MEDIA_HTML_OK);
    ASSUME_NOT_CNULL(doc);

    fossil_media_html_node_t *root = fossil_media_html_root(doc);
    fossil_media_html_node_t *pi = fossil_media_html_first_child(root);
    ASSUME_NOT_CNULL(pi);
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(pi), FOSSIL_MEDIA_HTML_NODE_PROCESSING_INSTRUCTION);
    ASSUME_ITS_TRUE(strstr(fossil_media_html_node_text(pi), "xml version=\"1.0\"") != NULL);

    fossil_media_html_free(doc);
}

FOSSIL_TEST_CASE(c_test_html_multiple_attributes) {
    const char *html = "<input type=\"text\" name=\"username\" value=\"user1\"/>";
    fossil_media_html_doc_t *doc = NULL;
    int rc = fossil_media_html_load_string(html, &doc);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_MEDIA_HTML_OK);
    ASSUME_NOT_CNULL(doc);

    fossil_media_html_node_t *root = fossil_media_html_root(doc);
    fossil_media_html_node_t *input = fossil_media_html_find_by_tag(root, "input");
    ASSUME_NOT_CNULL(input);

    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_get_attr(input, "type"), "text");
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_get_attr(input, "name"), "username");
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_get_attr(input, "value"), "user1");

    fossil_media_html_free(doc);
}

FOSSIL_TEST_CASE(c_test_html_nested_elements) {
    const char *html = "<ul><li>One</li><li>Two</li></ul>";
    fossil_media_html_doc_t *doc = NULL;
    int rc = fossil_media_html_load_string(html, &doc);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_MEDIA_HTML_OK);
    ASSUME_NOT_CNULL(doc);

    fossil_media_html_node_t *root = fossil_media_html_root(doc);
    fossil_media_html_node_t *ul = fossil_media_html_find_by_tag(root, "ul");
    ASSUME_NOT_CNULL(ul);

    fossil_media_html_node_t *li1 = fossil_media_html_first_child(ul);
    ASSUME_NOT_CNULL(li1);
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(li1), FOSSIL_MEDIA_HTML_NODE_ELEMENT);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_node_tag(li1), "li");

    fossil_media_html_node_t *li2 = fossil_media_html_next_sibling(li1);
    ASSUME_NOT_CNULL(li2);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_node_tag(li2), "li");

    fossil_media_html_free(doc);
}


FOSSIL_TEST_CASE(c_test_html_unclosed_tag) {
    const char *html = "<div><span>Text";
    fossil_media_html_doc_t *doc = NULL;
    int rc = fossil_media_html_load_string(html, &doc);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_MEDIA_HTML_OK);
    ASSUME_NOT_CNULL(doc);

    fossil_media_html_node_t *root = fossil_media_html_root(doc);
    fossil_media_html_node_t *div = fossil_media_html_find_by_tag(root, "div");
    ASSUME_NOT_CNULL(div);

    fossil_media_html_node_t *span = fossil_media_html_find_by_tag(div, "span");
    ASSUME_NOT_CNULL(span);
    fossil_media_html_node_t *text = fossil_media_html_first_child(span);
    ASSUME_NOT_CNULL(text);
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(text), FOSSIL_MEDIA_HTML_NODE_TEXT);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_node_text(text), "Text");

    fossil_media_html_free(doc);
}

FOSSIL_TEST_CASE(c_test_html_attribute_no_quotes) {
    const char *html = "<div id=main></div>";
    fossil_media_html_doc_t *doc = NULL;
    int rc = fossil_media_html_load_string(html, &doc);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_MEDIA_HTML_OK);
    ASSUME_NOT_CNULL(doc);

    fossil_media_html_node_t *root = fossil_media_html_root(doc);
    fossil_media_html_node_t *div = fossil_media_html_find_by_tag(root, "div");
    ASSUME_NOT_CNULL(div);

    const char *id_val = fossil_media_html_get_attr(div, "id");
    ASSUME_ITS_EQUAL_CSTR(id_val, "main");

    fossil_media_html_free(doc);
}

FOSSIL_TEST_CASE(c_test_html_multiple_comments) {
    const char *html = "<!--A--><!--B--><div></div>";
    fossil_media_html_doc_t *doc = NULL;
    int rc = fossil_media_html_load_string(html, &doc);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_MEDIA_HTML_OK);
    ASSUME_NOT_CNULL(doc);

    fossil_media_html_node_t *root = fossil_media_html_root(doc);
    fossil_media_html_node_t *comment1 = fossil_media_html_first_child(root);
    ASSUME_NOT_CNULL(comment1);
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(comment1), FOSSIL_MEDIA_HTML_NODE_COMMENT);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_node_text(comment1), "A");

    fossil_media_html_node_t *comment2 = fossil_media_html_next_sibling(comment1);
    ASSUME_NOT_CNULL(comment2);
    ASSUME_ITS_EQUAL_I32(fossil_media_html_node_type(comment2), FOSSIL_MEDIA_HTML_NODE_COMMENT);
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_node_text(comment2), "B");

    fossil_media_html_free(doc);
}

FOSSIL_TEST_CASE(c_test_html_empty_tag) {
    const char *html = "<div></div>";
    fossil_media_html_doc_t *doc = NULL;
    int rc = fossil_media_html_load_string(html, &doc);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_MEDIA_HTML_OK);
    ASSUME_NOT_CNULL(doc);

    fossil_media_html_node_t *root = fossil_media_html_root(doc);
    fossil_media_html_node_t *div = fossil_media_html_find_by_tag(root, "div");
    ASSUME_NOT_CNULL(div);
    ASSUME_ITS_CNULL(fossil_media_html_first_child(div));

    fossil_media_html_free(doc);
}

FOSSIL_TEST_CASE(c_test_html_large_input_timeout) {
    size_t big_size = 2000000;
    char *big_html = (char*)malloc(big_size + 32);
    memset(big_html, 'a', big_size);
    strcpy(big_html, "<div>");
    memset(big_html + 5, 'a', big_size - 5);
    strcpy(big_html + big_size - 6, "</div>");
    big_html[big_size + 1] = '\0';

    fossil_media_html_doc_t *doc = NULL;
    int rc = fossil_media_html_load_string(big_html, &doc);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_MEDIA_HTML_ERR_TIMEOUT);
    ASSUME_ITS_CNULL(doc);

    free(big_html);
}

FOSSIL_TEST_CASE(c_test_html_tag_with_single_quotes) {
    const char *html = "<div id='main' class='container'></div>";
    fossil_media_html_doc_t *doc = NULL;
    int rc = fossil_media_html_load_string(html, &doc);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_MEDIA_HTML_OK);
    ASSUME_NOT_CNULL(doc);

    fossil_media_html_node_t *root = fossil_media_html_root(doc);
    fossil_media_html_node_t *div = fossil_media_html_find_by_tag(root, "div");
    ASSUME_NOT_CNULL(div);

    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_get_attr(div, "id"), "main");
    ASSUME_ITS_EQUAL_CSTR(fossil_media_html_get_attr(div, "class"), "container");

    fossil_media_html_free(doc);
}

FOSSIL_TEST_CASE(c_test_html_text_outside_tags) {
    const char *html = "Hello<div>World</div>!";
    fossil_media_html_doc_t *doc = NULL;
    int rc = fossil_media_html_load_string(html, &doc);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_MEDIA_HTML_OK);
    ASSUME_NOT_CNULL(doc);

    fossil_media_html_node_t *root = fossil_media_html_root(doc);
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

    fossil_media_html_free(doc);
}

FOSSIL_TEST_CASE(c_test_html_complete_document) {
    const char *html =
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

    fossil_media_html_doc_t *doc = NULL;
    int rc = fossil_media_html_load_string(html, &doc);
    ASSUME_ITS_EQUAL_I32(rc, FOSSIL_MEDIA_HTML_OK);
    ASSUME_NOT_CNULL(doc);

    fossil_media_html_node_t *root = fossil_media_html_root(doc);
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

    fossil_media_html_free(doc);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_html_tests) {
    FOSSIL_TEST_ADD(c_html_fixture, c_test_html_load_string_and_root);
    FOSSIL_TEST_ADD(c_html_fixture, c_test_html_find_by_tag);
    FOSSIL_TEST_ADD(c_html_fixture, c_test_html_get_and_set_attr);
    FOSSIL_TEST_ADD(c_html_fixture, c_test_html_node_text);
    FOSSIL_TEST_ADD(c_html_fixture, c_test_html_serialize_roundtrip);
    FOSSIL_TEST_ADD(c_html_fixture, c_test_html_empty_string);
    FOSSIL_TEST_ADD(c_html_fixture, c_test_html_self_closing_tag);
    FOSSIL_TEST_ADD(c_html_fixture, c_test_html_comment_node);
    FOSSIL_TEST_ADD(c_html_fixture, c_test_html_cdata_node);
    FOSSIL_TEST_ADD(c_html_fixture, c_test_html_processing_instruction);
    FOSSIL_TEST_ADD(c_html_fixture, c_test_html_multiple_attributes);
    FOSSIL_TEST_ADD(c_html_fixture, c_test_html_nested_elements);
    FOSSIL_TEST_ADD(c_html_fixture, c_test_html_unclosed_tag);
    FOSSIL_TEST_ADD(c_html_fixture, c_test_html_attribute_no_quotes);
    FOSSIL_TEST_ADD(c_html_fixture, c_test_html_multiple_comments);
    FOSSIL_TEST_ADD(c_html_fixture, c_test_html_empty_tag);
    FOSSIL_TEST_ADD(c_html_fixture, c_test_html_large_input_timeout);
    FOSSIL_TEST_ADD(c_html_fixture, c_test_html_tag_with_single_quotes);
    FOSSIL_TEST_ADD(c_html_fixture, c_test_html_text_outside_tags);
    FOSSIL_TEST_ADD(c_html_fixture, c_test_html_complete_document);

    FOSSIL_TEST_REGISTER(c_html_fixture);
}
