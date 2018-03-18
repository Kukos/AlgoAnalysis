#ifndef KMIN_H
#define KMIN_H

/*
    Implementation of Kmin (counting distinct elements from multi-set without O(n) memory)

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL 3.0
*/

#include <stddef.h>
#include <stdint.h>

#define KMIN_HASH_FUNCTION hash_fnv

/*
    Get estimated number of distinct elements from t

    PARAMS
    @IN get_number - get next number from stream
    @IN len - len of stream
    @IN k - main algorithm parameter if k == 0 then is set to optimal k obtained from Chernoff formula
    @IN hash_f - pointer to hash function

    RETURN
    Estimated number of elements in set t' obtained from multi set t
*/
size_t kmin(long (*get_number)(void), size_t len, size_t k, uint32_t(*hash_f)(const void *data, size_t len));

#endif