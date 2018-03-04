#include <log.h>
#include <experiments.h>
#include <compiler.h>
#include <stddef.h>
#include <common.h>

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

int main(void)
{
    //size_t u_1[] = {10, 100, 1000, 5000, 10000};

    //experiments1(u_1, (size_t)ARRAY_SIZE(u_1));
    //experiments2(2000, 1);
    experiments3(100);
    return 0;
}
