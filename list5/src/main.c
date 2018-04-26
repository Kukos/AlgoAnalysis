#include <log.h>
#include <compiler.h>
#include <stddef.h>
#include <common.h>
#include <mis.h>
#include <graph.h>
#include <sys/resource.h>

___before_main___(1) void init(void);
___after_main___(1) void deinit(void);

___before_main___(1) void init(void)
{
    (void)log_init(stdout, NO_LOG_TO_FILE);
}

___after_main___(1) void deinit(void)
{
    log_deinit();
}

static void print_bytes(size_t n);
static void increase_task_prio(int prio);
static void increase_stack(size_t n);
static void help(void);

static void print_bytes(size_t n)
{
    size_t factor;
    const char *const b = "B";
    const char *const kb = "KB";
    const char *const mb = "MB";
    const char *const gb = "GB";
    const char *ptr;

    if (n > BIT(30))
    {
        factor = BIT(30);
        ptr = gb;
    }
    else if (n > BIT(20))
    {
        factor = BIT(20);
        ptr = mb;
    }
    else if (n > BIT(10))
    {
        factor = BIT(10);
        ptr = kb;
    }
    else
    {
        factor = 1;
        ptr = b;
    }

    (void)printf("%zu %s", n / factor, ptr);
}

static void increase_stack(size_t n)
{
    struct rlimit rl;

    if (getrlimit(RLIMIT_STACK, &rl))
        FATAL("Cant get stack limit\n");

    (void)printf("Stack Limit = ");
    print_bytes(rl.rlim_cur);
    (void)printf("\n");

    if (rl.rlim_cur < n)
    {
        rl.rlim_cur = n;
        if (setrlimit(RLIMIT_STACK, &rl))
            FATAL("Cant increase stack\n");

        (void)printf("Stack Limit increased to ");
        print_bytes(n);
        (void)printf("\n");
    }
}

static void increase_task_prio(int prio)
{
    int cp;

    cp = getpriority(PRIO_PROCESS, 0);
    (void)printf("Current Task prio = %d\n", cp);

    if (cp > prio)
    {
        if (setpriority(PRIO_PROCESS, 0, prio))
            return;

        (void)printf("Task prio increased to %d\n", prio);
    }
}

static void help(void)
{
    (void)printf("Maximum independent set simulation\n\n");
    (void)printf("PARAM:\n");
    (void)printf("N - number of proc in simulation\n\n");
}

int main(int argc, char **argv)
{
    Graph *g;
    int nproc;

    if (argc < 2)
    {
        help();
        return 1;
    }

    nproc = atoi(argv[1]);
    (void)printf("Maximum independent set simulation for %d proc\n\n\n", nproc);

    increase_task_prio(PRIO_MIN); /* max task prio is PRIO_MIN */
    increase_stack(BIT(27)); /* 128 MB Stack */

    g = graph_create((size_t)nproc);
    mis_proof(g);
    graph_destroy(g);

    (void)printf("Simulation done\n");

    return 0;
}
