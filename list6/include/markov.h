#ifndef MARKOV_H
#define MARKOV_H

/*
    Markov Probability / chains / matrix /distribution Implementation

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL 3.0
*/

#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>


/*
    Calculate vector of probability distribution

    PARAMS
    @IN start - start vector
    @IN mat - probability matrix
    @IN rnd - time

    RETURN
    NULL iff failure
    Pointer to distribution iff success
*/
gsl_vector *markov_prob_dist(const gsl_vector *start, const gsl_matrix *mat, size_t rnd);

/*
    Calculate statistic distribution

    PARAMS
    @IN start - start vector
    @IN mat - probability matrix
    @IN norm - function to calculate norm
    @IN eps - epsilon
    @OUT rnds - how many rnd needed to calculate dist

    RETURN
    NULL iff failure
    Pointer to distribution vector iff success
*/
gsl_vector *markov_static_dist( const gsl_vector *start,
                                const gsl_matrix *mat,
                                double (*norm)(const gsl_vector *vec, const gsl_vector *vec2),
                                double eps,
                                size_t *rnds);

/*
    Vector Norms

    NORM        - SUM(|vec[i] - vec2[i]|)
    NOMR MAX    - MAX (|vec[i| - vec2[i])

    PARAMS
    @IN vec - vector1
    @IN vec2 - vector2

    RETURN
    NORM
*/
double markov_vec_norm(const gsl_vector *vec, const gsl_vector *vec2);
double markov_vec_norm_max(const gsl_vector *vec, const gsl_vector *vec2);

/*
    V * M = V'

    PARAMS
    @IN m - matrix
    @IN v - vector

    RETURN
    V' iff success
    NULL iff failure
*/
gsl_vector *markov_m_v_prod(const gsl_matrix *m, const gsl_vector *v);

#endif