#include <log.h>
#include <compiler.h>
#include <stddef.h>
#include <common.h>
#include <dijkstra.h>
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
    (void)printf("Dijkstra Mutual Exclusion simulation\n\n");
    (void)printf("PARAM:\n");
    (void)printf("N - number of proc in simulation\n\n");
}

int main(int argc, char **argv)
{
    int nproc;

    if (argc < 2)
    {
        help();
        return 1;
    }

    nproc = atoi(argv[1]);
    (void)printf("Dijkstra Mutual Exclusion simulation for %d proc\n\n\n", nproc);

    increase_task_prio(PRIO_MIN); /* max task prio is PRIO_MIN */
    increase_stack(BIT(27)); /* 128 MB Stack */

    mutual_exclusion_proof((size_t)nproc);

    (void)printf("Simulation done\n");

    return 0;
}

/*
    only master can move
    r0 = r1 = r2 = ... rn-1

    N    RND
   ----------
    2  - 1
    3  - 4
    4  - 15
    5  - 26
    6  - 40
    7  - 57
    8  - 77
    9  - 100
    10 - 126


    only one (master or slave) can move

    N    RND
   ----------
    2  - 0
    3  - 2
    4  - 13
    5  - 24
    6  - 38
    7  - 55
    8  - 75
    9  - 98
    10 - 124

    CPU Intel 6700k 4.2 Ghz
    RAM 32 GB 2133Mhz

    N    TIME
   --------------
    2  - 0
    3  - 0
    4  - 0
    5  - 0
    6  - 0
    7  - 0.2s
    8  - 8.6s
    9  - 3m 53.9s
    10 - 98m 2s

*/