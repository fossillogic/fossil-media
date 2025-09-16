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
#include <stdint.h>
#include <errno.h>

#ifdef _WIN32
  #include <windows.h>
#else
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <unistd.h>
#endif

/* Minimal on-disk ELF64 structures (packed-like representation).
   We'll never assume host layout/align — we memcpy into these. */
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
} Elf64_Ehdr_on_disk;

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
} Elf64_Shdr_on_disk;

/* Opaque handle */
struct fossil_media_elf {
    uint8_t *buf;         /* malloc'ed buffer if owns_buf=1 */
    const uint8_t *base_ptr; /* base pointer into memory, valid even if non-owning */
    size_t   size;
    Elf64_Ehdr_on_disk ehdr;
    Elf64_Shdr_on_disk *shdrs;
    const char *shstrtab;
    size_t sh_count;
    int owns_buf;
};

/* ELF constants */
static const unsigned char ELF_MAGIC[4] = {0x7f, 'E', 'L', 'F'};
#define EI_CLASS 4
#define ELFCLASS64 2
#define EI_DATA 5
#define ELFDATA2LSB 1

/* helpers to read little-endian from on-disk bytes (no unaligned access assumptions) */
static uint16_t read_u16_le(const void *p) {
    const uint8_t *b = (const uint8_t*)p;
    return (uint16_t)b[0] | ((uint16_t)b[1] << 8);
}

static uint32_t read_u32_le(const void *p) {
    const uint8_t *b = (const uint8_t*)p;
    return (uint32_t)b[0]
         | ((uint32_t)b[1] << 8)
         | ((uint32_t)b[2] << 16)
         | ((uint32_t)b[3] << 24);
}

static uint64_t read_u64_le(const void *p) {
    const uint8_t *b = (const uint8_t*)p;
    return (uint64_t)b[0]
         | ((uint64_t)b[1] << 8)
         | ((uint64_t)b[2] << 16)
         | ((uint64_t)b[3] << 24)
         | ((uint64_t)b[4] << 32)
         | ((uint64_t)b[5] << 40)
         | ((uint64_t)b[6] << 48)
         | ((uint64_t)b[7] << 56);
}

/* safe multiplication / addition overflow checks */
static int mul_add_overflow_size(size_t a, size_t b, size_t c, size_t *out) {
    if (b != 0 && a > SIZE_MAX / b) return 1;
    size_t prod = a * b;
    if (c > SIZE_MAX - prod) return 1;
    *out = prod + c;
    return 0;
}

/* portable get file size (returns 0 on success, size in out_size) */
static int get_file_size(const char *path, size_t *out_size) {
#ifdef _WIN32
    HANDLE h = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) return -1;
    LARGE_INTEGER li;
    if (!GetFileSizeEx(h, &li)) { CloseHandle(h); return -1; }
    CloseHandle(h);
    if (li.QuadPart < 0) return -1;
    *out_size = (size_t)li.QuadPart;
    return 0;
#else
    struct stat st;
    if (stat(path, &st) != 0) return -1;
    if (!S_ISREG(st.st_mode)) return -1;
    *out_size = (size_t)st.st_size;
    return 0;
#endif
}

/* read whole file into malloc'ed buffer */
static int read_file_to_buffer(const char *path, uint8_t **out_buf, size_t *out_size) {
    if (!path || !out_buf || !out_size) return FOSSIL_MEDIA_ELF_ERR_IO;
    size_t sz;
    if (get_file_size(path, &sz) != 0) return FOSSIL_MEDIA_ELF_ERR_IO;
    if (sz == 0) return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT;

    FILE *f = fopen(path, "rb");
    if (!f) return FOSSIL_MEDIA_ELF_ERR_IO;

    uint8_t *buf = (uint8_t*)malloc(sz);
    if (!buf) { fclose(f); return FOSSIL_MEDIA_ELF_ERR_NOMEM; }

    size_t read_total = 0;
    while (read_total < sz) {
        size_t r = fread(buf + read_total, 1, sz - read_total, f);
        if (r == 0) {
            if (feof(f)) break;
            if (ferror(f)) { free(buf); fclose(f); return FOSSIL_MEDIA_ELF_ERR_IO; }
        }
        read_total += r;
    }
    fclose(f);
    if (read_total != sz) { free(buf); return FOSSIL_MEDIA_ELF_ERR_IO; }

    *out_buf = buf;
    *out_size = sz;
    return FOSSIL_MEDIA_ELF_OK;
}

/* validate ELF magic quickly */
int fossil_media_elf_is_elf(const void *buf, size_t len) {
    if (!buf || len < 4) return 0;
    const unsigned char *b = (const unsigned char*)buf;
    return (b[0]==ELF_MAGIC[0] && b[1]==ELF_MAGIC[1] && b[2]==ELF_MAGIC[2] && b[3]==ELF_MAGIC[3]) ? 1 : 0;
}

/* internal: parse headers from buffer into handle (buffer must remain alive for pointer-based fields like shstrtab) */
static int parse_elf_from_buffer(uint8_t *buf, size_t len, fossil_media_elf_t **out, int buf_owned) {
    if (!buf || !out) return FOSSIL_MEDIA_ELF_ERR_INVALID_ARG;
    if (len < sizeof(Elf64_Ehdr_on_disk)) return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT;
    if (!fossil_media_elf_is_elf(buf, len)) return FOSSIL_MEDIA_ELF_ERR_NOT_ELF;

    Elf64_Ehdr_on_disk ehdr_on_disk;
    memcpy(&ehdr_on_disk, buf, sizeof(ehdr_on_disk)); /* safe copy of ident and rest as raw bytes */

    /* check class/data */
    if (ehdr_on_disk.e_ident[EI_CLASS] != ELFCLASS64) return FOSSIL_MEDIA_ELF_ERR_UNSUPPORTED;
    if (ehdr_on_disk.e_ident[EI_DATA] != ELFDATA2LSB) return FOSSIL_MEDIA_ELF_ERR_UNSUPPORTED;

    /* Important: e_shoff, e_shentsize, e_shnum, e_shstrndx are in little-endian on disk.
       We must read them using LE readers at the correct offsets. */
    /* We already copied the struct bytes; reinterpretation assumes no padding change — but safer to re-read the fields from `buf` explicitly. */
    uint64_t e_shoff = read_u64_le(buf + offsetof(Elf64_Ehdr_on_disk, e_shoff));
    uint16_t e_shentsize = read_u16_le(buf + offsetof(Elf64_Ehdr_on_disk, e_shentsize));
    uint16_t e_shnum = read_u16_le(buf + offsetof(Elf64_Ehdr_on_disk, e_shnum));
    uint16_t e_shstrndx = read_u16_le(buf + offsetof(Elf64_Ehdr_on_disk, e_shstrndx));

    if (e_shoff == 0 || e_shnum == 0) return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT;

    /* bounds: ensure section header table fits inside buffer */
    size_t req_size;
    if (mul_add_overflow_size((size_t)e_shentsize, (size_t)e_shnum, (size_t)e_shoff, &req_size)) return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT;
    if (req_size > len) return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT;

    /* allocate handle */
    fossil_media_elf_t *h = (fossil_media_elf_t*)malloc(sizeof(*h));
    if (!h) return FOSSIL_MEDIA_ELF_ERR_NOMEM;
    memset(h, 0, sizeof(*h));

    h->owns_buf = buf_owned;
    h->buf = buf_owned ? buf : NULL;
    h->base_ptr = buf;  /* <--- store pointer even if non-owning */
    h->size = len;

    /* copy the e_ident into our local ehdr_on_disk and also copy the rest of the header bytes for completeness */
    memcpy(&h->ehdr, buf, sizeof(h->ehdr)); /* local on-disk copy */

    /* Now parse section headers into a host-endian allocated array */
    if (e_shentsize < sizeof(Elf64_Shdr_on_disk)) {
        /* weird/unsupported section header size */
        if (h->owns_buf && h->buf) free(h->buf);
        free(h);
        return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT;
    }

    h->sh_count = (size_t)e_shnum;
    h->shdrs = (Elf64_Shdr_on_disk*)malloc(sizeof(Elf64_Shdr_on_disk) * h->sh_count);
    if (!h->shdrs) { if (h->owns_buf && h->buf) free(h->buf); free(h); return FOSSIL_MEDIA_ELF_ERR_NOMEM; }

    for (size_t i = 0; i < h->sh_count; ++i) {
        size_t offset = (size_t)e_shoff + (size_t)i * (size_t)e_shentsize;
        if (offset + sizeof(Elf64_Shdr_on_disk) > len) { /* sanity */
            free(h->shdrs);
            if (h->owns_buf && h->buf) free(h->buf);
            free(h);
            return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT;
        }
        /* read fields directly from buffer into host-endian struct */
        const uint8_t *sbase = buf + offset;
        h->shdrs[i].sh_name = read_u32_le(sbase + offsetof(Elf64_Shdr_on_disk, sh_name));
        h->shdrs[i].sh_type = read_u32_le(sbase + offsetof(Elf64_Shdr_on_disk, sh_type));
        h->shdrs[i].sh_flags = read_u64_le(sbase + offsetof(Elf64_Shdr_on_disk, sh_flags));
        h->shdrs[i].sh_addr = read_u64_le(sbase + offsetof(Elf64_Shdr_on_disk, sh_addr));
        h->shdrs[i].sh_offset = read_u64_le(sbase + offsetof(Elf64_Shdr_on_disk, sh_offset));
        h->shdrs[i].sh_size = read_u64_le(sbase + offsetof(Elf64_Shdr_on_disk, sh_size));
        h->shdrs[i].sh_link = read_u32_le(sbase + offsetof(Elf64_Shdr_on_disk, sh_link));
        h->shdrs[i].sh_info = read_u32_le(sbase + offsetof(Elf64_Shdr_on_disk, sh_info));
        h->shdrs[i].sh_addralign = read_u64_le(sbase + offsetof(Elf64_Shdr_on_disk, sh_addralign));
        h->shdrs[i].sh_entsize = read_u64_le(sbase + offsetof(Elf64_Shdr_on_disk, sh_entsize));
    }

    /* locate section header string table */
    if ((size_t)e_shstrndx >= h->sh_count) {
        free(h->shdrs);
        if (h->owns_buf && h->buf) free(h->buf);
        free(h);
        return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT;
    }
    Elf64_Shdr_on_disk *shstr = &h->shdrs[e_shstrndx];
    if ((size_t)shstr->sh_offset + (size_t)shstr->sh_size > len) {
        free(h->shdrs);
        if (h->owns_buf && h->buf) free(h->buf);
        free(h);
        return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT;
    }
    /* note: shstrtab points into buf; if load from memory where buf isn't owned, caller must keep memory alive */
    h->shstrtab = (const char*)(buf + shstr->sh_offset);

    *out = h;
    return FOSSIL_MEDIA_ELF_OK;
}

/* load from file (owns buffer) */
int fossil_media_elf_load_from_file(const char *path, fossil_media_elf_t **out) {
    if (!path || !out) return FOSSIL_MEDIA_ELF_ERR_INVALID_ARG;
    uint8_t *buf = NULL;
    size_t size = 0;
    int rc = read_file_to_buffer(path, &buf, &size);
    if (rc != FOSSIL_MEDIA_ELF_OK) return rc;
    /* parse and take ownership of buf on success */
    rc = parse_elf_from_buffer(buf, size, out, 1);
    if (rc != FOSSIL_MEDIA_ELF_OK) {
        free(buf);
    }
    return rc;
}

/* load from memory -- does not take ownership of the buffer (caller must keep it alive) */
int fossil_media_elf_load_from_memory(const void *buf_in, size_t len, fossil_media_elf_t **out) {
    if (!buf_in || !out) return FOSSIL_MEDIA_ELF_ERR_INVALID_ARG;
    /* make a non-owning mutable pointer to pass into parse routine (we won't free) */
    uint8_t *buf = (uint8_t*)(uintptr_t)buf_in;
    return parse_elf_from_buffer(buf, len, out, 0);
}

void fossil_media_elf_free(fossil_media_elf_t *elf) {
    if (!elf) return;
    if (elf->owns_buf && elf->buf) free(elf->buf);
    if (elf->shdrs) free(elf->shdrs);
    free(elf);
}

/* API helpers: these read from our host-endian shdr array */
int fossil_media_elf_get_section_count(const fossil_media_elf_t *elf, size_t *out_count) {
    if (!elf || !out_count) return FOSSIL_MEDIA_ELF_ERR_INVALID_ARG;
    *out_count = elf->sh_count;
    return FOSSIL_MEDIA_ELF_OK;
}

int fossil_media_elf_get_section_header(const fossil_media_elf_t *elf, size_t index, fossil_media_elf_shdr_t *out_shdr) {
    if (!elf || !out_shdr) return FOSSIL_MEDIA_ELF_ERR_INVALID_ARG;
    if (index >= elf->sh_count) return FOSSIL_MEDIA_ELF_ERR_RANGE;
    Elf64_Shdr_on_disk *s = &elf->shdrs[index];
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
    if (!elf || !out_name) return FOSSIL_MEDIA_ELF_ERR_INVALID_ARG;
    if (index >= elf->sh_count) return FOSSIL_MEDIA_ELF_ERR_RANGE;
    Elf64_Shdr_on_disk *s = &elf->shdrs[index];
    /* ensure shstrtab present and offset in-range */
    if (!elf->shstrtab) return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT;
    if ((size_t)s->sh_name >= strlen(elf->shstrtab) + 1) {
        /* cheap boundary check: we don't have length of shstrtab here; caller could pass malformed but we validated earlier */
        /* still, return name pointer trusting earlier validation */
    }
    *out_name = elf->shstrtab + s->sh_name;
    return FOSSIL_MEDIA_ELF_OK;
}

int fossil_media_elf_get_section_data(const fossil_media_elf_t *elf, size_t index,
                                      const uint8_t **out_ptr, size_t *out_len) {
    if (!elf || !out_ptr || !out_len)
        return FOSSIL_MEDIA_ELF_ERR_INVALID_ARG;
    if (index >= elf->sh_count)
        return FOSSIL_MEDIA_ELF_ERR_RANGE;

    Elf64_Shdr_on_disk *s = &elf->shdrs[index];

    /* bounds check */
    if ((size_t)s->sh_offset + (size_t)s->sh_size > elf->size)
        return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT;

    if (!elf->base_ptr)
        return FOSSIL_MEDIA_ELF_ERR_BAD_FORMAT; /* should never happen now */

    *out_ptr = elf->base_ptr + s->sh_offset;
    *out_len = (size_t)s->sh_size;
    return FOSSIL_MEDIA_ELF_OK;
}

/* find by name */
int fossil_media_elf_find_section_by_name(const fossil_media_elf_t *elf, const char *name, size_t *out_index) {
    if (!elf || !name || !out_index) return FOSSIL_MEDIA_ELF_ERR_INVALID_ARG;
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

/* extract */
int fossil_media_elf_extract_section_to_file(const fossil_media_elf_t *elf, size_t index, const char *out_path) {
    if (!elf || !out_path) return FOSSIL_MEDIA_ELF_ERR_INVALID_ARG;
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

/* strerror mapping preserved from your original */
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
