#ifndef HYPER_LOG_LOG_H
#define HYPER_LOG_LOG_H

/*
    Implementation of Hyper Log Log (counting distinct elements from multi-set with log(log(n)) memory)

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL 3.0
*/

#include <stddef.h>
#include <stdint.h>
#include <hash.h>

#define HYPERLOGLOG_HASH_FUNCTION hash_jenkins_one_at_time

/*
    Hyper Log Log

    PARAMS
    @IN get_number - get next number from streaming
    @IN len - len of stream
    @IN b - {4, 5 ... 16} - size of register array is 2^b
    @IN hash_f - hash function

    RETURN
    Estimated number of distinct elements in stream
*/
size_t hyper_loglog(long (*get_number)(void), size_t len, size_t b, uint32_t(*hash_f)(const void *data, size_t len));

#endif