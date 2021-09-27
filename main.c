#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

#include "timer.h"
#include "task.h"
#include "scheduler_attr.h"
#include "scheduler.h"

#include "posix_timer.h"


void task_1(void *data)
{
    int *i;

    i = data;

    printf("task_1, i = %d\n", ++(*i));
}

void task_2(void *data)
{
    int *i;

    i = data;

    printf("task_2, i = %d\n", ++(*i));
}

void task_3(void *data)
{
    int *i;

    i = data;

    if (*i == 10)
    {
        printf("sleep\n");
        sleep(2);
    }

    printf("task_3 = %d\n", ++(*i));
}

void timing_error(void *data)
{
    printf("timing error\n");
}

int main(void)
{
    struct timer *timer;
    struct scheduler *scheduler;
    struct scheduler_attr *attr;

    timer = posix_timer_create();
    if (NULL == timer)
    {
        return -1;
    }

    attr = scheduler_attr_create();
    if (NULL == attr)
    {
        return -1;
    }

    scheduler_attr_interval_ms_set(attr, 1000);
    scheduler_attr_timer_set(attr, timer);
    scheduler_attr_timing_error_cb_set(attr, timing_error, NULL);

    scheduler = scheduler_create(attr);
    attr = scheduler_attr_destroy(attr);
    if (NULL == scheduler)
    {
        return -1;
    }

    int i = 0, j = 100, k = 10;
    scheduler->task_add(scheduler, task_1, &i);
    scheduler->task_add(scheduler, task_2, &j);

    scheduler->start(scheduler);

    while (i < 10){
        sleep(10);
        if (i == 5){
            scheduler->task_add(scheduler, task_3, &k);
        }
        printf("\n");
    }

    scheduler->destroy(scheduler);

    return 0;
}
