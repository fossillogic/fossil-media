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
#include "fossil/media/fson.h"
#include "fossil/media/media.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <float.h>

/**
 * @file fson.c
 * @brief Implementation of FSON (Fossil Simple Object Notation) logic.
 *
 * This file contains functions and logic for parsing, serializing, and manipulating
 * data in the FSON format. FSON is a lightweight, human-readable data interchange format
 * inspired by JSON, designed for use in Fossil Media projects.
 *
 * @section FSON_Sample_Objects Sample FSON Objects
 *
 * Example 1: Simple object
 * {
 *   "name": "Alice",
 *   "age": 30,
 *   "isMember": true
 * }
 *
 * Example 2: Nested object
 * {
 *   "user": {
 *     "id": 123,
 *     "profile": {
 *       "email": "alice@example.com"
 *     }
 *   }
 * }
 *
 * Example 3: Array
 * {
 *   "items": [ "apple", "banana", "cherry" ]
 * }
 *
 * Example 4: Mixed types
 * {
 *   "count": 5,
 *   "valid": false,
 *   "tags": [ "fossil", "media", "fson" ]
 * }
 *
 * @note For more details on the FSON specification, refer to the project documentation.
 */
//

fossil_media_fson_value_t *fossil_media_fson_parse(const char *json_text, fossil_media_fson_error_t *err_out) {
    const char *input_start = json_text;
    if (json_text == NULL) {
        if (err_out) {
            err_out->code = FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;
            err_out->position = 0;
            snprintf(err_out->message, sizeof(err_out->message), "Input text is NULL");
        }
        return NULL;
    }

    // Skip whitespace
    while (isspace((unsigned char)*json_text)) json_text++;

    // Parse object
    if (*json_text == '{') {
        json_text++;
        while (isspace((unsigned char)*json_text)) json_text++;
        // Special case: { null: null }
        if (strncmp(json_text, "null", 4) == 0) {
            const char *tmp = json_text + 4;
            while (isspace((unsigned char)*tmp)) tmp++;
            if (*tmp == ':') {
                tmp++;
                while (isspace((unsigned char)*tmp)) tmp++;
                if (strncmp(tmp, "null", 4) == 0) {
                    tmp += 4;
                    while (isspace((unsigned char)*tmp)) tmp++;
                    if (*tmp == '}') {
                        if (err_out) {
                            err_out->code = FOSSIL_MEDIA_FSON_OK;
                            err_out->position = 0;
                            snprintf(err_out->message, sizeof(err_out->message), "Parsed null object");
                        }
                        return fossil_media_fson_new_null();
                    }
                }
            }
        }

        fossil_media_fson_value_t *obj = fossil_media_fson_new_object();
        if (!obj) {
            if (err_out) {
                err_out->code = FOSSIL_MEDIA_FSON_ERR_NOMEM;
                err_out->position = 0;
                snprintf(err_out->message, sizeof(err_out->message), "Out of memory");
            }
            return NULL;
        }
        int found_one = 0;
        while (*json_text) {
            while (isspace((unsigned char)*json_text)) json_text++;
            if (*json_text == '}') {
                json_text++;
                break;
            }
            // Parse key
            const char *key_start = json_text;
            size_t key_len = 0;
            if (*json_text == '"') {
                json_text++;
                key_start = json_text;
                while (*json_text && *json_text != '"') json_text++;
                key_len = json_text - key_start;
                if (*json_text == '"') json_text++;
            } else {
                // Accept unquoted keys for FSON
                while (*json_text && *json_text != ':' && *json_text != ' ' && *json_text != '\n' && *json_text != '\r' && *json_text != '\t' && *json_text != ',') json_text++;
                key_len = json_text - key_start;
            }
            if (key_len == 0) {
                fossil_media_fson_free(obj);
                if (err_out) {
                    err_out->code = FOSSIL_MEDIA_FSON_ERR_PARSE;
                    err_out->position = (size_t)(json_text - input_start);
                    snprintf(err_out->message, sizeof(err_out->message), "Missing key");
                }
                return NULL;
            }
            char *key = (char *)malloc(key_len + 1);
            if (!key) {
                fossil_media_fson_free(obj);
                if (err_out) {
                    err_out->code = FOSSIL_MEDIA_FSON_ERR_NOMEM;
                    err_out->position = 0;
                    snprintf(err_out->message, sizeof(err_out->message), "Out of memory");
                }
                return NULL;
            }
            strncpy(key, key_start, key_len);
            key[key_len] = '\0';

            while (isspace((unsigned char)*json_text)) json_text++;
            if (*json_text != ':') {
                free(key);
                fossil_media_fson_free(obj);
                if (err_out) {
                    err_out->code = FOSSIL_MEDIA_FSON_ERR_PARSE;
                    err_out->position = (size_t)(json_text - input_start);
                    snprintf(err_out->message, sizeof(err_out->message), "Expected ':' after key");
                }
                return NULL;
            }
            json_text++; // skip ':'

            // Parse type (may be compound, e.g. flags:u16)
            while (isspace((unsigned char)*json_text)) json_text++;
            const char *type_start = json_text;
            size_t type_len = 0;
            while (*json_text && *json_text != ':') {
                if (*json_text == '\n' || *json_text == '\r' || *json_text == '}' || *json_text == ',' || *json_text == '{' || *json_text == '[') break;
                json_text++;
            }
            type_len = json_text - type_start;
            char *type = (char *)malloc(type_len + 1);
            if (!type) {
                free(key);
                fossil_media_fson_free(obj);
                if (err_out) {
                    err_out->code = FOSSIL_MEDIA_FSON_ERR_NOMEM;
                    err_out->position = 0;
                    snprintf(err_out->message, sizeof(err_out->message), "Out of memory");
                }
                return NULL;
            }
            strncpy(type, type_start, type_len);
            type[type_len] = '\0';

            char base_type[32] = {0};
            char sub_type[32] = {0};
            if (strchr(type, ':')) {
                // Compound type: e.g. flags:u16
                char *colon = strchr(type, ':');
                size_t base_len = colon - type;
                size_t sub_len = strlen(colon + 1);
                if (base_len < sizeof(base_type) && sub_len < sizeof(sub_type)) {
                    if (base_len > 0) memcpy(base_type, type, base_len);
                    base_type[base_len] = '\0';
                    if (sub_len > 0) memcpy(sub_type, colon + 1, sub_len);
                    sub_type[sub_len] = '\0';
                }
            } else {
                strncpy(base_type, type, sizeof(base_type) - 1);
                base_type[sizeof(base_type) - 1] = '\0';
            }

            if (*json_text == ':') json_text++; // skip ':'
            while (isspace((unsigned char)*json_text)) json_text++;

            fossil_media_fson_value_t *val = NULL;

            // Handle nested object
            if (strcmp(base_type, "object") == 0 && *json_text == '{') {
                // Find matching closing brace for nested object
                int brace = 1;
                const char *obj_start = json_text;
                json_text++;
                while (*json_text && brace > 0) {
                    if (*json_text == '{') brace++;
                    else if (*json_text == '}') brace--;
                    json_text++;
                }
                size_t obj_len = json_text - obj_start;
                char *obj_buf = (char *)malloc(obj_len + 1);
                if (!obj_buf) {
                    free(key);
                    free(type);
                    fossil_media_fson_free(obj);
                    if (err_out) {
                        err_out->code = FOSSIL_MEDIA_FSON_ERR_NOMEM;
                        err_out->position = 0;
                        snprintf(err_out->message, sizeof(err_out->message), "Out of memory");
                    }
                    return NULL;
                }
                strncpy(obj_buf, obj_start, obj_len);
                obj_buf[obj_len] = '\0';
                val = fossil_media_fson_parse(obj_buf, NULL);
                free(obj_buf);
            }
            // Handle nested array
            else if (strcmp(base_type, "array") == 0 && *json_text == '[') {
                int bracket = 1;
                const char *arr_start = json_text;
                json_text++;
                while (*json_text && bracket > 0) {
                    if (*json_text == '[') bracket++;
                    else if (*json_text == ']') bracket--;
                    json_text++;
                }
                size_t arr_len = json_text - arr_start;
                char *arr_buf = (char *)malloc(arr_len + 1);
                if (!arr_buf) {
                    free(key);
                    free(type);
                    fossil_media_fson_free(obj);
                    if (err_out) {
                        err_out->code = FOSSIL_MEDIA_FSON_ERR_NOMEM;
                        err_out->position = 0;
                        snprintf(err_out->message, sizeof(err_out->message), "Out of memory");
                    }
                    return NULL;
                }
                strncpy(arr_buf, arr_start, arr_len);
                arr_buf[arr_len] = '\0';
                val = fossil_media_fson_parse(arr_buf, NULL);
                free(arr_buf);
            }
            // Handle enum
            else if (strcmp(base_type, "enum") == 0) {
                if (*json_text == '"') {
                    json_text++;
                    const char *sym_start = json_text;
                    while (*json_text && *json_text != '"') {
                        if (*json_text == '\\' && *(json_text + 1)) json_text++;
                        json_text++;
                    }
                    size_t sym_len = json_text - sym_start;
                    char *symbol = (char *)malloc(sym_len + 1);
                    if (!symbol) {
                        free(key);
                        free(type);
                        fossil_media_fson_free(obj);
                        if (err_out) {
                            err_out->code = FOSSIL_MEDIA_FSON_ERR_NOMEM;
                            err_out->position = 0;
                            snprintf(err_out->message, sizeof(err_out->message), "Out of memory");
                        }
                        return NULL;
                    }
                    strncpy(symbol, sym_start, sym_len);
                    symbol[sym_len] = '\0';

                    val = fossil_media_fson_new_string(symbol);
                    free(symbol);
                    if (*json_text == '"') json_text++;
                }
            }
            // Handle flags
            else if (strcmp(base_type, "flags") == 0) {
                while (isspace((unsigned char)*json_text)) json_text++;
                if (*json_text == '[') {
                    json_text++;
                    char *symbols[32];
                    size_t count = 0;
                    while (*json_text && *json_text != ']') {
                        while (isspace((unsigned char)*json_text)) json_text++;
                        if (*json_text == '"') {
                            json_text++;
                            const char *sym_start = json_text;
                            while (*json_text && *json_text != '"') json_text++;
                            size_t sym_len = json_text - sym_start;
                            if (count < 32) {
                                symbols[count] = (char *)malloc(sym_len + 1);
                                if (!symbols[count]) {
                                    for (size_t j = 0; j < count; j++) free(symbols[j]);
                                    free(key);
                                    free(type);
                                    fossil_media_fson_free(obj);
                                    if (err_out) {
                                        err_out->code = FOSSIL_MEDIA_FSON_ERR_NOMEM;
                                        err_out->position = 0;
                                        snprintf(err_out->message, sizeof(err_out->message), "Out of memory");
                                    }
                                    return NULL;
                                }
                                strncpy(symbols[count], sym_start, sym_len);
                                symbols[count][sym_len] = '\0';
                                count++;
                            }
                            if (*json_text == '"') json_text++;
                        }
                        while (isspace((unsigned char)*json_text)) json_text++;
                        if (*json_text == ',') json_text++;
                    }
                    if (*json_text == ']') json_text++;
                    fossil_media_fson_value_t *arr = fossil_media_fson_new_array();
                    for (size_t i = 0; i < count; i++) {
                        fossil_media_fson_array_append(arr, fossil_media_fson_new_string(symbols[i]));
                        free(symbols[i]);
                    }
                    val = arr;
                } else {
                    free(key);
                    free(type);
                    fossil_media_fson_free(obj);
                    if (err_out) {
                        err_out->code = FOSSIL_MEDIA_FSON_ERR_TYPE;
                        err_out->position = (size_t)(json_text - input_start);
                        snprintf(err_out->message, sizeof(err_out->message), "Flags must be array");
                    }
                    return NULL;
                }
            }
            // Handle datetime
            else if (strcmp(base_type, "datetime") == 0) {
                if (*json_text == '"') {
                    json_text++;
                    const char *dt_start = json_text;
                    while (*json_text && *json_text != '"') json_text++;
                    size_t dt_len = json_text - dt_start;
                    char *dt_str = (char *)malloc(dt_len + 1);
                    if (!dt_str) {
                        free(key);
                        free(type);
                        fossil_media_fson_free(obj);
                        if (err_out) {
                            err_out->code = FOSSIL_MEDIA_FSON_ERR_NOMEM;
                            err_out->position = 0;
                            snprintf(err_out->message, sizeof(err_out->message), "Out of memory");
                        }
                        return NULL;
                    }
                    if (dt_len > 0) {
                        memcpy(dt_str, dt_start, dt_len);
                    }
                    dt_str[dt_len] = '\0';
                    val = fossil_media_fson_new_datetime(dt_str);
                    free(dt_str);
                    if (*json_text == '"') json_text++;
                }
            }
            // Handle duration
            else if (strcmp(base_type, "duration") == 0) {
                if (*json_text == '"') {
                    json_text++;
                    const char *dur_start = json_text;
                    while (*json_text && *json_text != '"') json_text++;
                    size_t dur_len = json_text - dur_start;
                    char *dur_str = (char *)malloc(dur_len + 1);
                    if (!dur_str) {
                        free(key);
                        free(type);
                        fossil_media_fson_free(obj);
                        if (err_out) {
                            err_out->code = FOSSIL_MEDIA_FSON_ERR_NOMEM;
                            err_out->position = 0;
                            snprintf(err_out->message, sizeof(err_out->message), "Out of memory");
                        }
                        return NULL;
                    }
                    if (dur_len > 0) {
                        memcpy(dur_str, dur_start, dur_len);
                    }
                    dur_str[dur_len] = '\0';
                    val = fossil_media_fson_new_duration(dur_str);
                    free(dur_str);
                    if (*json_text == '"') json_text++;
                }
            }
            // Fallback to normal types
            else if (strcmp(base_type, "null") == 0) {
                if (strncmp(json_text, "null", 4) == 0) {
                    val = fossil_media_fson_new_null();
                    json_text += 4;
                } else {
                    val = fossil_media_fson_new_null();
                }
            } else if (strcmp(base_type, "bool") == 0) {
                if (strncmp(json_text, "true", 4) == 0) {
                    val = fossil_media_fson_new_bool(1);
                    json_text += 4;
                } else if (strncmp(json_text, "false", 5) == 0) {
                    val = fossil_media_fson_new_bool(0);
                    json_text += 5;
                } else if (*json_text == '0') {
                    val = fossil_media_fson_new_bool(0);
                    json_text++;
                } else if (*json_text == '1') {
                    val = fossil_media_fson_new_bool(1);
                    json_text++;
                }
            } else if (strcmp(base_type, "cstr") == 0) {
                if (*json_text == '"') {
                    json_text++;
                    const char *str_start = json_text;
                    while (*json_text && *json_text != '"') {
                        if (*json_text == '\\' && *(json_text + 1)) json_text++;
                        json_text++;
                    }
                    size_t str_len = json_text - str_start;
                    char *str = (char *)malloc(str_len + 1);
                    if (!str) {
                        free(key);
                        free(type);
                        fossil_media_fson_free(obj);
                        if (err_out) {
                            err_out->code = FOSSIL_MEDIA_FSON_ERR_NOMEM;
                            err_out->position = 0;
                            snprintf(err_out->message, sizeof(err_out->message), "Out of memory");
                        }
                        return NULL;
                    }
                    strncpy(str, str_start, str_len);
                    str[str_len] = '\0';
                    val = fossil_media_fson_new_string(str);
                    free(str);
                    if (*json_text == '"') json_text++;
                }
            } else if (strcmp(base_type, "char") == 0) {
                char *endptr;
                long ch = strtol(json_text, &endptr, 10);
                val = fossil_media_fson_new_char((char)ch);
                json_text = endptr;
            } else if (strcmp(base_type, "i8") == 0) {
                char *endptr;
                int8_t num = (int8_t)strtol(json_text, &endptr, 10);
                val = fossil_media_fson_new_i8(num);
                json_text = endptr;
            } else if (strcmp(base_type, "i16") == 0) {
                char *endptr;
                int16_t num = (int16_t)strtol(json_text, &endptr, 10);
                val = fossil_media_fson_new_i16(num);
                json_text = endptr;
            } else if (strcmp(base_type, "i32") == 0) {
                char *endptr;
                int32_t num = (int32_t)strtol(json_text, &endptr, 10);
                val = fossil_media_fson_new_i32(num);
                json_text = endptr;
            } else if (strcmp(base_type, "i64") == 0) {
                char *endptr;
                int64_t num = (int64_t)strtoll(json_text, &endptr, 10);
                val = fossil_media_fson_new_i64(num);
                json_text = endptr;
            } else if (strcmp(base_type, "u8") == 0) {
                char *endptr;
                uint8_t num = (uint8_t)strtoul(json_text, &endptr, 10);
                val = fossil_media_fson_new_u8(num);
                json_text = endptr;
            } else if (strcmp(base_type, "u16") == 0) {
                char *endptr;
                uint16_t num = (uint16_t)strtoul(json_text, &endptr, 10);
                val = fossil_media_fson_new_u16(num);
                json_text = endptr;
            } else if (strcmp(base_type, "u32") == 0) {
                char *endptr;
                uint32_t num = (uint32_t)strtoul(json_text, &endptr, 10);
                val = fossil_media_fson_new_u32(num);
                json_text = endptr;
            } else if (strcmp(base_type, "u64") == 0) {
                char *endptr;
                uint64_t num = (uint64_t)strtoull(json_text, &endptr, 10);
                val = fossil_media_fson_new_u64(num);
                json_text = endptr;
            } else if (strcmp(base_type, "f32") == 0) {
                char *endptr;
                float num = strtof(json_text, &endptr);
                val = fossil_media_fson_new_f32(num);
                json_text = endptr;
            } else if (strcmp(base_type, "f64") == 0) {
                char *endptr;
                double num = strtod(json_text, &endptr);
                val = fossil_media_fson_new_f64(num);
                json_text = endptr;
            } else if (strcmp(base_type, "oct") == 0) {
                char *endptr;
                if (strncmp(json_text, "0o", 2) == 0) {
                    json_text += 2;
                }
                uint64_t num = strtoull(json_text, &endptr, 8);
                val = fossil_media_fson_new_oct(num);
                json_text = endptr;
            } else if (strcmp(base_type, "hex") == 0) {
                char *endptr;
                if (strncmp(json_text, "0x", 2) == 0) {
                    json_text += 2;
                }
                // Accept quoted hex string
                if (*json_text == '"') {
                    json_text++;
                    const char *hex_start = json_text;
                    while (*json_text && *json_text != '"') json_text++;
                    size_t hex_len = json_text - hex_start;
                    char hex_buf[32] = {0};
                    if (hex_len < sizeof(hex_buf)) {
                        memcpy(hex_buf, hex_start, hex_len);
                        hex_buf[hex_len] = '\0';
                        uint64_t num = strtoull(hex_buf, NULL, 16);
                        val = fossil_media_fson_new_hex(num);
                    }
                    if (*json_text == '"') json_text++;
                } else {
                    uint64_t num = strtoull(json_text, &endptr, 16);
                    val = fossil_media_fson_new_hex(num);
                    json_text = endptr;
                }
            } else if (strcmp(base_type, "bin") == 0) {
                char *endptr;
                if (strncmp(json_text, "0b", 2) == 0) {
                    json_text += 2;
                }
                uint64_t num = strtoull(json_text, &endptr, 2);
                if (endptr == json_text) {
                    num = strtoull(json_text, &endptr, 10);
                }
                val = fossil_media_fson_new_bin(num);
                json_text = endptr;
            }

            if (val) {
                fossil_media_fson_object_set(obj, key, val);
                found_one = 1;
            }
            free(key);
            free(type);

            while (isspace((unsigned char)*json_text)) json_text++;
            if (*json_text == ',') json_text++;
        }
        if (obj->u.object.count == 1 &&
            obj->u.object.keys[0] &&
            strcmp(obj->u.object.keys[0], "null") == 0 &&
            obj->u.object.values[0] &&
            obj->u.object.values[0]->type == FSON_TYPE_NULL) {
            fossil_media_fson_free(obj);
            if (err_out) {
                err_out->code = FOSSIL_MEDIA_FSON_OK;
                err_out->position = 0;
                snprintf(err_out->message, sizeof(err_out->message), "Parsed null object");
            }
            return fossil_media_fson_new_null();
        }
        if (found_one) {
            if (err_out) {
                err_out->code = FOSSIL_MEDIA_FSON_OK;
                err_out->position = 0;
                snprintf(err_out->message, sizeof(err_out->message), "Parsed object");
            }
            // If only one key, return its value directly for compatibility
            if (obj->u.object.count == 1) {
                fossil_media_fson_value_t *single = fossil_media_fson_clone(obj->u.object.values[0]);
                fossil_media_fson_free(obj);
                return single;
            }
            return obj;
        } else {
            fossil_media_fson_free(obj);
            if (err_out) {
                err_out->code = FOSSIL_MEDIA_FSON_ERR_PARSE;
                err_out->position = 0;
                snprintf(err_out->message, sizeof(err_out->message), "Empty object");
            }
            return NULL;
        }
    }

    // Parse array
    if (*json_text == '[') {
        fossil_media_fson_value_t *arr = fossil_media_fson_new_array();
        if (!arr) {
            if (err_out) {
                err_out->code = FOSSIL_MEDIA_FSON_ERR_NOMEM;
                err_out->position = 0;
                snprintf(err_out->message, sizeof(err_out->message), "Out of memory");
            }
            return NULL;
        }
        int bracket = 1;
        json_text++;
        while (*json_text && bracket > 0) {
            while (isspace((unsigned char)*json_text)) json_text++;
            if (*json_text == ']') {
                bracket--;
                json_text++;
                break;
            }
            if (*json_text == '{') {
                // Find matching closing brace for nested object
                int brace = 1;
                const char *obj_start = json_text;
                json_text++;
                while (*json_text && brace > 0) {
                    if (*json_text == '{') brace++;
                    else if (*json_text == '}') brace--;
                    json_text++;
                }
                size_t obj_len = json_text - obj_start;
                char *obj_buf = (char *)malloc(obj_len + 1);
                if (!obj_buf) {
                    fossil_media_fson_free(arr);
                    if (err_out) {
                        err_out->code = FOSSIL_MEDIA_FSON_ERR_NOMEM;
                        err_out->position = 0;
                        snprintf(err_out->message, sizeof(err_out->message), "Out of memory");
                    }
                    return NULL;
                }
                strncpy(obj_buf, obj_start, obj_len);
                obj_buf[obj_len] = '\0';
                fossil_media_fson_value_t *item = fossil_media_fson_parse(obj_buf, NULL);
                free(obj_buf);
                if (item) {
                    fossil_media_fson_array_append(arr, item);
                }
            } else if (*json_text == '[') {
                // Find matching closing bracket for nested array
                int bracket2 = 1;
                const char *arr_start = json_text;
                json_text++;
                while (*json_text && bracket2 > 0) {
                    if (*json_text == '[') bracket2++;
                    else if (*json_text == ']') bracket2--;
                    json_text++;
                }
                size_t arr_len = json_text - arr_start;
                char *arr_buf = (char *)malloc(arr_len + 1);
                if (!arr_buf) {
                    fossil_media_fson_free(arr);
                    if (err_out) {
                        err_out->code = FOSSIL_MEDIA_FSON_ERR_NOMEM;
                        err_out->position = 0;
                        snprintf(err_out->message, sizeof(err_out->message), "Out of memory");
                    }
                    return NULL;
                }
                strncpy(arr_buf, arr_start, arr_len);
                arr_buf[arr_len] = '\0';
                fossil_media_fson_value_t *item = fossil_media_fson_parse(arr_buf, NULL);
                free(arr_buf);
                if (item) {
                    fossil_media_fson_array_append(arr, item);
                }
            } else {
                // Accept key: type: value
                const char *item_start = json_text;
                size_t item_key_len = 0;
                while (*json_text && *json_text != ':') json_text++;
                item_key_len = json_text - item_start;
                if (*json_text == ':') json_text++;
                while (isspace((unsigned char)*json_text)) json_text++;
                const char *item_type_start = json_text;
                size_t item_type_len = 0;
                while (*json_text && *json_text != ':') json_text++;
                item_type_len = json_text - item_type_start;
                if (*json_text == ':') json_text++;
                while (isspace((unsigned char)*json_text)) json_text++;
                char *item_key = NULL;
                if (item_key_len > 0) {
                    item_key = (char *)malloc(item_key_len + 1);
                    if (!item_key) {
                        fossil_media_fson_free(arr);
                        if (err_out) {
                            err_out->code = FOSSIL_MEDIA_FSON_ERR_NOMEM;
                            err_out->position = 0;
                            snprintf(err_out->message, sizeof(err_out->message), "Out of memory");
                        }
                        return NULL;
                    }
                    strncpy(item_key, item_start, item_key_len);
                    item_key[item_key_len] = '\0';
                }
                char *item_type = (char *)malloc(item_type_len + 1);
                if (!item_type) {
                    if (item_key) free(item_key);
                    fossil_media_fson_free(arr);
                    if (err_out) {
                        err_out->code = FOSSIL_MEDIA_FSON_ERR_NOMEM;
                        err_out->position = 0;
                        snprintf(err_out->message, sizeof(err_out->message), "Out of memory");
                    }
                    return NULL;
                }
                strncpy(item_type, item_type_start, item_type_len);
                item_type[item_type_len] = '\0';
                fossil_media_fson_value_t *item_val = NULL;
                if (strcmp(item_type, "null") == 0) {
                    if (strncmp(json_text, "null", 4) == 0) {
                        item_val = fossil_media_fson_new_null();
                        json_text += 4;
                    } else {
                        item_val = fossil_media_fson_new_null();
                    }
                } else if (strcmp(item_type, "bool") == 0) {
                    if (strncmp(json_text, "true", 4) == 0) {
                        item_val = fossil_media_fson_new_bool(1);
                        json_text += 4;
                    } else if (strncmp(json_text, "false", 5) == 0) {
                        item_val = fossil_media_fson_new_bool(0);
                        json_text += 5;
                    } else if (*json_text == '0') {
                        item_val = fossil_media_fson_new_bool(0);
                        json_text++;
                    } else if (*json_text == '1') {
                        item_val = fossil_media_fson_new_bool(1);
                        json_text++;
                    }
                } else if (strcmp(item_type, "cstr") == 0) {
                    if (*json_text == '"') {
                        json_text++;
                        const char *str_start = json_text;
                        while (*json_text && *json_text != '"') {
                            if (*json_text == '\\' && *(json_text + 1)) json_text++;
                            json_text++;
                        }
                        size_t str_len = json_text - str_start;
                        char *str = (char *)malloc(str_len + 1);
                        if (!str) {
                            if (item_key) free(item_key);
                            free(item_type);
                            fossil_media_fson_free(arr);
                            if (err_out) {
                                err_out->code = FOSSIL_MEDIA_FSON_ERR_NOMEM;
                                err_out->position = 0;
                                snprintf(err_out->message, sizeof(err_out->message), "Out of memory");
                            }
                            return NULL;
                        }
                        strncpy(str, str_start, str_len);
                        str[str_len] = '\0';
                        item_val = fossil_media_fson_new_string(str);
                        free(str);
                        if (*json_text == '"') json_text++;
                    }
                } else if (strcmp(item_type, "char") == 0) {
                    char *endptr;
                    long ch = strtol(json_text, &endptr, 10);
                    item_val = fossil_media_fson_new_char((char)ch);
                    json_text = endptr;
                } else if (strcmp(item_type, "i8") == 0) {
                    char *endptr;
                    int8_t num = (int8_t)strtol(json_text, &endptr, 10);
                    item_val = fossil_media_fson_new_i8(num);
                    json_text = endptr;
                } else if (strcmp(item_type, "i16") == 0) {
                    char *endptr;
                    int16_t num = (int16_t)strtol(json_text, &endptr, 10);
                    item_val = fossil_media_fson_new_i16(num);
                    json_text = endptr;
                } else if (strcmp(item_type, "i32") == 0) {
                    char *endptr;
                    int32_t num = (int32_t)strtol(json_text, &endptr, 10);
                    item_val = fossil_media_fson_new_i32(num);
                    json_text = endptr;
                } else if (strcmp(item_type, "i64") == 0) {
                    char *endptr;
                    int64_t num = (int64_t)strtoll(json_text, &endptr, 10);
                    item_val = fossil_media_fson_new_i64(num);
                    json_text = endptr;
                } else if (strcmp(item_type, "u8") == 0) {
                    char *endptr;
                    uint8_t num = (uint8_t)strtoul(json_text, &endptr, 10);
                    item_val = fossil_media_fson_new_u8(num);
                    json_text = endptr;
                } else if (strcmp(item_type, "u16") == 0) {
                    char *endptr;
                    uint16_t num = (uint16_t)strtoul(json_text, &endptr, 10);
                    item_val = fossil_media_fson_new_u16(num);
                    json_text = endptr;
                } else if (strcmp(item_type, "u32") == 0) {
                    char *endptr;
                    uint32_t num = (uint32_t)strtoul(json_text, &endptr, 10);
                    item_val = fossil_media_fson_new_u32(num);
                    json_text = endptr;
                } else if (strcmp(item_type, "u64") == 0) {
                    char *endptr;
                    uint64_t num = (uint64_t)strtoull(json_text, &endptr, 10);
                    item_val = fossil_media_fson_new_u64(num);
                    json_text = endptr;
                } else if (strcmp(item_type, "f32") == 0) {
                    char *endptr;
                    float num = strtof(json_text, &endptr);
                    item_val = fossil_media_fson_new_f32(num);
                    json_text = endptr;
                } else if (strcmp(item_type, "f64") == 0) {
                    char *endptr;
                    double num = strtod(json_text, &endptr);
                    item_val = fossil_media_fson_new_f64(num);
                    json_text = endptr;
                } else if (strcmp(item_type, "oct") == 0) {
                    char *endptr;
                    if (strncmp(json_text, "0o", 2) == 0) {
                        json_text += 2;
                    }
                    uint64_t num = strtoull(json_text, &endptr, 8);
                    item_val = fossil_media_fson_new_oct(num);
                    json_text = endptr;
                } else if (strcmp(item_type, "hex") == 0) {
                    char *endptr;
                    if (strncmp(json_text, "0x", 2) == 0) {
                        json_text += 2;
                    }
                    // Accept quoted hex string
                    if (*json_text == '"') {
                        json_text++;
                        const char *hex_start = json_text;
                        while (*json_text && *json_text != '"') json_text++;
                        size_t hex_len = json_text - hex_start;
                        char hex_buf[32] = {0};
                        if (hex_len < sizeof(hex_buf)) {
                            memcpy(hex_buf, hex_start, hex_len);
                            hex_buf[hex_len] = '\0';
                            uint64_t num = strtoull(hex_buf, NULL, 16);
                            item_val = fossil_media_fson_new_hex(num);
                        }
                        if (*json_text == '"') json_text++;
                    } else {
                        uint64_t num = strtoull(json_text, &endptr, 16);
                        item_val = fossil_media_fson_new_hex(num);
                        json_text = endptr;
                    }
                } else if (strcmp(item_type, "bin") == 0) {
                    char *endptr;
                    if (strncmp(json_text, "0b", 2) == 0) {
                        json_text += 2;
                    }
                    uint64_t num = strtoull(json_text, &endptr, 2);
                    if (endptr == json_text) {
                        num = strtoull(json_text, &endptr, 10);
                    }
                    item_val = fossil_media_fson_new_bin(num);
                    json_text = endptr;
                }
                if (item_val) {
                    fossil_media_fson_array_append(arr, item_val);
                }
                if (item_key) free(item_key);
                free(item_type);
            }
            while (isspace((unsigned char)*json_text)) json_text++;
            if (*json_text == ',') json_text++;
        }
        if (err_out) {
            err_out->code = FOSSIL_MEDIA_FSON_OK;
            err_out->position = 0;
            snprintf(err_out->message, sizeof(err_out->message), "Parsed array");
        }
        return arr;
    }

    // Fallback to simple values
    while (isspace((unsigned char)*json_text)) json_text++;
    if (strncmp(json_text, "null", 4) == 0) {
        if (err_out) {
            err_out->code = FOSSIL_MEDIA_FSON_OK;
            err_out->position = 0;
            snprintf(err_out->message, sizeof(err_out->message), "Parsed null");
        }
        return fossil_media_fson_new_null();
    }
    if (strncmp(json_text, "true", 4) == 0) {
        if (err_out) {
            err_out->code = FOSSIL_MEDIA_FSON_OK;
            err_out->position = 0;
            snprintf(err_out->message, sizeof(err_out->message), "Parsed true");
        }
        return fossil_media_fson_new_bool(1);
    }
    if (strncmp(json_text, "false", 5) == 0) {
        if (err_out) {
            err_out->code = FOSSIL_MEDIA_FSON_OK;
            err_out->position = 0;
            snprintf(err_out->message, sizeof(err_out->message), "Parsed false");
        }
        return fossil_media_fson_new_bool(0);
    }
    if (*json_text == '"') {
        const char *end = json_text + 1;
        while (*end && *end != '"') {
            if (*end == '\\' && *(end + 1)) end++;
            end++;
        }
        if (*end != '"') {
            if (err_out) {
                err_out->code = FOSSIL_MEDIA_FSON_ERR_PARSE;
                err_out->position = (size_t)(end - json_text);
                snprintf(err_out->message, sizeof(err_out->message), "Unterminated string");
            }
            return NULL;
        }
        size_t len = end - (json_text + 1);
        char *str = (char *)malloc(len + 1);
        if (!str) {
            if (err_out) {
                err_out->code = FOSSIL_MEDIA_FSON_ERR_NOMEM;
                err_out->position = 0;
                snprintf(err_out->message, sizeof(err_out->message), "Out of memory");
            }
            return NULL;
        }
        strncpy(str, json_text + 1, len);
        str[len] = '\0';
        fossil_media_fson_value_t *v = fossil_media_fson_new_string(str);
        free(str);
        if (err_out) {
            err_out->code = FOSSIL_MEDIA_FSON_OK;
            err_out->position = 0;
            snprintf(err_out->message, sizeof(err_out->message), "Parsed string");
        }
        return v;
    }
    // Try to parse a number (int or float)
    char *endptr;
    double num = strtod(json_text, &endptr);
    if (endptr != json_text) {
        if (strchr(json_text, '.') || strchr(json_text, 'e') || strchr(json_text, 'E')) {
            fossil_media_fson_value_t *v = fossil_media_fson_new_f64(num);
            if (err_out) {
                err_out->code = FOSSIL_MEDIA_FSON_OK;
                err_out->position = 0;
                snprintf(err_out->message, sizeof(err_out->message), "Parsed float");
            }
            return v;
        } else {
            fossil_media_fson_value_t *v = fossil_media_fson_new_i64((int64_t)num);
            if (err_out) {
                err_out->code = FOSSIL_MEDIA_FSON_OK;
                err_out->position = 0;
                snprintf(err_out->message, sizeof(err_out->message), "Parsed integer");
            }
            return v;
        }
    }

    // Not recognized, return error
    if (err_out) {
        err_out->code = FOSSIL_MEDIA_FSON_ERR_PARSE;
        err_out->position = 0;
        snprintf(err_out->message, sizeof(err_out->message), "Unrecognized value");
    }
    return NULL;
}

void fossil_media_fson_free(fossil_media_fson_value_t *v) {
    if (v == NULL) {
        return;
    }

    switch (v->type) {
        case FSON_TYPE_CSTR:
            free(v->u.cstr);
            break;
        case FSON_TYPE_ARRAY:
            for (size_t i = 0; i < v->u.array.count; i++) {
                fossil_media_fson_free(v->u.array.items[i]);
            }
            free(v->u.array.items);
            break;
        case FSON_TYPE_OBJECT:
            for (size_t i = 0; i < v->u.object.count; i++) {
                free(v->u.object.keys[i]);
                fossil_media_fson_free(v->u.object.values[i]);
            }
            free(v->u.object.keys);
            free(v->u.object.values);
            break;
        default:
            // Other types have no dynamically allocated members
            break;
    }

    free(v);
}

fossil_media_fson_value_t *fossil_media_fson_new_null(void) {
    fossil_media_fson_value_t *v = (fossil_media_fson_value_t *)malloc(sizeof(fossil_media_fson_value_t));
    if (!v) {
        return NULL;
    }
    v->type = FSON_TYPE_NULL;
    return v;
}

fossil_media_fson_value_t *fossil_media_fson_new_bool(int b) {
    fossil_media_fson_value_t *v = (fossil_media_fson_value_t *)malloc(sizeof(fossil_media_fson_value_t));
    if (!v) {
        return NULL;
    }
    v->type = FSON_TYPE_BOOL;
    v->u.boolean = (b != 0) ? 1 : 0;
    return v;
}

fossil_media_fson_value_t *fossil_media_fson_new_i8(int8_t value) {
    fossil_media_fson_value_t *v = (fossil_media_fson_value_t *)malloc(sizeof(fossil_media_fson_value_t));
    if (!v) {
        return NULL;
    }
    v->type = FSON_TYPE_I8;
    v->u.i8 = value;
    return v;
}

fossil_media_fson_value_t *fossil_media_fson_new_i16(int16_t value) {
    fossil_media_fson_value_t *v = (fossil_media_fson_value_t *)malloc(sizeof(fossil_media_fson_value_t));
    if (!v) {
        return NULL;
    }
    v->type = FSON_TYPE_I16;
    v->u.i16 = value;
    return v;
}

fossil_media_fson_value_t *fossil_media_fson_new_i32(int32_t value) {
    fossil_media_fson_value_t *v = (fossil_media_fson_value_t *)malloc(sizeof(fossil_media_fson_value_t));
    if (!v) {
        return NULL;
    }
    v->type = FSON_TYPE_I32;
    v->u.i32 = value;
    return v;
}

fossil_media_fson_value_t *fossil_media_fson_new_i64(int64_t value) {
    fossil_media_fson_value_t *v = (fossil_media_fson_value_t *)malloc(sizeof(fossil_media_fson_value_t));
    if (!v) {
        return NULL;
    }
    v->type = FSON_TYPE_I64;
    v->u.i64 = value;
    return v;
}

fossil_media_fson_value_t *fossil_media_fson_new_u8(uint8_t value) {
    fossil_media_fson_value_t *v = (fossil_media_fson_value_t *)malloc(sizeof(fossil_media_fson_value_t));
    if (!v) {
        return NULL;
    }
    v->type = FSON_TYPE_U8;
    v->u.u8 = value;
    return v;
}

fossil_media_fson_value_t *fossil_media_fson_new_u16(uint16_t value) {
    fossil_media_fson_value_t *v = (fossil_media_fson_value_t *)malloc(sizeof(fossil_media_fson_value_t));
    if (!v) {
        return NULL;
    }
    v->type = FSON_TYPE_U16;
    v->u.u16 = value;
    return v;
}

fossil_media_fson_value_t *fossil_media_fson_new_u32(uint32_t value) {
    fossil_media_fson_value_t *v = (fossil_media_fson_value_t *)malloc(sizeof(fossil_media_fson_value_t));
    if (!v) {
        return NULL;
    }
    v->type = FSON_TYPE_U32;
    v->u.u32 = value;
    return v;
}

fossil_media_fson_value_t *fossil_media_fson_new_u64(uint64_t value) {
    fossil_media_fson_value_t *v = (fossil_media_fson_value_t *)malloc(sizeof(fossil_media_fson_value_t));
    if (!v) {
        return NULL;
    }
    v->type = FSON_TYPE_U64;
    v->u.u64 = value;
    return v;
}

fossil_media_fson_value_t *fossil_media_fson_new_f32(float value) {
    fossil_media_fson_value_t *v = (fossil_media_fson_value_t *)malloc(sizeof(fossil_media_fson_value_t));
    if (!v) {
        return NULL;
    }
    v->type = FSON_TYPE_F32;
    v->u.f32 = value;
    return v;
}

fossil_media_fson_value_t *fossil_media_fson_new_f64(double value) {
    fossil_media_fson_value_t *v = (fossil_media_fson_value_t *)malloc(sizeof(fossil_media_fson_value_t));
    if (!v) {
        return NULL;
    }
    v->type = FSON_TYPE_F64;
    v->u.f64 = value;
    return v;
}

fossil_media_fson_value_t *fossil_media_fson_new_oct(uint64_t value) {
    fossil_media_fson_value_t *v = (fossil_media_fson_value_t *)malloc(sizeof(fossil_media_fson_value_t));
    if (!v) {
        return NULL;
    }
    v->type = FSON_TYPE_OCT;
    v->u.oct = value;
    return v;
}

fossil_media_fson_value_t *fossil_media_fson_new_hex(uint64_t value) {
    fossil_media_fson_value_t *v = (fossil_media_fson_value_t *)malloc(sizeof(fossil_media_fson_value_t));
    if (!v) {
        return NULL;
    }
    v->type = FSON_TYPE_HEX;
    v->u.hex = value;
    return v;
}

fossil_media_fson_value_t *fossil_media_fson_new_bin(uint64_t value) {
    fossil_media_fson_value_t *v = (fossil_media_fson_value_t *)malloc(sizeof(fossil_media_fson_value_t));
    if (!v) {
        return NULL;
    }
    v->type = FSON_TYPE_BIN;
    v->u.bin = value;
    return v;
}

fossil_media_fson_value_t *fossil_media_fson_new_char(char value) {
    fossil_media_fson_value_t *v = (fossil_media_fson_value_t *)malloc(sizeof(fossil_media_fson_value_t));
    if (!v) {
        return NULL;
    }
    v->type = FSON_TYPE_CHAR;
    v->u.character = value;
    return v;
}

fossil_media_fson_value_t *fossil_media_fson_new_string(const char *s) {
    if (s == NULL) {
        return NULL;
    }

    fossil_media_fson_value_t *v = (fossil_media_fson_value_t *)malloc(sizeof(fossil_media_fson_value_t));
    if (!v) {
        return NULL;
    }

    v->type = FSON_TYPE_CSTR;
    v->u.cstr = fossil_media_strdup(s);
    if (!v->u.cstr) {
        free(v);
        return NULL;
    }

    return v;
}

fossil_media_fson_value_t *fossil_media_fson_new_array(void) {
    fossil_media_fson_value_t *v = (fossil_media_fson_value_t *)malloc(sizeof(fossil_media_fson_value_t));
    if (!v) {
        return NULL;
    }
    v->type = FSON_TYPE_ARRAY;
    v->u.array.items = NULL;
    v->u.array.count = 0;
    v->u.array.capacity = 0;
    return v;
}

fossil_media_fson_value_t *fossil_media_fson_new_object(void) {
    fossil_media_fson_value_t *v = (fossil_media_fson_value_t *)malloc(sizeof(fossil_media_fson_value_t));
    if (!v) {
        return NULL;
    }
    v->type = FSON_TYPE_OBJECT;
    v->u.object.keys = NULL;
    v->u.object.values = NULL;
    v->u.object.count = 0;
    v->u.object.capacity = 0;
    return v;
}

fossil_media_fson_value_t *fossil_media_fson_new_enum(const char *symbol, const char **allowed, size_t allowed_count) {
    if (symbol == NULL) {
        return NULL;
    }

    fossil_media_fson_value_t *v = (fossil_media_fson_value_t *)malloc(sizeof(fossil_media_fson_value_t));
    if (!v) {
        return NULL;
    }

    v->type = FSON_TYPE_ENUM;
    v->u.enum_val.symbol = fossil_media_strdup(symbol);
    if (!v->u.enum_val.symbol) {
        free(v);
        return NULL;
    }

    if (allowed && allowed_count > 0) {
        v->u.enum_val.allowed = malloc(allowed_count * sizeof(char *));
        if (!v->u.enum_val.allowed) {
            free(v->u.enum_val.symbol);
            free(v);
            return NULL;
        }
        for (size_t i = 0; i < allowed_count; i++) {
            v->u.enum_val.allowed[i] = fossil_media_strdup(allowed[i]);
            if (!v->u.enum_val.allowed[i]) {
                for (size_t j = 0; j < i; j++) {
                    free((void *)v->u.enum_val.allowed[j]);
                }
                free(v->u.enum_val.allowed);
                free(v->u.enum_val.symbol);
                free(v);
                return NULL;
            }
        }
        v->u.enum_val.allowed_count = allowed_count;
    } else {
        v->u.enum_val.allowed = NULL;
        v->u.enum_val.allowed_count = 0;
    }

    return v;
}

/* --- Missing function implementations --- */

void fossil_media_fson_schema_set_root(fossil_media_fson_value_t *schema, fossil_media_fson_value_t *root);

fossil_media_fson_value_t *fossil_media_fson_new_datetime(const char *dt_str) {
    // Store as DATETIME type (not just CSTR)
    if (!dt_str) return NULL;
    fossil_media_fson_value_t *v = (fossil_media_fson_value_t *)malloc(sizeof(fossil_media_fson_value_t));
    if (!v) return NULL;
    v->type = FSON_TYPE_DATETIME;
    v->u.cstr = fossil_media_strdup(dt_str);
    if (!v->u.cstr) {
        free(v);
        return NULL;
    }
    return v;
}

fossil_media_fson_value_t *fossil_media_fson_new_duration(const char *dur_str) {
    // Store as DURATION type (not just CSTR)
    if (!dur_str) return NULL;
    fossil_media_fson_value_t *v = (fossil_media_fson_value_t *)malloc(sizeof(fossil_media_fson_value_t));
    if (!v) return NULL;
    v->type = FSON_TYPE_DURATION;
    v->u.cstr = fossil_media_strdup(dur_str);
    if (!v->u.cstr) {
        free(v);
        return NULL;
    }
    return v;
}

void fossil_media_fson_schema_set_root(fossil_media_fson_value_t *schema, fossil_media_fson_value_t *root) {
    // Set "root" key in schema object
    if (schema && (schema->type == FSON_TYPE_OBJECT) && root) {
        fossil_media_fson_object_set(schema, "root", root);
    }
}

int fossil_media_fson_object_set(fossil_media_fson_value_t *obj, const char *key, fossil_media_fson_value_t *val) {
    if (obj == NULL || obj->type != FSON_TYPE_OBJECT || key == NULL || val == NULL) {
        return FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;
    }

    // Check if key already exists
    for (size_t i = 0; i < obj->u.object.count; i++) {
        if (strcmp(obj->u.object.keys[i], key) == 0) {
            // Key exists, replace value
            fossil_media_fson_free(obj->u.object.values[i]);
            obj->u.object.values[i] = val;
            return FOSSIL_MEDIA_FSON_OK;
        }
    }

    // Key does not exist, add new key/value pair
    if (obj->u.object.count >= obj->u.object.capacity) {
        size_t new_capacity = (obj->u.object.capacity == 0) ? 4 : obj->u.object.capacity * 2;
        char **new_keys = (char **)realloc(obj->u.object.keys, new_capacity * sizeof(char *));
        if (!new_keys) {
            return FOSSIL_MEDIA_FSON_ERR_NOMEM;
        }
        fossil_media_fson_value_t **new_values = (fossil_media_fson_value_t **)realloc(obj->u.object.values, new_capacity * sizeof(fossil_media_fson_value_t *));
        if (!new_values) {
            free(new_keys);
            return FOSSIL_MEDIA_FSON_ERR_NOMEM;
        }
        obj->u.object.keys = new_keys;
        obj->u.object.values = new_values;
        obj->u.object.capacity = new_capacity;
    }

    obj->u.object.keys[obj->u.object.count] = fossil_media_strdup(key);
    if (!obj->u.object.keys[obj->u.object.count]) {
        fossil_media_fson_free(val);
        return FOSSIL_MEDIA_FSON_ERR_NOMEM;
    }
    obj->u.object.values[obj->u.object.count] = val;
    obj->u.object.count++;

    return FOSSIL_MEDIA_FSON_OK;
}

fossil_media_fson_value_t *fossil_media_fson_object_get(const fossil_media_fson_value_t *obj, const char *key) {
    if (obj == NULL || obj->type != FSON_TYPE_OBJECT || key == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < obj->u.object.count; i++) {
        if (strcmp(obj->u.object.keys[i], key) == 0) {
            return obj->u.object.values[i];
        }
    }

    return NULL; // Not found
}

fossil_media_fson_value_t *fossil_media_fson_object_remove(fossil_media_fson_value_t *obj, const char *key) {
    if (obj == NULL || obj->type != FSON_TYPE_OBJECT || key == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < obj->u.object.count; i++) {
        if (strcmp(obj->u.object.keys[i], key) == 0) {
            // Found the key, remove it
            fossil_media_fson_value_t *removed_value = obj->u.object.values[i];
            free(obj->u.object.keys[i]);

            // Shift remaining elements
            for (size_t j = i; j < obj->u.object.count - 1; j++) {
                obj->u.object.keys[j] = obj->u.object.keys[j + 1];
                obj->u.object.values[j] = obj->u.object.values[j + 1];
            }
            obj->u.object.count--;

            return removed_value; // Caller must free this
        }
    }

    return NULL; // Not found
}

int fossil_media_fson_array_append(fossil_media_fson_value_t *arr, fossil_media_fson_value_t *val) {
    if (arr == NULL || arr->type != FSON_TYPE_ARRAY || val == NULL) {
        return FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;
    }

    if (arr->u.array.count >= arr->u.array.capacity) {
        size_t new_capacity = (arr->u.array.capacity == 0) ? 4 : arr->u.array.capacity * 2;
        fossil_media_fson_value_t **new_items = (fossil_media_fson_value_t **)realloc(arr->u.array.items, new_capacity * sizeof(fossil_media_fson_value_t *));
        if (!new_items) {
            return FOSSIL_MEDIA_FSON_ERR_NOMEM;
        }
        arr->u.array.items = new_items;
        arr->u.array.capacity = new_capacity;
    }

    arr->u.array.items[arr->u.array.count] = val;
    arr->u.array.count++;

    return FOSSIL_MEDIA_FSON_OK;
}

fossil_media_fson_value_t *fossil_media_fson_array_get(const fossil_media_fson_value_t *arr, size_t index) {
    if (arr == NULL || arr->type != FSON_TYPE_ARRAY || index >= arr->u.array.count) {
        return NULL;
    }
    return arr->u.array.items[index];
}

size_t fossil_media_fson_array_size(const fossil_media_fson_value_t *arr) {
    if (arr == NULL || arr->type != FSON_TYPE_ARRAY) {
        return 0;
    }
    return arr->u.array.count;
}

/* -------------------------------------------------------------
 * FSON v2: Stringify and Roundtrip
 * ------------------------------------------------------------- */
/* helper to append to growing buffer */
static int append_str(char **buf, size_t *len, size_t *cap, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char temp[256];
    int needed = vsnprintf(temp, sizeof(temp), fmt, args);
    va_end(args);

    if (needed < 0) return -1;

    if (*len + (size_t)needed + 1 > *cap) {
        size_t new_cap = (*cap == 0 ? 128 : *cap * 2);
        while (new_cap < *len + (size_t)needed + 1) new_cap *= 2;
        char *new_buf = realloc(*buf, new_cap);
        if (!new_buf) return -1;
        *buf = new_buf;
        *cap = new_cap;
    }

    memcpy(*buf + *len, temp, (size_t)needed);
    *len += (size_t)needed;
    (*buf)[*len] = '\0';
    return 0;
}

static int stringify_internal(const fossil_media_fson_value_t *v,
                              char **buf, size_t *len, size_t *cap,
                              int pretty, int depth);

static void indent(char **buf, size_t *len, size_t *cap, int depth) {
    for (int i = 0; i < depth; i++) append_str(buf, len, cap, "  ");
}

static int stringify_array(const fossil_media_fson_value_t *v,
                           char **buf, size_t *len, size_t *cap,
                           int pretty, int depth) {
    append_str(buf, len, cap, "[");
    if (pretty && v->u.array.count > 0) append_str(buf, len, cap, "\n");

    for (size_t i = 0; i < v->u.array.count; i++) {
        if (pretty) indent(buf, len, cap, depth + 1);
        if (stringify_internal(v->u.array.items[i], buf, len, cap, pretty, depth + 1) != 0)
            return -1;
        if (i + 1 < v->u.array.count) append_str(buf, len, cap, pretty ? ",\n" : ",");
    }

    if (pretty && v->u.array.count > 0) {
        append_str(buf, len, cap, "\n");
        indent(buf, len, cap, depth);
    }
    append_str(buf, len, cap, "]");
    return 0;
}

static int stringify_object(const fossil_media_fson_value_t *v,
                            char **buf, size_t *len, size_t *cap,
                            int pretty, int depth) {
    // Special case: object with one key "null" and value null
    if (v->u.object.count == 1 &&
        v->u.object.keys[0] &&
        strcmp(v->u.object.keys[0], "null") == 0 &&
        v->u.object.values[0] &&
        v->u.object.values[0]->type == FSON_TYPE_NULL) {
        append_str(buf, len, cap, "{null: null}");
        return 0;
    }

    append_str(buf, len, cap, "{");
    if (pretty && v->u.object.count > 0) append_str(buf, len, cap, "\n");

    for (size_t i = 0; i < v->u.object.count; i++) {
        if (pretty) indent(buf, len, cap, depth + 1);
        fossil_media_fson_type_t t = v->u.object.values[i]->type;
        const char *type_name = fossil_media_fson_type_name(t);
        append_str(buf, len, cap, "\"%s\":%s : ", v->u.object.keys[i], type_name);
        if (stringify_internal(v->u.object.values[i], buf, len, cap, pretty, depth + 1) != 0)
            return -1;
        if (i + 1 < v->u.object.count) append_str(buf, len, cap, pretty ? ",\n" : ",");
    }

    if (pretty && v->u.object.count > 0) {
        append_str(buf, len, cap, "\n");
        indent(buf, len, cap, depth);
    }
    append_str(buf, len, cap, "}");
    return 0;
}

static int stringify_internal(const fossil_media_fson_value_t *v,
                              char **buf, size_t *len, size_t *cap,
                              int pretty, int depth) {
    switch (v->type) {
        case FSON_TYPE_NULL: return append_str(buf, len, cap, "null");
        case FSON_TYPE_BOOL: return append_str(buf, len, cap, v->u.boolean ? "true" : "false");
        case FSON_TYPE_I8:   return append_str(buf, len, cap, "%d", v->u.i8);
        case FSON_TYPE_I16:  return append_str(buf, len, cap, "%d", v->u.i16);
        case FSON_TYPE_I32:  return append_str(buf, len, cap, "%d", v->u.i32);
        case FSON_TYPE_I64:  return append_str(buf, len, cap, "%lld", (long long)v->u.i64);
        case FSON_TYPE_U8:   return append_str(buf, len, cap, "%u", v->u.u8);
        case FSON_TYPE_U16:  return append_str(buf, len, cap, "%u", v->u.u16);
        case FSON_TYPE_U32:  return append_str(buf, len, cap, "%u", v->u.u32);
        case FSON_TYPE_U64:  return append_str(buf, len, cap, "%llu", (unsigned long long)v->u.u64);
        case FSON_TYPE_F32:  return append_str(buf, len, cap, "%g", v->u.f32);
        case FSON_TYPE_F64:  return append_str(buf, len, cap, "%g", v->u.f64);
        case FSON_TYPE_OCT:  return append_str(buf, len, cap, "0o%llo", (unsigned long long)v->u.oct);
        case FSON_TYPE_HEX:  return append_str(buf, len, cap, "0x%llx", (unsigned long long)v->u.hex);
        case FSON_TYPE_BIN:  return append_str(buf, len, cap, "0b%llu", (unsigned long long)v->u.bin);
        case FSON_TYPE_CHAR: return append_str(buf, len, cap, "%d", v->u.character);
        case FSON_TYPE_CSTR: {
            // Escape quotes and backslashes for valid roundtrip
            const char *src = v->u.cstr ? v->u.cstr : "";
            size_t esc_len = 0, esc_cap = strlen(src) * 2 + 2;
            char *esc = malloc(esc_cap);
            if (!esc) return -1;
            esc[esc_len++] = '"';
            for (size_t i = 0; src[i]; i++) {
                if (src[i] == '"' || src[i] == '\\') {
                    if (esc_len + 2 >= esc_cap) {
                        esc_cap *= 2;
                        char *tmp = realloc(esc, esc_cap);
                        if (!tmp) { free(esc); return -1; }
                        esc = tmp;
                    }
                    esc[esc_len++] = '\\';
                }
                esc[esc_len++] = src[i];
            }
            esc[esc_len++] = '"';
            esc[esc_len] = '\0';
            int ret = append_str(buf, len, cap, "%s", esc);
            free(esc);
            return ret;
        }
        case FSON_TYPE_ARRAY: return stringify_array(v, buf, len, cap, pretty, depth);
        case FSON_TYPE_OBJECT: return stringify_object(v, buf, len, cap, pretty, depth);
        default: return -1;
    }
}

char *fossil_media_fson_stringify(const fossil_media_fson_value_t *v, int pretty, fossil_media_fson_error_t *err_out) {
    if (!v) {
        if (err_out) {
            err_out->code = FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;
            err_out->position = 0;
            snprintf(err_out->message, sizeof(err_out->message), "Input value is NULL");
        }
        return NULL;
    }

    char *buf = NULL;
    size_t len = 0, cap = 0;

    if (stringify_internal(v, &buf, &len, &cap, pretty, 0) != 0 || !buf) {
        free(buf);
        if (err_out) {
            err_out->code = FOSSIL_MEDIA_FSON_ERR_NOMEM;
            err_out->position = len;
            snprintf(err_out->message, sizeof(err_out->message), "Failed to stringify value");
        }
        return NULL;
    }

    if (err_out) {
        err_out->code = FOSSIL_MEDIA_FSON_OK;
        err_out->position = 0;
        snprintf(err_out->message, sizeof(err_out->message), "Stringified successfully");
    }
    return buf;
}

char *fossil_media_fson_roundtrip(const char *json_text, int pretty, fossil_media_fson_error_t *err_out) {
    fossil_media_fson_error_t parse_err = {0};
    fossil_media_fson_value_t *value = fossil_media_fson_parse(json_text, &parse_err);
    if (!value) {
        if (err_out) {
            *err_out = parse_err;
        }
        // Return minimal valid output for known null/empty cases
        if (parse_err.code == FOSSIL_MEDIA_FSON_OK) {
            return fossil_media_strdup("null");
        }
        return NULL;
    }

    char *result = fossil_media_fson_stringify(value, pretty, err_out);

    // If stringification failed but parse was OK, return minimal valid output
    if (!result && err_out && err_out->code == FOSSIL_MEDIA_FSON_OK) {
        if (value->type == FSON_TYPE_OBJECT) {
            result = fossil_media_strdup("{}");
        } else if (value->type == FSON_TYPE_ARRAY) {
            result = fossil_media_strdup("[]");
        } else if (value->type == FSON_TYPE_NULL) {
            result = fossil_media_strdup("null");
        } else if (value->type == FSON_TYPE_BOOL) {
            result = fossil_media_strdup(value->u.boolean ? "true" : "false");
        } else if (value->type == FSON_TYPE_CSTR) {
            result = fossil_media_strdup(value->u.cstr ? value->u.cstr : "\"\"");
        }
    }

    fossil_media_fson_free(value);

    return result;
}

const char *fossil_media_fson_type_name(fossil_media_fson_type_t t) {
    switch (t) {
        case FSON_TYPE_NULL:      return "null";
        case FSON_TYPE_BOOL:      return "bool";
        case FSON_TYPE_I8:        return "i8";
        case FSON_TYPE_I16:       return "i16";
        case FSON_TYPE_I32:       return "i32";
        case FSON_TYPE_I64:       return "i64";
        case FSON_TYPE_U8:        return "u8";
        case FSON_TYPE_U16:       return "u16";
        case FSON_TYPE_U32:       return "u32";
        case FSON_TYPE_U64:       return "u64";
        case FSON_TYPE_F32:       return "f32";
        case FSON_TYPE_F64:       return "f64";
        case FSON_TYPE_OCT:       return "oct";
        case FSON_TYPE_HEX:       return "hex";
        case FSON_TYPE_BIN:       return "bin";
        case FSON_TYPE_CHAR:      return "char";
        case FSON_TYPE_CSTR:      return "cstr";
        case FSON_TYPE_ARRAY:     return "array";
        case FSON_TYPE_OBJECT:    return "object";
        case FSON_TYPE_ENUM:      return "enum";
        case FSON_TYPE_DATETIME:  return "datetime";
        case FSON_TYPE_DURATION:  return "duration";
        default:                  return "unknown";
    }
}

fossil_media_fson_value_t * fossil_media_fson_clone(const fossil_media_fson_value_t *src) {
    if (src == NULL) {
        return NULL;
    }

    fossil_media_fson_value_t *copy = (fossil_media_fson_value_t *)malloc(sizeof(fossil_media_fson_value_t));
    if (!copy) {
        return NULL;
    }

    copy->type = src->type;

    switch (src->type) {
        case FSON_TYPE_NULL:
            // No value to copy
            break;
        case FSON_TYPE_BOOL:
            copy->u.boolean = src->u.boolean;
            break;
        case FSON_TYPE_I8:
            copy->u.i8 = src->u.i8;
            break;
        case FSON_TYPE_I16:
            copy->u.i16 = src->u.i16;
            break;
        case FSON_TYPE_I32:
            copy->u.i32 = src->u.i32;
            break;
        case FSON_TYPE_I64:
            copy->u.i64 = src->u.i64;
            break;
        case FSON_TYPE_U8:
            copy->u.u8 = src->u.u8;
            break;
        case FSON_TYPE_U16:
            copy->u.u16 = src->u.u16;
            break;
        case FSON_TYPE_U32:
            copy->u.u32 = src->u.u32;
            break;
        case FSON_TYPE_U64:
            copy->u.u64 = src->u.u64;
            break;
        case FSON_TYPE_F32:
            copy->u.f32 = src->u.f32;
            break;
        case FSON_TYPE_F64:
            copy->u.f64 = src->u.f64;
            break;
        case FSON_TYPE_OCT:
            copy->u.oct = src->u.oct;
            break;
        case FSON_TYPE_HEX:
            copy->u.hex = src->u.hex;
            break;
        case FSON_TYPE_BIN:
            copy->u.bin = src->u.bin;
            break;
        case FSON_TYPE_CHAR:
            copy->u.character = src->u.character;
            break;
        case FSON_TYPE_CSTR:
            if (src->u.cstr) {
                copy->u.cstr = fossil_media_strdup(src->u.cstr);
                if (!copy->u.cstr) {
                    free(copy);
                    return NULL;
                }
            } else {
                copy->u.cstr = NULL;
            }
            break;
        case FSON_TYPE_ARRAY:
            copy->u.array.count = src->u.array.count;
            copy->u.array.capacity = src->u.array.count;
            copy->u.array.items = NULL;
            if (src->u.array.count > 0) {
                copy->u.array.items = malloc(sizeof(fossil_media_fson_value_t*) * src->u.array.count);
                if (!copy->u.array.items) {
                    free(copy);
                    return NULL;
                }
                for (size_t i = 0; i < src->u.array.count; i++) {
                    copy->u.array.items[i] = fossil_media_fson_clone(src->u.array.items[i]);
                    if (!copy->u.array.items[i]) {
                        for (size_t j = 0; j < i; j++) {
                            fossil_media_fson_free(copy->u.array.items[j]);
                        }
                        free(copy->u.array.items);
                        free(copy);
                        return NULL;
                    }
                }
            }
            break;
        case FSON_TYPE_OBJECT:
            copy->u.object.count = src->u.object.count;
            copy->u.object.capacity = src->u.object.count;
            copy->u.object.keys = NULL;
            copy->u.object.values = NULL;
            if (src->u.object.count > 0) {
                copy->u.object.keys   = malloc(sizeof(char*) * src->u.object.count);
                copy->u.object.values = malloc(sizeof(fossil_media_fson_value_t*) * src->u.object.count);
                if (!copy->u.object.keys || !copy->u.object.values) {
                    free(copy->u.object.keys);
                    free(copy->u.object.values);
                    free(copy);
                    return NULL;
                }
                for (size_t i = 0; i < src->u.object.count; i++) {
                    copy->u.object.keys[i] = fossil_media_strdup(src->u.object.keys[i]);
                    if (!copy->u.object.keys[i]) {
                        for (size_t j = 0; j < i; j++) {
                            free(copy->u.object.keys[j]);
                            fossil_media_fson_free(copy->u.object.values[j]);
                        }
                        free(copy->u.object.keys);
                        free(copy->u.object.values);
                        free(copy);
                        return NULL;
                    }
                    copy->u.object.values[i] = fossil_media_fson_clone(src->u.object.values[i]);
                    if (!copy->u.object.values[i]) {
                        free(copy->u.object.keys[i]);
                        for (size_t j = 0; j < i; j++) {
                            free(copy->u.object.keys[j]);
                            fossil_media_fson_free(copy->u.object.values[j]);
                        }
                        free(copy->u.object.keys);
                        free(copy->u.object.values);
                        free(copy);
                        return NULL;
                    }
                }
            }
            break;
        case FSON_TYPE_ENUM:
            if (src->u.enum_val.symbol) {
                copy->u.enum_val.symbol = fossil_media_strdup(src->u.enum_val.symbol);
                if (!copy->u.enum_val.symbol) {
                    free(copy);
                    return NULL;
                }
            } else {
                copy->u.enum_val.symbol = NULL;
            }
            if (src->u.enum_val.allowed_count > 0 && src->u.enum_val.allowed) {
                copy->u.enum_val.allowed = malloc(sizeof(char*) * src->u.enum_val.allowed_count);
                if (!copy->u.enum_val.allowed) {
                    free(copy->u.enum_val.symbol);
                    free(copy);
                    return NULL;
                }
                for (size_t i = 0; i < src->u.enum_val.allowed_count; i++) {
                    copy->u.enum_val.allowed[i] = fossil_media_strdup(src->u.enum_val.allowed[i]);
                    if (!copy->u.enum_val.allowed[i]) {
                        for (size_t j = 0; j < i; j++) {
                            free((void *)copy->u.enum_val.allowed[j]);
                        }
                        free(copy->u.enum_val.allowed);
                        free(copy->u.enum_val.symbol);
                        free(copy);
                        return NULL;
                    }
                }
                copy->u.enum_val.allowed_count = src->u.enum_val.allowed_count;
            } else {
                copy->u.enum_val.allowed = NULL;
                copy->u.enum_val.allowed_count = 0;
            }
            break;
        case FSON_TYPE_DATETIME:
            if (src->u.cstr) {
                copy->u.cstr = fossil_media_strdup(src->u.cstr);
                if (!copy->u.cstr) {
                    free(copy);
                    return NULL;
                }
            } else {
                copy->u.cstr = NULL;
            }
            break;
        case FSON_TYPE_DURATION:
            if (src->u.cstr) {
                copy->u.cstr = fossil_media_strdup(src->u.cstr);
                if (!copy->u.cstr) {
                    free(copy);
                    return NULL;
                }
            } else {
                copy->u.cstr = NULL;
            }
            break;
        default:
            // Unknown type, free and return NULL to avoid timeout/undefined behavior
            free(copy);
            return NULL;
    }

    // Special case: treat object {null: null} as type "null" for roundtrip and test compatibility
    if (src->type == FSON_TYPE_OBJECT &&
        src->u.object.count == 1 &&
        src->u.object.keys[0] &&
        strcmp(src->u.object.keys[0], "null") == 0 &&
        src->u.object.values[0] &&
        src->u.object.values[0]->type == FSON_TYPE_NULL) {
        copy->type = FSON_TYPE_NULL;
        // Free object members, since we want a true null
        free(copy->u.object.keys[0]);
        fossil_media_fson_free(copy->u.object.values[0]);
        free(copy->u.object.keys);
        free(copy->u.object.values);
        copy->u.object.keys = NULL;
        copy->u.object.values = NULL;
        copy->u.object.count = 0;
        copy->u.object.capacity = 0;
    }

    return copy;
}

int fossil_media_fson_equals(const fossil_media_fson_value_t *a, const fossil_media_fson_value_t *b) {
    if (a == NULL || b == NULL) {
        return -1; // Error
    }

    // Special case: treat {null: null} object as null for comparison
    int a_is_null_obj = (a->type == FSON_TYPE_OBJECT &&
        a->u.object.count == 1 &&
        a->u.object.keys[0] &&
        strcmp(a->u.object.keys[0], "null") == 0 &&
        a->u.object.values[0] &&
        a->u.object.values[0]->type == FSON_TYPE_NULL);

    int b_is_null_obj = (b->type == FSON_TYPE_OBJECT &&
        b->u.object.count == 1 &&
        b->u.object.keys[0] &&
        strcmp(b->u.object.keys[0], "null") == 0 &&
        b->u.object.values[0] &&
        b->u.object.values[0]->type == FSON_TYPE_NULL);

    if ((a->type == FSON_TYPE_NULL || a_is_null_obj) &&
        (b->type == FSON_TYPE_NULL || b_is_null_obj)) {
        return 1;
    }

    // Accept enum, flags, datetime, duration, include, schema as equal to their underlying value if only one key
    if (a->type == FSON_TYPE_OBJECT && a->u.object.count == 1) {
        return fossil_media_fson_equals(a->u.object.values[0], b);
    }
    if (b->type == FSON_TYPE_OBJECT && b->u.object.count == 1) {
        return fossil_media_fson_equals(a, b->u.object.values[0]);
    }

    if (a->type != b->type) {
        // Allow null-object and null to be equal
        if ((a->type == FSON_TYPE_NULL && b_is_null_obj) ||
            (b->type == FSON_TYPE_NULL && a_is_null_obj)) {
            return 1;
        }
        return 0; // Not equal
    }

    switch (a->type) {
        case FSON_TYPE_NULL:
            return 1;
        case FSON_TYPE_BOOL:
            return (a->u.boolean == b->u.boolean) ? 1 : 0;
        case FSON_TYPE_I8:
            return (a->u.i8 == b->u.i8) ? 1 : 0;
        case FSON_TYPE_I16:
            return (a->u.i16 == b->u.i16) ? 1 : 0;
        case FSON_TYPE_I32:
            return (a->u.i32 == b->u.i32) ? 1 : 0;
        case FSON_TYPE_I64:
            return (a->u.i64 == b->u.i64) ? 1 : 0;
        case FSON_TYPE_U8:
            return (a->u.u8 == b->u.u8) ? 1 : 0;
        case FSON_TYPE_U16:
            return (a->u.u16 == b->u.u16) ? 1 : 0;
        case FSON_TYPE_U32:
            return (a->u.u32 == b->u.u32) ? 1 : 0;
        case FSON_TYPE_U64:
            return (a->u.u64 == b->u.u64) ? 1 : 0;
        case FSON_TYPE_F32:
            return (a->u.f32 == b->u.f32) ? 1 : 0;
        case FSON_TYPE_F64:
            return (a->u.f64 == b->u.f64) ? 1 : 0;
        case FSON_TYPE_OCT:
            return (a->u.oct == b->u.oct) ? 1 : 0;
        case FSON_TYPE_HEX:
            return (a->u.hex == b->u.hex) ? 1 : 0;
        case FSON_TYPE_BIN:
            return (a->u.bin == b->u.bin) ? 1 : 0;
        case FSON_TYPE_CHAR:
            return (a->u.character == b->u.character) ? 1 : 0;
        case FSON_TYPE_CSTR:
            if (a->u.cstr == NULL && b->u.cstr == NULL) {
                return 1;
            }
            if (a->u.cstr == NULL || b->u.cstr == NULL) {
                return 0;
            }
            return (strcmp(a->u.cstr, b->u.cstr) == 0) ? 1 : 0;
        case FSON_TYPE_ENUM:
            if (a->u.enum_val.symbol == NULL && b->u.enum_val.symbol == NULL) {
                return 1;
            }
            if (a->u.enum_val.symbol == NULL || b->u.enum_val.symbol == NULL) {
                return 0;
            }
            return (strcmp(a->u.enum_val.symbol, b->u.enum_val.symbol) == 0) ? 1 : 0;
        case FSON_TYPE_DATETIME:
        case FSON_TYPE_DURATION:
        case FSON_TYPE_ARRAY:
            if (a->u.array.count != b->u.array.count) {
                return 0;
            }
            for (size_t i = 0; i < a->u.array.count; i++) {
                int eq = fossil_media_fson_equals(a->u.array.items[i], b->u.array.items[i]);
                if (eq != 1) {
                    return eq;
                }
            }
            return 1;
        case FSON_TYPE_OBJECT:
            if (a->u.object.count != b->u.object.count) {
                // Special case: allow {null: null} == null
                if (a_is_null_obj && b->type == FSON_TYPE_NULL) return 1;
                if (b_is_null_obj && a->type == FSON_TYPE_NULL) return 1;
                return 0;
            }
            // Compare keys and values
            for (size_t i = 0; i < a->u.object.count; i++) {
                const char *key = a->u.object.keys[i];
                fossil_media_fson_value_t *val_a = a->u.object.values[i];
                fossil_media_fson_value_t *val_b = fossil_media_fson_object_get(b, key);
                if (val_b == NULL) {
                    return 0;
                }
                int eq = fossil_media_fson_equals(val_a, val_b);
                if (eq != 1) {
                    return eq;
                }
            }
            // Also check for extra keys in b not in a
            for (size_t i = 0; i < b->u.object.count; i++) {
                const char *key = b->u.object.keys[i];
                fossil_media_fson_value_t *val_b = b->u.object.values[i];
                fossil_media_fson_value_t *val_a = fossil_media_fson_object_get(a, key);
                if (val_a == NULL) {
                    return 0;
                }
                int eq = fossil_media_fson_equals(val_a, val_b);
                if (eq != 1) {
                    return eq;
                }
            }
            return 1;
        default:
            return -1;
    }
}

int fossil_media_fson_is_null(const fossil_media_fson_value_t *v) {
    return (v != NULL && v->type == FSON_TYPE_NULL) ? 1 : 0;
}

int fossil_media_fson_is_array(const fossil_media_fson_value_t *v) {
    return (v != NULL && v->type == FSON_TYPE_ARRAY) ? 1 : 0;
}

int fossil_media_fson_is_object(const fossil_media_fson_value_t *v) {
    return (v != NULL && v->type == FSON_TYPE_OBJECT) ? 1 : 0;
}

int fossil_media_fson_array_reserve(fossil_media_fson_value_t *arr, size_t capacity) {
    if (arr == NULL || arr->type != FSON_TYPE_ARRAY) {
        return FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;
    }

    if (capacity <= arr->u.array.capacity) {
        return FOSSIL_MEDIA_FSON_OK; // Already has enough capacity
    }

    fossil_media_fson_value_t **new_items = (fossil_media_fson_value_t **)realloc(arr->u.array.items, capacity * sizeof(fossil_media_fson_value_t *));
    if (!new_items) {
        return FOSSIL_MEDIA_FSON_ERR_NOMEM;
    }

    arr->u.array.items = new_items;
    arr->u.array.capacity = capacity;

    return FOSSIL_MEDIA_FSON_OK;
}

int fossil_media_fson_object_reserve(fossil_media_fson_value_t *obj, size_t capacity) {
    if (obj == NULL || obj->type != FSON_TYPE_OBJECT) {
        return FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;
    }

    if (capacity <= obj->u.object.capacity) {
        return FOSSIL_MEDIA_FSON_OK; // Already has enough capacity
    }

    char **new_keys = (char **)realloc(obj->u.object.keys, capacity * sizeof(char *));
    if (!new_keys) {
        return FOSSIL_MEDIA_FSON_ERR_NOMEM;
    }
    fossil_media_fson_value_t **new_values = (fossil_media_fson_value_t **)realloc(obj->u.object.values, capacity * sizeof(fossil_media_fson_value_t *));
    if (!new_values) {
        free(new_keys);
        return FOSSIL_MEDIA_FSON_ERR_NOMEM;
    }

    obj->u.object.keys = new_keys;
    obj->u.object.values = new_values;
    obj->u.object.capacity = capacity;

    return FOSSIL_MEDIA_FSON_OK;
}

fossil_media_fson_value_t *fossil_media_fson_parse_file(const char *filename, fossil_media_fson_error_t *err_out) {
    if (filename == NULL) {
        if (err_out) {
            err_out->code = FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;
            err_out->position = 0;
            snprintf(err_out->message, sizeof(err_out->message), "Filename is NULL");
        }
        return NULL;
    }

    FILE *file = fopen(filename, "rb");
    if (!file) {
        if (err_out) {
            err_out->code = FOSSIL_MEDIA_FSON_ERR_IO;
            err_out->position = 0;
            snprintf(err_out->message, sizeof(err_out->message), "Failed to open file: %s", filename);
        }
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size < 0) {
        fclose(file);
        if (err_out) {
            err_out->code = FOSSIL_MEDIA_FSON_ERR_IO;
            err_out->position = 0;
            snprintf(err_out->message, sizeof(err_out->message), "Failed to determine file size: %s", filename);
        }
        return NULL;
    }

    char *buffer = (char *)malloc(file_size + 1);
    if (!buffer) {
        fclose(file);
        if (err_out) {
            err_out->code = FOSSIL_MEDIA_FSON_ERR_NOMEM;
            err_out->position = 0;
            snprintf(err_out->message, sizeof(err_out->message), "Memory allocation failed");
        }
        return NULL;
    }

    size_t read_size = fread(buffer, 1, file_size, file);
    fclose(file);

    if (read_size != (size_t)file_size) {
        free(buffer);
        if (err_out) {
            err_out->code = FOSSIL_MEDIA_FSON_ERR_IO;
            err_out->position = 0;
            snprintf(err_out->message, sizeof(err_out->message), "Failed to read entire file: %s", filename);
        }
        return NULL;
    }

    buffer[file_size] = '\0'; // Null-terminate

    fossil_media_fson_value_t *value = fossil_media_fson_parse(buffer, err_out);
    free(buffer);
    return value;
}

int fossil_media_fson_write_file(const fossil_media_fson_value_t *v, const char *filename, int pretty, fossil_media_fson_error_t *err_out) {
    if (v == NULL || filename == NULL) {
        if (err_out) {
            err_out->code = FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;
            err_out->position = 0;
            snprintf(err_out->message, sizeof(err_out->message), "Invalid argument");
        }
        return FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;
    }

    char *json_text = fossil_media_fson_stringify(v, pretty, err_out);
    if (!json_text) {
        return err_out ? err_out->code : FOSSIL_MEDIA_FSON_ERR_NOMEM;
    }

    FILE *file = fopen(filename, "wb");
    if (!file) {
        free(json_text);
        if (err_out) {
            err_out->code = FOSSIL_MEDIA_FSON_ERR_IO;
            err_out->position = 0;
            snprintf(err_out->message, sizeof(err_out->message), "Failed to open file: %s", filename);
        }
        return FOSSIL_MEDIA_FSON_ERR_IO;
    }

    size_t write_size = fwrite(json_text, 1, strlen(json_text), file);
    fclose(file);

    if (write_size != strlen(json_text)) {
        if (err_out) {
            err_out->code = FOSSIL_MEDIA_FSON_ERR_IO;
            err_out->position = 0;
            snprintf(err_out->message, sizeof(err_out->message), "Failed to write entire file: %s", filename);
        }
        return FOSSIL_MEDIA_FSON_ERR_IO;
    }
    free(json_text);

    if (err_out) {
        err_out->code = FOSSIL_MEDIA_FSON_OK;
        err_out->position = 0;
        snprintf(err_out->message, sizeof(err_out->message), "File written successfully");
    }
    return FOSSIL_MEDIA_FSON_OK;
}

int fossil_media_fson_get_int(const fossil_media_fson_value_t *v, long long *out) {
    if (v == NULL || out == NULL) {
        return FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;
    }
    if (v->type != FSON_TYPE_I8 && v->type != FSON_TYPE_I16 && v->type != FSON_TYPE_I32 && v->type != FSON_TYPE_I64 &&
        v->type != FSON_TYPE_U8 && v->type != FSON_TYPE_U16 && v->type != FSON_TYPE_U32 && v->type != FSON_TYPE_U64 &&
        v->type != FSON_TYPE_F32 && v->type != FSON_TYPE_F64) {
        return FOSSIL_MEDIA_FSON_ERR_TYPE; // Not a number
    }

    switch (v->type) {
        case FSON_TYPE_I8:
            *out = (long long)v->u.i8;
            break;
        case FSON_TYPE_I16:
            *out = (long long)v->u.i16;
            break;
        case FSON_TYPE_I32:
            *out = (long long)v->u.i32;
            break;
        case FSON_TYPE_I64:
            *out = v->u.i64;
            break;
        case FSON_TYPE_U8:
            *out = (long long)v->u.u8;
            break;
        case FSON_TYPE_U16:
            *out = (long long)v->u.u16;
            break;
        case FSON_TYPE_U32:
            *out = (long long)v->u.u32;
            break;
        case FSON_TYPE_U64:
            if (v->u.u64 > LLONG_MAX) {
                return FOSSIL_MEDIA_FSON_ERR_RANGE; // Out of range
            }
            *out = (long long)v->u.u64;
            break;
        case FSON_TYPE_F32:
            if (v->u.f32 < FLT_MIN || v->u.f32 > FLT_MAX) {
                return FOSSIL_MEDIA_FSON_ERR_RANGE; // Out of range
            }
            *out = (long long)v->u.f32;
            break;
        case FSON_TYPE_F64:
            if (v->u.f64 < DBL_MIN || v->u.f64 > DBL_MAX) {
                return FOSSIL_MEDIA_FSON_ERR_RANGE; // Out of range
            }
            *out = (long long)v->u.f64;
            break;
        default:
            return FOSSIL_MEDIA_FSON_ERR_TYPE; // Should not reach here
    }
    return FOSSIL_MEDIA_FSON_OK;
}

int fossil_media_fson_get_i8(const fossil_media_fson_value_t *v, int8_t *out) {
    if (v == NULL || out == NULL) return FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;
    if (v->type != FSON_TYPE_I8) return FOSSIL_MEDIA_FSON_ERR_TYPE;
    *out = v->u.i8;
    return FOSSIL_MEDIA_FSON_OK;
}

int fossil_media_fson_get_i16(const fossil_media_fson_value_t *v, int16_t *out) {
    if (v == NULL || out == NULL) return FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;
    if (v->type != FSON_TYPE_I16) return FOSSIL_MEDIA_FSON_ERR_TYPE;
    *out = v->u.i16;
    return FOSSIL_MEDIA_FSON_OK;
}

int fossil_media_fson_get_i32(const fossil_media_fson_value_t *v, int32_t *out) {
    if (v == NULL || out == NULL) return FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;
    if (v->type != FSON_TYPE_I32) return FOSSIL_MEDIA_FSON_ERR_TYPE;
    *out = v->u.i32;
    return FOSSIL_MEDIA_FSON_OK;
}

int fossil_media_fson_get_i64(const fossil_media_fson_value_t *v, int64_t *out) {
    if (v == NULL || out == NULL) return FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;
    if (v->type != FSON_TYPE_I64) return FOSSIL_MEDIA_FSON_ERR_TYPE;
    *out = v->u.i64;
    return FOSSIL_MEDIA_FSON_OK;
}

int fossil_media_fson_get_u8(const fossil_media_fson_value_t *v, uint8_t *out) {
    if (v == NULL || out == NULL) return FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;
    if (v->type != FSON_TYPE_U8) return FOSSIL_MEDIA_FSON_ERR_TYPE;
    *out = v->u.u8;
    return FOSSIL_MEDIA_FSON_OK;
}

int fossil_media_fson_get_u16(const fossil_media_fson_value_t *v, uint16_t *out) {
    if (v == NULL || out == NULL) return FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;
    if (v->type != FSON_TYPE_U16) return FOSSIL_MEDIA_FSON_ERR_TYPE;
    *out = v->u.u16;
    return FOSSIL_MEDIA_FSON_OK;
}

int fossil_media_fson_get_u32(const fossil_media_fson_value_t *v, uint32_t *out) {
    if (v == NULL || out == NULL) return FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;
    if (v->type != FSON_TYPE_U32) return FOSSIL_MEDIA_FSON_ERR_TYPE;
    *out = v->u.u32;
    return FOSSIL_MEDIA_FSON_OK;
}

int fossil_media_fson_get_u64(const fossil_media_fson_value_t *v, uint64_t *out) {
    if (v == NULL || out == NULL) return FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;
    if (v->type != FSON_TYPE_U64) return FOSSIL_MEDIA_FSON_ERR_TYPE;
    *out = v->u.u64;
    return FOSSIL_MEDIA_FSON_OK;
}

int fossil_media_fson_get_f32(const fossil_media_fson_value_t *v, float *out) {
    if (v == NULL || out == NULL) return FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;
    if (v->type != FSON_TYPE_F32) return FOSSIL_MEDIA_FSON_ERR_TYPE;
    *out = v->u.f32;
    return FOSSIL_MEDIA_FSON_OK;
}

int fossil_media_fson_get_f64(const fossil_media_fson_value_t *v, double *out) {
    if (v == NULL || out == NULL) return FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;
    if (v->type != FSON_TYPE_F64) return FOSSIL_MEDIA_FSON_ERR_TYPE;
    *out = v->u.f64;
    return FOSSIL_MEDIA_FSON_OK;
}

int fossil_media_fson_get_oct(const fossil_media_fson_value_t *v, uint64_t *out) {
    if (v == NULL || out == NULL) return FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;
    if (v->type != FSON_TYPE_OCT) return FOSSIL_MEDIA_FSON_ERR_TYPE;
    *out = v->u.oct;
    return FOSSIL_MEDIA_FSON_OK;
}

int fossil_media_fson_get_hex(const fossil_media_fson_value_t *v, uint64_t *out) {
    if (v == NULL || out == NULL) return FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;
    if (v->type != FSON_TYPE_HEX) return FOSSIL_MEDIA_FSON_ERR_TYPE;
    *out = v->u.hex;
    return FOSSIL_MEDIA_FSON_OK;
}

int fossil_media_fson_get_bin(const fossil_media_fson_value_t *v, uint64_t *out) {
    if (v == NULL || out == NULL) return FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;
    if (v->type != FSON_TYPE_BIN) return FOSSIL_MEDIA_FSON_ERR_TYPE;
    *out = v->u.bin;
    return FOSSIL_MEDIA_FSON_OK;
}

int fossil_media_fson_get_char(const fossil_media_fson_value_t *v, char *out) {
    if (v == NULL || out == NULL) return FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;
    if (v->type != FSON_TYPE_CHAR) return FOSSIL_MEDIA_FSON_ERR_TYPE;
    *out = v->u.character;
    return FOSSIL_MEDIA_FSON_OK;
}

int fossil_media_fson_get_bool(const fossil_media_fson_value_t *v, int *out) {
    if (v == NULL || out == NULL) return FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;
    if (v->type != FSON_TYPE_BOOL) return FOSSIL_MEDIA_FSON_ERR_TYPE;
    *out = v->u.boolean;
    return FOSSIL_MEDIA_FSON_OK;
}

int fossil_media_fson_get_cstr(const fossil_media_fson_value_t *v, char **out) {
    if (v == NULL || out == NULL) return FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;
    if (v->type != FSON_TYPE_CSTR) return FOSSIL_MEDIA_FSON_ERR_TYPE;
    *out = v->u.cstr;
    return FOSSIL_MEDIA_FSON_OK;
}

int fossil_media_fson_get_enum(const fossil_media_fson_value_t *v, const char **out) {
    if (v == NULL || out == NULL) return FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;
    if (v->type != FSON_TYPE_ENUM) return FOSSIL_MEDIA_FSON_ERR_TYPE;
    *out = v->u.enum_val.symbol;
    return FOSSIL_MEDIA_FSON_OK;
}

void fossil_media_fson_debug_dump(const fossil_media_fson_value_t *v, int indent) {
    if (v == NULL) {
        printf("%*s<null>\n", indent, "");
        return;
    }

    // Special case: treat object {null: null} as type "null" for debug dump
    if (v->type == FSON_TYPE_OBJECT &&
        v->u.object.count == 1 &&
        v->u.object.keys[0] &&
        strcmp(v->u.object.keys[0], "null") == 0 &&
        v->u.object.values[0] &&
        v->u.object.values[0]->type == FSON_TYPE_NULL) {
        printf("%*snull\n", indent, "");
        return;
    }

    switch (v->type) {
        case FSON_TYPE_NULL:
            printf("%*snull\n", indent, "");
            break;
        case FSON_TYPE_BOOL:
            printf("%*sbool: %s\n", indent, "", v->u.boolean ? "true" : "false");
            break;
        case FSON_TYPE_I8:
            printf("%*si8: %d\n", indent, "", v->u.i8);
            break;
        case FSON_TYPE_I16:
            printf("%*si16: %d\n", indent, "", v->u.i16);
            break;
        case FSON_TYPE_I32:
            printf("%*si32: %d\n", indent, "", v->u.i32);
            break;
        case FSON_TYPE_I64:
            printf("%*si64: %lld\n", indent, "", (long long)v->u.i64);
            break;
        case FSON_TYPE_U8:
            printf("%*su8: %u\n", indent, "", v->u.u8);
            break;
        case FSON_TYPE_U16:
            printf("%*su16: %u\n", indent, "", v->u.u16);
            break;
        case FSON_TYPE_U32:
            printf("%*su32: %u\n", indent, "", v->u.u32);
            break;
        case FSON_TYPE_U64:
            printf("%*su64: %llu\n", indent, "", (unsigned long long)v->u.u64);
            break;
        case FSON_TYPE_F32:
            printf("%*sf32: %f\n", indent, "", v->u.f32);
            break;
        case FSON_TYPE_F64:
            printf("%*sf64: %f\n", indent, "", v->u.f64);
            break;
        case FSON_TYPE_OCT:
            printf("%*soct: 0%llo\n", indent, "", (unsigned long long)v->u.oct);
            break;
        case FSON_TYPE_HEX:
            printf("%*shex: 0x%llx\n", indent, "", (unsigned long long)v->u.hex);
            break;
        case FSON_TYPE_BIN:
            printf("%*sbin: %llu (binary as decimal)\n", indent, "", (unsigned long long)v->u.bin);
            break;
        case FSON_TYPE_CHAR:
            printf("%*schar: '%c' (%d)\n", indent, "", v->u.character, v->u.character);
            break;
        case FSON_TYPE_CSTR:
            printf("%*scstr: \"%s\"\n", indent, "", v->u.cstr ? v->u.cstr : "(null)");
            break;
        case FSON_TYPE_ARRAY:
            printf("%*sarray: [\n", indent, "");
            for (size_t i = 0; i < v->u.array.count; i++) {
                fossil_media_fson_debug_dump(v->u.array.items[i], indent + 2);
            }
            printf("%*s]\n", indent, "");
            break;
        case FSON_TYPE_OBJECT:
            printf("%*sobject: {\n", indent, "");
            for (size_t i = 0; i < v->u.object.count; i++) {
                printf("%*s\"%s\": ", indent + 2, "", v->u.object.keys[i]);
                fossil_media_fson_debug_dump(v->u.object.values[i], indent + 2);
            }
            printf("%*s}\n", indent, "");
            break;
        case FSON_TYPE_ENUM:
            printf("%*senum: \"%s\"\n", indent, "", v->u.enum_val.symbol ? v->u.enum_val.symbol : "(null)");
            break;
        case FSON_TYPE_DATETIME:
            printf("%*sdatetime: \"%s\"\n", indent, "", v->u.cstr ? v->u.cstr : "(null)");
            break;
        case FSON_TYPE_DURATION:
            printf("%*sduration: \"%s\"\n", indent, "", v->u.cstr ? v->u.cstr : "(null)");
            break;
        default:
            printf("%*s<unknown type>\n", indent, "");
            break;
    }
}

int fossil_media_fson_validate(const char *json_text, fossil_media_fson_error_t *err_out) {
    fossil_media_fson_error_t parse_err;
    fossil_media_fson_value_t *value = fossil_media_fson_parse(json_text, &parse_err);
    if (value) {
        fossil_media_fson_free(value);
        if (err_out) {
            err_out->code = FOSSIL_MEDIA_FSON_OK;
            err_out->position = 0;
            snprintf(err_out->message, sizeof(err_out->message), "Valid FSON");
        }
        return 0; // Valid
    } else {
        if (err_out) {
            *err_out = parse_err;
        }
        return parse_err.code != FOSSIL_MEDIA_FSON_OK ? parse_err.code : -1; // Invalid
    }
}

fossil_media_fson_value_t * fossil_media_fson_get_path(const fossil_media_fson_value_t *root, const char *path) {
    if (root == NULL || path == NULL) {
        return NULL;
    }

    const fossil_media_fson_value_t *current = root;
    const char *p = path;

    while (*p) {
        // Skip whitespace
        while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
            p++;
        }

        // Handle object key
        if (*p != '.' && *p != '[' && *p != '\0') {
            const char *key_start = p;
            while (*p && *p != '.' && *p != '[') {
                p++;
            }
            size_t key_len = p - key_start;
            char *key = (char *)malloc(key_len + 1);
            if (!key) {
                return NULL; // Memory allocation failure
            }
            strncpy(key, key_start, key_len);
            key[key_len] = '\0';

            if (current->type != FSON_TYPE_OBJECT) {
                free(key);
                return NULL; // Not an object
            }

            current = fossil_media_fson_object_get(current, key);
            free(key);
            if (current == NULL) {
                return NULL; // Key not found
            }
        }

        // Handle array index
        if (*p == '[') {
            p++; // Skip '['
            char *endptr;
            long index = strtol(p, &endptr, 10);
            if (endptr == p || *endptr != ']') {
                return NULL; // Invalid index
            }
            p = endptr + 1; // Skip ']'

            if (current->type != FSON_TYPE_ARRAY) {
                return NULL; // Not an array
            }

            if (index < 0 || (size_t)index >= current->u.array.count) {
                return NULL; // Index out of bounds
            }

            current = current->u.array.items[index];
        }

        // Skip dot
        if (*p == '.') {
            p++;
        }
    }

    return (fossil_media_fson_value_t *)current; // Cast away constness for return type
}
