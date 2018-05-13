#include <log.h>
#include <compiler.h>
#include <stddef.h>
#include <common.h>
#include <graph.h>
#include <pagerank.h>

___before_main___(1) void init(void);
___after_main___(1) void deinit(void);

___before_main___(1) void init(void)
{
    (void)log_init(stdout, NO_LOG_TO_FILE);
}

___after_main___(1) void deinit(void)
{
    log_deinit();
}

static void experiment_with_alpha(Graph *g, double *alpha, size_t n)
{
    MGoogle *mg;
    double *rank;
    size_t i;
    size_t j;

    TRACE();

    for (i = 0; i < n; ++i)
    {
        mg = mgoogle_create(g, alpha[i]);
        rank = pagerank(mg);

        for (j = 0; j < g->____n; ++j)
            printf("RANK( %zu ) = %lf\n", j, rank[j]);

        FREE(rank);
        mgoogle_destroy(mg);
    }
}

int main(void)
{
    const size_t n = 6;
    Graph *g = graph_create(n);

    double alpha[] = {0.0, 0.15, 0.5, 1.0};

    graph_add_arc(g, 0, 0);
    graph_add_arc(g, 1, 2);
    graph_add_arc(g, 1, 4);
    graph_add_arc(g, 2, 0);
    graph_add_arc(g, 3, 1);
    graph_add_arc(g, 3, 4);
    graph_add_arc(g, 4, 3);
    graph_add_arc(g, 5, 2);

    experiment_with_alpha(g, alpha, ARRAY_SIZE(alpha));

    printf("\n\nWithout ARC %d --> %d\n\n", 1, 2);
    graph_delete_arc(g, 1, 2);

    experiment_with_alpha(g, alpha, ARRAY_SIZE(alpha));

    graph_destroy(g);

    return 0;
}