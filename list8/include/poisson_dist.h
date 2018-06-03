#ifndef POISSON_DIST_H
#define POISSON_DIST_H

/*
    Rand value with Poisson distribution

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL 3.0
*/

#include <common.h>
#include <math.h>
#include <stdlib.h>

static ___inline___ double poisson_rand(double lambda)
{
    int r = rand();
    double uni_rand_01 = (double)r / (double)RAND_MAX;
    return (double)(-1.0 * log(uni_rand_01) / lambda);
}

#endif