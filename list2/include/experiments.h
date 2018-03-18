#ifndef EXPERIMENTS_H
#define EXPERIMENTS_H

/*
    Experiments for k min algorithm

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL 3.0
*/

#include <stddef.h>

/*
    Write est / n ratio to file
    with name "./kmin_ratio_for_k";

    PARAMS
    @IN k - array of k to check
    @IN k_len - len(k)
    @IN n - max n for each k

    RETURN
    This is a void function
*/
void experiment1(size_t *k, size_t k_len, size_t n);

/*
    Check if optimal k i truly optimal
    for i from [n] do experiments
    check if |estimated / n < 10%| / n < percent

    PARAMS
    @IN n - max size of Set
    @IN percent - trust percents

    RETURN
    This is a void function
*/
void experiment2(size_t n, int percent);

/*
    Check when kmin doesnt work with 32 bit hash

    PARAMS
    @IN epsilon - e error

    RETURN
    This is a void function
*/
void experiment3(double epsilon);


#endif