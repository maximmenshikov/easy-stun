#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "es_node.h"
#include "es_status.h"
#include "stun.h"
#include "es_msg.h"
#include "es_bool.h"
#include "debug.h"
#include "helper.h"
#include <signal.h>
#include <time.h>
#ifdef __APPLE__
#include "mac_time.h"
#endif

#ifdef __APPLE__
#define USED_CLOCK (CLOCK_REALTIME)
#else
#define USED_SIG (SIGRTMIN + 2)
#define USED_CLOCK (CLOCK_MONOTONIC)
#endif

static timer_t timer;
static es_node *loop_node;
static int counter = 0;

static void
timer_callback_rebind(void)
{
    es_bool rebind = ES_FALSE;

    counter++;

    if (es_status_is_conn_broken(es_local_recv(loop_node)))
    {
        dbg("[%s:%u] Connection is broken - rebind",
            loop_node->params.remote_addr,
            (unsigned)loop_node->params.remote_port);
        rebind = ES_TRUE;
    }

    if ((counter % loop_node->params.keepalive_interval) == 0)
    {
        dbg("[%s:%u] Connection needs keepalive - rebind",
            loop_node->params.remote_addr,
            (unsigned)loop_node->params.remote_port);
        rebind = ES_TRUE;
    }

    if (rebind)
    {
        es_twoway_bind(loop_node);
    }
}

#ifdef __APPLE__
void
timer_callback(union sigval signo)
{
    timer_callback_rebind();
}
#else
void
timer_callback(int signo, siginfo_t *info,
               void *context)
{
    if (info->si_code == SI_TIMER)
    {
        timer_callback_rebind();
    }
}
#endif

es_status
es_local_start_recv(es_node *node)
{
    struct itimerspec value;
    sigset_t mask;
    siginfo_t info;
    struct sigevent sigev;
    struct sigaction sa;

    sigemptyset(&mask);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);

    UNUSED(info);

    loop_node = node;
#ifndef __APPLE__
    sa.sa_sigaction = timer_callback;

    if (sigaction(USED_SIG, &sa, NULL) == -1)
    {
        err("sigaction() failed: %s", strerror(errno));
        return ES_EFAIL;
    }
#endif

#ifdef __APPLE__
    sigev.sigev_notify = SIGEV_THREAD;
    sigev.sigev_notify_function = timer_callback;
#else
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = USED_SIG;
#endif
    sigev.sigev_value.sival_int = 1;

    value.it_value.tv_sec = 1;
    value.it_value.tv_nsec = 0;
    value.it_interval.tv_sec = 1;
    value.it_interval.tv_nsec = 0;

    if (timer_create(USED_CLOCK, &sigev, &timer) != 0)
    {
        printf("timer_create() failed: %s\n", strerror(errno));
        return ES_EFAIL;
    }

    timer_settime(timer, 0, &value, NULL);
    return ES_EOK;
}
