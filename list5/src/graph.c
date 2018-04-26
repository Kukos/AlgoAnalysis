#include <graph.h>
#include <stdlib.h>
#include <common.h>
#include <log.h>

Graph *graph_create(size_t proc)
{
    size_t i;
    size_t j;
    size_t k;
    Graph *g;

    TRACE();

    g = (Graph *)malloc(sizeof(Graph));
    if (g == NULL)
        ERROR("malloc error\n", NULL);

    g->____nodes = (Node *)malloc(sizeof(Node) * proc);
    if (g->____nodes == NULL)
        ERROR("malloc error\n", NULL);

    g->____nproc = proc;

    /* create K_n */
    for (i = 0; i < proc; ++i)
    {
        g->____nodes[i].____id = i;
        g->____nodes[i].____neigh_len = proc - 1;
        g->____nodes[i].____neigh = (Node **)malloc(sizeof(Node *) * (proc - 1));
        if (g->____nodes[i].____neigh == NULL)
            ERROR("malloc error\n", NULL);

        k = 0;
        for (j = 0; j < i; ++j)
            g->____nodes[i].____neigh[k++] = &g->____nodes[j];

        for (j = i + 1; j < proc; ++j)
            g->____nodes[i].____neigh[k++] = &g->____nodes[j];
    }

    return g;
}

void graph_destroy(Graph *g)
{
    size_t i;

    TRACE();

    if (g == NULL)
        return;

    for (i = 0; i < g->____nproc; ++i)
        FREE(g->____nodes[i].____neigh);

    FREE(g->____nodes);
    FREE(g);
}
