#ifndef PAGE_RANK_H
#define PAGE_RANK_H

/*
    Implementation of simple version of PageRank Algo

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL 3.0
*/

#include <stddef.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <graph.h>

typedef struct MGoogle
{
    size_t ____n;
    double ____alpha;
    gsl_matrix *____mg;

} MGoogle;

/*
    Create Google Matrix from Graph g

    PARAMS
    @IN g - graph
    @IN alpha - alpha for PR Algo

    RETURN
    Pointer to MGoogle iff success
    NULL iff failure
*/
MGoogle *mgoogle_create(const Graph *g, double alpha);

/*
    Destroy Google Matrix

    PARAMS
    @IN mg - pointer to Google Matrix

    RETURN
    This is a void function
*/
void mgoogle_destroy(MGoogle *mg);

/*
    Calculate rank of mgoogle matrix

    PARAMS
    @IN mg - google Matrix

    RETURN
    NULL iff failure
    Pointer to rank vector (for each page) iff success
*/
gsl_vector *pagerank(const MGoogle *mg);

#endif