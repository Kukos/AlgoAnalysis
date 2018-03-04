#ifndef EXPERIMENTS_H
#define EXPERIMENTS_H

/*
    Simple experiments to help analysis election algo

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL 3.0+
*/

#include <stddef.h>

/*
    4 situation, for each situation slots needed to choose leader

    Situtations:
    N is known (N = U)
    N is unknown and N = 2
    N is unknown and N = U / 2
    N is unknown and N = U

    PARAMS
    @IN u - array of U
    @IN len - len(u)

    RETURN
    This is a void function
*/
void experiments1(size_t *u, size_t len);

/*
    Get arg probability that election finish with r round
    for every n from 2  to u

    PARAMS
    @u - upper bound of diods
    @r - n of round for which we measure probability

    RETURN
    This is a void function
*/
void experiments2(size_t u, int r);


/*
    4 situation, for each situation, measurent for slots needed to find leader

    Situtations:
    N is known (N = U)
    N is unknown and N = 2
    N is unknown and N = U / 2
    N is unknown and N = U

    PARAMS
    n - number of diods

    RETURN
    This is a void function

*/
void experiments3(size_t n);

#endif