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

FOSSIL_TEST_SUITE(c_csv_fixture);

FOSSIL_SETUP(c_csv_fixture) {
    // Setup the test fixture
}

FOSSIL_TEARDOWN(c_csv_fixture) {
    // Teardown the test fixture
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Cases
// * * * * * * * * * * * * * * * * * * * * * * * *
// The test cases below are provided as samples, inspired
// by the Meson build system's approach of using test cases
// as samples for library usage.
// * * * * * * * * * * * * * * * * * * * * * * * *

FOSSIL_TEST_CASE(c_test_parse_simple_csv) {
    const char *csv = "a,b,c\n1,2,3\n";
    fossil_media_csv_error_t err;
    fossil_media_csv_doc_t *doc = fossil_media_csv_parse(csv, ',', &err);
    ASSUME_ITS_TRUE(doc != NULL);
    ASSUME_ITS_TRUE(err == FOSSIL_MEDIA_CSV_OK);
    ASSUME_ITS_TRUE(doc->row_count == 2);
    ASSUME_ITS_TRUE(doc->rows[0].field_count == 3);
    ASSUME_ITS_TRUE(strcmp(doc->rows[0].fields[0], "a") == 0);
    ASSUME_ITS_TRUE(strcmp(doc->rows[1].fields[2], "3") == 0);
    fossil_media_csv_free(doc);
}

FOSSIL_TEST_CASE(c_test_parse_quoted_fields) {
    const char *csv = "\"a\",\"b\",\"c\"\n\"1,2\",\"3\",\"4\"\n";
    fossil_media_csv_error_t err;
    fossil_media_csv_doc_t *doc = fossil_media_csv_parse(csv, ',', &err);
    ASSUME_ITS_TRUE(doc != NULL);
    ASSUME_ITS_TRUE(doc->row_count == 2);
    ASSUME_ITS_TRUE(strcmp(doc->rows[1].fields[0], "1,2") == 0);
    fossil_media_csv_free(doc);
}

FOSSIL_TEST_CASE(c_test_parse_empty_fields) {
    const char *csv = "a,,c\n,,\n";
    fossil_media_csv_error_t err;
    fossil_media_csv_doc_t *doc = fossil_media_csv_parse(csv, ',', &err);
    ASSUME_ITS_TRUE(doc != NULL);
    ASSUME_ITS_TRUE(doc->row_count == 2);
    ASSUME_ITS_TRUE(strcmp(doc->rows[0].fields[1], "") == 0);
    ASSUME_ITS_TRUE(strcmp(doc->rows[1].fields[2], "") == 0);
    fossil_media_csv_free(doc);
}

FOSSIL_TEST_CASE(c_test_stringify_roundtrip) {
    const char *csv = "a,b,\"c,d\"\n1,2,3\n";
    fossil_media_csv_error_t err;
    fossil_media_csv_doc_t *doc = fossil_media_csv_parse(csv, ',', &err);
    ASSUME_ITS_TRUE(doc != NULL);
    char *out = fossil_media_csv_stringify(doc, ',', &err);
    ASSUME_ITS_TRUE(out != NULL);
    // Should contain all original fields (not necessarily identical formatting)
    ASSUME_ITS_TRUE(strstr(out, "c,d") != NULL);
    fossil_media_csv_free(doc);
    free(out);
}

FOSSIL_TEST_CASE(c_test_append_row) {
    fossil_media_csv_error_t err;
    fossil_media_csv_doc_t *doc = fossil_media_csv_parse("a,b\n", ',', &err);
    ASSUME_ITS_TRUE(doc != NULL);
    const char *fields[] = {"1", "2"};
    int rc = fossil_media_csv_append_row(doc, fields, 2);
    ASSUME_ITS_TRUE(rc == 0);
    ASSUME_ITS_TRUE(doc->row_count == 2);
    ASSUME_ITS_TRUE(strcmp(doc->rows[1].fields[1], "2") == 0);
    fossil_media_csv_free(doc);
}

FOSSIL_TEST_CASE(c_test_parse_invalid_input) {
    fossil_media_csv_error_t err = FOSSIL_MEDIA_CSV_OK;
    fossil_media_csv_doc_t *doc = fossil_media_csv_parse(NULL, ',', &err);
    ASSUME_ITS_TRUE(doc == NULL);
    ASSUME_ITS_TRUE(err == FOSSIL_MEDIA_CSV_ERR_INVALID_ARG);
}

FOSSIL_TEST_CASE(c_test_parse_single_row) {
    const char *csv = "foo,bar,baz\n";
    fossil_media_csv_error_t err;
    fossil_media_csv_doc_t *doc = fossil_media_csv_parse(csv, ',', &err);
    ASSUME_ITS_TRUE(doc != NULL);
    ASSUME_ITS_TRUE(doc->row_count == 1);
    ASSUME_ITS_TRUE(strcmp(doc->rows[0].fields[0], "foo") == 0);
    fossil_media_csv_free(doc);
}

FOSSIL_TEST_CASE(c_test_parse_trailing_newline) {
    const char *csv = "x,y,z\n1,2,3\n\n";
    fossil_media_csv_error_t err;
    fossil_media_csv_doc_t *doc = fossil_media_csv_parse(csv, ',', &err);
    ASSUME_ITS_TRUE(doc != NULL);
    ASSUME_ITS_TRUE(doc->row_count == 3);
    fossil_media_csv_free(doc);
}

FOSSIL_TEST_CASE(c_test_parse_custom_delimiter) {
    const char *csv = "a;b;c\n1;2;3\n";
    fossil_media_csv_error_t err;
    fossil_media_csv_doc_t *doc = fossil_media_csv_parse(csv, ';', &err);
    ASSUME_ITS_TRUE(doc != NULL);
    ASSUME_ITS_TRUE(doc->row_count == 2);
    ASSUME_ITS_TRUE(strcmp(doc->rows[1].fields[2], "3") == 0);
    fossil_media_csv_free(doc);
}

FOSSIL_TEST_CASE(c_test_stringify_empty_doc) {
    fossil_media_csv_doc_t empty_doc = {0};
    fossil_media_csv_error_t err;
    char *out = fossil_media_csv_stringify(&empty_doc, ',', &err);
    ASSUME_ITS_TRUE(out != NULL);
    ASSUME_ITS_TRUE(strlen(out) == 0);
    free(out);
}

FOSSIL_TEST_CASE(c_test_parse_only_newlines) {
    const char *csv = "\n\n\n";
    fossil_media_csv_error_t err;
    fossil_media_csv_doc_t *doc = fossil_media_csv_parse(csv, ',', &err);
    ASSUME_ITS_TRUE(doc != NULL);
    ASSUME_ITS_TRUE(doc->row_count == 3);
    fossil_media_csv_free(doc);
}

FOSSIL_TEST_CASE(c_test_parse_only_delimiters) {
    const char *csv = ",,,\n,,,\n";
    fossil_media_csv_error_t err;
    fossil_media_csv_doc_t *doc = fossil_media_csv_parse(csv, ',', &err);
    ASSUME_ITS_TRUE(doc != NULL);
    ASSUME_ITS_TRUE(doc->row_count == 2);
    ASSUME_ITS_TRUE(doc->rows[0].field_count == 4);
    ASSUME_ITS_TRUE(strcmp(doc->rows[0].fields[2], "") == 0);
    fossil_media_csv_free(doc);
}

FOSSIL_TEST_CASE(c_test_parse_escaped_quotes) {
    const char *csv = "\"a\"\"b\",c\n";
    fossil_media_csv_error_t err;
    fossil_media_csv_doc_t *doc = fossil_media_csv_parse(csv, ',', &err);
    ASSUME_ITS_TRUE(doc != NULL);
    ASSUME_ITS_TRUE(strcmp(doc->rows[0].fields[0], "a\"b") == 0);
    fossil_media_csv_free(doc);
}

FOSSIL_TEST_CASE(c_test_parse_long_field) {
    char long_field[1024];
    memset(long_field, 'x', sizeof(long_field) - 1);
    long_field[sizeof(long_field) - 1] = '\0';
    char csv[1100];
    snprintf(csv, sizeof(csv), "%s,1\n", long_field);
    fossil_media_csv_error_t err;
    fossil_media_csv_doc_t *doc = fossil_media_csv_parse(csv, ',', &err);
    ASSUME_ITS_TRUE(doc != NULL);
    ASSUME_ITS_TRUE(strcmp(doc->rows[0].fields[0], long_field) == 0);
    fossil_media_csv_free(doc);
}

FOSSIL_TEST_CASE(c_test_parse_no_fields) {
    const char *csv = "";
    fossil_media_csv_error_t err;
    fossil_media_csv_doc_t *doc = fossil_media_csv_parse(csv, ',', &err);
    ASSUME_ITS_TRUE(doc != NULL);
    ASSUME_ITS_TRUE(doc->row_count == 0);
    fossil_media_csv_free(doc);
}

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Pool
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_GROUP(c_csv_tests) {    
    FOSSIL_TEST_ADD(c_csv_fixture, c_test_parse_simple_csv);
    FOSSIL_TEST_ADD(c_csv_fixture, c_test_parse_quoted_fields);
    FOSSIL_TEST_ADD(c_csv_fixture, c_test_parse_empty_fields);
    FOSSIL_TEST_ADD(c_csv_fixture, c_test_stringify_roundtrip);
    FOSSIL_TEST_ADD(c_csv_fixture, c_test_append_row);
    FOSSIL_TEST_ADD(c_csv_fixture, c_test_parse_invalid_input);
    FOSSIL_TEST_ADD(c_csv_fixture, c_test_parse_single_row);
    FOSSIL_TEST_ADD(c_csv_fixture, c_test_parse_trailing_newline);
    FOSSIL_TEST_ADD(c_csv_fixture, c_test_parse_custom_delimiter);
    FOSSIL_TEST_ADD(c_csv_fixture, c_test_stringify_empty_doc);
    FOSSIL_TEST_ADD(c_csv_fixture, c_test_parse_only_newlines);
    FOSSIL_TEST_ADD(c_csv_fixture, c_test_parse_only_delimiters);
    FOSSIL_TEST_ADD(c_csv_fixture, c_test_parse_escaped_quotes);
    FOSSIL_TEST_ADD(c_csv_fixture, c_test_parse_long_field);
    FOSSIL_TEST_ADD(c_csv_fixture, c_test_parse_no_fields);

    FOSSIL_TEST_REGISTER(c_csv_fixture);
} // end of tests
