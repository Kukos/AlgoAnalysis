#include <log.h>
#include <compiler.h>
#include <stddef.h>
#include <common.h>
#include <graph.h>
#include <pagerank.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <markov.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#define BUFFER_SIZE 128

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

static void ex1_experiment_with_alpha(Graph *g, double *alpha, size_t n);
static void ex3_experiment_with_alpha(Graph *g, double *alpha, size_t n, size_t limit);

static void ex1(void);
static void ex2(void);
static void ex3(void);

static void ex1_experiment_with_alpha(Graph *g, double *alpha, size_t n)
{
    MGoogle *mg;
    gsl_vector *rank;
    size_t i;
    size_t j;

    TRACE();

    for (i = 0; i < n; ++i)
    {
        mg = mgoogle_create(g, alpha[i]);
        rank = pagerank(mg);

        for (j = 0; j < rank->size; ++j)
            printf("RANK( %zu ) = %lf\n", j, gsl_vector_get(rank, j));

        gsl_vector_free(rank);
        mgoogle_destroy(mg);
    }
}

static void ex3_experiment_with_alpha(Graph *g, double *alpha, size_t n, size_t limit)
{
    MGoogle *mg;
    gsl_vector *rank;
    gsl_vector *prev;
    size_t i;
    size_t j;
    size_t k;

    double prob[limit][g->____n];

    int fd;
    char name[BUFFER_SIZE];
    const char *const prefix = "./ex3_alpha=";
    TRACE();

    for (i = 0; i < n; ++i)
    {
        mg = mgoogle_create(g, alpha[i]);
        rank = gsl_vector_alloc(mg->____mg->size1);
        gsl_vector_set_all(rank, 1.0 / (double)mg->____mg->size1);
        for (j = 0; j < limit; ++j)
        {
            prev = rank;
            rank = markov_prob_dist(rank, mg->____mg, 1);
            printf("    STEP %zu\n", j);
            for (k = 0; k < rank->size; ++k)
            {
                prob[j][k] = gsl_vector_get(rank, k);
                printf("        SD( %zu ) = %lf\n", k, gsl_vector_get(rank, k));
            }

            gsl_vector_free(prev);
        }

        gsl_vector_free(rank);
        mgoogle_destroy(mg);

        /* write experiments to file */
        (void)snprintf(name, BUFFER_SIZE, "%s%0.2lf.txt", prefix, alpha[i]);

        fd = open(name, O_RDWR | O_CREAT, 0644);
        if (fd == -1)
            FATAL("Cannot open file\n");

        for (j = 0; j < limit; ++j)
        {
            dprintf(fd, "%zu\t", j + 1);
            for (k = 0; k < g->____n; ++k)
                dprintf(fd, "%0.8lf\t", prob[j][k]);

            dprintf(fd, "\n");
        }

        close(fd);
    }
}

static void ex1(void)
{
    const size_t n = 6;
    Graph *g = graph_create(n);

    double alpha[] = {0.0, 0.15, 0.5, 1.0};

    TRACE();

    printf("\n\nEX1\n\n");

    graph_add_arc(g, 0, 0);
    graph_add_arc(g, 1, 2);
    graph_add_arc(g, 1, 4);
    graph_add_arc(g, 2, 0);
    graph_add_arc(g, 3, 1);
    graph_add_arc(g, 3, 4);
    graph_add_arc(g, 4, 3);
    graph_add_arc(g, 5, 2);

    ex1_experiment_with_alpha(g, alpha, ARRAY_SIZE(alpha));

    printf("\n\nWithout ARC %d --> %d\n\n", 1, 2);
    graph_delete_arc(g, 1, 2);

    ex1_experiment_with_alpha(g, alpha, ARRAY_SIZE(alpha));

    graph_destroy(g);
}

static void ex2(void)
{
    const size_t n = 4;
    double prob[4][4] = {
                            {0.0, 0.3, 0.1, 0.6},
                            {0.1, 0.1, 0.7, 0.1},
                            {0.1, 0.7, 0.1, 0.1},
                            {0.9, 0.1, 0.0, 0.0}
                        };

    gsl_matrix *mat;
    gsl_vector *start;
    gsl_vector *dist;
    gsl_vector *vec;
    gsl_vector *prev_vec;

    size_t i;
    size_t j;
    size_t rnd;

    double eps[] = {0.1, 0.01, 0.001};

    TRACE();
    printf("\n\nEX2\n\n");

    mat = gsl_matrix_alloc(n, n);

    for (i = 0; i < n; ++i)
    {
        for (j = 0; j < n; ++j)
        {
            printf("%0.4lf    ", prob[i][j]);
            gsl_matrix_set(mat, i, j, prob[i][j]);
        }
        printf("\n");
    }

    start = gsl_vector_alloc(n);
    gsl_vector_set_all(start, 1.0 / (double)n);

    /* static distribution */
    dist = markov_static_dist(start, mat, NULL, 0.0, NULL);
    for (i = 0; i < n; ++i)
        printf("SD [%zu] = %lf\n", i, gsl_vector_get(dist, i));

    gsl_vector_free(dist);
    gsl_vector_set_zero(start);
    gsl_vector_set(start, 0, 1.0);

    /* prob after 32 moves */
    dist = markov_prob_dist(start, mat, 32);
    printf("PROB [3] after 32 moves = %g\n", gsl_vector_get(dist, 3));

    gsl_vector_free(dist);
    gsl_vector_set_all(start, 1.0 / (double)n);

    /* prob after 128 moves */
    dist = markov_prob_dist(start, mat, 128);
    printf("PROB [3] after 128 moves = %g\n", gsl_vector_get(dist, 3));
    gsl_vector_free(dist);

    /* get static dist */
    gsl_vector_set_zero(start);
    gsl_vector_set(start, 0, 1.0);
    dist = markov_static_dist(start, mat, NULL, 0.0, NULL);
    for (i = 0; i < ARRAY_SIZE(eps); ++i)
    {
        rnd = 0;
        prev_vec = gsl_vector_alloc(start->size);
        vec = gsl_vector_alloc(start->size);
        gsl_vector_memcpy(vec, start);
        do {
            ++rnd;
            gsl_vector_free(prev_vec);
            prev_vec = vec;
            vec = markov_m_v_prod(mat, vec);
        } while (markov_vec_norm_max(dist, vec) > eps[i]);

        printf("EPS = %lf, rnd = %zu\n", eps[i], rnd);

        gsl_vector_free(vec);
        gsl_vector_free(prev_vec);
    }

    gsl_vector_free(dist);
    gsl_vector_free(start);
    gsl_matrix_free(mat);
}

static void ex3(void)
{
    const size_t n = 5;
    const size_t steps = 25;
    Graph *g = graph_create(n);

    double alpha[] = {0.0, 0.25, 0.5, 0.75, 0.85, 1.0};

    TRACE();

    printf("\n\nEX3\n\n");

    graph_add_arc(g, 0, 1);
    graph_add_arc(g, 0, 2);
    graph_add_arc(g, 1, 3);
    graph_add_arc(g, 2, 1);
    graph_add_arc(g, 2, 3);
    graph_add_arc(g, 2, 4);
    graph_add_arc(g, 3, 0);

    ex3_experiment_with_alpha(g, alpha, ARRAY_SIZE(alpha), steps);

    graph_destroy(g);
}

int main(void)
{
    ex1();
    ex2();
    ex3();

    return 0;
}