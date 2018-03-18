#include <kmin.h>
#include <log.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>
#include <sort.h>
#include <common.h>
#include <hash.h>

#define OPTIMAL_K 766 /* from Chernoff formula */

#define MAX_HASH UINT32_MAX

static int cmp_hash(const void *a, const void *b);

static int cmp_hash(const void *a, const void *b)
{
    uint32_t _a = *(uint32_t *)a;
    uint32_t _b = *(uint32_t *)b;

    if (_a < _b)
        return -1;

    if (_a > _b)
        return 1;

    return 0;
}

size_t kmin(long (*get_number)(void), size_t len, size_t k, uint32_t(*hash_f)(const void *data, size_t len))
{
    uint32_t *hashes;
    uint32_t h;

    long number;
    size_t estimated_n;
    size_t i;

    TRACE();

    if (k == 0)
        k = OPTIMAL_K;

    if (hash_f == NULL)
        hash_f = KMIN_HASH_FUNCTION;

    hashes = (uint32_t *)malloc(sizeof(uint32_t) * k);
    if (hashes == NULL)
        ERROR("malloc error\n", 0);

    /* start from MAX_HASH */
    (void)memset(hashes, (int)MAX_HASH, sizeof(uint32_t) * k);

    for (i = 0; i < len; ++i)
    {
        number = get_number();
        h = hash_f((const void *)&number, sizeof(long));
        if (h < hashes[k - 1] && find_first_sorted((const void *)&h, hashes, k, cmp_hash, sizeof(uint32_t)) == -1)
        {
            hashes[k - 1] = h;
            (void)insort(hashes, k, cmp_hash, sizeof(uint32_t));
        }
    }

    if (hashes[k - 1] == MAX_HASH)
    {
        i = 0;
        while (i < k && hashes[i] < MAX_HASH)
            ++i;

        estimated_n = i;
    }
    else
        estimated_n = (size_t)((long double)(k - 1) / (long double)((long double)hashes[k - 1] / (long double)MAX_HASH));

    FREE(hashes);
    return estimated_n;
}