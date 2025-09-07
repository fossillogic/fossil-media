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
#include "fossil/media/json.h"
#include "fossil/media/media.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <math.h>

/* Internal helpers and allocator wrappers */
static void *fm_malloc(size_t n){ return malloc(n); }
static void fm_free(void *p){ free(p); }
static void *fm_realloc(void *p, size_t n){ return realloc(p, n); }

/* Error helpers */
static void set_error(fossil_media_json_error_t *err, int code, size_t pos, const char *fmt, ...) {
    if (!err) return;
    err->code = code;
    err->position = pos;
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(err->message, sizeof(err->message), fmt, ap);
    va_end(ap);
}

/* Forward parse functions */
typedef struct {
    const char *s;
    size_t i;
} ctx_t;

static void skip_ws(ctx_t *c) {
    const char *s = c->s;
    size_t i = c->i;
    while (s[i] && (s[i]==' ' || s[i]=='\n' || s[i]=='\r' || s[i]=='\t')) i++;
    c->i = i;
}

/* Allocate new value */
static fossil_media_json_value_t *alloc_value(void) {
    fossil_media_json_value_t *v = fm_malloc(sizeof(*v));
    if (!v) return NULL;
    memset(v, 0, sizeof(*v));
    return v;
}

/* Free helpers */
void fossil_media_json_free(fossil_media_json_value_t *v) {
    if (!v) return;
    size_t k;
    switch (v->type) {
        case FOSSIL_MEDIA_JSON_STRING:
            fm_free(v->u.string);
            break;
        case FOSSIL_MEDIA_JSON_ARRAY:
            for (k = 0; k < v->u.array.count; ++k) fossil_media_json_free(v->u.array.items[k]);
            fm_free(v->u.array.items);
            break;
        case FOSSIL_MEDIA_JSON_OBJECT:
            for (k = 0; k < v->u.object.count; ++k) {
                fm_free(v->u.object.keys[k]);
                fossil_media_json_free(v->u.object.values[k]);
            }
            fm_free(v->u.object.keys);
            fm_free(v->u.object.values);
            break;
        default: break;
    }
    fm_free(v);
}

/* Constructors */
fossil_media_json_value_t *fossil_media_json_new_null(void) {
    fossil_media_json_value_t *v = alloc_value();
    if (v) v->type = FOSSIL_MEDIA_JSON_NULL;
    return v;
}

fossil_media_json_value_t *fossil_media_json_new_bool(int b) {
    fossil_media_json_value_t *v = alloc_value();
    if (v) { v->type = FOSSIL_MEDIA_JSON_BOOL; v->u.boolean = b ? 1 : 0; }
    return v;
}

fossil_media_json_value_t *fossil_media_json_new_number(double n) {
    fossil_media_json_value_t *v = alloc_value();
    if (v) { v->type = FOSSIL_MEDIA_JSON_NUMBER; v->u.number = n; }
    return v;
}

fossil_media_json_value_t *fossil_media_json_new_string(const char *s) {
    fossil_media_json_value_t *v = alloc_value();
    if (!v) return NULL;
    v->type = FOSSIL_MEDIA_JSON_STRING;
    if (!s) { v->u.string = fm_malloc(1); v->u.string[0] = '\0'; }
    else {
        v->u.string = fm_malloc(strlen(s) + 1);
        if (!v->u.string) { fm_free(v); return NULL; }
        strcpy(v->u.string, s);
    }
    return v;
}

fossil_media_json_value_t *fossil_media_json_new_array(void) {
    fossil_media_json_value_t *v = alloc_value();
    if (v) { v->type = FOSSIL_MEDIA_JSON_ARRAY; v->u.array.items = NULL; v->u.array.count = v->u.array.capacity = 0; }
    return v;
}

fossil_media_json_value_t *fossil_media_json_new_object(void) {
    fossil_media_json_value_t *v = alloc_value();
    if (v) { v->type = FOSSIL_MEDIA_JSON_OBJECT; v->u.object.keys = NULL; v->u.object.values = NULL; v->u.object.count = v->u.object.capacity = 0; }
    return v;
}

/* Utility: duplicate string */
static char *dupe_string(const char *s) {
    if (!s) return NULL;
    size_t n = strlen(s);
    char *r = fm_malloc(n + 1);
    if (!r) return NULL;
    memcpy(r, s, n+1);
    return r;
}

/* Object set helper (replaces existing) */
int fossil_media_json_object_set(fossil_media_json_value_t *obj, const char *key, fossil_media_json_value_t *val) {
    if (!obj || obj->type != FOSSIL_MEDIA_JSON_OBJECT || !key) return -1;
    size_t i;
    for (i = 0; i < obj->u.object.count; ++i) {
        if (strcmp(obj->u.object.keys[i], key) == 0) {
            fossil_media_json_free(obj->u.object.values[i]);
            obj->u.object.values[i] = val;
            return 0;
        }
    }
    if (obj->u.object.count == obj->u.object.capacity) {
        size_t newcap = obj->u.object.capacity ? obj->u.object.capacity * 2 : 4;
        char **nk = fm_realloc(obj->u.object.keys, sizeof(*nk) * newcap);
        fossil_media_json_value_t **nv = fm_realloc(obj->u.object.values, sizeof(*nv) * newcap);
        if (!nk || !nv) { fm_free(nk); fm_free(nv); return -1; }
        obj->u.object.keys = nk;
        obj->u.object.values = nv;
        obj->u.object.capacity = newcap;
    }
    obj->u.object.keys[obj->u.object.count] = dupe_string(key);
    obj->u.object.values[obj->u.object.count] = val;
    obj->u.object.count++;
    return 0;
}

fossil_media_json_value_t *fossil_media_json_object_get(const fossil_media_json_value_t *obj, const char *key) {
    if (!obj || obj->type != FOSSIL_MEDIA_JSON_OBJECT || !key) return NULL;
    for (size_t i = 0; i < obj->u.object.count; ++i)
        if (strcmp(obj->u.object.keys[i], key) == 0) return obj->u.object.values[i];
    return NULL;
}

fossil_media_json_value_t *fossil_media_json_object_remove(fossil_media_json_value_t *obj, const char *key) {
    if (!obj || obj->type != FOSSIL_MEDIA_JSON_OBJECT || !key) return NULL;
    for (size_t i = 0; i < obj->u.object.count; ++i) {
        if (strcmp(obj->u.object.keys[i], key) == 0) {
            fossil_media_json_value_t *val = obj->u.object.values[i];
            fm_free(obj->u.object.keys[i]);
            /* shift */
            for (size_t j = i + 1; j < obj->u.object.count; ++j) {
                obj->u.object.keys[j-1] = obj->u.object.keys[j];
                obj->u.object.values[j-1] = obj->u.object.values[j];
            }
            obj->u.object.count--;
            return val;
        }
    }
    return NULL;
}

/* Array helpers */
int fossil_media_json_array_append(fossil_media_json_value_t *arr, fossil_media_json_value_t *val) {
    if (!arr || arr->type != FOSSIL_MEDIA_JSON_ARRAY) return -1;
    if (arr->u.array.count == arr->u.array.capacity) {
        size_t newcap = arr->u.array.capacity ? arr->u.array.capacity * 2 : 4;
        fossil_media_json_value_t **tmp = fm_realloc(arr->u.array.items, sizeof(*tmp) * newcap);
        if (!tmp) return -1;
        arr->u.array.items = tmp;
        arr->u.array.capacity = newcap;
    }
    arr->u.array.items[arr->u.array.count++] = val;
    return 0;
}

fossil_media_json_value_t *fossil_media_json_array_get(const fossil_media_json_value_t *arr, size_t index) {
    if (!arr || arr->type != FOSSIL_MEDIA_JSON_ARRAY) return NULL;
    if (index >= arr->u.array.count) return NULL;
    return arr->u.array.items[index];
}

size_t fossil_media_json_array_size(const fossil_media_json_value_t *arr) {
    if (!arr || arr->type != FOSSIL_MEDIA_JSON_ARRAY) return 0;
    return arr->u.array.count;
}

/* Parsing primitives */

/* parse_literal: true/false/null */
static fossil_media_json_value_t *parse_literal(ctx_t *c, fossil_media_json_error_t *err) {
    const char *s = c->s;
    size_t i = c->i;
    if (strncmp(s + i, "true", 4) == 0) { c->i += 4; return fossil_media_json_new_bool(1); }
    if (strncmp(s + i, "false", 5) == 0) { c->i += 5; return fossil_media_json_new_bool(0); }
    if (strncmp(s + i, "null", 4) == 0) { c->i += 4; return fossil_media_json_new_null(); }
    set_error(err, 1, i, "Unexpected token when parsing literal");
    return NULL;
}

/* parse number: simple implementation using strtod */
static fossil_media_json_value_t *parse_number(ctx_t *c, fossil_media_json_error_t *err) {
    const char *s = c->s + c->i;
    char *endptr = NULL;
    double val = strtod(s, &endptr);
    if (endptr == s) {
        set_error(err, 1, c->i, "Invalid number");
        return NULL;
    }
    size_t consumed = (size_t)(endptr - s);
    c->i += consumed;
    return fossil_media_json_new_number(val);
}

/* parse string with escapes */
static fossil_media_json_value_t *parse_string(ctx_t *c, fossil_media_json_error_t *err) {
    const char *s = c->s;
    size_t i = c->i;
    if (s[i] != '"') { set_error(err, 1, i, "Expected '\"'"); return NULL; }
    i++;
    size_t start = i;
    /* Build result in a temp buffer */
    size_t cap = 32;
    char *buf = fm_malloc(cap);
    if (!buf) return NULL;
    size_t len = 0;
    while (s[i]) {
        char ch = s[i++];
        if (ch == '"') {
            buf[len] = '\0';
            c->i = i;
            fossil_media_json_value_t *v = fossil_media_json_new_string(buf);
            fm_free(buf);
            return v;
        } else if (ch == '\\') {
            char esc = s[i++];
            char out = 0;
            if (!esc) break;
            if (esc == '"' || esc == '\\' || esc == '/') out = esc;
            else if (esc == 'b') out = '\b';
            else if (esc == 'f') out = '\f';
            else if (esc == 'n') out = '\n';
            else if (esc == 'r') out = '\r';
            else if (esc == 't') out = '\t';
            else if (esc == 'u') {
                /* Unicode escape: \uXXXX -> encode as UTF-8 */
                unsigned int code = 0;
                for (int k = 0; k < 4; ++k) {
                    char ch2 = s[i++];
                    if (!ch2) { set_error(err, 1, i, "Truncated \\u escape"); fm_free(buf); return NULL; }
                    int digit = -1;
                    if (ch2 >= '0' && ch2 <= '9') digit = ch2 - '0';
                    else if (ch2 >= 'A' && ch2 <= 'F') digit = 10 + (ch2 - 'A');
                    else if (ch2 >= 'a' && ch2 <= 'f') digit = 10 + (ch2 - 'a');
                    if (digit < 0) { set_error(err, 1, i, "Invalid \\u hex digit"); fm_free(buf); return NULL; }
                    code = (code << 4) | digit;
                }
                /* encode code in UTF-8 */
                char tmp[4];
                int tlen = 0;
                if (code <= 0x7F) {
                    tmp[0] = (char)code; tlen = 1;
                } else if (code <= 0x7FF) {
                    tmp[0] = (char)(0xC0 | ((code >> 6) & 0x1F));
                    tmp[1] = (char)(0x80 | (code & 0x3F));
                    tlen = 2;
                } else {
                    tmp[0] = (char)(0xE0 | ((code >> 12) & 0x0F));
                    tmp[1] = (char)(0x80 | ((code >> 6) & 0x3F));
                    tmp[2] = (char)(0x80 | (code & 0x3F));
                    tlen = 3;
                }
                if (len + tlen + 1 > cap) { cap = (len + tlen + 1) * 2; buf = fm_realloc(buf, cap); if (!buf) return NULL; }
                for (int a = 0; a < tlen; ++a) buf[len++] = tmp[a];
                continue;
            } else {
                set_error(err, 1, i, "Invalid escape \\%c", esc); fm_free(buf); return NULL;
            }
            if (len + 2 > cap) { cap = (len + 2) * 2; buf = fm_realloc(buf, cap); if (!buf) return NULL; }
            buf[len++] = out;
        } else {
            if (len + 2 > cap) { cap = (len + 2) * 2; buf = fm_realloc(buf, cap); if (!buf) return NULL; }
            buf[len++] = ch;
        }
    }
    set_error(err, 1, start, "Unterminated string");
    fm_free(buf);
    return NULL;
}

/* Forward declarations */
static fossil_media_json_value_t *parse_value(ctx_t *c, fossil_media_json_error_t *err);

static fossil_media_json_value_t *parse_array(ctx_t *c, fossil_media_json_error_t *err) {
    const char *s = c->s;
    size_t i = c->i;
    if (s[i] != '[') { set_error(err,1,i,"Expected '['"); return NULL; }
    i++;
    c->i = i;
    skip_ws(c);
    fossil_media_json_value_t *arr = fossil_media_json_new_array();
    if (!arr) { set_error(err,1,c->i,"OOM"); return NULL; }
    if (c->s[c->i] == ']') { c->i++; return arr; }
    while (1) {
        skip_ws(c);
        fossil_media_json_value_t *elem = parse_value(c, err);
        if (!elem) { fossil_media_json_free(arr); return NULL; }
        if (fossil_media_json_array_append(arr, elem) != 0) { fossil_media_json_free(elem); fossil_media_json_free(arr); set_error(err,1,c->i,"OOM"); return NULL; }
        skip_ws(c);
        if (c->s[c->i] == ',') { c->i++; continue; }
        else if (c->s[c->i] == ']') { c->i++; break; }
        else { fossil_media_json_free(arr); set_error(err,1,c->i,"Expected ',' or ']' in array"); return NULL; }
    }
    return arr;
}

static fossil_media_json_value_t *parse_object(ctx_t *c, fossil_media_json_error_t *err) {
    const char *s = c->s;
    size_t i = c->i;
    if (s[i] != '{') { set_error(err,1,i,"Expected '{'"); return NULL; }
    i++;
    c->i = i;
    skip_ws(c);
    fossil_media_json_value_t *obj = fossil_media_json_new_object();
    if (!obj) { set_error(err,1,c->i,"OOM"); return NULL; }
    if (c->s[c->i] == '}') { c->i++; return obj; }
    while (1) {
        skip_ws(c);
        if (c->s[c->i] != '"') { fossil_media_json_free(obj); set_error(err,1,c->i,"Expected string key"); return NULL; }
        fossil_media_json_value_t *keyv = parse_string(c, err);
        if (!keyv) { fossil_media_json_free(obj); return NULL; }
        char *key = keyv->u.string;
        /* keyv was allocated as a string node; we steal the char* and free keyv */
        fm_free(keyv);
        skip_ws(c);
        if (c->s[c->i] != ':') { fm_free(key); fossil_media_json_free(obj); set_error(err,1,c->i,"Expected ':' after key"); return NULL; }
        c->i++;
        skip_ws(c);
        fossil_media_json_value_t *val = parse_value(c, err);
        if (!val) { fm_free(key); fossil_media_json_free(obj); return NULL; }
        /* insert key/val */
        if (obj->u.object.count == obj->u.object.capacity) {
            size_t newcap = obj->u.object.capacity ? obj->u.object.capacity * 2 : 4;
            char **nk = fm_realloc(obj->u.object.keys, sizeof(*nk) * newcap);
            fossil_media_json_value_t **nv = fm_realloc(obj->u.object.values, sizeof(*nv) * newcap);
            if (!nk || !nv) { fm_free(nk); fm_free(nv); fm_free(key); fossil_media_json_free(val); fossil_media_json_free(obj); set_error(err,1,c->i,"OOM"); return NULL; }
            obj->u.object.keys = nk;
            obj->u.object.values = nv;
            obj->u.object.capacity = newcap;
        }
        obj->u.object.keys[obj->u.object.count] = key;
        obj->u.object.values[obj->u.object.count] = val;
        obj->u.object.count++;
        skip_ws(c);
        if (c->s[c->i] == ',') { c->i++; continue; }
        else if (c->s[c->i] == '}') { c->i++; break; }
        else { fossil_media_json_free(obj); set_error(err,1,c->i,"Expected ',' or '}' in object"); return NULL; }
    }
    return obj;
}

static fossil_media_json_value_t *parse_value(ctx_t *c, fossil_media_json_error_t *err) {
    skip_ws(c);
    char ch = c->s[c->i];
    if (!ch) { set_error(err,1,c->i,"Unexpected end of input"); return NULL; }
    if (ch == '"') return parse_string(c, err);
    if (ch == '-' || (ch >= '0' && ch <= '9')) return parse_number(c, err);
    if (ch == '{') return parse_object(c, err);
    if (ch == '[') return parse_array(c, err);
    /* literals */
    if (ch == 't' || ch == 'f' || ch == 'n') return parse_literal(c, err);
    set_error(err,1,c->i,"Unexpected token '%c'", ch);
    return NULL;
}

/* Public parse */
fossil_media_json_value_t *fossil_media_json_parse(const char *json_text, fossil_media_json_error_t *err_out) {
    fossil_media_json_error_t errtmp = {0,0,""};
    if (!json_text) { set_error(&errtmp,1,0,"NULL input"); if (err_out) *err_out = errtmp; return NULL; }
    ctx_t c = { json_text, 0 };
    skip_ws(&c);
    fossil_media_json_value_t *root = parse_value(&c, &errtmp);
    if (!root) { if (err_out) *err_out = errtmp; return NULL; }
    skip_ws(&c);
    if (c.s[c.i] != '\0') {
        /* trailing garbage */
        fossil_media_json_free(root);
        set_error(&errtmp,1,c.i,"Trailing characters after JSON value");
        if (err_out) *err_out = errtmp;
        return NULL;
    }
    if (err_out) *err_out = errtmp;
    return root;
}

/* String escaping for stringifier */
static void append_escaped(char **bufp, size_t *lenp, size_t *cap, const char *s) {
    while (*s) {
        unsigned char c = (unsigned char)*s++;
        const char *esc = NULL;
        char tmp[7];
        size_t add = 1;
        if (c == '"' ) { esc = "\\\""; add = 2; }
        else if (c == '\\') { esc = "\\\\"; add = 2; }
        else if (c == '\b') { esc = "\\b"; add = 2; }
        else if (c == '\f') { esc = "\\f"; add = 2; }
        else if (c == '\n') { esc = "\\n"; add = 2; }
        else if (c == '\r') { esc = "\\r"; add = 2; }
        else if (c == '\t') { esc = "\\t"; add = 2; }
        else if (c < 0x20) {
            snprintf(tmp, sizeof(tmp), "\\u%04x", c);
            esc = tmp;
            add = 6;
        }
        if (esc) {
            if (*lenp + add + 1 > *cap) { *cap = (*lenp + add + 1) * 2; *bufp = fm_realloc(*bufp, *cap); }
            memcpy(*bufp + *lenp, esc, add); *lenp += add; continue;
        }
        if (*lenp + 2 > *cap) { *cap = (*lenp + 2) * 2; *bufp = fm_realloc(*bufp, *cap); }
        (*bufp)[(*lenp)++] = c;
    }
}

/* stringify core */
static int stringify_value(const fossil_media_json_value_t *v, char **bufp, size_t *lenp, size_t *cap, int pretty, int depth) {
    if (!v) return -1;
    if (*bufp == NULL) { *cap = 256; *bufp = fm_malloc(*cap); if (!*bufp) return -1; *lenp = 0; }
    switch (v->type) {
        case FOSSIL_MEDIA_JSON_NULL:
            if (*lenp + 5 > *cap) { *cap = (*lenp + 5) * 2; *bufp = fm_realloc(*bufp, *cap); }
            memcpy(*bufp + *lenp, "null", 4); *lenp += 4;
            break;
        case FOSSIL_MEDIA_JSON_BOOL: {
            const char *t = v->u.boolean ? "true" : "false";
            size_t n = v->u.boolean ? 4 : 5;
            if (*lenp + n + 1 > *cap) { *cap = (*lenp + n + 1) * 2; *bufp = fm_realloc(*bufp, *cap); }
            memcpy(*bufp + *lenp, t, n); *lenp += n;
            break;
        }
        case FOSSIL_MEDIA_JSON_NUMBER: {
            char tmp[64];
            int n = snprintf(tmp, sizeof(tmp), "%.17g", v->u.number);
            if (*lenp + (size_t)n + 1 > *cap) { *cap = (*lenp + n + 1) * 2; *bufp = fm_realloc(*bufp, *cap); }
            memcpy(*bufp + *lenp, tmp, n); *lenp += n;
            break;
        }
        case FOSSIL_MEDIA_JSON_STRING: {
            if (*lenp + 3 > *cap) { *cap = (*lenp + 3) * 2; *bufp = fm_realloc(*bufp, *cap); }
            (*bufp)[(*lenp)++] = '"';
            append_escaped(bufp, lenp, cap, v->u.string ? v->u.string : "");
            if (*lenp + 2 > *cap) { *cap = (*lenp + 2) * 2; *bufp = fm_realloc(*bufp, *cap); }
            (*bufp)[(*lenp)++] = '"';
            break;
        }
        case FOSSIL_MEDIA_JSON_ARRAY: {
            if (*lenp + 1 > *cap) { *cap = (*lenp + 1) * 2; *bufp = fm_realloc(*bufp, *cap); }
            (*bufp)[(*lenp)++] = '[';
            for (size_t i = 0; i < v->u.array.count; ++i) {
                if (i) {
                    if (*lenp + 1 > *cap) { *cap = (*lenp + 1) * 2; *bufp = fm_realloc(*bufp, *cap); }
                    (*bufp)[(*lenp)++] = ',';
                }
                if (pretty) {
                    if (*lenp + 1 > *cap) { *cap = (*lenp + 1) * 2; *bufp = fm_realloc(*bufp, *cap); }
                    (*bufp)[(*lenp)++] = '\n';
                    for (int d = 0; d < depth+1; ++d) {
                        if (*lenp + 2 > *cap) { *cap = (*lenp + 2) * 2; *bufp = fm_realloc(*bufp, *cap); }
                        (*bufp)[(*lenp)++] = '\t';
                    }
                }
                if (stringify_value(v->u.array.items[i], bufp, lenp, cap, pretty, depth+1) != 0) return -1;
            }
            if (pretty && v->u.array.count) {
                if (*lenp + 1 > *cap) { *cap = (*lenp + 1) * 2; *bufp = fm_realloc(*bufp, *cap); }
                (*bufp)[(*lenp)++] = '\n';
                for (int d = 0; d < depth; ++d) {
                    if (*lenp + 2 > *cap) { *cap = (*lenp + 2) * 2; *bufp = fm_realloc(*bufp, *cap); }
                    (*bufp)[(*lenp)++] = '\t';
                }
            }
            if (*lenp + 1 > *cap) { *cap = (*lenp + 1) * 2; *bufp = fm_realloc(*bufp, *cap); }
            (*bufp)[(*lenp)++] = ']';
            break;
        }
        case FOSSIL_MEDIA_JSON_OBJECT: {
            if (*lenp + 1 > *cap) { *cap = (*lenp + 1) * 2; *bufp = fm_realloc(*bufp, *cap); }
            (*bufp)[(*lenp)++] = '{';
            for (size_t i = 0; i < v->u.object.count; ++i) {
                if (i) {
                    if (*lenp + 1 > *cap) { *cap = (*lenp + 1) * 2; *bufp = fm_realloc(*bufp, *cap); }
                    (*bufp)[(*lenp)++] = ',';
                }
                if (pretty) {
                    if (*lenp + 1 > *cap) { *cap = (*lenp + 1) * 2; *bufp = fm_realloc(*bufp, *cap); }
                    (*bufp)[(*lenp)++] = '\n';
                    for (int d = 0; d < depth+1; ++d) {
                        if (*lenp + 2 > *cap) { *cap = (*lenp + 2) * 2; *bufp = fm_realloc(*bufp, *cap); }
                        (*bufp)[(*lenp)++] = '\t';
                    }
                }
                /* key */
                if (*lenp + 2 > *cap) { *cap = (*lenp + 2) * 2; *bufp = fm_realloc(*bufp, *cap); }
                (*bufp)[(*lenp)++] = '"';
                append_escaped(bufp, lenp, cap, v->u.object.keys[i]);
                if (*lenp + 3 > *cap) { *cap = (*lenp + 3) * 2; *bufp = fm_realloc(*bufp, *cap); }
                (*bufp)[(*lenp)++] = '"';
                (*bufp)[(*lenp)++] = ':';
                if (pretty) {
                    (*bufp)[(*lenp)++] = '\t';
                }
                if (stringify_value(v->u.object.values[i], bufp, lenp, cap, pretty, depth+1) != 0) return -1;
            }
            if (pretty && v->u.object.count) {
                if (*lenp + 1 > *cap) { *cap = (*lenp + 1) * 2; *bufp = fm_realloc(*bufp, *cap); }
                (*bufp)[(*lenp)++] = '\n';
                for (int d = 0; d < depth; ++d) {
                    if (*lenp + 2 > *cap) { *cap = (*lenp + 2) * 2; *bufp = fm_realloc(*bufp, *cap); }
                    (*bufp)[(*lenp)++] = '\t';
                }
            }
            if (*lenp + 1 > *cap) { *cap = (*lenp + 1) * 2; *bufp = fm_realloc(*bufp, *cap); }
            (*bufp)[(*lenp)++] = '}';
            break;
        }
        default: return -1;
    }
    /* ensure NUL at the top-level only (caller will NUL-terminate) */
    return 0;
}

char *fossil_media_json_stringify(const fossil_media_json_value_t *v, int pretty, fossil_media_json_error_t *err_out) {
    fossil_media_json_error_t errtmp = {0,0,""};
    char *buf = NULL;
    size_t len = 0, cap = 0;
    if (!v) { set_error(&errtmp,1,0,"NULL value"); if (err_out) *err_out = errtmp; return NULL; }
    if (stringify_value(v, &buf, &len, &cap, pretty ? 1 : 0, 0) != 0) { fm_free(buf); set_error(&errtmp,1,0,"Stringify failed"); if (err_out) *err_out = errtmp; return NULL; }
    /* NUL-terminate */
    if (len + 1 > cap) { cap = len + 1; buf = fm_realloc(buf, cap); if (!buf) { set_error(&errtmp,1,0,"OOM"); if (err_out) *err_out = errtmp; return NULL; } }
    buf[len] = '\0';
    if (err_out) *err_out = errtmp;
    return buf;
}

char *fossil_media_json_roundtrip(const char *json_text, int pretty, fossil_media_json_error_t *err_out) {
    fossil_media_json_error_t err;
    fossil_media_json_value_t *v = fossil_media_json_parse(json_text, &err);
    if (!v) { if (err_out) *err_out = err; return NULL; }
    char *s = fossil_media_json_stringify(v, pretty, &err);
    fossil_media_json_free(v);
    if (err_out) *err_out = err;
    return s;
}

const char *fossil_media_json_type_name(fossil_media_json_type_t t) {
    switch (t) {
        case FOSSIL_MEDIA_JSON_NULL: return "null";
        case FOSSIL_MEDIA_JSON_BOOL: return "bool";
        case FOSSIL_MEDIA_JSON_NUMBER: return "number";
        case FOSSIL_MEDIA_JSON_STRING: return "string";
        case FOSSIL_MEDIA_JSON_ARRAY: return "array";
        case FOSSIL_MEDIA_JSON_OBJECT: return "object";
        default: return "unknown";
    }
}

// -----------------------------------------------------------------------------
// Clone & Equality
// -----------------------------------------------------------------------------

static fossil_media_json_value_t *fossil_media_json_clone_internal(const fossil_media_json_value_t *src) {
    if (!src) return NULL;

    fossil_media_json_value_t *copy = NULL;
    switch (src->type) {
    case FOSSIL_MEDIA_JSON_NULL:
        copy = fossil_media_json_new_null();
        break;
    case FOSSIL_MEDIA_JSON_BOOL:
        copy = fossil_media_json_new_bool(src->u.boolean);
        break;
    case FOSSIL_MEDIA_JSON_NUMBER:
        copy = fossil_media_json_new_number(src->u.number);
        break;
    case FOSSIL_MEDIA_JSON_STRING:
        copy = fossil_media_json_new_string(src->u.string);
        break;
    case FOSSIL_MEDIA_JSON_ARRAY:
        copy = fossil_media_json_new_array();
        if (copy) {
            for (size_t i = 0; i < src->u.array.count; i++) {
                fossil_media_json_value_t *child = fossil_media_json_clone_internal(src->u.array.items[i]);
                if (!child || fossil_media_json_array_append(copy, child) != 0) {
                    fossil_media_json_free(copy);
                    return NULL;
                }
            }
        }
        break;
    case FOSSIL_MEDIA_JSON_OBJECT:
        copy = fossil_media_json_new_object();
        if (copy) {
            for (size_t i = 0; i < src->u.object.count; i++) {
                fossil_media_json_value_t *child = fossil_media_json_clone_internal(src->u.object.values[i]);
                if (!child || fossil_media_json_object_set(copy, src->u.object.keys[i], child) != 0) {
                    fossil_media_json_free(copy);
                    return NULL;
                }
            }
        }
        break;
    }
    return copy;
}

fossil_media_json_value_t *
fossil_media_json_clone(const fossil_media_json_value_t *src) {
    return fossil_media_json_clone_internal(src);
}

int fossil_media_json_equals(const fossil_media_json_value_t *a,
                             const fossil_media_json_value_t *b) {
    if (!a || !b) return -1;
    if (a->type != b->type) return 0;

    switch (a->type) {
    case FOSSIL_MEDIA_JSON_NULL:
        return 1;
    case FOSSIL_MEDIA_JSON_BOOL:
        return a->u.boolean == b->u.boolean;
    case FOSSIL_MEDIA_JSON_NUMBER:
        return a->u.number == b->u.number;
    case FOSSIL_MEDIA_JSON_STRING:
        return strcmp(a->u.string, b->u.string) == 0;
    case FOSSIL_MEDIA_JSON_ARRAY:
        if (a->u.array.count != b->u.array.count) return 0;
        for (size_t i = 0; i < a->u.array.count; i++) {
            if (!fossil_media_json_equals(a->u.array.items[i], b->u.array.items[i]))
                return 0;
        }
        return 1;
    case FOSSIL_MEDIA_JSON_OBJECT:
        if (a->u.object.count != b->u.object.count) return 0;
        for (size_t i = 0; i < a->u.object.count; i++) {
            fossil_media_json_value_t *val_b = fossil_media_json_object_get(b, a->u.object.keys[i]);
            if (!val_b || !fossil_media_json_equals(a->u.object.values[i], val_b))
                return 0;
        }
        return 1;
    }
    return 0;
}

// -----------------------------------------------------------------------------
// Type Helpers
// -----------------------------------------------------------------------------

int fossil_media_json_is_null(const fossil_media_json_value_t *v) {
    return v && v->type == FOSSIL_MEDIA_JSON_NULL;
}

int fossil_media_json_is_array(const fossil_media_json_value_t *v) {
    return v && v->type == FOSSIL_MEDIA_JSON_ARRAY;
}

int fossil_media_json_is_object(const fossil_media_json_value_t *v) {
    return v && v->type == FOSSIL_MEDIA_JSON_OBJECT;
}

// -----------------------------------------------------------------------------
// Memory & Capacity
// -----------------------------------------------------------------------------

int fossil_media_json_array_reserve(fossil_media_json_value_t *arr, size_t capacity) {
    if (!arr || arr->type != FOSSIL_MEDIA_JSON_ARRAY) return -1;
    if (capacity <= arr->u.array.capacity) return 0;

    fossil_media_json_value_t **new_items =
        realloc(arr->u.array.items, capacity * sizeof(*new_items));
    if (!new_items) return -1;

    arr->u.array.items = new_items;
    arr->u.array.capacity = capacity;
    return 0;
}

int fossil_media_json_object_reserve(fossil_media_json_value_t *obj, size_t capacity) {
    if (!obj || obj->type != FOSSIL_MEDIA_JSON_OBJECT) return -1;
    if (capacity <= obj->u.object.capacity) return 0;

    char **new_keys = realloc(obj->u.object.keys, capacity * sizeof(*new_keys));
    fossil_media_json_value_t **new_vals =
        realloc(obj->u.object.values, capacity * sizeof(*new_vals));
    if (!new_keys || !new_vals) return -1;

    obj->u.object.keys = new_keys;
    obj->u.object.values = new_vals;
    obj->u.object.capacity = capacity;
    return 0;
}

// -----------------------------------------------------------------------------
// File I/O
// -----------------------------------------------------------------------------

fossil_media_json_value_t *
fossil_media_json_parse_file(const char *filename, fossil_media_json_error_t *err_out) {
    if (!filename) return NULL;
    FILE *f = fopen(filename, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *buf = (char *)malloc(size + 1);
    if (!buf) {
        fclose(f);
        return NULL;
    }
    fread(buf, 1, size, f);
    buf[size] = '\0';
    fclose(f);

    fossil_media_json_value_t *v = fossil_media_json_parse(buf, err_out);
    free(buf);
    return v;
}

int fossil_media_json_write_file(const fossil_media_json_value_t *v,
                                 const char *filename,
                                 int pretty,
                                 fossil_media_json_error_t *err_out) {
    if (!filename || !v) return -1;
    FILE *f = fopen(filename, "wb");
    if (!f) return -1;

    char *s = fossil_media_json_stringify(v, pretty, err_out);
    if (!s) {
        fclose(f);
        return -1;
    }
    fputs(s, f);
    free(s);
    fclose(f);
    return 0;
}

// -----------------------------------------------------------------------------
// Number Handling
// -----------------------------------------------------------------------------

fossil_media_json_value_t *fossil_media_json_new_int(long long i) {
    return fossil_media_json_new_number((double)i);
}

int fossil_media_json_get_int(const fossil_media_json_value_t *v, long long *out) {
    if (!v || v->type != FOSSIL_MEDIA_JSON_NUMBER || !out) return -1;
    *out = (long long)v->u.number;
    return 0;
}

// -----------------------------------------------------------------------------
// Debug & Validation
// -----------------------------------------------------------------------------

void fossil_media_json_debug_dump(const fossil_media_json_value_t *v, int indent) {
    if (!v) {
        printf("%*s(null)\n", indent, "");
        return;
    }
    const char *type = fossil_media_json_type_name(v->type);
    printf("%*sType: %s\n", indent, "", type);

    switch (v->type) {
    case FOSSIL_MEDIA_JSON_NULL:
        break;
    case FOSSIL_MEDIA_JSON_BOOL:
        printf("%*sValue: %s\n", indent + 2, "", v->u.boolean ? "true" : "false");
        break;
    case FOSSIL_MEDIA_JSON_NUMBER:
        printf("%*sValue: %g\n", indent + 2, "", v->u.number);
        break;
    case FOSSIL_MEDIA_JSON_STRING:
        printf("%*sValue: \"%s\"\n", indent + 2, "", v->u.string);
        break;
    case FOSSIL_MEDIA_JSON_ARRAY:
        for (size_t i = 0; i < v->u.array.count; i++) {
            printf("%*s[%zu]\n", indent + 2, "", i);
            fossil_media_json_debug_dump(v->u.array.items[i], indent + 4);
        }
        break;
    case FOSSIL_MEDIA_JSON_OBJECT:
        for (size_t i = 0; i < v->u.object.count; i++) {
            printf("%*s\"%s\":\n", indent + 2, "", v->u.object.keys[i]);
            fossil_media_json_debug_dump(v->u.object.values[i], indent + 4);
        }
        break;
    }
}

int fossil_media_json_validate(const char *json_text, fossil_media_json_error_t *err_out) {
    fossil_media_json_value_t *v = fossil_media_json_parse(json_text, err_out);
    if (!v) return -1;
    fossil_media_json_free(v);
    return 0;
}

// -----------------------------------------------------------------------------
// Path Access
// -----------------------------------------------------------------------------

// Very simple dotted path: "foo.bar[2].baz"
fossil_media_json_value_t *
fossil_media_json_get_path(const fossil_media_json_value_t *root, const char *path) {
    if (!root || !path) return NULL;

    const fossil_media_json_value_t *cur = root;
    char *tokenized = strdup(path);
    if (!tokenized) return NULL;

    char *tok = strtok(tokenized, ".");
    while (tok && cur) {
        if (cur->type == FOSSIL_MEDIA_JSON_OBJECT) {
            cur = fossil_media_json_object_get(cur, tok);
        } else if (cur->type == FOSSIL_MEDIA_JSON_ARRAY) {
            char *end;
            long idx = strtol(tok, &end, 10);
            if (*end == '\0') {
                cur = fossil_media_json_array_get(cur, (size_t)idx);
            } else {
                free(tokenized);
                return NULL;
            }
        } else {
            free(tokenized);
            return NULL;
        }
        tok = strtok(NULL, ".");
    }
    free(tokenized);
    return (fossil_media_json_value_t *)cur;
}
