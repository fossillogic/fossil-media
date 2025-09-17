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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Internal representation of a node in the parsed tree */
typedef struct fson_internal_node {
    char *key;
    fossil_media_fson_type_t type;
    union {
        int64_t i64;
        uint64_t u64;
        float f32;
        double f64;
        char c;
        char *str;
    } value;
    size_t child_count;
    struct fson_internal_node *children;
} fson_internal_node_t;

struct fossil_media_fson {
    fson_internal_node_t root;
    char *buffer;  /* holds original text, for string reuse */
};

/* --- Forward declarations --- */
static int parse_document(const char *text, fson_internal_node_t *root);
static void free_node(fson_internal_node_t *node);
static void serialize_node(const fson_internal_node_t *node, FILE *fp, int indent);

/* --- Lexer --- */
typedef enum {
    TOK_EOF, TOK_LBRACE, TOK_RBRACE, TOK_LBRACKET, TOK_RBRACKET,
    TOK_COLON, TOK_COMMA, TOK_STRING, TOK_NUMBER, TOK_IDENT
} token_type_t;

typedef struct {
    token_type_t type;
    const char *start;
    size_t len;
    char suffix[8]; // for type suffixes like i32, f64
} token_t;

typedef struct {
    const char *src;
    size_t pos;
    size_t len;
    token_t current;
} lexer_t;

static void lexer_init(lexer_t *lex, const char *src) {
    lex->src = src;
    lex->pos = 0;
    lex->len = strlen(src);
    lex->current.type = TOK_EOF;
}

static void lexer_skip_ws(lexer_t *lex) {
    while (lex->pos < lex->len && isspace(lex->src[lex->pos]))
        lex->pos++;
}

static int lexer_peek(lexer_t *lex) {
    return lex->pos < lex->len ? lex->src[lex->pos] : 0;
}

static void lexer_next(lexer_t *lex) {
    lexer_skip_ws(lex);
    size_t p = lex->pos;
    if (p >= lex->len) {
        lex->current.type = TOK_EOF;
        return;
    }
    char c = lex->src[p];
    if (c == '{') {
        lex->current.type = TOK_LBRACE;
        lex->current.start = &lex->src[p];
        lex->current.len = 1;
        lex->pos++;
    } else if (c == '}') {
        lex->current.type = TOK_RBRACE;
        lex->current.start = &lex->src[p];
        lex->current.len = 1;
        lex->pos++;
    } else if (c == '[') {
        lex->current.type = TOK_LBRACKET;
        lex->current.start = &lex->src[p];
        lex->current.len = 1;
        lex->pos++;
    } else if (c == ']') {
        lex->current.type = TOK_RBRACKET;
        lex->current.start = &lex->src[p];
        lex->current.len = 1;
        lex->pos++;
    } else if (c == ':') {
        lex->current.type = TOK_COLON;
        lex->current.start = &lex->src[p];
        lex->current.len = 1;
        lex->pos++;
    } else if (c == ',') {
        lex->current.type = TOK_COMMA;
        lex->current.start = &lex->src[p];
        lex->current.len = 1;
        lex->pos++;
    } else if (c == '"') {
        lex->pos++;
        size_t start = lex->pos;
        while (lex->pos < lex->len && lex->src[lex->pos] != '"') {
            if (lex->src[lex->pos] == '\\' && lex->pos + 1 < lex->len)
                lex->pos += 2;
            else
                lex->pos++;
        }
        lex->current.type = TOK_STRING;
        lex->current.start = &lex->src[start];
        lex->current.len = lex->pos - start;
        if (lex->pos < lex->len) lex->pos++; // skip closing quote
    } else if (isdigit(c) || c == '-' || c == '+') {
        size_t start = p;
        while (lex->pos < lex->len && (isdigit(lex->src[lex->pos]) || lex->src[lex->pos] == '.' || lex->src[lex->pos] == 'e' || lex->src[lex->pos] == 'E' || lex->src[lex->pos] == '-' || lex->src[lex->pos] == '+'))
            lex->pos++;
        size_t num_len = lex->pos - start;
        size_t suf_start = lex->pos;
        size_t suf_len = 0;
        while (lex->pos < lex->len && isalpha(lex->src[lex->pos]) && suf_len < sizeof(lex->current.suffix) - 1) {
            lex->current.suffix[suf_len++] = lex->src[lex->pos++];
        }
        lex->current.suffix[suf_len] = 0;
        lex->current.type = TOK_NUMBER;
        lex->current.start = &lex->src[start];
        lex->current.len = num_len;
    } else if (isalpha(c) || c == '_') {
        size_t start = p;
        while (lex->pos < lex->len && (isalnum(lex->src[lex->pos]) || lex->src[lex->pos] == '_'))
            lex->pos++;
        lex->current.type = TOK_IDENT;
        lex->current.start = &lex->src[start];
        lex->current.len = lex->pos - start;
        lex->current.suffix[0] = 0;
    } else {
        lex->current.type = TOK_EOF;
        lex->pos++;
    }
}

/* --- Parser --- */
static char *strndup_local(const char *s, size_t n) {
    char *out = (char *)malloc(n + 1);
    if (!out) return NULL;
    memcpy(out, s, n);
    out[n] = 0;
    return out;
}

/* --- Parser --- */
static int parse_value(lexer_t *lex, fson_internal_node_t *node);

static int parse_object(lexer_t *lex, fson_internal_node_t *node) {
    node->type = FSON_TYPE_OBJECT;
    node->key = NULL;
    node->child_count = 0;
    node->children = NULL;

    lexer_next(lex); /* consume '{' */
    size_t cap = 4;
    node->children = (fson_internal_node_t *)calloc(cap, sizeof(fson_internal_node_t));
    if (!node->children) return FOSSIL_MEDIA_FSON_ERR_NOMEM;

    while (lex->current.type != TOK_RBRACE && lex->current.type != TOK_EOF) {
        if (lex->current.type != TOK_STRING && lex->current.type != TOK_IDENT)
            return FOSSIL_MEDIA_FSON_ERR_PARSE;

        char *key = strndup_local(lex->current.start, lex->current.len);
        lexer_next(lex);
        if (lex->current.type != TOK_COLON) {
            free(key);
            return FOSSIL_MEDIA_FSON_ERR_PARSE;
        }
        lexer_next(lex); /* consume ':' */

        if (node->child_count == cap) {
            cap *= 2;
            fson_internal_node_t *tmp = realloc(node->children, cap * sizeof(fson_internal_node_t));
            if (!tmp) {
                free(key);
                return FOSSIL_MEDIA_FSON_ERR_NOMEM;
            }
            node->children = tmp;
        }

        fson_internal_node_t *child = &node->children[node->child_count++];
        memset(child, 0, sizeof(*child));
        child->key = key;

        int rc = parse_value(lex, child);
        if (rc != FOSSIL_MEDIA_FSON_OK) return rc;

        if (lex->current.type == TOK_COMMA)
            lexer_next(lex);
    }

    if (lex->current.type != TOK_RBRACE)
        return FOSSIL_MEDIA_FSON_ERR_PARSE;

    lexer_next(lex); /* consume '}' */
    return FOSSIL_MEDIA_FSON_OK;
}

static int parse_array(lexer_t *lex, fson_internal_node_t *node) {
    node->type = FSON_TYPE_ARRAY;
    node->key = NULL;
    node->child_count = 0;
    node->children = NULL;

    lexer_next(lex); /* consume '[' */
    size_t cap = 4;
    node->children = (fson_internal_node_t *)calloc(cap, sizeof(fson_internal_node_t));
    if (!node->children) return FOSSIL_MEDIA_FSON_ERR_NOMEM;

    while (lex->current.type != TOK_RBRACKET && lex->current.type != TOK_EOF) {
        if (node->child_count == cap) {
            cap *= 2;
            fson_internal_node_t *tmp = realloc(node->children, cap * sizeof(fson_internal_node_t));
            if (!tmp) return FOSSIL_MEDIA_FSON_ERR_NOMEM;
            node->children = tmp;
        }

        fson_internal_node_t *child = &node->children[node->child_count++];
        memset(child, 0, sizeof(*child));

        int rc = parse_value(lex, child);
        if (rc != FOSSIL_MEDIA_FSON_OK) return rc;

        if (lex->current.type == TOK_COMMA)
            lexer_next(lex);
    }

    if (lex->current.type != TOK_RBRACKET)
        return FOSSIL_MEDIA_FSON_ERR_PARSE;

    lexer_next(lex); /* consume ']' */
    return FOSSIL_MEDIA_FSON_OK;
}

static int parse_value(lexer_t *lex, fson_internal_node_t *node) {
    if (lex->current.type == TOK_STRING) {
        node->type = FSON_TYPE_CSTR;
        node->value.str = strndup_local(lex->current.start, lex->current.len);
        lexer_next(lex);
        return FOSSIL_MEDIA_FSON_OK;
    }
    else if (lex->current.type == TOK_NUMBER) {
        const char *numstr = lex->current.start;
        int base = 10;

        /* Type suffix dispatch */
        if (strcmp(lex->current.suffix, "i8") == 0) {
            node->type = FSON_TYPE_I8;
            node->value.i64 = (int8_t)strtol(numstr, NULL, base);
        } else if (strcmp(lex->current.suffix, "i16") == 0) {
            node->type = FSON_TYPE_I16;
            node->value.i64 = (int16_t)strtol(numstr, NULL, base);
        } else if (strcmp(lex->current.suffix, "i32") == 0) {
            node->type = FSON_TYPE_I32;
            node->value.i64 = (int32_t)strtol(numstr, NULL, base);
        } else if (strcmp(lex->current.suffix, "i64") == 0) {
            node->type = FSON_TYPE_I64;
            node->value.i64 = strtoll(numstr, NULL, base);
        } else if (strcmp(lex->current.suffix, "u8") == 0) {
            node->type = FSON_TYPE_U8;
            node->value.u64 = (uint8_t)strtoul(numstr, NULL, base);
        } else if (strcmp(lex->current.suffix, "u16") == 0) {
            node->type = FSON_TYPE_U16;
            node->value.u64 = (uint16_t)strtoul(numstr, NULL, base);
        } else if (strcmp(lex->current.suffix, "u32") == 0) {
            node->type = FSON_TYPE_U32;
            node->value.u64 = (uint32_t)strtoul(numstr, NULL, base);
        } else if (strcmp(lex->current.suffix, "u64") == 0) {
            node->type = FSON_TYPE_U64;
            node->value.u64 = strtoull(numstr, NULL, base);
        } else if (strcmp(lex->current.suffix, "f32") == 0) {
            node->type = FSON_TYPE_F32;
            node->value.f32 = strtof(numstr, NULL);
        } else if (strcmp(lex->current.suffix, "f64") == 0) {
            node->type = FSON_TYPE_F64;
            node->value.f64 = strtod(numstr, NULL);
        } else if (strcmp(lex->current.suffix, "oct") == 0) {
            node->type = FSON_TYPE_OCT;
            node->value.u64 = strtoull(numstr, NULL, 8);
        } else if (strcmp(lex->current.suffix, "hex") == 0) {
            node->type = FSON_TYPE_HEX;
            node->value.u64 = strtoull(numstr, NULL, 16);
        } else if (strcmp(lex->current.suffix, "bin") == 0) {
            node->type = FSON_TYPE_BIN;
            node->value.u64 = strtoull(numstr, NULL, 2);
        } else if (strcmp(lex->current.suffix, "c") == 0) {
            node->type = FSON_TYPE_CHAR;
            node->value.c = (char)strtol(numstr, NULL, base);
        } else {
            /* Default: assume signed 64-bit integer */
            node->type = FSON_TYPE_I64;
            node->value.i64 = strtoll(numstr, NULL, base);
        }

        lexer_next(lex);
        return FOSSIL_MEDIA_FSON_OK;
    }
    else if (lex->current.type == TOK_LBRACE) {
        return parse_object(lex, node);
    }
    else if (lex->current.type == TOK_LBRACKET) {
        return parse_array(lex, node);
    }
    else if (lex->current.type == TOK_IDENT) {
        if (lex->current.len == 4 && strncmp(lex->current.start, "true", 4) == 0) {
            node->type = FSON_TYPE_BOOL;
            node->value.i64 = 1;
        } else if (lex->current.len == 5 && strncmp(lex->current.start, "false", 5) == 0) {
            node->type = FSON_TYPE_BOOL;
            node->value.i64 = 0;
        } else if (lex->current.len == 4 && strncmp(lex->current.start, "null", 4) == 0) {
            node->type = FSON_TYPE_NULL;
        } else {
            return FOSSIL_MEDIA_FSON_ERR_PARSE;
        }
        lexer_next(lex);
        return FOSSIL_MEDIA_FSON_OK;
    }
    return FOSSIL_MEDIA_FSON_ERR_PARSE;
}

static int parse_document(const char *text, fson_internal_node_t *root) {
    lexer_t lex;
    lexer_init(&lex, text);
    lexer_next(&lex);

    int rc = parse_value(&lex, root);
    if (rc != FOSSIL_MEDIA_FSON_OK)
        return rc;

    if (lex.current.type != TOK_EOF)
        return FOSSIL_MEDIA_FSON_ERR_PARSE;

    return FOSSIL_MEDIA_FSON_OK;
}

/* --- API Implementation --- */

int fossil_media_fson_load_file(const char *path, fossil_media_fson_t **out) {
    if (!path || !out)
        return FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;

    FILE *fp = fopen(path, "rb");
    if (!fp)
        return FOSSIL_MEDIA_FSON_ERR_IO;

    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    rewind(fp);

    char *buf = (char *)malloc(len + 1);
    if (!buf) {
        fclose(fp);
        return FOSSIL_MEDIA_FSON_ERR_NOMEM;
    }
    fread(buf, 1, len, fp);
    buf[len] = '\0';
    fclose(fp);

    fossil_media_fson_t *fson = (fossil_media_fson_t *)calloc(1, sizeof(*fson));
    if (!fson) {
        free(buf);
        return FOSSIL_MEDIA_FSON_ERR_NOMEM;
    }

    fson->buffer = buf;
    int rc = parse_document(buf, &fson->root);
    if (rc != FOSSIL_MEDIA_FSON_OK) {
        fossil_media_fson_free(fson);
        return rc;
    }

    *out = fson;
    return FOSSIL_MEDIA_FSON_OK;
}

int fossil_media_fson_load_memory(const void *buf, size_t len, fossil_media_fson_t **out) {
    if (!buf || !out)
        return FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;

    char *copy = (char *)malloc(len + 1);
    if (!copy)
        return FOSSIL_MEDIA_FSON_ERR_NOMEM;

    memcpy(copy, buf, len);
    copy[len] = '\0';

    fossil_media_fson_t *fson = (fossil_media_fson_t *)calloc(1, sizeof(*fson));
    if (!fson) {
        free(copy);
        return FOSSIL_MEDIA_FSON_ERR_NOMEM;
    }

    fson->buffer = copy;
    int rc = parse_document(copy, &fson->root);
    if (rc != FOSSIL_MEDIA_FSON_OK) {
        fossil_media_fson_free(fson);
        return rc;
    }

    *out = fson;
    return FOSSIL_MEDIA_FSON_OK;
}

void fossil_media_fson_free(fossil_media_fson_t *fson) {
    if (!fson)
        return;
    free_node(&fson->root);
    free(fson->buffer);
    free(fson);
}

const fossil_media_fson_node_t *fossil_media_fson_root(const fossil_media_fson_t *fson) {
    return fson ? (const fossil_media_fson_node_t *)&fson->root : NULL;
}

const fossil_media_fson_node_t *fossil_media_fson_get(const fossil_media_fson_node_t *node, const char *key) {
    if (!node || node->type != FSON_TYPE_OBJECT)
        return NULL;
    const fson_internal_node_t *internal = (const fson_internal_node_t *)node;
    for (size_t i = 0; i < internal->child_count; ++i) {
        if (internal->children[i].key && strcmp(internal->children[i].key, key) == 0)
            return (const fossil_media_fson_node_t *)&internal->children[i];
    }
    return NULL;
}

const fossil_media_fson_node_t *fossil_media_fson_at(const fossil_media_fson_node_t *node, size_t index) {
    if (!node || node->type != FSON_TYPE_ARRAY)
        return NULL;
    const fson_internal_node_t *internal = (const fson_internal_node_t *)node;
    if (index >= internal->child_count)
        return NULL;
    return (const fossil_media_fson_node_t *)&internal->children[index];
}

int fossil_media_fson_serialize(const fossil_media_fson_t *fson, char **out_text, size_t *out_len) {
    if (!fson || !out_text)
        return FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;

    /* Serialize to a memory stream */
    FILE *mem = open_memstream(out_text, out_len);
    if (!mem)
        return FOSSIL_MEDIA_FSON_ERR_IO;

    serialize_node(&fson->root, mem, 0);
    fclose(mem);
    return FOSSIL_MEDIA_FSON_OK;
}

int fossil_media_fson_validate(const fossil_media_fson_t *fson) {
    if (!fson)
        return FOSSIL_MEDIA_FSON_ERR_INVALID_ARG;

    /* Minimal validation: check type consistency recursively */
    /* For now, assume parse_document enforces correctness */
    return FOSSIL_MEDIA_FSON_OK;
}

/* --- Internal Functions --- */

static void free_node(fson_internal_node_t *node) {
    if (!node) return;
    for (size_t i = 0; i < node->child_count; ++i)
        free_node(&node->children[i]);
    free(node->children);
    free(node->key);
    if (node->type == FSON_TYPE_CSTR && node->value.str)
        free(node->value.str);
}

/* Dummy serializer: just print "{}" for now */
static void serialize_node(const fson_internal_node_t *node, FILE *fp, int indent) {
    (void)indent;
    if (node->type == FSON_TYPE_OBJECT) {
        fprintf(fp, "{}");
    } else if (node->type == FSON_TYPE_ARRAY) {
        fprintf(fp, "[]");
    }
}
