#include <pagerank.h>
#include <common.h>
#include <log.h>
#include <float.h>

#define PR_MAX_IR 256
#define NORM_EPSILON 0.00001

static MGoogle *mgoogle_alloc(size_t n, double alpha);
static void mgoogle_init_raw(MGoogle *mg, const Graph *g);
static void mgoogle_add_dangling_nodes(MGoogle *mg, const Graph *g);
static int mgoogle_check_stochastic(const MGoogle *mg);
static void mgoogle_print(const MGoogle *mg);
static double mgoogle_vec_norm(const gsl_matrix *vec, const gsl_matrix *vec2, size_t n);

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

static int mgoogle_check_stochastic(const MGoogle *mg)
{
    size_t i;
    size_t j;
    double sum;
    size_t n;

    TRACE();

    n = mg->____n;
    for (i = 0; i < n; ++i)
    {
        sum = 0.0;
        for (j = 0; j < n; ++j)
            sum += gsl_matrix_get(mg->____mg, i, j);

        if (ABS(sum - 1.0) > DBL_EPSILON)
            return 1;
    }

    return 0;
}

static void mgoogle_print(const MGoogle *mg)
{
    size_t i;
    size_t j;
    size_t n;

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
    gsl_matrix_transpose(ones);
    gsl_matrix_add(mg->____mg, ones);

    gsl_matrix_free(ones);

    if (mgoogle_check_stochastic(mg))
        ERROR("MGoogle is not stochastic\n", NULL);


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

static double mgoogle_vec_norm(const gsl_matrix *vec, const gsl_matrix *vec2, size_t n)
{
    size_t i;
    double sum;

    TRACE();

    sum = 0.0;
    for (i = 0; i < n; ++i)
        sum += ABS((gsl_matrix_get(vec, 0, i) - gsl_matrix_get(vec2, 0, i)));

    return sum;
}

static void mgoogle_m_v_prod(gsl_matrix *m, gsl_matrix *v, size_t n)
{
    gsl_matrix *vec;
    size_t i;
    size_t j;
    double sum;

    TRACE();

    vec = gsl_matrix_calloc(1, n);
    for (i = 0; i < n; ++i)
    {
        sum = 0.0;
        for (j = 0; j < n; ++j)
            sum += gsl_matrix_get(m, j, i) * gsl_matrix_get(v, 0, j);

        gsl_matrix_set(vec, 0, i, sum);
    }

     gsl_matrix_memcpy(v, vec);
     gsl_matrix_free(vec);
}

double *pagerank(const MGoogle *mg)
{
    size_t i;
    double *rank;
    gsl_matrix *vec;
    gsl_matrix *prev_vec;

    TRACE();

    rank = (double *)malloc(sizeof(double) * mg->____n);
    if (rank == NULL)
        ERROR("malloc error\n", NULL);

    vec = gsl_matrix_calloc(1, mg->____n);
    if (vec == NULL)
        ERROR("gsl_matrix_alloc error\n", NULL);

    prev_vec = gsl_matrix_calloc(1, mg->____n);
    if (prev_vec == NULL)
        ERROR("gsl_matrix_alloc error\n", NULL);

    gsl_matrix_set_all(vec, 1.0 / (double)(mg->____n));

    i = 0;
    do {
        gsl_matrix_memcpy(prev_vec, vec);
        mgoogle_m_v_prod(mg->____mg, vec, mg->____n);
        ++i;
    } while (i < PR_MAX_IR && mgoogle_vec_norm(vec, prev_vec, mg->____n) > NORM_EPSILON);

    for (i = 0; i < mg->____n; ++i)
        rank[i] = gsl_matrix_get(vec, 0, i);

    gsl_matrix_free(vec);
    gsl_matrix_free(prev_vec);

    return rank;
}