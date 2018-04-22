#ifndef DIJKSTRA_H
#define DIJKSTRA_H

/*
    Dijkstra's mutual exclusion

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL 3.0
*/

#include <stddef.h>

/*
    Proof that from every Conf mutual will go to correct conf

    PARAMS
    @IN n - n proc

    RETURN
    This is a void function
*/
void mutual_exclusion_proof(size_t n);

#endif