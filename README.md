# coops

`coops` is a simple platform independent *coop*erative *s*cheduler for small
embedded systems.

## How to use?

The main file is `scheduler.h`.

To run the scheduler, create a scheduler object by using the function
`scheduler_create()` and call the `start()` function.

In order for the scheduler to work, it requires a `timer` object. The `timer`
object has to implement the `timer` interface. The implementation of the
`timer` interface is platform specific.

Example (return value checking omitted):
```c
#include <stdio.h>

#include "timer.h"
#include "posix_timer.h"
#include "scheduler_attr.h"
#include "scheduler.h"

static void task_1(void *data)
{
    printf("task_1 running\n");
}

static void task_2(void *data)
{
    printf("task_2 running\n");
}

int main(void)
{
    struct scheduler *scheduler;
    struct scheduler_attr *attr;
    struct timer *timer;
    
    timer = posix_timer_create();
    
    attr = schedulder_attr_create();
    
    scheduler_attr_interval_ms_set(attr, 50);
    scheduler_attr_timer_set(attr, timer);
    
    scheduler = scheduler_create(attr);
    
    scheduler->task_add(scheduler, task_1, NULL);
    scheduler->task_add(scheduler, task_2, NULL);

    while (1)
    {
        sleep(1)
    }

    scheduler->stop(sch);

    scheduler = scheduler->destroy(scheduler);
}
```

