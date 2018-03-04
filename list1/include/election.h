#ifndef ELECTION_H
#define ELECTION_H

/*
    Implementation of distributed election randomized algorithm

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL 3.0+
*/

#include <stddef.h>
#include <common.h>

#ifndef INT_DIV_CEIL
    #define INT_DIV_CEIL(n, k) \
    __extension__ \
    ({ \
        typeof(n + 1) ____n = (n); \
        typeof(k + 1) ____k = (k); \
        (____n + ____k - 1) / ____k; \
    })
#endif

#define ROUND_LEN(u) \
    __extension__ \
    ({ \
        typeof(u + 1) ________u = (u); \
        LOG2(________u - 1) + 1; \
    })

#define GET_ROUND_NUMBER_FROM_SLOTS(slots, u) \
    __extension__ \
    ({ \
        typeof(slots + 1) ____s = (slots); \
        typeof(u + 1) ____u = (u); \
        INT_DIV_CEIL(____s, ROUND_LEN(____u)); \
    })

/*
    Simulation of randomized election algo

    PARAMS
    @IN n - number of station
    @IN u - upper bound of n iff u == 0 then n is well known

    RETURN
    Number of slot time consumed by algo
*/
size_t election(size_t n, size_t u);

#endif