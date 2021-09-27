#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "containerof.h"
#include "list.h"

#include "timer.h"
#include "task.h"
#include "scheduler_attr_priv.h"
#include "scheduler.h"

struct scheduler_priv
{
    list_t tasks;
    struct scheduler ops;
    struct interval *interval;
    scheduler_timing_error_cb_fn timing_error_cb_fn;
    void *timing_error_cb_data;
    int executing_tasks;
};

struct scheduler_task
{
    list_t node;
    task_fn fn;
    void *data;
};

static int scheduler_start(struct scheduler *scheduler)
{
    struct scheduler_priv *priv;

    if (NULL == scheduler)
    {
        return -1;
    }

    priv = container_of(scheduler, struct scheduler_priv, ops);

    priv->interval->start(priv->interval);

    return 0;
}

static int scheduler_stop(struct scheduler *scheduler)
{
    struct scheduler_priv *priv;

    if (NULL == scheduler)
    {
        return -1;
    }

    priv = container_of(scheduler, struct scheduler_priv, ops);

    priv->interval->stop(priv->interval);

    return 0;
}

static int scheduler_task_add(struct scheduler *scheduler, task_fn fn, void *data)
{
    struct scheduler_priv *priv;
    struct scheduler_task *task;

    if (NULL == scheduler || NULL == fn)
    {
        return -1;
    }

    priv = container_of(scheduler, struct scheduler_priv, ops);

    task = malloc(sizeof(*task));
    if (NULL == task)
    {
        return -1;
    }
    memset(task, 0, sizeof(*task));
    task->fn = fn;
    task->data = data;

    list_add_tail(&priv->tasks, &task->node);

    return 0;
}

static struct scheduler* scheduler_destroy(struct scheduler *scheduler)
{
    struct scheduler_priv *priv;
    struct scheduler_task *task;

    if (NULL == scheduler)
    {
        return NULL;
    }

    priv = container_of(scheduler, struct scheduler_priv, ops);

    priv->interval->stop(priv->interval);

    while (!list_empty(&priv->tasks))
    {
        task = list_entry(list_head(&priv->tasks), struct scheduler_task, node);
        list_remove(&task->node);
        free(task);
    }

    free(priv);

    return NULL;
}

static void scheduler_timing_error_cb_noop(void *data){
    //noop
}

static void scheduler_interval_expired(void *data)
{
    struct scheduler_priv *priv;
    struct scheduler_task *task;
    list_t *node;

    priv = data;

    if (priv->executing_tasks++)
    {
        priv->timing_error_cb_fn(priv->timing_error_cb_data);
    }
    else
    {
        node = list_head(&priv->tasks);
        while (node != &priv->tasks)
        {
            task = list_entry(node, struct scheduler_task, node);
            task->fn(task->data);
            node = node->next;
        }
    }
    priv->executing_tasks--;
}

struct scheduler* scheduler_create(struct scheduler_attr *attr)
{
    struct scheduler_priv *priv;

    if (NULL == attr || NULL == attr->timer || 0 == attr->interval)
    {
        return NULL;
    }

    priv = malloc(sizeof(*priv));
    if (NULL == priv){
        return NULL;
    }
    memset(priv, 0, sizeof(*priv));

    list_init(&priv->tasks);
    priv->ops.start = scheduler_start;
    priv->ops.stop = scheduler_stop;
    priv->ops.task_add = scheduler_task_add;
    priv->ops.destroy = scheduler_destroy;

    priv->timing_error_cb_fn = scheduler_timing_error_cb_noop;
    if (attr->timing_error_cb_fn)
    {
        priv->timing_error_cb_fn = attr->timing_error_cb_fn;
        priv->timing_error_cb_data = attr->timing_error_cb_data;
    }

    priv->interval = attr->timer->interval_create(attr->timer, attr->interval, scheduler_interval_expired, priv);
    if (NULL == priv->interval)
    {
        free(priv);
    }

    return &priv->ops;
}
