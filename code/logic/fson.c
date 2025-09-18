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
 * Minimal FSON parser for the extended format:
 * Supports: { "key":type : value, ... }, arrays, objects, numbers, bool, null, char, oct, hex, bin, cstr, etc.
 * Only parses a subset of the FSON format for demonstration.
 */
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
        fossil_media_fson_value_t *obj = fossil_media_fson_new_object();
        json_text++;
        while (*json_text) {
            while (isspace((unsigned char)*json_text)) json_text++;
            if (*json_text == '}') {
                json_text++;
                break;
            }
            // Parse key
            if (*json_text != '"') {
                if (err_out) {
                    err_out->code = FOSSIL_MEDIA_FSON_ERR_PARSE;
                    err_out->position = (int)(json_text - input_start);
                    snprintf(err_out->message, sizeof(err_out->message), "Expected '\"' for key");
                }
                fossil_media_fson_free(obj);
                return NULL;
            }
            json_text++;
            const char *key_start = json_text;
            while (*json_text && *json_text != '"') json_text++;
            if (*json_text != '"') {
                if (err_out) {
                    err_out->code = FOSSIL_MEDIA_FSON_ERR_PARSE;
                    err_out->position = (int)(json_text - input_start);
                    snprintf(err_out->message, sizeof(err_out->message), "Unterminated key string");
                }
                fossil_media_fson_free(obj);
                return NULL;
            }
            size_t key_len = json_text - key_start;
            char *key = (char *)malloc(key_len + 1);
            strncpy(key, key_start, key_len);
            key[key_len] = '\0';
            json_text++; // skip closing quote

            while (isspace((unsigned char)*json_text)) json_text++;
            if (*json_text != ':') {
                free(key);
                fossil_media_fson_free(obj);
                if (err_out) {
                    err_out->code = FOSSIL_MEDIA_FSON_ERR_PARSE;
                    err_out->position = (int)(json_text - input_start);
                    snprintf(err_out->message, sizeof(err_out->message), "Expected ':' after key");
                }
                return NULL;
            }
            json_text++; // skip ':'

            // Parse type
            while (isspace((unsigned char)*json_text)) json_text++;
            const char *type_start = json_text;
            while (*json_text && *json_text != ':') json_text++;
            if (*json_text != ':') {
                free(key);
                fossil_media_fson_free(obj);
                if (err_out) {
                    err_out->code = FOSSIL_MEDIA_FSON_ERR_PARSE;
                    err_out->position = (int)(json_text - input_start);
                    snprintf(err_out->message, sizeof(err_out->message), "Expected ':' after type");
                }
                return NULL;
            }
            size_t type_len = json_text - type_start;
            char *type = (char *)malloc(type_len + 1);
            strncpy(type, type_start, type_len);
            type[type_len] = '\0';
            json_text++; // skip ':'

            while (isspace((unsigned char)*json_text)) json_text++;

            // Parse value
            fossil_media_fson_value_t *val = NULL;
            if (strcmp(type, "null") == 0) {
                // Accept "null" literal
                if (strncmp(json_text, "null", 4) == 0) {
                    val = fossil_media_fson_new_null();
                    json_text += 4;
                } else {
                    val = fossil_media_fson_new_null();
                }
            } else if (strcmp(type, "bool") == 0) {
                if (strncmp(json_text, "true", 4) == 0) {
                    val = fossil_media_fson_new_bool(1);
                    json_text += 4;
                } else if (strncmp(json_text, "false", 5) == 0) {
                    val = fossil_media_fson_new_bool(0);
                    json_text += 5;
                } else {
                    // Accept 0/1 as bool
                    if (*json_text == '0') {
                        val = fossil_media_fson_new_bool(0);
                        json_text++;
                    } else if (*json_text == '1') {
                        val = fossil_media_fson_new_bool(1);
                        json_text++;
                    }
                }
            } else if (strcmp(type, "cstr") == 0) {
                if (*json_text == '"') {
                    json_text++;
                    const char *str_start = json_text;
                    while (*json_text && *json_text != '"') {
                        if (*json_text == '\\' && *(json_text + 1)) json_text++;
                        json_text++;
                    }
                    size_t str_len = json_text - str_start;
                    char *str = (char *)malloc(str_len + 1);
                    strncpy(str, str_start, str_len);
                    str[str_len] = '\0';
                    val = fossil_media_fson_new_string(str);
                    free(str);
                    if (*json_text == '"') json_text++;
                }
            } else if (strcmp(type, "char") == 0) {
                char *endptr;
                long ch = strtol(json_text, &endptr, 10);
                val = fossil_media_fson_new_char((char)ch);
                json_text = endptr;
            } else if (strcmp(type, "i32") == 0) {
                char *endptr;
                int32_t num = (int32_t)strtol(json_text, &endptr, 10);
                val = fossil_media_fson_new_i32(num);
                json_text = endptr;
            } else if (strcmp(type, "i64") == 0) {
                char *endptr;
                int64_t num = (int64_t)strtoll(json_text, &endptr, 10);
                val = fossil_media_fson_new_i64(num);
                json_text = endptr;
            } else if (strcmp(type, "u8") == 0) {
                char *endptr;
                uint8_t num = (uint8_t)strtoul(json_text, &endptr, 10);
                val = fossil_media_fson_new_u8(num);
                json_text = endptr;
            } else if (strcmp(type, "u16") == 0) {
                char *endptr;
                uint16_t num = (uint16_t)strtoul(json_text, &endptr, 10);
                val = fossil_media_fson_new_u16(num);
                json_text = endptr;
            } else if (strcmp(type, "u32") == 0) {
                char *endptr;
                uint32_t num = (uint32_t)strtoul(json_text, &endptr, 10);
                val = fossil_media_fson_new_u32(num);
                json_text = endptr;
            } else if (strcmp(type, "u64") == 0) {
                char *endptr;
                uint64_t num = (uint64_t)strtoull(json_text, &endptr, 10);
                val = fossil_media_fson_new_u64(num);
                json_text = endptr;
            } else if (strcmp(type, "f32") == 0) {
                char *endptr;
                float num = strtof(json_text, &endptr);
                val = fossil_media_fson_new_f32(num);
                json_text = endptr;
            } else if (strcmp(type, "f64") == 0) {
                char *endptr;
                double num = strtod(json_text, &endptr);
                val = fossil_media_fson_new_f64(num);
                json_text = endptr;
            } else if (strcmp(type, "oct") == 0) {
                char *endptr;
                // Accept both "077" and "0o77" formats
                if (strncmp(json_text, "0o", 2) == 0) {
                    json_text += 2;
                }
                uint64_t num = strtoull(json_text, &endptr, 8);
                val = fossil_media_fson_new_oct(num);
                json_text = endptr;
            } else if (strcmp(type, "hex") == 0) {
                char *endptr;
                // Accept both "0xFF" and "FF"
                if (strncmp(json_text, "0x", 2) == 0) {
                    json_text += 2;
                }
                uint64_t num = strtoull(json_text, &endptr, 16);
                val = fossil_media_fson_new_hex(num);
                json_text = endptr;
            } else if (strcmp(type, "bin") == 0) {
                char *endptr;
                // Accept both "0b101010" and "101010"
                if (strncmp(json_text, "0b", 2) == 0) {
                    json_text += 2;
                }
                uint64_t num = strtoull(json_text, &endptr, 2);
                // fallback: if no binary digits, try decimal
                if (endptr == json_text) {
                    num = strtoull(json_text, &endptr, 10);
                }
                val = fossil_media_fson_new_bin(num);
                json_text = endptr;
            } else if (strcmp(type, "array") == 0) {
                while (isspace((unsigned char)*json_text)) json_text++;
                if (*json_text == '[') {
                    json_text++;
                    val = fossil_media_fson_new_array();
                    while (*json_text) {
                        while (isspace((unsigned char)*json_text)) json_text++;
                        if (*json_text == ']') {
                            json_text++;
                            break;
                        }
                        if (*json_text == '{') {
                            fossil_media_fson_value_t *item = fossil_media_fson_parse(json_text, NULL);
                            if (item) {
                                fossil_media_fson_array_append(val, item);
                            }
                            // Move to next item
                            int brace = 1;
                            json_text++;
                            while (*json_text && brace > 0) {
                                if (*json_text == '{') brace++;
                                else if (*json_text == '}') brace--;
                                json_text++;
                            }
                        }
                        while (isspace((unsigned char)*json_text)) json_text++;
                        if (*json_text == ',') json_text++;
                    }
                }
            } else if (strcmp(type, "object") == 0) {
                while (isspace((unsigned char)*json_text)) json_text++;
                if (*json_text == '{') {
                    val = fossil_media_fson_parse(json_text, NULL);
                    // Move to end of object
                    int brace = 1;
                    json_text++;
                    while (*json_text && brace > 0) {
                        if (*json_text == '{') brace++;
                        else if (*json_text == '}') brace--;
                        json_text++;
                    }
                }
            } else {
                // Unknown type
                free(key);
                fossil_media_fson_free(obj);
                if (err_out) {
                    err_out->code = FOSSIL_MEDIA_FSON_ERR_PARSE;
                    err_out->position = (int)(json_text - input_start);
                    snprintf(err_out->message, sizeof(err_out->message), "Unknown type: %s", type);
                }
                return NULL;
            }

            if (val) {
                fossil_media_fson_object_set(obj, key, val);
            }
            free(key);
            free(type);

            while (isspace((unsigned char)*json_text)) json_text++;
            if (*json_text == ',') json_text++;
        }
        if (err_out) {
            err_out->code = FOSSIL_MEDIA_FSON_OK;
            err_out->position = 0;
            snprintf(err_out->message, sizeof(err_out->message), "Parsed object");
        }
        return obj;
    }

    // Parse array
    if (*json_text == '[') {
        fossil_media_fson_value_t *arr = fossil_media_fson_new_array();
        json_text++;
        while (*json_text) {
            while (isspace((unsigned char)*json_text)) json_text++;
            if (*json_text == ']') {
                json_text++;
                break;
            }
            if (*json_text == '{') {
                fossil_media_fson_value_t *item = fossil_media_fson_parse(json_text, NULL);
                if (item) {
                    fossil_media_fson_array_append(arr, item);
                }
                // Move to next item
                int brace = 1;
                json_text++;
                while (*json_text && brace > 0) {
                    if (*json_text == '{') brace++;
                    else if (*json_text == '}') brace--;
                    json_text++;
                }
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
                err_out->position = (int)(end - json_text);
                snprintf(err_out->message, sizeof(err_out->message), "Unterminated string");
            }
            return NULL;
        }
        size_t len = end - (json_text + 1);
        char *str = (char *)malloc(len + 1);
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
        // Check if it's integer or float
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
    append_str(buf, len, cap, "{");
    if (pretty && v->u.object.count > 0) append_str(buf, len, cap, "\n");

    for (size_t i = 0; i < v->u.object.count; i++) {
        if (pretty) indent(buf, len, cap, depth + 1);
        // Output key:type : value for roundtrip
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
        case FSON_TYPE_CHAR: return append_str(buf, len, cap, "%d", v->u.character); // output as integer for roundtrip
        case FSON_TYPE_CSTR: return append_str(buf, len, cap, "\"%s\"", v->u.cstr ? v->u.cstr : "");
        case FSON_TYPE_ARRAY: return stringify_array(v, buf, len, cap, pretty, depth);
        case FSON_TYPE_OBJECT: return stringify_object(v, buf, len, cap, pretty, depth);
    }
    return -1;
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
    fossil_media_fson_error_t parse_err;
    fossil_media_fson_value_t *value = fossil_media_fson_parse(json_text, &parse_err);
    if (!value) {
        if (err_out) {
            *err_out = parse_err;
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
        }
    }

    fossil_media_fson_free(value);

    return result;
}

const char *fossil_media_fson_type_name(fossil_media_fson_type_t t) {
    switch (t) {
        case FSON_TYPE_NULL: return "null";
        case FSON_TYPE_BOOL: return "bool";
        case FSON_TYPE_I8: return "i8";
        case FSON_TYPE_I16: return "i16";
        case FSON_TYPE_I32: return "i32";
        case FSON_TYPE_I64: return "i64";
        case FSON_TYPE_U8: return "u8";
        case FSON_TYPE_U16: return "u16";
        case FSON_TYPE_U32: return "u32";
        case FSON_TYPE_U64: return "u64";
        case FSON_TYPE_F32: return "f32";
        case FSON_TYPE_F64: return "f64";
        case FSON_TYPE_OCT: return "oct";
        case FSON_TYPE_HEX: return "hex";
        case FSON_TYPE_BIN: return "bin";
        case FSON_TYPE_CHAR: return "char";
        case FSON_TYPE_CSTR: return "cstr";
        case FSON_TYPE_ARRAY: return "array";
        case FSON_TYPE_OBJECT: return "object";
        default: return "unknown";
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
            copy->u.array.capacity = src->u.array.count; /* clone uses same capacity */
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
                        /* Rollback on failure */
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
                    /* Duplicate key */
                    copy->u.object.keys[i] = fossil_media_strdup(src->u.object.keys[i]);
                    if (!copy->u.object.keys[i]) {
                        /* Rollback */
                        for (size_t j = 0; j < i; j++) {
                            free(copy->u.object.keys[j]);
                            fossil_media_fson_free(copy->u.object.values[j]);
                        }
                        free(copy->u.object.keys);
                        free(copy->u.object.values);
                        free(copy);
                        return NULL;
                    }

                    /* Clone value */
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
    }

    return copy;
}

int fossil_media_fson_equals(const fossil_media_fson_value_t *a, const fossil_media_fson_value_t *b) {
    if (a == NULL || b == NULL) {
        return -1; // Error
    }
    if (a->type != b->type) {
        return 0; // Not equal
    }

    switch (a->type) {
        case FSON_TYPE_NULL:
            return 1; // Both are null
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
                return 1; // Both NULL
            }
            if (a->u.cstr == NULL || b->u.cstr == NULL) {
                return 0; // One is NULL, the other is not
            }
            return (strcmp(a->u.cstr, b->u.cstr) == 0) ? 1 : 0;
        case FSON_TYPE_ARRAY:
            if (a->u.array.count != b->u.array.count) {
                return 0; // Different sizes
            }
            for (size_t i = 0; i < a->u.array.count; i++) {
                int eq = fossil_media_fson_equals(a->u.array.items[i], b->u.array.items[i]);
                if (eq != 1) {
                    return eq; // Not equal or error
                }
            }
            return 1; // All items equal
        case FSON_TYPE_OBJECT:
            if (a->u.object.count != b->u.object.count) {
                return 0; // Different sizes
            }
            for (size_t i = 0; i < a->u.object.count; i++) {
                const char *key = a->u.object.keys[i];
                fossil_media_fson_value_t *val_a = a->u.object.values[i];
                fossil_media_fson_value_t *val_b = fossil_media_fson_object_get(b, key);
                if (val_b == NULL) {
                    return 0; // Key not found in b
                }
                int eq = fossil_media_fson_equals(val_a, val_b);
                if (eq != 1) {
                    return eq; // Not equal or error
                }
            }
            return 1; // All key/value pairs equal
        default:
            return -1; // Unknown type, error
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

void fossil_media_fson_debug_dump(const fossil_media_fson_value_t *v, int indent) {
    if (v == NULL) {
        printf("%*s<null>\n", indent, "");
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
