#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <signal.h>
#include <time.h>

#include "containerof.h"
#include "list.h"
#include "timer.h"
#include "posix_timer.h"

struct posix_timer_priv
{
    list_t timers;
    struct timer timer;
};

struct posix_interval_priv
{
    list_t node;
    struct interval interval;
    timer_expired_cb cb;
    void *cb_data;
    timer_t timerid;
    struct itimerspec its;
};

static int posix_timer_interval_start(struct interval *interval)
{
    struct posix_interval_priv *priv;

    if (NULL == interval){
        return -1;
    }

    priv = container_of(interval, struct posix_interval_priv, interval);

    if (timer_settime(priv->timerid, 0, &priv->its, NULL) == -1){
        printf("timer_settime() failed\n");
        return -1;
    }

    return 0;
}

static int posix_timer_interval_stop(struct interval *interval)
{
    struct itimerspec its;
    struct posix_interval_priv *priv;

    if (NULL == interval){
        return -1;
    }

    priv = container_of(interval, struct posix_interval_priv, interval);

    its.it_value.tv_nsec = 0;
    its.it_value.tv_sec = 0;
    its.it_interval.tv_nsec = 0;
    its.it_interval.tv_sec = 0;


    if (timer_settime(priv->timerid, 0, &its, NULL) == -1){
        printf("timer_settime() failed\n");
        return -1;
    }

    return 0;
}

static void timer_expired_handler(int sig, siginfo_t *si, void *uc)
{
    struct posix_interval_priv *priv;

    priv = ((union sigval)si->si_value).sival_ptr;

    priv->cb(priv->cb_data);
}

static struct interval* posix_timer_interval_create(struct timer *timer, unsigned int interval_ms, timer_expired_cb cb, void *cb_data)
{
    struct posix_interval_priv *priv;
    struct posix_timer_priv *timer_priv;
    struct sigaction sa;
    struct sigevent sev;

    if (NULL == timer || 0 == interval_ms)
    {
        return NULL;
    }

    timer_priv = container_of(timer, struct posix_timer_priv, timer);

    priv = malloc(sizeof(*priv));
    if (NULL == priv)
    {
        return &priv->interval;
    }
    memset(priv, 0, sizeof(*priv));

    priv->cb = cb;
    priv->cb_data = cb_data;
    priv->interval.start = posix_timer_interval_start;
    priv->interval.stop = posix_timer_interval_stop;

    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timer_expired_handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGRTMIN, &sa, NULL) == -1){
        printf("sigaction() failed\n");
    }

    // create the timer
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_value.sival_ptr = &priv->timerid;
    sev.sigev_value = (union sigval)(void*)priv;
    if (timer_create(CLOCK_REALTIME, &sev, &priv->timerid) == -1){
        printf("timer_create() failed\n");
    }

    // setup the interval for later use
    priv->its.it_value.tv_sec = interval_ms / 1000;
    priv->its.it_value.tv_nsec = interval_ms % 1000;
    priv->its.it_interval.tv_sec = priv->its.it_value.tv_sec;
    priv->its.it_interval.tv_nsec = priv->its.it_value.tv_nsec;

    list_add_tail(&timer_priv->timers, &priv->node);

    return &priv->interval;
}

static struct timer* destroy(struct timer *timer)
{
    struct posix_timer_priv *timer_priv;
    struct posix_interval_priv *priv;

    if (NULL == timer){
        return NULL;
    }

    timer_priv = container_of(timer, struct posix_timer_priv, timer);

    while (!list_empty(&timer_priv->timers)){
        priv = list_entry(list_head(&timer_priv->timers), struct posix_interval_priv, node);
        list_remove(&priv->node);
        priv->interval.stop(&priv->interval);
        free(priv);
    }

    free(timer_priv);

    return NULL;
}

struct timer *posix_timer_create(void)
{
    struct posix_timer_priv *priv;

    priv = malloc(sizeof(*priv));
    if (NULL == priv)
    {
        return NULL;
    }
    memset(priv, 0, sizeof(*priv));

    list_init(&priv->timers);

    priv->timer.destroy = destroy;
    priv->timer.interval_create = posix_timer_interval_create;

    return &priv->timer;
}
