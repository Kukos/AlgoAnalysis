#include <dijkstra.h>
#include <log.h>
#include <dconfig.h>
#include <ccache.h>
#include <common.h>
#include <stdbool.h>

#define PASTE_TO_ARRAY_1(x)  x
#define PASTE_TO_ARRAY_2(x)  PASTE_TO_ARRAY_1(x), PASTE_TO_ARRAY_1(x)
#define PASTE_TO_ARRAY_4(x)  PASTE_TO_ARRAY_2(x), PASTE_TO_ARRAY_2(x)
#define PASTE_TO_ARRAY_8(x)  PASTE_TO_ARRAY_4(x), PASTE_TO_ARRAY_4(x)
#define PASTE_TO_ARRAY_16(x) PASTE_TO_ARRAY_8(x), PASTE_TO_ARRAY_8(x)
#define PASTE_TO_ARRAY_32(x) PASTE_TO_ARRAY_16(x), PASTE_TO_ARRAY_16(x)

#define PROGRESS_BAR_LEN 32

/* check proc possibility to move */
#define master_can_move(conf) (conf->____regs[0] == conf->____regs[conf->____nproc - 1])
#define slave_can_move(conf, i) (conf->____regs[i] != conf->____regs[i - 1])

#define master_move(nconf, conf) \
    nconf->____regs[0] = (dconfig_t)((size_t)(nconf->____regs[0] + 1) % (conf->____nproc + 1))

#define slave_move(nconf, conf, index) \
    nconf->____regs[index] = conf->____regs[index - 1];

#define __save_conf(buf, conf, i)       buf = conf->____regs[i]
#define __restore_conf(conf, buf, i)    conf->____regs[i] = buf

#define save_master_conf(buf, conf)     __save_conf(buf, conf, 0)
#define restore_master_conf(conf, buf)  __restore_conf(conf, buf, 0)

#define save_slave_conf(buf, conf, index)   __save_conf(buf, conf, index)
#define restore_slave_conf(conf, buf, index) __restore_conf(conf, buf, index)

/*
    Check configure corretness

    PARAMS
    @IN conf - DConf

    RETURN
    true iff conf is correct
    false iff not
*/
static ___inline___ bool correct_conf(DConfig *conf);

/*
    Recursive heart of proof

    PARAMS
    @IN conf - current conf
    @IN rnd - current rnd

    RETURN
    Max rnd to get this conf
*/
static size_t _proof(DConfig *conf, size_t rnd, CCache *cache);

static ___inline___ bool correct_conf(DConfig *conf)
{
    size_t i;
    size_t counter = 0;

    TRACE();

    if (master_can_move(conf))
        ++counter;

    for (i = 1; i < conf->____nproc; ++i)
        if (slave_can_move(conf, i))
        {
            ++counter;
            if (counter > 1)
                return false;
        }

    return counter == 1;
}

static size_t _proof(DConfig *conf, size_t rnd, CCache *cache)
{
    size_t tm = 0;
    size_t ts = 0;
    size_t t = 0;
    size_t temp = 0;

    size_t i;
    size_t n = conf->____nproc;

    dconfig_t buf;

    dconfig_code_t code;
    dconfig_t cache_val;

    TRACE();

    if (correct_conf(conf))
        return rnd;

    code = dconfig_encode(conf);
    cache_val = ccache_get_dconfig(cache, (size_t)code);

    if (cache_val != 0)
        return cache_val + rnd;

    if (master_can_move(conf))
    {
        save_master_conf(buf, conf);

        master_move(conf, conf);
        tm = _proof(conf, rnd + 1, cache);

        restore_master_conf(conf, buf);
    }

    for (i = 1; i < n; ++i)
        if (slave_can_move(conf, i))
        {
            save_slave_conf(buf, conf, i);

            slave_move(conf, conf, i);
            temp = _proof(conf, rnd + 1, cache);
            ts = MAX(ts, temp);

            restore_slave_conf(conf, buf, i);
        }

    t = MAX(tm, ts);

    ccache_set_dconfig(cache, (size_t)code, (dconfig_t)(t - rnd));
    return t;
}

void mutual_exclusion_proof(size_t n)
{
    size_t t = 0;
    size_t temp;
    dconfig_code_t code;
    DConfig *conf;
    CCache *cache;

    const char str1[] = {PASTE_TO_ARRAY_32('=')};
    const char str2[] = {PASTE_TO_ARRAY_32(' ')};

    size_t factor;

    TRACE();

    (void)printf("Creating cache ...");
    fflush(stdout);
    cache = ccache_create(NCONFS(n));
    if (cache == NULL)
        FATAL("ccache_create error\n");

    (void)printf(" DONE\n");

    (void)printf("Checking %zu(< 2^%ld) configs ...\n", NCONFS(n), LOG2(NCONFS(n) - 1) + 1);
    conf = dconfig_create(n);
    factor = MAX((uint64_t)1, NCONFS(n) / PROGRESS_BAR_LEN);

    (void)printf("\r|%.*s|", (int)(MIN((size_t)NCONFS(n), (size_t)PROGRESS_BAR_LEN)), str2);
    (void)fflush(stdout);
    for_each_dconfig(code, n)
    {
        dconfig_decode(conf, code);
        if (code && (size_t)code % factor == 0)
        {
            (void)printf("\r|%.*s%.*s|", (int)(((size_t)code + factor - 1) / factor), str1, (int)((NCONFS(n) - (size_t)code) / factor), str2);
            (void)fflush(stdout);
        }
        temp = _proof(conf, 0, cache);

        t = MAX(t, temp);
    }

    (void)printf("\r|%.*s|", (int)(MIN((size_t)NCONFS(n), (size_t)PROGRESS_BAR_LEN)), str1);
    (void)printf("\nMAX Time for %zu proc = %zu rounds\n", n, t);

    dconfig_destroy(conf);
    ccache_destroy(cache);
}
