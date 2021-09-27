#ifndef __SCHEDULER_ATTR_PRIV_H__
#define __SCHEDULER_ATTR_PRIV_H__

typedef void (*scheduler_timing_error_cb_fn)(void *data);

struct scheduler_attr
{
    unsigned int interval;
    scheduler_timing_error_cb_fn timing_error_cb_fn;
    void *timing_error_cb_data;
    struct timer *timer;
};

#endif
