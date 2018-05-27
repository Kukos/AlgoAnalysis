#ifndef QUEUE_MODEL_H
#define QUEUE_MODEL_H

/*
    Discrete time queue model

    Let p1 p2 be the probability factors, p1 + p2 <= 1

    Then when we are in state i
        with PR = p1 we go to (i + 1) state
        with PR = p2 go to (i - 1) state
        with PR = 1 - p1 - p2 stay in i state

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL 3.0
*/

#include <darray.h>
#include <stdint.h>

typedef uint64_t round_t;

typedef struct QState
{
    uint64_t ____counter;
    round_t  ____last_visit;
    uint64_t ____sum_time_come_back;

} QState;


typedef struct Queue_model
{
    int ____p_next;
    int ____p_prev;
    int ____p_stay;
    Darray *____q;

} Queue_model;

/*
    Create new QState

    PARAMS
    NO PARAMS

    RETURN
    NULL iff failure
    Pointer to new Qstate iff success
*/
QState *qstate_create(void);

/*
    Destroy QState

    PARAMS
    @IN qstate - QState

    RETURN
    This is a void function
*/
void qstate_destroy(QState *qstate);

/*
    Visit the Qstate in round rnd

    PARAMS
    @IN qstate - state
    @IN rnd - round

    RETURN
    This is a void function
*/
void qstate_visit(QState *qstate, round_t rnd);

/*
    Create Queue model

    PARAMS
    @IN p1 - probability such that we go to the next state
    @IN p2 - probability such that we go to the prev state

    RETURN
    NULL iff failure
    Pointer to new Model iff success
*/
Queue_model *qmodel_create(double p1, double p2);

/*
    Destroy Queue model

    PARAMS
    @IN qmodel - Queue model

    RETURN
    This is a void function
*/
void qmodel_destroy(Queue_model *qmodel);

/*
    Run Simulatioon on Queue model for @rnd rounds

    PARAMS
    @IN qmodel - Queue model
    @IN rnd - rounds

    RETURN
    This is a void function
*/
void qmodel_run(Queue_model *qmodel, round_t rnd);

/*
    Collect results from Qmodel

    PARAMS
    @IN qmodel - Queue Model
    @IN file - file

    RETURN
    This is a void function
*/
void qmodel_results(const Queue_model *qmodel, const char *file1, const char *file2);

#endif