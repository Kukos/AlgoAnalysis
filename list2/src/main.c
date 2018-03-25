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
    size_t k[] = {2, 3, 10, 100, 400};
    double prob[] = {0.95, 0.99, 0.995};

    experiment1(k, ARRAY_SIZE(k), 10000);
    experiment2(10000, 5);
    experiment3(0.1);
    experiment4(10000, 400, prob, ARRAY_SIZE(prob));

    return 0;
}
