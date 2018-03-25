#include <experiments.h>
#include <hash.h>
#include <stdlib.h>
#include <stdio.h>
#include <log.h>
#include <limits.h>
#include <kmin.h>
#include <common.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <math.h>

#define BUFFER_SIZE 128
#define CORRECT_EPSILON 0.1
#define PREC_MULT 1000000

static long __counter = 1;

/*
    Reset counter
*/
static ___inline___ void reset_counter(void);

/*
    Get new number for set
*/
static ___inline___ long get_new_number(void);

/*
    Check kmin on set with n size

    PARAMS
    @IN n - set size
    @IN k - k parameter

    RETURN
    Res of kmin(n, k)
*/
static ___inline___ size_t check_kmin(size_t n, size_t k);

/*
    Check iff kmin estimated n with epsilon error

    PARAMS
    @IN n - size of set
    @IN epsilon - e error

    RETURN
    false iff not correct
    true iff correct
*/
static ___inline___ bool is_kmin_correct(size_t n, double epsilon);

/*
    Check estimated counter stored in array t is correct with epsilon

    PARAMS
    @IN t - array with kmin results
    @IN len - len(t)
    @IN epsilon - estimation error
    @IN prob - prob such that kmin have to fullfil

    RETURN
    false iff array is not correct
    true iff array is correct
*/
static ___inline___ bool is_array_from_kmin_correct(size_t *t, size_t len, double epsilon, double prob);

/*
    Calculate eps from Czebyszew's formula

    PARAMS
    @IN n - size of set
    @IN k - k from kmin
    @IN prob - expected probability

    RETURN
    Calculated epsilon
*/
static ___inline___ double eps_from_czebyszew_formula(size_t n, size_t k, double prob);

/*
    Calculate eps from Czernoff formula

    PARAMS
    @IN k - k from kmin
    @IN prob - expected probability

    RETURN
    Calculated epsilon
*/
static ___inline___ double eps_from_czernoff_formula(size_t k, double prob);

static ___inline___ void reset_counter(void)
{
    __counter = 1;
}

static ___inline___ long get_new_number(void)
{
    long c = __counter++;

    if (__counter == LONG_MAX)
        reset_counter();

    return c;
}

static ___inline___ size_t check_kmin(size_t n, size_t k)
{
    return kmin(get_new_number, n, k, KMIN_HASH_FUNCTION);
}

static ___inline___ bool is_kmin_correct(size_t n, double epsilon)
{
    ssize_t estimated = (ssize_t)check_kmin(n, 0);
    if (estimated >= (ssize_t)((1 - epsilon) * (double)n) && estimated <= (ssize_t)((1 + epsilon) * (double)n))
        return true;

    return false;
}

static ___inline___ bool is_array_from_kmin_correct(size_t *t, size_t len, double epsilon, double prob)
{
    size_t i;
    size_t correct = 0;

    for (i = 0; i < len; ++i)
        if ((ssize_t)t[i] >= (ssize_t)((1 - epsilon) * (double)(i + 1)) && (ssize_t)t[i] <= (ssize_t)((1 + epsilon) * (double)(i + 1)))
            ++correct;

    return (double)((double)correct / (double)len) >= prob;
}

static ___inline___ double eps_from_czebyszew_formula(size_t n, size_t k, double prob)
{
    double var = (1.0 / (double)(k - 2)) + 1.0 / (double)n;

    return sqrt(var / (1.0 - prob));
}

#define f_k_x(k, x) \
    __extension__ \
    ({ \
        typeof(k) __k = (k); \
        typeof(x) __x = (x); \
        exp(__k * __x) * pow(1.0 - __x, __k); \
    })

static ___inline___ double eps_from_czernoff_formula(size_t k, double prob)
{
    double res = 1.0;
    double x = 0.9;
    const double step = 0.0001;

    while (res > prob)
    {
        res = 1 - f_k_x((double)k, x) - f_k_x((double)k, -x);
        x -= step;
    }

    return x + step;
}

#undef f_k_x

void experiment1(size_t *k, size_t k_len, size_t n)
{
    size_t i;
    size_t j;
    size_t estimated;
    double ratio;

    int fd;
    char name[BUFFER_SIZE];
    const char *const prefix = "./kmin_ratio_for_k=";
    const char *const file_fmt = "%zu\t%0.2lf\n";

    printf("Experiment 1\n");
    for (j = 0; j < k_len; ++j)
    {
        reset_counter();
        (void)snprintf(name, BUFFER_SIZE, "%s%zu.txt", prefix, k[j]);

        fd = open(name, O_RDWR | O_CREAT, 0644);
        if (fd == -1)
            FATAL("Cannot open file\n");

        for (i = 1; i <= n; ++i)
        {
            estimated = check_kmin(i, k[j]);
            ratio = (double)estimated / (double)i;

            dprintf(fd, file_fmt, i, ratio);
        }
        close(fd);
    }
    printf("DONE\n");
}

void experiment2(size_t n, int percent)
{
    size_t i;
    size_t correct = 0;
    double res;

    TRACE();

    printf("Experiment 2\n");
    reset_counter();
    for (i = 1; i <= n; ++i)
        correct += is_kmin_correct(i, CORRECT_EPSILON);

    res = (double)(correct * 100) / (double)n;

    if (res > (double)(100 - percent))
        printf("K is optimal [%lf percent]\n", res);
    else
        printf("K is not optimal [Only %lf percent]\n", res);

    printf("DONE\n");
}

void experiment3(double epsilon)
{
    size_t i = BIT(sizeof(uint32_t) << 2); /* birthday parados */

    TRACE();

    reset_counter();
    printf("Experiment 3\n");
    while (is_kmin_correct(i, epsilon))
    {
        reset_counter();
        i <<= 1;
    }

    printf("For N = %zu(2^%zu), kmin is not correct with e = %lf\n", i, LOG2(i), epsilon);

    printf("DONE\n");
}

void experiment4(size_t n, size_t k, double *prob, size_t prob_len)
{
    size_t i;
    size_t j;

    size_t *t;

    size_t left;
    size_t right;
    size_t mid;

    double czebyszew_eps;
    double eps;
    double czernoff_eps;

    int fd;
    char name[BUFFER_SIZE];
    const char *const prefix = "./delta_for_prob=";
    const char *const file_fmt = "%zu\t%0.4lf\t%0.4lf\t%.04lf\t%0.4lf\t%0.4lf\t%0.4lf\t%0.4lf\n";

    printf("Experiment 4\n");
    reset_counter();

    t = (size_t *)malloc(sizeof(size_t) * n);
    if (t == NULL)
        FATAL("Malloc error\n");

    /* write results to temp array */
    for (i = 1; i <= n; ++i)
        t[i - 1] = check_kmin(i, k);

    for (j = 0; j < prob_len; ++j) /* foreach prob */
    {
        (void)snprintf(name, BUFFER_SIZE, "%s%0.4lf.txt", prefix, prob[j]);

        fd = open(name, O_RDWR | O_CREAT, 0644);
        if (fd == -1)
            FATAL("Cannot open file\n");

        czernoff_eps = eps_from_czernoff_formula(k, prob[j]);
        for (i = 0; i < n; ++i) /* foreach subarray */
        {
            eps = 0.0;

            left = 0;
            right = 100 * PREC_MULT;

            /* binarysearch for finding min eps */
            while (left < right)
            {
                mid = (left + right) >> 1;
                eps = (double)mid / (double)PREC_MULT;
                if (!is_array_from_kmin_correct(t, i + 1, eps, prob[j]))
                    left = mid + 1;
                else
                    right = mid;
            }

            czebyszew_eps = eps_from_czebyszew_formula(i + 1, k, prob[j]);

            /* write results to file */
            dprintf(fd, file_fmt, i + 1,
                                  (double)((double)t[i] / (double)(i + 1)),
                                  1.0 + eps,
                                  1.0 - eps,
                                  1.0 + czebyszew_eps,
                                  1.0 - czebyszew_eps,
                                  1.0 + czernoff_eps,
                                  1.0 - czernoff_eps);
        }

        close(fd);
    }

    FREE(t);
    printf("DONE\n");
}