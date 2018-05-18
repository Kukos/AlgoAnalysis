#include <hyperloglog.h>
#include <common.h>
#include <stdlib.h>
#include <string.h>
#include <log.h>
#include <math.h>

#define HYPER_LOGLOG_CORRECTION_VALUE 0.7
#define HYPER_LOGLOG_GET_INDEX(hash, b) ((hash & ((BIT(b) - 1) << ((sizeof(hash) << 3) - b))) >> ((sizeof(hash) << 3) - b))
#define HYPER_LOGLOG_GET_VALUE(hash, b) (hash & ((BIT((sizeof(hash) << 3) - b)) - 1))

size_t hyper_loglog(long (*get_number)(void), size_t len, size_t b, uint32_t(*hash_f)(const void *data, size_t len))
{
    int8_t *t;
    size_t i;

    long number;
    uint32_t h;
    uint32_t index;
    uint32_t value;

    size_t m = BIT(b);
    double sum;
    size_t estimated_n;

    size_t counter;

    TRACE();

    t = (int8_t *)malloc(sizeof(int8_t) * m);
    if (t == NULL)
        ERROR("Malloc error\n", 0);

    (void)memset(t, 0, sizeof(int8_t) * m);

    /* fill array */
    for (i = 0; i < len; ++i)
    {
        number = get_number();
        h = hash_f((void *)&number, sizeof(long));

        index = (uint32_t)HYPER_LOGLOG_GET_INDEX(h, b);
        value = (uint32_t)HYPER_LOGLOG_GET_VALUE(h, b);

        t[index] = (int8_t)MAX(t[index], (int8_t)((int)leading_0(value) + 1 - (int)b));
    }

    /* calc estimated n */
    sum = 0.0;
    for (i = 0; i < m; ++i)
        sum += pow(2, -t[i]);

    estimated_n = (size_t)(HYPER_LOGLOG_CORRECTION_VALUE * (double)(m * m) * pow(sum, -1));

    /* corrections */
    if (estimated_n <= ((5 * m) >> 1))
    {
        counter = 0;
        for (i = 0; i < m; ++i)
            if (t[i] == 0)
                ++counter;

        if (counter)
            estimated_n = (size_t)((double)m * log((double)(m) / (double)counter));
    }

    if (estimated_n > (double)BIT(32) / 30.0)
        estimated_n = (size_t)((double)BIT(32) * log((double)(1 - ((double)estimated_n / (double)(BIT(32))))));

    FREE(t);
    return estimated_n;
}
