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
#ifndef FOSSIL_MEDIA_MD_H
#define FOSSIL_MEDIA_MD_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Markdown element types
 */
typedef enum {
    FOSSIL_MEDIA_MD_TEXT,       /**< Plain text */
    FOSSIL_MEDIA_MD_HEADING,    /**< # Heading */
    FOSSIL_MEDIA_MD_BOLD,       /**< **bold** */
    FOSSIL_MEDIA_MD_ITALIC,     /**< *italic* */
    FOSSIL_MEDIA_MD_CODE,       /**< Inline `code` */
    FOSSIL_MEDIA_MD_CODE_BLOCK, /**< ```code block``` */
    FOSSIL_MEDIA_MD_LIST_ITEM,  /**< - List item */
    FOSSIL_MEDIA_MD_LINK,       /**< [text](url) */
    FOSSIL_MEDIA_MD_PARAGRAPH,  /**< Paragraph block */
} fossil_media_md_type_t;

/**
 * @brief Parsed Markdown node
 */
typedef struct fossil_media_md_node_t {
    fossil_media_md_type_t type;   /**< Node type */
    char *content;                 /**< Text content or inline data */
    char *extra;                   /**< Extra data (e.g., link URL) */

    struct fossil_media_md_node_t **children; /**< Child nodes (for lists, blocks) */
    size_t child_count;
    int level;

    struct fossil_media_md_node_t *parent;    /**< Parent node */
} fossil_media_md_node_t;

/**
 * @brief Parse Markdown text into a tree of nodes
 */
fossil_media_md_node_t *fossil_media_md_parse(const char *input);

/**
 * @brief Serialize a Markdown node tree back into Markdown text
 */
char *fossil_media_md_serialize(const fossil_media_md_node_t *root);

/**
 * @brief Free a Markdown node tree
 */
void fossil_media_md_free(fossil_media_md_node_t *node);

#ifdef __cplusplus
}
#include <string>
#include <stdexcept>
#include <utility>

namespace fossil {

    namespace media {



    } // namespace media

} // namespace fossil

#endif

#endif /* FOSSIL_MEDIA_MD_H */
