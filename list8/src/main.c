#include <log.h>
#include <compiler.h>
#include <stddef.h>
#include <common.h>
#include <stdio.h>
#include <queue_model.h>
#include <time.h>

#define QUEUES 100
#define SERVICE_LAMBDA 1.0
#define SERVICE_TIME 1.0
#define RUN_TIME 10000

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

static void experiment_poisson_time(double *lambda, size_t size)
{
    Queue_model *model;
    size_t i;

    TRACE();

    srand((unsigned int)time(NULL));
    printf("POISSON SERVICE TIME\n");
    for (i = 0; i < size; ++i)
    {
        model = queue_model_create(QUEUES, SERVICE_POISSON_DIST_TIME, lambda[i], SERVICE_LAMBDA, 0);
        printf("LAMBDA = %.02lf, AVG = %lf\n", lambda[i], queue_model_run(model, RUN_TIME));
        queue_model_destroy(model);
    }
}

static void experiment_const_time(double *lambda, size_t size)
{
    Queue_model *model;
    size_t i;

    TRACE();

    srand((unsigned int)time(NULL));
    printf("CONST SERVICE TIME\n");
    for (i = 0; i < size; ++i)
    {
        model = queue_model_create(QUEUES, SERVICE_CONST_TIME, lambda[i], SERVICE_LAMBDA, SERVICE_TIME);
        printf("LAMBDA = %.02lf, AVG = %lf\n", lambda[i], queue_model_run(model, RUN_TIME));
        queue_model_destroy(model);
    }
}


int main(void)
{
    double lambda[] = {0.5, 0.8, 0.9, 0.99};

    experiment_poisson_time(lambda, ARRAY_SIZE(lambda));
    experiment_const_time(lambda, ARRAY_SIZE(lambda));
    return 0;
}