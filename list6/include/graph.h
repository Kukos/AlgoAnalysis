#ifndef GRAPH_H
#define GRAPH_H

/*
    Graph simple impl

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL3.0
*/

#include <stddef.h>
#include <gsl/gsl_matrix.h>

typedef struct Graph
{
  size_t ____n;
  gsl_matrix_int *____im;
} Graph;


/*
    Create Graph

    PARAMS
    @IN n - |V|

    RETURN
    NULL iff failure
    Pointer to Graph iff success
*/
Graph *graph_create(size_t n);

/*
    Destroy Graph

    PARAMS
    @IN g - graph to destroy

    RETURN
    This is a void function
*/
void graph_destroy(Graph *g);

/*
    Add an Arc to Graph

    PARAMS
    @IN g - graph
    @IN src - src node num
    @IN dst - dst node num

    RETURN
    This is a void function
*/
void graph_add_arc(Graph *g, size_t src, size_t dst);

/*
    Remove an Arc from Graph

    PARAMS
    @IN g - graph
    @IN src - src node num
    @IN dst - dst node num

    RETURN
    This is a void function
*/
void graph_delete_arc(Graph *g, size_t src, size_t dst);

/*
    Get node degree

    PARAMS
    @IN g - graph
    @IN node - node num

    RETURN
    Degree of node
*/
size_t graph_get_node_degree(const Graph *g, size_t node);

#endif