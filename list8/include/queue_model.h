#ifndef QUEUE_MODEL_H
#define QUEUE_MODEL_H

/*
    M/M/1 Queue model

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL 3.0
*/

#include <darray.h>
#include <stdint.h>
#include <fifo.h>
#include <heap.h>
#include <stdbool.h>

typedef double round_t;

typedef enum Event_type
{
    EVENT_NEW_CLIENT,
    EVENT_SERVICE_COMPLETE
} event_t;

typedef struct Event
{
    event_t type;
    round_t start;
    round_t end;
    size_t q;
} Event;

typedef enum Service_dist
{
    SERVICE_CONST_TIME,
    SERVICE_POISSON_DIST_TIME
} service_t;

typedef struct Queue_model
{
    service_t stype;

    Fifo **queues;
    size_t queue_num;

    Heap *events;

    double arriv_lambda;
    double service_lambda;
    round_t servcie_time;

    bool *in_progress;

} Queue_model;

/*
    Create new Queue model

    PARAMS
    @IN n - num of queues
    @IN type - service type
    @IN arriv_lambda - poisson dist parameter for clients arrivial
    @IN service_lambda - poisson dist parameter for service time (ignored when type == CONST_TIME)
    @IN s_time - service time (ignored when type == POISSON_DOST_TIME)

    RETURN
    NULL iff failure
    New pointer to queue model iff success
*/
Queue_model *queue_model_create(size_t n, service_t type, double arriv_lambda, double service_lambda, round_t s_time);

/*
    Destroy queue model

    PARAMS
    @IN qmodel - pointer to Queue model

    RETURN
    This is a void function
*/
void queue_model_destroy(Queue_model *qmodel);

/*
    Start queue simulation for @limit rounds

    PARAMS
    @IN qmodel - qmodel
    @IN limit  - time limit in rounds

    RETURN
    Avg time spent in queue by client
*/
double queue_model_run(Queue_model *qmodel, round_t limit);

#endif