#ifndef CCACHE_H
#define CCACHE_H

/*
    Config Cache

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE GPL3.0
*/

#include <stddef.h>
#include <common.h>

typedef struct CCache
{
    int ____fd;
    char *____cache;
    size_t ____len;
    size_t ____mmap_size;
} CCache;

/*
    Create CCache

    PARAMS
    @IN size - ccache size

    RETURN
    NULL iff failure
    Pointer to new CCache iff success
*/
CCache *ccache_create(size_t size);

/*
    Destroy CCache

    PARAMS
    @IN cache - pointer to CCache

    RETURN
    This is a void function
*/
void ccache_destroy(CCache *cache);

/*
    Get cache[index] state

    PARAMS
    @IN cache - pointer to CCache
    @IN index - state index

    RETURN
    Cache[index]
*/
static ___inline___ bool ccache_conf_proved(const CCache *cache, size_t index) ___nonull___(1);

/*
    Set cache[index]

    PARAMS
    @IN cache - pointer to CCache
    @IN index - state index

    RETURN
    This is a void function
*/
static ___inline___ void ccache_set_proof(CCache *cache, size_t index) ___nonull___(1);

static ___inline___ bool ccache_conf_proved(const CCache *cache, size_t index)
{
    return ((bool *)cache->____cache)[index];
}

static ___inline___ void ccache_set_proof(CCache *cache, size_t index)
{
    ((bool *)cache->____cache)[index] = true;
}

#endif