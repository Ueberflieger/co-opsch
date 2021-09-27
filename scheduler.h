#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

struct scheduler_attr;
typedef void (*task_fn)(void *data);
typedef void (*scheduler_timing_error_cb_fn)(void *data);

struct scheduler
{
    int (*start)(struct scheduler *scheduler);
    int (*stop)(struct scheduler *scheduler);
    int (*task_add)(struct scheduler *scheduler, task_fn task, void *data);
    struct scheduler *(*destroy)(struct scheduler *scheduler);
};

struct scheduler* scheduler_create(struct scheduler_attr *attr);

#endif
