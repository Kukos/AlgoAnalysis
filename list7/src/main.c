#include <log.h>
#include <compiler.h>
#include <stddef.h>
#include <common.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <queue_model.h>

#define BUFFER_SIZE 256

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

static void experiment_with_prob(double *prob_next, double *prob_prev, size_t n, size_t rnds)
{
    char file1[BUFFER_SIZE];
    char file2[BUFFER_SIZE];

    size_t i;
    Queue_model *qmodel;

    for (i = 0; i < n; ++i)
    {
        (void)snprintf(file1, BUFFER_SIZE, "visit_next=%0.2lf_prev=%0.2lf.txt", prob_next[i], prob_prev[i]);
        (void)snprintf(file2, BUFFER_SIZE, "avgtime_next=%0.2lf_prev=%0.2lf.txt", prob_next[i], prob_prev[i]);

        qmodel = qmodel_create(prob_next[i], prob_prev[i]);
        qmodel_run(qmodel, rnds);
        qmodel_results(qmodel, file1, file2);

        qmodel_destroy(qmodel);
    }
}

int main(void)
{
    double prob_next[] = {0.5, 0.3, 0.4};
    double prob_prev[] = {0.3, 0.5, 0.4};

    const size_t rnds = BIT(20);
    experiment_with_prob(prob_next, prob_prev, ARRAY_SIZE(prob_next), rnds);

    return 0;
}