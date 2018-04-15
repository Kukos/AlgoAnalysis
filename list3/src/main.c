#include <log.h>
#include <compiler.h>
#include <stddef.h>
#include <common.h>
#include <experiments.h>
#include <math.h>

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
    size_t b[] = {4, 7, 10, 16};

    experiment1(b, ARRAY_SIZE(b), 10000);
    experiment2(b, ARRAY_SIZE(b), 10000);

    return 0;
}
