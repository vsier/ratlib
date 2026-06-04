// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>
// SPDX-License-Identifier: MIT

#include <rat/ralcCustom.h>
#include <assert.h>


void ralcCustom_init(ralcCustom *p, void *userData_p, ralcCustom_fn_alloc alloc_fn, ralcCustom_fn_realloc realloc_fn, ralcCustom_fn_free free_fn, ralcCustom_fn_clear clear_fn, ralcCustom_fn_getUsage getUsage_fn)
{
    assert(p && alloc_fn && realloc_fn && free_fn && clear_fn && getUsage_fn);

    p->userData = userData_p;
    p->alloc = alloc_fn;
    p->realloc = realloc_fn;
    p->free = free_fn;
    p->clear = clear_fn;
    p->getUsage = getUsage_fn;
    p->ifaceRalc.kind = RALC_CUSTOM;
}

void *ralcCustom_alloc(ralcCustom *p, size_t size, size_t *out_actualSize)
{
    assert(p && p->alloc && out_actualSize);

    return p->alloc(p, size, out_actualSize);
}
void *ralcCustom_realloc(ralcCustom *p, void *ptr, size_t newSize, size_t *out_actualNewSize)
{
    assert(p && p->realloc && out_actualNewSize);

    return p->realloc(p, ptr, newSize, out_actualNewSize);
}
void ralcCustom_free(ralcCustom *p, void *ptr)
{
    assert(p && p->free && ptr);

    p->free(p, ptr);
}
void ralcCustom_clear(ralcCustom *p)
{
    assert(p && p->clear);

    p->clear(p);
}
void ralcCustom_getUsage(ralcCustom *p, ralc_usage *out_usage)
{
    assert(p && p->getUsage && out_usage);

    p->getUsage(p, out_usage);
}
