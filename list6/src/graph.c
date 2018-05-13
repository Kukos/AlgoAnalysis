#include <graph.h>
#include <log.h>
#include <gsl/gsl_matrix.h>
#include <common.h>

Graph *graph_create(size_t n)
{
    Graph *g;

    TRACE();

    g = (Graph *)malloc(sizeof(Graph));
    if (g == NULL)
        ERROR("malloc error\n", NULL);

    g->____n = n;
    g->____im = gsl_matrix_int_calloc(n, n);
    if (g->____im == NULL)
        ERROR("gsl_matrix_int_alloc error\n", NULL);

    return g;
}

void graph_destroy(Graph *g)
{
    TRACE();

    if (g == NULL)
        return;

    gsl_matrix_int_free(g->____im);
    FREE(g);
}

void graph_add_arc(Graph *g, size_t src, size_t dst)
{
    TRACE();

    if (g == NULL)
        return;

    gsl_matrix_int_set(g->____im, src, dst, 1);
}

void graph_delete_arc(Graph *g, size_t src, size_t dst)
{
    TRACE();

    if (g == NULL)
        return;

    gsl_matrix_int_set(g->____im, src, dst, 0);
}

size_t graph_get_node_degree(const Graph *g, size_t node)
{
    size_t i;
    size_t deg = 0;

    TRACE();

    if (g == NULL)
        ERROR("g == NULL\n", 0);

    for (i = 0; i < g->____n; ++i)
        deg += (size_t)gsl_matrix_int_get(g->____im, node, i);

    return deg;
}