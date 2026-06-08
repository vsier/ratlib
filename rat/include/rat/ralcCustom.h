// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>

#ifndef RAT_RALCCUSTOM_H_
#define RAT_RALCCUSTOM_H_

#include <rat/ralc.h>


// Allocator adapter backed by user-provided callbacks.
typedef struct ralcCustom ralcCustom;

// Allocates size bytes and reports the actual user size.
typedef void *(*ralcCustom_fn_alloc)(ralcCustom *p, size_t size, size_t *out_actualSize);

// Resizes ptr to newSize bytes. Unsupported non-NULL realloc returns NULL
// without freeing ptr.
typedef void *(*ralcCustom_fn_realloc)(ralcCustom *p, void *ptr, size_t newSize, size_t *out_actualNewSize);

// Frees a non-NULL ptr according to the custom allocator's semantics.
typedef void (*ralcCustom_fn_free)(ralcCustom *p, void *ptr);

// Releases all allocations owned by the custom allocator.
typedef void (*ralcCustom_fn_clear)(ralcCustom *p);

// Fills out_usage with the custom allocator's accounting.
typedef void (*ralcCustom_fn_getUsage)(ralcCustom *p, ralc_usage *out_usage);

struct ralcCustom
{
    void *userData;
    ralcCustom_fn_alloc alloc;
    ralcCustom_fn_realloc realloc;
    ralcCustom_fn_free free;
    ralcCustom_fn_clear clear;
    ralcCustom_fn_getUsage getUsage;
    ralc_iface ifaceRalc;
};

// Initializes p with userData_p and the supplied callback table.
void ralcCustom_init(ralcCustom *p, void *userData_p, ralcCustom_fn_alloc alloc_fn, ralcCustom_fn_realloc realloc_fn, ralcCustom_fn_free free_fn, ralcCustom_fn_clear clear_fn, ralcCustom_fn_getUsage getUsage_fn);

// Forwards allocation to p->alloc.
void *ralcCustom_alloc(ralcCustom *p, size_t size, size_t *out_actualSize);
// Forwards reallocation to p->realloc.
void *ralcCustom_realloc(ralcCustom *p, void *ptr, size_t newSize, size_t *out_actualNewSize);
// Forwards a non-NULL live object to p->free.
void ralcCustom_free(ralcCustom *p, void *ptr);

// Forwards clear to p->clear.
void ralcCustom_clear(ralcCustom *p);

// Forwards usage accounting to p->getUsage.
void ralcCustom_getUsage(ralcCustom *p, ralc_usage *out_usage);

#endif
