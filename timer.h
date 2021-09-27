#ifndef __TIMER_H__
#define __TIMER_H__


struct interval
{
    int (*start)(struct interval *interval);
    int (*stop)(struct interval *interval);
};

typedef void (*timer_expired_cb)(void *cb_data);

struct timer
{
    struct interval* (*interval_create)(struct timer *timer, unsigned int interval_ms, timer_expired_cb cb, void *cb_data);
    struct timer *(*destroy)(struct timer *timer);
};

#endif
