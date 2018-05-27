#include <queue_model.h>
#include <log.h>
#include <stdlib.h>
#include <common.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

static void qstate_destroy_wrapper(void *state);
static double qstate_avg_time_to_come_back(QState *qstate);

static void qstate_destroy_wrapper(void *state)
{
    QState *qstate = *(QState **)state;
    qstate_destroy(qstate);
}

static double qstate_avg_time_to_come_back(QState *qstate)
{
    TRACE();

    if (qstate->____sum_time_come_back == 0)
        return 0.0;

    return (double)qstate->____sum_time_come_back / (double)qstate->____counter;
}

QState *qstate_create(void)
{
    QState *qstate;

    TRACE();

    qstate = (QState *)malloc(sizeof(QState));
    if (qstate == NULL)
        ERROR("malloc error\n", NULL);

    qstate->____counter = 0;
    qstate->____last_visit = 0;
    qstate->____sum_time_come_back = 0;

    return qstate;
}

void qstate_destroy(QState *qstate)
{
    TRACE();

    if (qstate == NULL)
        return;

    FREE(qstate);
}

void qstate_visit(QState *qstate, round_t rnd)
{
    round_t time_to_come_back;
    TRACE();

    if (qstate == NULL)
        return;

    ++qstate->____counter;

    if (qstate->____last_visit)
    {
        time_to_come_back = rnd - qstate->____last_visit;
        qstate->____sum_time_come_back += time_to_come_back;
    }

    qstate->____last_visit = rnd;
}

Queue_model *qmodel_create(double p1, double p2)
{
    Queue_model *qmodel;

    TRACE();

    if (p1 + p2 > 1.0)
        ERROR("p1 + p2 must be < 1.0\n", NULL);

    qmodel = (Queue_model *)malloc(sizeof(Queue_model));
    if (qmodel == NULL)
        ERROR("malloc error\n", NULL);

    qmodel->____p_next = (int)(p1 * 100);
    qmodel->____p_prev = (int)(p2 * 100);
    qmodel->____p_stay = 100 - qmodel->____p_next - qmodel->____p_prev;

    qmodel->____q = darray_create(DARRAY_UNSORTED, 0, sizeof(QState *), NULL, qstate_destroy_wrapper);
    if (qmodel->____q == NULL)
        ERROR("darray_create error\n", NULL);

    return qmodel;
}

void qmodel_destroy(Queue_model *qmodel)
{
    TRACE();

    if (qmodel == NULL)
        return;

    darray_destroy_with_entries(qmodel->____q);
    FREE(qmodel);
}

void qmodel_run(Queue_model *qmodel, round_t rnd)
{
    round_t i;
    size_t index;
    int r;
    int temp;

    QState *qstate;

    TRACE();

    if (qmodel == NULL)
        return;

    srand((unsigned)time(NULL));

    index = 0;
    qstate = qstate_create();
    darray_insert(qmodel->____q, (const void *)&qstate);

    for (i = 0; i < rnd; ++i)
    {
        r = rand() % 100;
        temp = r;
        if (r < qmodel->____p_next)
        {
            LOG("R = %d, PN = %d, PP = %d, PS = %d, go to next\n", temp, qmodel->____p_next, qmodel->____p_prev, qmodel->____p_stay);

            ++index;
            if (index >= (size_t)darray_get_num_entries(qmodel->____q))
            {
                qstate = qstate_create();
                darray_insert(qmodel->____q, (const void *)&qstate);
            }

            qstate_visit(((QState **)darray_get_array(qmodel->____q))[index], i);
            continue;
        }

        r -= qmodel->____p_next;
        if (index && r < qmodel->____p_prev)
        {
            LOG("R = %d, PN = %d, PP = %d, PS = %d, go to prev\n", temp, qmodel->____p_next, qmodel->____p_prev, qmodel->____p_stay);
            --index;
            qstate_visit(((QState **)darray_get_array(qmodel->____q))[index], i);
            continue;
        }

        LOG("R = %d, PN = %d, PP = %d, PS = %d, stay\n", temp, qmodel->____p_next, qmodel->____p_prev, qmodel->____p_stay);
        qstate_visit(((QState **)darray_get_array(qmodel->____q))[index], i);
    }
}

void qmodel_results(const Queue_model *qmodel, const char *file1, const char *file2)
{
    int fd;
    QState *qstate;
    size_t i = 0;

    TRACE();

    /* write counters */
    fd = open(file1, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
        FATAL("Cannot open file\n");

    for_each_data(qmodel->____q, Darray, qstate)
        dprintf(fd, "%zu\t%zu\n", i++, qstate->____counter);

    close(fd);

    /* write avg */
    fd = open(file2, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
        FATAL("Cannot open file\n");

    i = 0;
    for_each_data(qmodel->____q, Darray, qstate)
        dprintf(fd, "%zu\t%0.4lf\n", i++, qstate_avg_time_to_come_back(qstate));

    close(fd);
}