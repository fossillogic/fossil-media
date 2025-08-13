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
#ifndef FOSSIL_MEDIA_YAML_H
#define FOSSIL_MEDIA_YAML_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief YAML key-value pair node.
 */
typedef struct fossil_media_yaml_node_t {
    char *key;                           /**< YAML key string */
    char *value;                         /**< YAML value string */
    size_t indent;                       /**< Indentation level */
    struct fossil_media_yaml_node_t *next; /**< Next node in linked list */
} fossil_media_yaml_node_t;

/**
 * @brief Parse YAML string into a linked list of nodes.
 * @param input YAML text (null-terminated)
 * @return Head of linked list, or NULL on failure.
 */
fossil_media_yaml_node_t *fossil_media_yaml_parse(const char *input);

/**
 * @brief Free a linked list of YAML nodes.
 * @param head Head of the linked list
 */
void fossil_media_yaml_free(fossil_media_yaml_node_t *head);

/**
 * @brief Get the value for a given key (first match).
 * @param head YAML node list
 * @param key Key string to search
 * @return Value string, or NULL if not found.
 */
const char *fossil_media_yaml_get(const fossil_media_yaml_node_t *head, const char *key);

/**
 * @brief Print the YAML node list to stdout (debug).
 * @param head YAML node list
 */
void fossil_media_yaml_print(const fossil_media_yaml_node_t *head);

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

#endif /* FOSSIL_MEDIA_YAML_H */
