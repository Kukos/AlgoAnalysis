#include <dconfig.h>
#include <stdlib.h>
#include <common.h>
#include <log.h>

DConfig *dconfig_create(size_t proc)
{
    DConfig *conf;

    TRACE();

    conf = (DConfig *)malloc(sizeof(DConfig) + (sizeof(dconfig_t) * proc));
    if (conf == NULL)
        ERROR("malloc error\n", NULL);

    conf->____nproc = proc;

    return conf;
}

void dconfig_destroy(DConfig *conf)
{
    TRACE();

    if (conf == NULL)
        return;

    FREE(conf);
}