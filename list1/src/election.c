#include <log.h>
#include <election.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <common.h>

static size_t __j = 0;
static size_t __i = 0;
static size_t __prob = RAND_MAX >> 1;

/*
    Send signal with probablity optimal in situation
    when n is well known

    PARAMS
    @IN n - number of diods

    RETURN
    false iff dont send
    true iff send
*/
static bool send_signal_if_n_is_known(size_t n);

/*
    Send signal with probablity optimal in situation
    when n is unknown

    PARAMS
    @IN n - number of diods
    @IN u - upper bound of number of diods

    RETURN
    false iff dont send
    true iff send
*/
static bool send_signal_if_n_is_unknown(size_t n, size_t u);

/*
    Reset election state

    PARAMS
    @NO PARAMS

    RETURN
    This is a void function
*/
static ___inline___ void reset_state(void);

static bool send_signal_if_n_is_known(size_t n)
{
    /* 1/n to send signal else dont send */
    return rand() <= (int)(RAND_MAX / n);
}

static bool send_signal_if_n_is_unknown(size_t n, size_t u)
{
    bool ret;

    /* 1/2^i to send signal, else dont send */
    ret = rand() <= (int)__prob;
    ++__j;
    if (__j < n)
        return ret;

    __j = 0;
    __prob >>= 1;
    ++__i;

    LOG("PROB = %zu, I = %zu, RET = %d\n", __prob, __i, ret);

    /* end rnd ? */
    if (__i >= ROUND_LEN(u))
        reset_state();

    return ret;
}

static ___inline___ void reset_state(void)
{
    __j = 0;
    __i = 0;
    __prob = RAND_MAX >> 1;
}

size_t election(size_t n, size_t u)
{
    size_t slots = 0;
    size_t eter = 0;
    size_t i;

    TRACE();

    LOG("n = %zu, u = %zu\n", n, u);

    if (u == 0)
        LOG("N is known\n");
    else
        LOG("N is unknown\n");

    reset_state();
    while (eter != 1)
    {
        eter = 0;
        LOG("ETER = %zu\n", eter);
        for (i = 0; i < n; ++i)
            if (u == 0)
                eter += send_signal_if_n_is_known(n);
            else
                eter += send_signal_if_n_is_unknown(n, u);

        ++slots;
    }

    LOG("We have leader, slots = %zu\n", slots);
    return slots;
}