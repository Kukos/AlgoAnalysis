#ifndef GRAPH_H
#define GRAPH_H

/*
    Graph simple impl

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL3.0
*/

#include <stdint.h>
#include <stddef.h>
#include <common.h>
#include <sys/types.h>

typedef enum node_state
{
    NODE_OUT_OF_MIS = 0,
    NODE_IN_MIS = 1
} node_state_t;

typedef struct Node
{
    size_t ____id;
    node_state_t ____state;
    struct Node **____neigh;
    size_t ____neigh_len;
} Node;

typedef int64_t graph_code_t;

typedef struct Graph
{
    size_t ____nproc;
    Node *____nodes;
} Graph;

#define NCONFS(n) (BIT(n))

#define for_each_graph_config(code, n) for (code = 0; code < (int64_t)NCONFS(n); ++code)

/*
    Create Graph K_n

    PARAMS
    @IN proc - num of proc

    RETURN
    NULL iff failure
    Pointer to Graph iff success
*/
Graph *graph_create(size_t proc);

/*
    Destroy Graph

    PARAMS
    @IN g - graph to destroy

    RETURN
    This is a void function
*/
void graph_destroy(Graph *g);

/*
    Encode graph into code

    PARAMS
    @IN g - graph

    RETURN
    Code
*/
static ___inline___ graph_code_t graph_encode(const Graph *g) ___nonull___(1);

/*
    Decode code into graph

    PARAMS
    @IN g - graph where code will be decoded
    @IN code - code typ decode

    RETURN
    This is a void function
*/
static ___inline___ void graph_decode(Graph *g, graph_code_t code) ___nonull___(1);


static ___inline___ graph_code_t graph_encode(const Graph *g)
{
    size_t i;
    long long unsigned code = 0;

    for (i = 0; i < g->____nproc; ++i)
        SET_BIT_VALUE(code, i, (unsigned long long)g->____nodes[i].____state);

    return (graph_code_t)code;
}

static ___inline___ void graph_decode(Graph *g, graph_code_t code)
{
    size_t i;

    for (i = 0; i < g->____nproc; ++i)
        g->____nodes[i].____state = (node_state_t)GET_BIT((unsigned long long)code, i);
}

#endif