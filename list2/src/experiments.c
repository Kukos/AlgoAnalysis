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

#define BUFFER_SIZE 128
#define CORRECT_EPSILON 0.1

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
    size_t estimated = check_kmin(n, 0);
    if (estimated >= (size_t)((1 - epsilon) * (double)n) && estimated <= (size_t)((1 + epsilon) * (double)n))
        return true;

    return false;
}

void experiment1(size_t *k, size_t k_len, size_t n)
{
    size_t i;
    size_t j;
    size_t estimated;
    double ratio;

    int fd;
    char name[BUFFER_SIZE];
    const char * const prefix = "./kmin_ratio_for_k=";

    printf("Experiment 1\n");
    for (j = 0; j < k_len; ++j)
    {
        reset_counter();
        (void)snprintf(name, BUFFER_SIZE, "%s%zu", prefix, k[j]);

        fd = open(name, O_RDWR | O_CREAT, 0644);
        if (fd == -1)
            perror("Cannot open file\n");

        for (i = 1; i <= n; ++i)
        {
            estimated = check_kmin(i, k[j]);
            ratio = (double)estimated / (double)i;

            dprintf(fd, "%zu\t%0.2lf\n", i, ratio);
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