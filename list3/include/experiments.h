#ifndef EXPERIMENTS_H
#define EXPERIMENTS_H

/*
    Experiments for Hyper LogLog algorithm

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL 3.0
*/

#include <stddef.h>

/*
    Write est / n ratio to file
    with name "./hll_ratio_for_m";

    PARAMS
    @IN b - array of b
    @IN b_len - len(b)
    @IN n - max n for each b

    RETURN
    This is a void function
*/
void experiment1(size_t *b, size_t b_len, size_t n);


/*
    Kmin vs Hll with the same amount of memory
    Write est / n ratio to file
    with name "./hll_vs_kmin_ratio_for_m_";

    PARAMS
    @IN b - array of b
    @IN b_len - len(b)
    @IN n - max n for each b

    RETURN
    This is a void function
*/
void experiment2(size_t *b, size_t b_len, size_t n);

#endif