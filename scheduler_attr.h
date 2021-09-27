#ifndef __SCHEDULER_ATTR_H__
#define __SCHEDULER_ATTR_H__

struct timer;
struct scheduler_attr;
typedef void (*scheduler_timing_error_cb_fn)(void *data);

struct scheduler_attr* scheduler_attr_create();
struct scheduler_attr* scheduler_attr_destroy(struct scheduler_attr *attr);

// this attribute is mandatory
int scheduler_attr_interval_ms_set(struct scheduler_attr *attr, unsigned int interval);

// this attribute is mandatory
int scheduler_attr_timer_set(struct scheduler_attr *attr, struct timer *timer);

// this attribute is optional
// The error cb is called when in one scheduler interval not all tasks could be executed
int scheduler_attr_timing_error_cb_set(struct scheduler_attr *attr, scheduler_timing_error_cb_fn cb, void *data);

#endif
