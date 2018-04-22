#ifndef DCONFIG_H
#define DCONFIG_H

/*
    Dijkstra Config

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL3.0
*/

#include <stdint.h>
#include <stddef.h>
#include <common.h>
#include <sys/types.h>

typedef int64_t dconfig_code_t;
typedef uint8_t dconfig_t;

typedef struct DConfig
{
    size_t ____nproc;
    __extension__ dconfig_t ____regs[]; /* placeholder for data */
}DConfig;

#define NCONFS(n) fpow((uint64_t)(n + 1), (uint64_t)n)

#define for_each_dconfig(code, n) for (code = 0; code < (int64_t)NCONFS(n); ++code)

/*
    Create DConfig with @proc nproc

    PARAMS
    @IN proc - num of proc

    RETURN
    NULL iff failure
    Pointer to DCong iff success
*/
DConfig *dconfig_create(size_t proc);

/*
    Destroy Dconfig

    PARAMS
    @IN conf - conf to destroy

    RETURN
    This is a void function
*/
void dconfig_destroy(DConfig *conf);

/*
    Encode config into code

    PARAMS
    @IN conf - config

    RETURN
    Code
*/
static ___inline___ dconfig_code_t dconfig_encode(const DConfig *conf) ___nonull___(1);

/*
    Decode code into config

    PARAMS
    @IN conf - config where code will be decoded
    @IN code - code typ decode

    RETURN
    This is a void function
*/
static ___inline___ void dconfig_decode(DConfig *conf, dconfig_code_t code) ___nonull___(1);

/*
    Fast power base ^ exp

    PARAMS
    @IN base - base
    @IN exp - exponent

    RETURN
    base ^ exp
*/
static ___inline___ uint64_t fpow(uint64_t base, uint64_t exp);

static ___inline___ uint64_t fpow(uint64_t base, uint64_t exp)
{
    uint64_t result = 1;
    while (exp != 0)
    {
        if (ODD(exp))
            result *= base;

        exp >>= 1;
        base *= base;
    }
    return result;
}

static ___inline___ dconfig_code_t dconfig_encode(const DConfig *conf)
{
    size_t i;
    dconfig_code_t code = 0;
    uint64_t factor = 1;

    for (i = 0; i < conf->____nproc; ++i)
    {
        code += (dconfig_code_t)conf->____regs[i] * (dconfig_code_t)factor;
        factor *= conf->____nproc + 1;
    }

    return code;
}

static ___inline___ void dconfig_decode(DConfig *conf, dconfig_code_t code)
{
    ssize_t i;
    uint64_t factor;

    factor = fpow((uint64_t)(conf->____nproc + 1),  (uint64_t)(conf->____nproc - 1));
    for (i = (ssize_t)conf->____nproc - 1; i >= 0; --i)
    {
        conf->____regs[i] = (dconfig_t)(code / (dconfig_code_t)factor);
        code %= (dconfig_code_t)factor;
        factor /= conf->____nproc + 1;
    }
}

#endif