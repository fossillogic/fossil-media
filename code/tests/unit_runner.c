
// Generated Fossil Logic Test Runner
#include <fossil/pizza/framework.h>

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test List
// * * * * * * * * * * * * * * * * * * * * * * * *
FOSSIL_TEST_EXPORT(cpp_ini_tests);
FOSSIL_TEST_EXPORT(cpp_text_tests);
FOSSIL_TEST_EXPORT(c_json_tests);
FOSSIL_TEST_EXPORT(cpp_yaml_tests);
FOSSIL_TEST_EXPORT(c_markdown_tests);
FOSSIL_TEST_EXPORT(c_csv_tests);
FOSSIL_TEST_EXPORT(cpp_json_tests);
FOSSIL_TEST_EXPORT(c_xml_tests);
FOSSIL_TEST_EXPORT(c_ini_tests);
FOSSIL_TEST_EXPORT(c_yaml_tests);
FOSSIL_TEST_EXPORT(cpp_xml_tests);
FOSSIL_TEST_EXPORT(cpp_markdown_tests);
FOSSIL_TEST_EXPORT(cpp_csv_tests);
FOSSIL_TEST_EXPORT(c_text_tests);

// * * * * * * * * * * * * * * * * * * * * * * * *
// * Fossil Logic Test Runner
// * * * * * * * * * * * * * * * * * * * * * * * *
int main(int argc, char **argv) {
    FOSSIL_TEST_START(argc, argv);
    FOSSIL_TEST_IMPORT(cpp_ini_tests);
    FOSSIL_TEST_IMPORT(cpp_text_tests);
    FOSSIL_TEST_IMPORT(c_json_tests);
    FOSSIL_TEST_IMPORT(cpp_yaml_tests);
    FOSSIL_TEST_IMPORT(c_markdown_tests);
    FOSSIL_TEST_IMPORT(c_csv_tests);
    FOSSIL_TEST_IMPORT(cpp_json_tests);
    FOSSIL_TEST_IMPORT(c_xml_tests);
    FOSSIL_TEST_IMPORT(c_ini_tests);
    FOSSIL_TEST_IMPORT(c_yaml_tests);
    FOSSIL_TEST_IMPORT(cpp_xml_tests);
    FOSSIL_TEST_IMPORT(cpp_markdown_tests);
    FOSSIL_TEST_IMPORT(cpp_csv_tests);
    FOSSIL_TEST_IMPORT(c_text_tests);

    FOSSIL_RUN_ALL();
    FOSSIL_SUMMARY();
    return FOSSIL_END();
} // end of main
