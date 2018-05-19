#include <markov.h>
#include <log.h>
#include <common.h>
#include <float.h>
#include <stdbool.h>
#include <math.h>

#define PR_MAX_IR 1024
#define NORM_EPSILON 0.00000000001

/*
    Check stochastic property

    PARAMS
    @IN mat - matrix

    RETURN
    true iff matirx is stochastic
    false iff not
*/
static bool markov_check_stochastic(const gsl_matrix *mat);

/*
    Calculate probability distribution

    PARAMS
    @IN start - start vector
    @IN mat - matrix
    @IN norm - pointer to norm
    @IN eps - epsilon
    @IN limit - limit for loop
    @OUT rnds - rnds needed

    RETURN
    NULL iff failure
    Pointer to dist vector iff success
*/
static gsl_vector *__markov_dist(const gsl_vector *start,
                                 const gsl_matrix *mat,
                                 double (*norm)(const gsl_vector *vec, const gsl_vector *vec2),
                                 double eps,
                                 size_t limit,
                                 size_t *rnds);

static bool markov_check_stochastic(const gsl_matrix *mat)
{
    size_t i;
    size_t j;
    double sum;
    size_t n;

    TRACE();

    n = mat->size1;
    for (i = 0; i < n; ++i)
    {
        sum = 0.0;
        for (j = 0; j < n; ++j)
            sum += gsl_matrix_get(mat, i, j);

        if (ABS(sum - 1.0) > DBL_EPSILON)
            return false;
    }

    return true;
}

gsl_vector *markov_m_v_prod(const gsl_matrix *m, const gsl_vector *v)
{
    gsl_vector *vec;
    size_t i;
    size_t j;
    size_t n;
    double sum;

    TRACE();

    n = v->size;
    vec = gsl_vector_calloc(n);
    if (vec == NULL)
        ERROR("vector alloc error\n", NULL);

    for (i = 0; i < n; ++i)
    {
        sum = 0.0;
        for (j = 0; j < n; ++j)
            sum += gsl_matrix_get(m, j, i) * gsl_vector_get(v, j);

        gsl_vector_set(vec, i, sum);
    }

    return vec;
}

double markov_vec_norm(const gsl_vector *vec, const gsl_vector *vec2)
{
    size_t i;
    double sum;
    size_t n;

    TRACE();

    sum = 0.0;
    n = vec->size;
    for (i = 0; i < n; ++i)
        sum += ABS((gsl_vector_get(vec, i) - gsl_vector_get(vec2, i)));

    return sum;
}

double markov_vec_norm_max(const gsl_vector *vec, const gsl_vector *vec2)
{
    size_t i;
    size_t n;
    double max;
    double temp;

    TRACE();

    max = 0.0;
    n = vec->size;

    for (i = 0; i < n; ++i)
    {
        temp = ABS((gsl_vector_get(vec, i) - gsl_vector_get(vec2, i)));
        if (temp > max)
            max = temp;
    }

    return max;
}


gsl_vector *markov_static_dist( const gsl_vector *start,
                                const gsl_matrix *mat,
                                double (*norm)(const gsl_vector *vec, const gsl_vector *vec2),
                                double eps,
                                size_t *rnds)
{
  return __markov_dist(start, mat, norm, eps, PR_MAX_IR, rnds);
}

gsl_vector *markov_prob_dist(const gsl_vector *start, const gsl_matrix *mat, size_t rnd)
{
    return __markov_dist(start, mat, NULL, 0.0, rnd, NULL);
}

static gsl_vector *__markov_dist(const gsl_vector *start,
                                 const gsl_matrix *mat,
                                 double (*norm)(const gsl_vector *vec, const gsl_vector *vec2),
                                 double eps,
                                 size_t limit,
                                 size_t *rnds)
{
    size_t i;
    gsl_vector *vec;
    gsl_vector *prev_vec;

    TRACE();

    if (start == NULL)
        ERROR("start vector == NULL\n", NULL);

    if (mat == NULL)
        ERROR("Matrix == NULL\n", NULL);

    if (markov_check_stochastic(mat) == false)
        ERROR("Matrix is not stochastic !!!\n", NULL);

    if (norm == NULL)
        norm = markov_vec_norm;

    if (eps == 0.0)
        eps = NORM_EPSILON;

    vec = gsl_vector_alloc(start->size);
    if (vec == NULL)
        ERROR("gsl_vector_alloc error\n", NULL);

    prev_vec = gsl_vector_alloc(start->size);
    if (prev_vec == NULL)
        ERROR("gsl_vector_alloc error\n", NULL);

    gsl_vector_memcpy(vec, start);
    i = 0;
    do {
        gsl_vector_free(prev_vec);
        prev_vec = vec;
        vec = markov_m_v_prod(mat, vec);
        ++i;
    } while (i < limit && norm(vec, prev_vec) > eps);

    gsl_vector_free(prev_vec);
    if (rnds != NULL)
        *rnds = i;

    return vec;
}