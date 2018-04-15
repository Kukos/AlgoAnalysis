#include <experiments.h>
#include <hash.h>
#include <stdlib.h>
#include <stdio.h>
#include <log.h>
#include <limits.h>
#include <kmin.h>
#include <hyperloglog.h>
#include <common.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <math.h>

#define BUFFER_SIZE 128

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
    Check hyperloglog on set with n size

    PARAMS
    @IN n - set size
    @IN b - b parameter

    RETURN
    Res of hll(n, b)
*/
static ___inline___ size_t check_hll(size_t n, size_t b);

/*
    Check kmin on set with n size

    PARAMS
    @IN n - set size
    @IN k - k parameter

    RETURN
    Res of kmin(n, k)
*/
static ___inline___ size_t check_kmin(size_t n, size_t k);


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

static ___inline___ size_t check_hll(size_t n, size_t b)
{
    return hyper_loglog(get_new_number, n, b, HYPERLOGLOG_HASH_FUNCTION);
}

static ___inline___ size_t check_kmin(size_t n, size_t k)
{
    return kmin(get_new_number, n, k, KMIN_HASH_FUNCTION);
}


void experiment1(size_t *b, size_t b_len, size_t n)
{
    size_t i;
    size_t j;
    size_t estimated;
    double ratio;

    int fd;
    char name[BUFFER_SIZE];
    const char *const prefix = "./hll_ratio_for_m=";
    const char *const file_fmt = "%zu\t%0.2lf\n";

    printf("Experiment 1\n");
    for (j = 0; j < b_len; ++j)
    {
        reset_counter();
        (void)snprintf(name, BUFFER_SIZE, "%s%zu.txt", prefix, (size_t)BIT(b[j]));

        fd = open(name, O_RDWR | O_CREAT, 0644);
        if (fd == -1)
            FATAL("Cannot open file\n");

        for (i = 1; i <= n; ++i)
        {
            estimated = check_hll(i, b[j]);
            ratio = (double)estimated / (double)i;

            dprintf(fd, file_fmt, i, ratio);
        }
        close(fd);
    }
    printf("DONE\n");
}

void experiment2(size_t *b, size_t b_len, size_t n)
{
    size_t i;
    size_t j;
    size_t estimated;

    int fd;
    char name[BUFFER_SIZE];
    const char *const prefix = "./hll_vs_kmin_ratio_for_m=";
    const char *const file_fmt = "%zu\t%0.2lf\t%0.2lf\n";

    double *kmin_ratio;
    double *hll_ratio;

    kmin_ratio = (double *)malloc(sizeof(double) * n);
    if (kmin_ratio == NULL)
        FATAL("malloc error\n");

    hll_ratio = (double *)malloc(sizeof(double) * n);
    if (hll_ratio == NULL)
        FATAL("malloc error\n");

    printf("Experiment 2\n");
    for (j = 0; j < b_len; ++j)
    {
        reset_counter();
        (void)snprintf(name, BUFFER_SIZE, "%s%zu.txt", prefix, (size_t)BIT(b[j]));

        fd = open(name, O_RDWR | O_CREAT, 0644);
        if (fd == -1)
            FATAL("Cannot open file\n");

        for (i = 1; i <= n; ++i)
        {
            estimated = check_hll(i, b[j]);
            hll_ratio[i - 1] = (double)estimated / (double)i;
        }

        reset_counter();

        for (i = 1; i <= n; ++i)
        {
            estimated = check_kmin(i, (BIT(b[j]) * 5) >> 5);
            kmin_ratio[i - 1] = (double)estimated / (double)i;
        }

        for (i = 0; i < n; ++i)
            dprintf(fd, file_fmt, i + 1, kmin_ratio[i], hll_ratio[i]);


        close(fd);
    }
    printf("DONE\n");

    FREE(kmin_ratio);
    FREE(hll_ratio);
}