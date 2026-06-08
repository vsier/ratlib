// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2026 vsier <https://github.com/vsier>

#include <rat/extra.h>
#include <rat/ralcArena.h>
#include <rat/ralcCustom.h>
#include <rat/ralcGen.h>
#include <rat/ralcPool.h>
#include <rat/rdyn.h>
#include <rat/rlist.h>
#include <assert.h>


void *ralc_alloc(ralc_iface *p, size_t size, size_t *out_actualSize)
{
    assert(p && p->kind >= RALC_GENERAL && p->kind <= RALC_CUSTOM && out_actualSize);

    switch (p->kind)
    {
        case RALC_GENERAL:
        {
            ralcGen *genRalc_p = RAT_M_TO_S(ralcGen, ifaceRalc, p);
            return ralcGen_alloc(genRalc_p, size, out_actualSize);
        }
        case RALC_LIST:
        {
            rlist *listRalc_p = RAT_M_TO_S(rlist, ifaceRalc, p);

            void *obj_p = rlist_push(listRalc_p, size);
            if (obj_p) *out_actualSize = rlist_elemSize(obj_p);
            return obj_p;
        }
        case RALC_ARENA:
        {
            ralcArena *arenaRalc_p = RAT_M_TO_S(ralcArena, ifaceRalc, p);
            return ralcArena_push(arenaRalc_p, size, out_actualSize);
        }
        case RALC_POOL:
        {
            ralcPool *poolRalc_p = RAT_M_TO_S(ralcPool, ifaceRalc, p);
            size = RAT_ALIGN(size ? size : 1);
            if (size > poolRalc_p->elemSize) return NULL;

            void *obj_p = ralcPool_alloc(poolRalc_p);
            if (obj_p) *out_actualSize = poolRalc_p->elemSize;
            return obj_p;
        }
        case RALC_CUSTOM:
        {
            ralcCustom *customRalc_p = RAT_M_TO_S(ralcCustom, ifaceRalc, p);
            return ralcCustom_alloc(customRalc_p, size, out_actualSize);
        }
    }

    return NULL;
}

void *ralc_realloc(ralc_iface *p, void *ptr, size_t newSize, size_t *out_actualNewSize)
{
    assert(p && p->kind >= RALC_GENERAL && p->kind <= RALC_CUSTOM && out_actualNewSize);

    switch (p->kind)
    {
        case RALC_GENERAL:
        {
            ralcGen *genRalc_p = RAT_M_TO_S(ralcGen, ifaceRalc, p);
            return ralcGen_realloc(genRalc_p, ptr, newSize, out_actualNewSize);
        }
        case RALC_LIST:
        {
            rlist *listRalc_p = RAT_M_TO_S(rlist, ifaceRalc, p);

            void *obj_p;
            if (!ptr)
                obj_p = rlist_push(listRalc_p, newSize);
            else
                obj_p = rlist_resize(listRalc_p, ptr, newSize);

            if (obj_p) *out_actualNewSize = rlist_elemSize(obj_p);
            return obj_p;
        }
        case RALC_ARENA:
        {
            if (ptr) break;
            ralcArena *arenaRalc_p = RAT_M_TO_S(ralcArena, ifaceRalc, p);
            return ralcArena_push(arenaRalc_p, newSize, out_actualNewSize);
        }
        case RALC_POOL:
        {
            if (ptr) break;
            ralcPool *poolRalc_p = RAT_M_TO_S(ralcPool, ifaceRalc, p);
            newSize = RAT_ALIGN(newSize ? newSize : 1);
            if (newSize > poolRalc_p->elemSize) return NULL;

            void *obj_p = ralcPool_alloc(poolRalc_p);
            if (obj_p) *out_actualNewSize = poolRalc_p->elemSize;
            return obj_p;
        }
        case RALC_CUSTOM:
        {
            ralcCustom *customRalc_p = RAT_M_TO_S(ralcCustom, ifaceRalc, p);
            return ralcCustom_realloc(customRalc_p, ptr, newSize, out_actualNewSize);
        }
    }

    return NULL;
}

void ralc_free(ralc_iface *p, void *ptr)
{
    assert(p && p->kind >= RALC_GENERAL && p->kind <= RALC_CUSTOM && ptr);

    switch (p->kind)
    {
        case RALC_GENERAL:
        {
            ralcGen *genRalc_p = RAT_M_TO_S(ralcGen, ifaceRalc, p);
            ralcGen_free(genRalc_p, ptr);
            break;
        }
        case RALC_LIST:
        {
            rlist *listRalc_p = RAT_M_TO_S(rlist, ifaceRalc, p);
            rlist_remove(listRalc_p, ptr, 1);
            break;
        }
        case RALC_ARENA:
        {
            break;
        }
        case RALC_POOL:
        {
            ralcPool *poolRalc_p = RAT_M_TO_S(ralcPool, ifaceRalc, p);
            ralcPool_free(poolRalc_p, ptr);
            break;
        }
        case RALC_CUSTOM:
        {
            ralcCustom *customRalc_p = RAT_M_TO_S(ralcCustom, ifaceRalc, p);
            ralcCustom_free(customRalc_p, ptr);
            break;
        }
    }
}

void ralc_getUsage(ralc_iface *p, ralc_usage *out_usage)
{
    assert(p && p->kind >= RALC_GENERAL && p->kind <= RALC_CUSTOM && out_usage);

    switch (p->kind)
    {
        case RALC_GENERAL:
        {
            ralcGen *genRalc_p = RAT_M_TO_S(ralcGen, ifaceRalc, p);
            ralcGen_getUsage(genRalc_p, out_usage);
            break;
        }
        case RALC_LIST:
        {
            rlist *listRalc_p = RAT_M_TO_S(rlist, ifaceRalc, p);
            rlist_getUsage(listRalc_p, out_usage);
            break;
        }
        case RALC_ARENA:
        {
            ralcArena *arenaRalc_p = RAT_M_TO_S(ralcArena, ifaceRalc, p);
            ralcArena_getUsage(arenaRalc_p, out_usage);
            break;
        }
        case RALC_POOL:
        {
            ralcPool *poolRalc_p = RAT_M_TO_S(ralcPool, ifaceRalc, p);
            ralcPool_getUsage(poolRalc_p, out_usage);
            break;
        }
        case RALC_CUSTOM:
        {
            ralcCustom *customRalc_p = RAT_M_TO_S(ralcCustom, ifaceRalc, p);
            ralcCustom_getUsage(customRalc_p, out_usage);
            break;
        }
    }
}
