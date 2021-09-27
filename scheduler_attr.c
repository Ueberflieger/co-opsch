#include <string.h>
#include <stdlib.h>

#include "scheduler_attr.h"
#include "scheduler_attr_priv.h"

struct scheduler_attr* scheduler_attr_create()
{
    struct scheduler_attr *attr;

    attr = malloc(sizeof(*attr));
    if (NULL == attr)
    {
        return NULL;
    }
    memset(attr, 0, sizeof(*attr));

    return attr;
}

struct scheduler_attr* scheduler_attr_destroy(struct scheduler_attr *attr)
{
    if (NULL == attr)
    {
        return NULL;
    }

    free(attr);

    return NULL;
}

int scheduler_attr_interval_ms_set(struct scheduler_attr *attr, unsigned int interval)
{
    if (NULL == attr || 0 == interval)
    {
        return -1;
    }

    attr->interval = interval;

    return 0;
}

int scheduler_attr_timer_set(struct scheduler_attr *attr, struct timer *timer)
{
    if (NULL == attr || NULL == timer)
    {
        return -1;
    }

    attr->timer = timer;

    return 0;
}

int scheduler_attr_timing_error_cb_set(struct scheduler_attr *attr, scheduler_timing_error_cb_fn cb, void *data)
{
    if (NULL == attr || NULL == cb)
    {
        return -1;
    }

    attr->timing_error_cb_fn = cb;
    attr->timing_error_cb_data = data;
}
