#include <pagerank.h>
#include <common.h>
#include <log.h>
#include <float.h>
#include <markov.h>

/*
    Alloc memory for google matrix

    PARAMS
    @IN n - size
    @IN alpha - alpha

    RETURN
    Pointer to new MGoogle iff success
    NULL iff failure
*/
static MGoogle *mgoogle_alloc(size_t n, double alpha);

/*
    Init MGoogle with Raw values

    PARAMS
    @IN mg - MGoogle
    @IN g - graph

    RETURN
    This is a void function
*/
static void mgoogle_init_raw(MGoogle *mg, const Graph *g);

/*
    Add dangling nodes to MGoogle

    PARAMS
    @IN mg - MGoogle
    @IN g- graph

    RETURN
    This is a void function
*/
static void mgoogle_add_dangling_nodes(MGoogle *mg, const Graph *g);

/*
    Print MGoogle

    PARAMS
    @IN mg - MGoogle

    RETURN
    This is a void function
*/
static void mgoogle_print(const MGoogle *mg);

static MGoogle *mgoogle_alloc(size_t n, double alpha)
{
    MGoogle *mg;

    TRACE();

    mg = (MGoogle *)malloc(sizeof(MGoogle));
    if (mg == NULL)
        ERROR("malloc error\n", NULL);

    mg->____n = n;
    mg->____alpha = alpha;

    mg->____mg = gsl_matrix_calloc(n, n);
    if (mg->____mg == NULL)
        ERROR("gsl_matrix_alloc error\n", NULL);

    return mg;
}

static void mgoogle_init_raw(MGoogle *mg, const Graph *g)
{
    size_t i;
    size_t j;
    size_t deg;
    size_t n;

    TRACE();

    n = mg->____n;
    for (i = 0; i < n; ++i)
    {
        deg = graph_get_node_degree(g, i);
        if (deg)
            for (j = 0; j < n; ++j)
                if (gsl_matrix_int_get(g->____im, i, j))
                    gsl_matrix_set(mg->____mg, i, j, 1.0 / (double)deg);
    }
}

static void mgoogle_add_dangling_nodes(MGoogle *mg, const Graph *g)
{
    size_t i;
    size_t j;
    size_t deg;
    size_t n;

    TRACE();

    n = mg->____n;
    for (i = 0; i < n; ++i)
    {
        deg = graph_get_node_degree(g, i);
        if (deg == 0)
            for (j = 0; j < n; ++j)
                gsl_matrix_set(mg->____mg, i, j, 1.0 / (double)n);
    }
}

static void mgoogle_print(const MGoogle *mg)
{
    size_t i;
    size_t j;
    size_t n;

    TRACE();

    n = mg->____n;
    printf("Google Matrix %zu x %zu (Alpha = %0.2lf)\n", mg->____n, mg->____n, mg->____alpha);
    for (i = 0; i < n; ++i)
    {
        for (j = 0; j < n; ++j)
            printf("%0.4lf    ", gsl_matrix_get(mg->____mg, i, j));

        printf("\n");
    }
}

MGoogle *mgoogle_create(const Graph *g, double alpha)
{
    MGoogle *mg;
    gsl_matrix *ones;

    TRACE();

    mg = mgoogle_alloc(g->____n, alpha);
    if (mg == NULL)
        ERROR("MGoogle alloc error\n", NULL);

    mgoogle_init_raw(mg, g);
    mgoogle_add_dangling_nodes(mg, g);

    ones = gsl_matrix_alloc(g->____n, g->____n);
    if (ones == NULL)
        ERROR("gsl_matrix_alloc error\n", NULL);

    /* Jn = 1.0 .* (alpha) */
    gsl_matrix_set_all(ones, 1.0);
    gsl_matrix_scale(ones, 1.0 / ((double)g->____n) * (alpha));

    gsl_matrix_scale(mg->____mg, 1.0 - alpha);
    gsl_matrix_add(mg->____mg, ones);

    gsl_matrix_free(ones);

    mgoogle_print(mg);
    return mg;
}

void mgoogle_destroy(MGoogle *mg)
{
    TRACE();

    if (mg == NULL)
        return;

    gsl_matrix_free(mg->____mg);
    FREE(mg);
}

gsl_vector *pagerank(const MGoogle *mg)
{
    gsl_vector *vec;
    gsl_vector *res;

    TRACE();

    vec = gsl_vector_alloc(mg->____n);
    if (vec == NULL)
        ERROR("vector alloc error\n", NULL);

    gsl_vector_set_all(vec, 1.0 / (double)mg->____n);
    res = markov_static_dist(vec, mg->____mg, NULL, 0.0, NULL);

    gsl_vector_free(vec);

    return res;
}