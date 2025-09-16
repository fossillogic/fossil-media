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
#include "fossil/media/elf.h"
#include "fossil/media/media.h"

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* Minimal ELF64 structures (System V) - only the fields we need */
typedef struct {
    unsigned char e_ident[16];
    uint16_t      e_type;
    uint16_t      e_machine;
    uint32_t      e_version;
    uint64_t      e_entry;
    uint64_t      e_phoff;
    uint64_t      e_shoff;
    uint32_t      e_flags;
    uint16_t      e_ehsize;
    uint16_t      e_phentsize;
    uint16_t      e_phnum;
    uint16_t      e_shentsize;
    uint16_t      e_shnum;
    uint16_t      e_shstrndx;
} Elf64_Ehdr;

typedef struct {
    uint32_t   sh_name;
    uint32_t   sh_type;
    uint64_t   sh_flags;
    uint64_t   sh_addr;
    uint64_t   sh_offset;
    uint64_t   sh_size;
    uint32_t   sh_link;
    uint32_t   sh_info;
    uint64_t   sh_addralign;
    uint64_t   sh_entsize;
} Elf64_Shdr;

/* Opaque handle definition */
struct fossil_media_elf {
    uint8_t *buf;         /* file bytes (malloc) */
    size_t   size;        /* file size */
    Elf64_Ehdr *ehdr;     /* pointer into buf */
    Elf64_Shdr *shdrs;    /* pointer into buf */
    const char *shstrtab; /* pointer into buf (section header string table) */
    size_t sh_count;
};

/* Constants for ELF header */
static const unsigned char ELF_MAGIC[4] = {0x7f, 'E', 'L', 'F'};
#define EI_CLASS 4
#define ELFCLASS64 2
#define EI_DATA 5
#define ELFDATA2LSB 1

int fossil_media_elf_is_elf(const void *buf, size_t len) {
    if (!buf || len < 4) return 0;
    const unsigned char *b = (const unsigned char*)buf;
    return (b[0]==ELF_MAGIC[0] && b[1]==ELF_MAGIC[1] && b[2]==ELF_MAGIC[2] && b[3]==ELF_MAGIC[3]) ? 1 : 0;
}

/* internal helpers */
static int read_file_to_buffer(const char *path, uint8_t **out_buf, size_t *out_size) {
    FILE *f = fopen(path, "rb");
    if (!f) return FOSSIL_MEDIA_ELF_ERR_IO;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return FOSSIL_MEDIA_ELF_ERR_IO; }
    long sz = ftell(f);
    if (sz < 0) { fclose(f); return FOSSIL_MEDIA_ELF_ERR_IO; }
    if (fseek(f, 0, SEEK_SET) != 0) { fclose(f); return FOSSIL_MEDIA_ELF_ERR_IO; }
    uint8_t *buf = (uint8_t*)malloc((size_t)sz);
    if (!buf) { fclose(f); return FOSSIL_MEDIA_ELF_ERR_NOMEM; }
    size_t r = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (r != (size_t)sz) { free(buf); return FOSSIL_MEDIA_ELF_ERR_IO; }
    *out_buf = buf;
    *out_size = (size_t)sz;
    return FOSSIL_MEDIA_ELF_OK;
}

int fossil_media_elf_load_from_file(const char *path, fossil_media_elf_t **out) {
    if (!path || !out) return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT;
    uint8_t *buf = NULL;
    size_t size = 0;
    int rc = read_file_to_buffer(path, &buf, &size);
    if (rc != FOSSIL_MEDIA_ELF_OK) return rc;

    if (size < sizeof(Elf64_Ehdr)) { free(buf); return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT; }
    if (!fossil_media_elf_is_elf(buf, size)) { free(buf); return FOSSIL_MEDIA_ELF_ERR_NOT_ELF; }

    Elf64_Ehdr *ehdr = (Elf64_Ehdr*)buf;

    /* Check class and data: only support ELF64 little-endian for now. */
    if (ehdr->e_ident[EI_CLASS] != ELFCLASS64) { free(buf); return FOSSIL_MEDIA_ELF_ERR_UNSUPPORTED; }
    if (ehdr->e_ident[EI_DATA] != ELFDATA2LSB) { free(buf); return FOSSIL_MEDIA_ELF_ERR_UNSUPPORTED; }

    /* Basic bounds checks */
    uint64_t shoff = ehdr->e_shoff;
    uint16_t shentsize = ehdr->e_shentsize;
    uint16_t shnum = ehdr->e_shnum;
    uint16_t shstrndx = ehdr->e_shstrndx;

    if (shoff == 0 || shnum == 0) {
        /* no section table */
        free(buf);
        return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT;
    }

    /* Ensure section header table fits */
    if ((uint64_t)shoff + (uint64_t)shentsize * (uint64_t)shnum > size) {
        free(buf);
        return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT;
    }

    Elf64_Shdr *shdrs = (Elf64_Shdr*)(buf + shoff);

    /* locate section header string table */
    if (shstrndx >= shnum) { free(buf); return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT; }
    Elf64_Shdr *shstr_sh = &shdrs[shstrndx];
    if (shstr_sh->sh_offset + shstr_sh->sh_size > size) { free(buf); return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT; }
    const char *shstrtab = (const char*)(buf + shstr_sh->sh_offset);

    fossil_media_elf_t *h = (fossil_media_elf_t*)malloc(sizeof(fossil_media_elf_t));
    if (!h) { free(buf); return FOSSIL_MEDIA_ELF_ERR_NOMEM; }
    h->buf = buf;
    h->size = size;
    h->ehdr = ehdr;
    h->shdrs = shdrs;
    h->shstrtab = shstrtab;
    h->sh_count = shnum;
    *out = h;
    return FOSSIL_MEDIA_ELF_OK;
}

void fossil_media_elf_free(fossil_media_elf_t *elf) {
    if (!elf) return;
    if (elf->buf) free(elf->buf);
    free(elf);
}

int fossil_media_elf_get_section_count(const fossil_media_elf_t *elf, size_t *out_count) {
    if (!elf || !out_count) return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT;
    *out_count = elf->sh_count;
    return FOSSIL_MEDIA_ELF_OK;
}

int fossil_media_elf_get_section_header(const fossil_media_elf_t *elf, size_t index, fossil_media_elf_shdr_t *out_shdr) {
    if (!elf || !out_shdr) return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT;
    if (index >= elf->sh_count) return FOSSIL_MEDIA_ELF_ERR_RANGE;
    Elf64_Shdr *s = &elf->shdrs[index];
    out_shdr->sh_name = s->sh_name;
    out_shdr->sh_type = s->sh_type;
    out_shdr->sh_flags = s->sh_flags;
    out_shdr->sh_addr = s->sh_addr;
    out_shdr->sh_offset = s->sh_offset;
    out_shdr->sh_size = s->sh_size;
    out_shdr->sh_link = s->sh_link;
    out_shdr->sh_info = s->sh_info;
    out_shdr->sh_addralign = s->sh_addralign;
    out_shdr->sh_entsize = s->sh_entsize;
    return FOSSIL_MEDIA_ELF_OK;
}

int fossil_media_elf_get_section_name(const fossil_media_elf_t *elf, size_t index, const char **out_name) {
    if (!elf || !out_name) return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT;
    if (index >= elf->sh_count) return FOSSIL_MEDIA_ELF_ERR_RANGE;
    Elf64_Shdr *s = &elf->shdrs[index];
    uint32_t off = s->sh_name;
    /* ensure within shstrtab */
    /* find shstrtab bounds */
    uint16_t shstrndx = elf->ehdr->e_shstrndx;
    if (shstrndx >= elf->sh_count) return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT;
    Elf64_Shdr *shstr = &elf->shdrs[shstrndx];
    if ((uint64_t)off >= shstr->sh_size) return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT;
    *out_name = elf->shstrtab + off;
    return FOSSIL_MEDIA_ELF_OK;
}

int fossil_media_elf_get_section_data(const fossil_media_elf_t *elf, size_t index, const uint8_t **out_ptr, size_t *out_len) {
    if (!elf || !out_ptr || !out_len) return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT;
    if (index >= elf->sh_count) return FOSSIL_MEDIA_ELF_ERR_RANGE;
    Elf64_Shdr *s = &elf->shdrs[index];
    uint64_t off = s->sh_offset;
    uint64_t sz = s->sh_size;
    if (off + sz > elf->size) return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT;
    *out_ptr = elf->buf + off;
    *out_len = (size_t)sz;
    return FOSSIL_MEDIA_ELF_OK;
}

int fossil_media_elf_find_section_by_name(const fossil_media_elf_t *elf, const char *name, size_t *out_index) {
    if (!elf || !name || !out_index) return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT;
    for (size_t i = 0; i < elf->sh_count; ++i) {
        const char *sname = NULL;
        if (fossil_media_elf_get_section_name(elf, i, &sname) != FOSSIL_MEDIA_ELF_OK) continue;
        if (sname && strcmp(sname, name) == 0) {
            *out_index = i;
            return FOSSIL_MEDIA_ELF_OK;
        }
    }
    return FOSSIL_MEDIA_ELF_ERR_NO_SECTION;
}

int fossil_media_elf_extract_section_to_file(const fossil_media_elf_t *elf, size_t index, const char *out_path) {
    if (!elf || !out_path) return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT;
    const uint8_t *ptr = NULL;
    size_t len = 0;
    int rc = fossil_media_elf_get_section_data(elf, index, &ptr, &len);
    if (rc != FOSSIL_MEDIA_ELF_OK) return rc;
    FILE *f = fopen(out_path, "wb");
    if (!f) return FOSSIL_MEDIA_ELF_ERR_IO;
    size_t w = fwrite(ptr, 1, len, f);
    fclose(f);
    if (w != len) return FOSSIL_MEDIA_ELF_ERR_IO;
    return FOSSIL_MEDIA_ELF_OK;
}

const char* fossil_media_elf_strerror(int err) {
    switch (err) {
        case FOSSIL_MEDIA_ELF_OK: return "OK";
        case FOSSIL_MEDIA_ELF_ERR_IO: return "I/O error";
        case FOSSIL_MEDIA_ELF_ERR_NOMEM: return "Out of memory";
        case FOSSIL_MEDIA_ELF_ERR_NOT_ELF: return "Not an ELF file";
        case FOSSIL_MEDIA_ELF_ERR_UNSUPPORTED: return "Unsupported ELF format";
        case FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT: return "Malformed ELF file";
        case FOSSIL_MEDIA_ELF_ERR_RANGE: return "Index out of range";
        case FOSSIL_MEDIA_ELF_ERR_NO_SECTION: return "Section not found";
        case FOSSIL_MEDIA_ELF_ERR_INVALID_ARG: return "Invalid argument";
        default: return "Unknown error";
    }
}

int fossil_media_elf_load_from_memory(const void *buf, size_t len, fossil_media_elf_t **out) {
    if (!buf || !out || len < sizeof(Elf64_Ehdr))
        return FOSSIL_MEDIA_ELF_ERR_INVALID_ARG;
    if (!fossil_media_elf_is_elf(buf, len))
        return FOSSIL_MEDIA_ELF_ERR_NOT_ELF;

    Elf64_Ehdr *ehdr = (Elf64_Ehdr*)buf;
    if (ehdr->e_ident[EI_CLASS] != ELFCLASS64)
        return FOSSIL_MEDIA_ELF_ERR_UNSUPPORTED;
    if (ehdr->e_ident[EI_DATA] != ELFDATA2LSB)
        return FOSSIL_MEDIA_ELF_ERR_UNSUPPORTED;

    uint64_t shoff = ehdr->e_shoff;
    uint16_t shentsize = ehdr->e_shentsize;
    uint16_t shnum = ehdr->e_shnum;
    uint16_t shstrndx = ehdr->e_shstrndx;

    if (shoff == 0 || shnum == 0)
        return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT;
    if ((uint64_t)shoff + (uint64_t)shentsize * (uint64_t)shnum > len)
        return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT;

    Elf64_Shdr *shdrs = (Elf64_Shdr *)((const uint8_t *)buf + shoff);

    if (shstrndx >= shnum)
        return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT;
    Elf64_Shdr *shstr_sh = &shdrs[shstrndx];
    if (shstr_sh->sh_offset + shstr_sh->sh_size > len)
        return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT;
    const char *shstrtab = (const char *)((const uint8_t *)buf + shstr_sh->sh_offset);

    fossil_media_elf_t *h = (fossil_media_elf_t *)malloc(sizeof(fossil_media_elf_t));
    if (!h)
        return FOSSIL_MEDIA_ELF_ERR_NOMEM;
    h->buf = NULL; // Not owned, do not free
    h->size = len;
    h->ehdr = ehdr;
    h->shdrs = shdrs;
    h->shstrtab = shstrtab;
    h->sh_count = shnum;
    *out = h;
    return FOSSIL_MEDIA_ELF_OK;
}

int fossil_media_elf_get_section_info(const fossil_media_elf_t *elf,
                                      size_t index,
                                      const char **out_name,
                                      const uint8_t **out_ptr,
                                      size_t *out_len) {
    if (!elf)
        return FOSSIL_MEDIA_ELF_ERR_INVALID_ARG;
    if (index >= elf->sh_count)
        return FOSSIL_MEDIA_ELF_ERR_RANGE;

    const Elf64_Shdr *shdr = &elf->shdrs[index];

    if (out_name && elf->shstrtab)
        *out_name = elf->shstrtab + shdr->sh_name;
    if (out_ptr)
        *out_ptr = (const uint8_t *)((const uint8_t *)(elf->ehdr) + shdr->sh_offset);
    if (out_len)
        *out_len = (size_t)shdr->sh_size;

    return FOSSIL_MEDIA_ELF_OK;
}
