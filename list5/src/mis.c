#include <mis.h>
#include <log.h>
#include <graph.h>
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

typedef enum mis_state
{
    MIS_BLACK,
    MIS_WHITE,
    MIS_RED,
    MIS_GREY
} mis_state_t;


/*
    Check configure corretness

    PARAMS
    @IN g - graph

    RETURN
    true iff conf is correct
    false iff not
*/
static ___inline___ bool correct_conf(Graph *g);

/*
    Recursive heart of proof

    PARAMS
    @IN g - current state of graph
    @IN cache - cache

    RETURN
    This is a void function
*/
static void _proof(Graph *g, CCache *cache);

/*
    Get State of node in MIS

    PARAMS
    @IN n - node

    RETURN
    MIS state
*/
static ___inline___ mis_state_t node_get_mis_state(Node *n);

static ___inline___ bool node_mis_state_black(Node *node);
static ___inline___ bool node_mis_state_white(Node *node);
static ___inline___ bool node_mis_state_red(Node *node);
static ___inline___ bool node_mis_state_grey(Node *node);

static ___inline___ bool node_mis_state_black(Node *node)
{
    size_t n;
    size_t i;

    if (node->____state == NODE_OUT_OF_MIS)
        return false;

    n = node->____neigh_len;
    for (i = 0; i < n; ++i)
        if (node->____neigh[i]->____state == NODE_IN_MIS)
            return false;

    return true;
}

static ___inline___ bool node_mis_state_white(Node *node)
{
    size_t n;
    size_t i;

    if (node->____state == NODE_IN_MIS)
        return false;

    n = node->____neigh_len;
    for (i = 0; i < n; ++i)
        if (node->____neigh[i]->____state == NODE_IN_MIS)
            return true;

    return false;
}

static ___inline___ bool node_mis_state_red(Node *node)
{
    size_t n;
    size_t i;

    if (node->____state == NODE_OUT_OF_MIS)
        return false;

    n = node->____neigh_len;
    for (i = 0; i < n; ++i)
        if (node->____neigh[i]->____state == NODE_IN_MIS)
            return true;

    return false;
}

static ___inline___ bool node_mis_state_grey(Node *node)
{
    size_t n;
    size_t i;

    if (node->____state == NODE_IN_MIS)
        return false;

    n = node->____neigh_len;
    for (i = 0; i < n; ++i)
        if (node->____neigh[i]->____state == NODE_IN_MIS)
            return false;

    return true;
}


static ___inline___ mis_state_t node_get_mis_state(Node *n)
{
    if (node_mis_state_white(n))
        return MIS_WHITE;

    if (node_mis_state_black(n))
        return MIS_BLACK;

    if (node_mis_state_grey(n))
        return MIS_GREY;

    if (node_mis_state_red(n))
        return MIS_RED;

    return MIS_RED;
}

static ___inline___ bool correct_conf(Graph *g)
{
    size_t i;
    mis_state_t state;

    TRACE();

    for (i = 0; i < g->____nproc; ++i)
    {
        state = node_get_mis_state(&g->____nodes[i]);
        if (state != MIS_BLACK && state != MIS_WHITE)
            return false;
    }

    return true;
}

static void _proof(Graph *g, CCache *cache)
{
    size_t i;
    size_t n = g->____nproc;

    graph_code_t code;

    TRACE();

    if (correct_conf(g))
        return;

    code = graph_encode(g);

    if (ccache_conf_proved(cache, (size_t)code))
        return;

    for (i = 0; i < n; ++i)
    {
        if (node_get_mis_state(&g->____nodes[i]) == MIS_RED)
        {
            g->____nodes[i].____state = NODE_OUT_OF_MIS;
            _proof(g, cache);
            g->____nodes[i].____state = NODE_IN_MIS;
        }
        else if (node_get_mis_state(&g->____nodes[i]) == MIS_GREY)
        {
            g->____nodes[i].____state = NODE_IN_MIS;
            _proof(g, cache);
            g->____nodes[i].____state = NODE_OUT_OF_MIS;
        }
    }

    ccache_set_proof(cache, (size_t)code);
    return;
}

void mis_proof(Graph *g)
{
    graph_code_t code;
    CCache *cache;
    size_t n = g->____nproc;

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

    (void)printf("Checking %zu(< 2^%ld) configs ...\n", (size_t)NCONFS(n), LOG2(NCONFS(n) - 1) + 1);
    factor = MAX((uint64_t)1, (uint64_t)NCONFS(n) / PROGRESS_BAR_LEN);

    (void)printf("\r|%.*s|", (int)(MIN((size_t)NCONFS(n), (size_t)PROGRESS_BAR_LEN)), str2);
    (void)fflush(stdout);
    for_each_graph_config(code, n)
    {
        graph_decode(g, code);
        if (code && (size_t)code % factor == 0)
        {
            (void)printf("\r|%.*s%.*s|", (int)(((size_t)code + factor - 1) / factor), str1, (int)((NCONFS(n) - (size_t)code) / factor), str2);
            (void)fflush(stdout);
        }
        _proof(g, cache);
    }

    (void)printf("\r|%.*s|", (int)(MIN((size_t)NCONFS(n), (size_t)PROGRESS_BAR_LEN)), str1);
    (void)printf("\nPROVED\n");

    ccache_destroy(cache);
}