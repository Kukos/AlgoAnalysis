#include <election.h>
#include <experiments.h>
#include <common.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <histogram.h>
#include <getch.h>

#define MAX_SLOTS 60
#define REPEATS 5000
#define BUFFER_SIZE 128

/*
    Single step (situation) for experiment1 purpose

    PARAMS
    @IN n - number of diods
    @IN u - upper bound of n

    RETURN
    This is a void function
*/
static ___inline___ void experiment1_single_step(size_t n, size_t u);


/*
    Single step (situation) for experiment2 purpose

    PARAMS
    @IN n - number of diods
    @IN u - upper bound of n
    @IN r - round

    RETURN
    This is a void function
*/
static ___inline___ double experiment2_single_step(size_t n, size_t u, int r);

/*
    Single step (situation) for experiment3 purpose

    PARAMS
    @IN n - number of diods
    @IN u - upper bound of n

    RETURN
    This is a void function
*/
static ___inline___ void experiment3_single_step(size_t n, size_t u);

static ___inline___ void experiment1_single_step(size_t n, size_t u)
{
    double avg = 0.0;
    double var = 0.0;
    size_t t[REPEATS];
    size_t i;

    for (i = 0; i < REPEATS; ++i)
    {
        t[i] = election(n, u);
        avg += (double)t[i];
    }

    avg /= (double)REPEATS;
    for (i = 0; i < REPEATS; ++i)
        var += (double)((double)t[i] - avg) * ((double)t[i] - (avg));

    var /= (double)REPEATS;
    (void)printf("N = %6zu, U = %6zu, E[Slots] = %8.4lf, Var[Slots] = %8.4lf\n", n, u, avg, var);
}

static ___inline___ double experiment2_single_step(size_t n, size_t u, int r)
{
    size_t rnd_r = 0;
    size_t slots;
    size_t rnd;
    size_t i;

    for (i = 0; i < REPEATS; ++i)
    {
        slots = election(n, u);
        rnd = GET_ROUND_NUMBER_FROM_SLOTS(slots, u);
        if (rnd <= (size_t)r)
            ++rnd_r;
    }

    return (double)rnd_r / (double)REPEATS;
}

static ___inline___ void experiment3_single_step(size_t n, size_t u)
{
    int64_t t[MAX_SLOTS + 1] = {0};
    int64_t x[MAX_SLOTS + 1] = {0};
    size_t i;
    size_t slots;
    char title[BUFFER_SIZE];
    const char *const x_axis_str = "Slots";
    const char *const y_axis_str = "Number of ends in slot X";

    (void)snprintf(title, BUFFER_SIZE, "Ending of election for N = %zu, U = %zu", n, u);

    for (i = 0; i < REPEATS; ++i)
    {
        slots = election(n, u);
        if (slots < ARRAY_SIZE(t))
            ++t[slots];
    }

    for (i = 0; i <= MAX_SLOTS; ++i)
        x[i] = (int64_t)i;

    histogram_int_draw(x, t, ARRAY_SIZE(x), ARRAY_SIZE(t), title, x_axis_str, y_axis_str, false);
    getch();
}

void experiments1(size_t *u, size_t len)
{
    size_t i;

    srand((unsigned)time(NULL));

    /* 2nd situation */
    (void)printf("\n");
    for (i = 0; i < len; ++i)
        experiment1_single_step(u[i], 0);

    /* 2nd situation */
    (void)printf("\n");
    for (i = 0; i < len; ++i)
        experiment1_single_step(2, u[i]);

    /* 3rd situation */
    (void)printf("\n");
    for (i = 0; i < len; ++i)
        experiment1_single_step(u[i] >> 1, u[i]);

    /* 4th situation */
    (void)printf("\n");
    for (i = 0; i < len; ++i)
        experiment1_single_step(u[i], u[i]);
}

void experiments2(size_t u, int r)
{
    size_t i;
    double prob = 0.0;

    srand((unsigned)(time(NULL)));

    for (i = 100; i <= u; i += 100)
    {
        prob = experiment2_single_step(i, u, r);
        (void)printf("PROB[finish <= %d R] = %8.4lf\tfor n = %6zu\tu = %6zu\n", r, prob, i, u);
    }
}

void experiments3(size_t n)
{
    srand((unsigned)time(NULL));

    /* 1st situation */
    experiment3_single_step(n, 0);

    /* 2nd situation */
    experiment3_single_step(2, n);

    /* 3rd situation */
    experiment3_single_step(n >> 1, n);

    /* 4th situation */
    experiment3_single_step(n, n);
}