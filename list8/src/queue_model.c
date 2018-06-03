#include <queue_model.h>
#include <log.h>
#include <stdlib.h>
#include <common.h>
#include <time.h>
#include <poisson_dist.h>
#include <string.h>

static Event *event_create(event_t type, round_t start, round_t end, size_t q)
{
    Event *e;

    TRACE();

    e = (Event *)malloc(sizeof(Event));
    if (e == NULL)
        ERROR("malloc error\n", NULL);

    e->type = type;
    e->start = start;
    e->end = end;
    e->q = q;

    return e;
}

static void event_destroy(Event *e)
{
    TRACE();

    if (e == NULL)
        return;

    FREE(e);
}

static int event_cmp(const Event *e1, const Event *e2)
{
    if (e1->end < e2->end)
        return -1;

    if (e1->end > e2->end)
        return 1;

    return 0;
}

static int event_cmp_wrapper(const void *e1, const void *e2)
{
    const Event *_e1 = *(const Event **)e1;
    const Event *_e2 = *(const Event **)e2;

    return event_cmp(_e1, _e2);
}

static void event_destroy_wrapper(void *e)
{
    Event *_e = *(Event **)e;
    event_destroy(_e);
}

Queue_model *queue_model_create(size_t n, service_t type, double arriv_lambda, double service_lambda, round_t s_time)
{
    Queue_model *qmodel;
    size_t i;

    TRACE();

    qmodel = (Queue_model *)malloc(sizeof(Queue_model));
    if (qmodel == NULL)
        ERROR("malloc error\n", NULL);

    qmodel->queue_num = n;
    qmodel->stype = type;
    qmodel->arriv_lambda = arriv_lambda;
    qmodel->service_lambda = service_lambda;
    qmodel->servcie_time = s_time;

    qmodel->in_progress = (bool *)malloc(sizeof(bool) * n);
    if (qmodel == NULL)
        ERROR("malloc error\n", NULL);

    (void)memset(qmodel->in_progress, 0, sizeof(bool) * n);

    qmodel->events = heap_create(HEAP_MIN, sizeof(Event *), 2, event_cmp_wrapper, event_destroy_wrapper);
    if (qmodel->events == NULL)
        ERROR("heap_create error\n", NULL);

    qmodel->queues = (Fifo **)malloc(sizeof(Fifo *) * n);
    if (qmodel->queues == NULL)
        ERROR("malloc error\n", NULL);

    for (i = 0; i < n; ++i)
    {
        qmodel->queues[i] = fifo_create(sizeof(round_t), NULL);
        if (qmodel->queues[i] == NULL)
            ERROR("fifo_create error\n", NULL);
    }

    return qmodel;
}

void queue_model_destroy(Queue_model *qmodel)
{
    size_t i;

    TRACE();

    if (qmodel == NULL)
        return;

    FREE(qmodel->in_progress);
    heap_destroy_with_entries(qmodel->events);

    for (i = 0; i < qmodel->queue_num; ++i)
        fifo_destroy(qmodel->queues[i]);

    FREE(qmodel->queues);
    FREE(qmodel);
}

double queue_model_run(Queue_model *qmodel, round_t limit)
{
    double last_event_time = 0.0;
    double event_time = 0.0;
    round_t cur_time = 0;
    round_t client_time;

    size_t i;

    round_t clients_time_in_queue = 0.0;
    size_t completed = 0;

    Heap_entry *he;
    Event *e;

    Heap_entry *top;
    Event *now;

    TRACE();

    for (i = 0; i < qmodel->queue_num; ++i)
    {
        event_time = poisson_rand(qmodel->arriv_lambda * (double)qmodel->queue_num) + last_event_time;
        last_event_time = event_time;
        e = event_create(EVENT_NEW_CLIENT, event_time, event_time, i);
        he = heap_entry_create((const void *)&e, sizeof(Event *));
        heap_insert(qmodel->events, he);
    }

    while (cur_time <= limit)
    {
        top = heap_extract_top(qmodel->events);
        now = *(Event **)heap_entry_get_data(top);
        cur_time = now->end;

        if (cur_time > limit)
        {
            heap_entry_destroy_with_data(top, event_destroy_wrapper);
            break;
        }

        switch (now->type)
        {
            case EVENT_NEW_CLIENT:
            {
                /* we need to calculate new event now */
                event_time = poisson_rand(qmodel->arriv_lambda) + cur_time;

                e = event_create(EVENT_NEW_CLIENT, event_time, event_time, now->q);
                he = heap_entry_create((const void *)&e, sizeof(Event *));
                heap_insert(qmodel->events, he);


                /* have not any clients, come on */
                if (!qmodel->in_progress[now->q])
                {
                    qmodel->in_progress[now->q] = true;
                    if (qmodel->stype == SERVICE_CONST_TIME)
                        event_time = cur_time + qmodel->servcie_time;
                    else
                        event_time = cur_time + poisson_rand(qmodel->service_lambda);

                    e = event_create(EVENT_SERVICE_COMPLETE, cur_time, event_time, now->q);
                    he = heap_entry_create((const void *)&e, sizeof(Event *));
                    heap_insert(qmodel->events, he);
                }
                else /* must wait in queue */
                {
                    fifo_enqueue(qmodel->queues[now->q], (const void *)&cur_time);
                }

                break;
            }
            case EVENT_SERVICE_COMPLETE:
            {
                ++completed;
                clients_time_in_queue += now->end - now->start;
                qmodel->in_progress[now->q] = false; /* service completed */

                if (fifo_get_num_entries(qmodel->queues[now->q]) > 0) /* someone is waiting */
                {
                    fifo_dequeue(qmodel->queues[now->q], (void *)&client_time);
                    if (qmodel->stype == SERVICE_CONST_TIME)
                        event_time = cur_time + qmodel->servcie_time;
                    else
                        event_time = cur_time + poisson_rand(qmodel->service_lambda);

                    e = event_create(EVENT_SERVICE_COMPLETE, client_time, event_time, now->q);
                    he = heap_entry_create((const void *)&e, sizeof(Event *));
                    heap_insert(qmodel->events, he);
                    qmodel->in_progress[now->q] = true;
                }
                break;
            }
            default:
            {
                ERROR("Unsupported event type\n", 0.0);
                break;
            }
        }
        heap_entry_destroy_with_data(top, event_destroy_wrapper);
    }

    heap_destroy_with_entries(qmodel->events);
    qmodel->events = heap_create(HEAP_MIN, sizeof(Event *), 2, event_cmp_wrapper, event_destroy_wrapper);

    return clients_time_in_queue / (double)completed;
}