/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_TIMER_
#define _H_COM_DIAG_DIMINUTO_TIMER_

/**
 * @file
 * @copyright Copyright 2009-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements monotonic one-shot and periodic timers using POSIX timers.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Timer feature uses the real-time POSIX timer_create(2) and
 * timer_settime(2) facilites to mimic the setitimer(2) semantics, but
 * with a monotonic clock. The resulting one-shot or periodic timer is,
 * like setitimer(2), a singleton.
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_condition.h"
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <sched.h>

/**
 * @def COM_DIAG_DIMINUTO_TIMER_FREQUENCY
 * This manifest constant is the frequency in Hertz at which this feature
 * operates. The inverse of this value is the smallest unit of time in fractions
 * of a second that this feature can express or use. This constant is provided
 * for use in those cases where it is useful to have the value at compile time.
 * However, you should always prefer to use the inline function when possible.
 */
#define COM_DIAG_DIMINUTO_TIMER_FREQUENCY (1000000000LL)

/**
 * Return the resolution of the Diminuto timer units in ticks per second
 * (Hertz). Timer intervals smaller than the equivalent period in ticks will
 * not yield the expected results. In fact, there is no guarantee that the
 * underlying software platform or hardware target can support timer intervals
 * with even this resolution.
 * @return the resolution in ticks per second.
 */
static inline diminuto_sticks_t diminuto_timer_frequency(void)
{
    return COM_DIAG_DIMINUTO_TIMER_FREQUENCY;
}

/**
 * This type defines the prototype of a timer function (effectively an
 * interrupt service routine). It is a little different from the
 * prototype defined for POSIX timer functions.
 */
typedef void * (diminuto_timer_function_t)(void *);

/**
 * A timer can be in one of these states. The feature uses these states
 * along with a mutex and condition to eliminate the race condition
 * between the caller and the timer callback function.
 */
typedef enum DiminutoTimerState {
    DIMINUTO_TIMER_STATE_IDLE       = 'I',
    DIMINUTO_TIMER_STATE_ARM        = 'A',
    DIMINUTO_TIMER_STATE_DISARM     = 'D',
} diminuto_timer_state_t;

/**
 * This type defines the structure of a Diminuto timer object. The timer can
 * be a one-shot or periodic.
 */
typedef struct DiminutoTimer {
    diminuto_timer_function_t * function;   /**< Pointer to timer function. */
    void *                      context;    /**< Function context. */
    void *                      value;      /**< Function value. */
    diminuto_ticks_t            ticks;      /**< Timer interval in ticks. */
    diminuto_condition_t        condition;  /**< Diminuto condition object. */
    struct sigevent             event;      /**< Event: thread or signal? */
    struct sched_param          param;      /**< Scheduling parameters. */
    pthread_attr_t              attributes; /**< POSIX timer attributes. */
    struct itimerspec           current;    /**< Current time interval. */
    struct itimerspec           remaining;  /**< Remaining time interval. */
    timer_t                     timer;      /**< POSIX timer object. */
    diminuto_errno_t            error;      /**< Latest error number */
    diminuto_timer_state_t      state;      /**< Diminuto timer state. */
    bool                        periodic;   /**< Periodic or one-shot? */
} diminuto_timer_t;

/**
 * Initialize a timer object. The behavior is undefined if the timer is
 * active. The timer must either call a function, or send a signal,
 * but not both. The timer is initialized in the disarmed (idle) state.
 * @param tp points to the timer object.
 * @param periodic if true makes the timer periodic, else a one-shot.
 * @param fp points to the timer function or NULL.
 * @param signum is the signal number or 0.
 * @return a pointer to the timer object if successful or NULL otherwise.
 */
extern diminuto_timer_t * diminuto_timer_init_generic(diminuto_timer_t * tp, int periodic, diminuto_timer_function_t * fp, int signum);

/**
 * Initialize a timer object. The behavior is undefined if the timer is
 * active. The timer calls a function. The timer is initialized in the
 * disarmed (idle) state.
 * @param tp points to the timer object.
 * @param periodic if true makes the timer periodic, else a one-shot.
 * @param fp points to the timer function or NULL.
 * @return a pointer to the timer object if successful or NULL otherwise.
 */
static inline diminuto_timer_t * diminuto_timer_init(diminuto_timer_t * tp, int periodic, diminuto_timer_function_t * fp)
{
    return diminuto_timer_init_generic(tp, periodic, fp, 0);
}

/**
 * Initialize a timer oneshot object. The behavior is undefined if the timer
 * is active. The timer calls a function. The timer is initialized in the
 * disarmed (idle) state.
 * @param tp points to the timer object.
 * @param fp points to the timer function or NULL.
 * @return a pointer to the timer object if successful or NULL otherwise.
 */
static inline diminuto_timer_t * diminuto_timer_init_oneshot(diminuto_timer_t * tp, diminuto_timer_function_t * fp)
{
    return diminuto_timer_init(tp, 0, fp);
}

/**
 * Initialize a timer periodic object. The behavior is undefined if the timer
 * is active. The timer calls a function. The timer is initialized in the
 * disarmed (idle) state.
 * @param tp points to the timer object.
 * @param fp points to the timer function or NULL.
 * @return a pointer to the timer object if successful or NULL otherwise.
 */
static inline diminuto_timer_t * diminuto_timer_init_periodic(diminuto_timer_t * tp, diminuto_timer_function_t * fp)
{
    return diminuto_timer_init(tp, !0, fp);
}

/**
 * Release any resources associated with a timer object, The behavior is
 * undefined if the timer is active.
 * @param tp points to the timer object.
 * @return NULL if successful or a pointer to the timer object otherwise.
 */
extern diminuto_timer_t * diminuto_timer_fini(diminuto_timer_t * tp);

/**
 * Start the timer of a timer object. This arms the timer i.e. places
 * it in the active state.
 * @param tp points to the timer object.
 * @param ticks is the period or duration of the timer in Diminuto ticks.
 * @param cp points to a context that is passed to the optional timer function.
 * @return the number of ticks left on the timer or <0 if an error occurred.
 */
extern diminuto_sticks_t diminuto_timer_start(diminuto_timer_t * tp, diminuto_ticks_t ticks, void * cp);

/**
 * Stop the timer of a timer object. This disarms the timer i.e. places
 * it in the inactive state. This function blocks the caller until the
 * timer acknowledges that it is stopped.
 * @param tp points to the timer object.
 * @return the number of ticks left on the timer or <0 if an error occurred.
 */
extern diminuto_sticks_t diminuto_timer_stop(diminuto_timer_t * tp);

/**
 * Return any error code produced during the execution of code in
 * support of the caller-specified callback function (for example:
 * a failure in a condition signal). This error field is cleared when
 * the timer is initialized, and again when it is started. The error
 * field not applicable for timers that are not configured to invoke
 * a callback function.
 * @param tp points to the timer object.
 * @return an error code or zero if none.
 */
static inline int diminuto_timer_error(const diminuto_timer_t * tp)
{
    return tp->error;
}

/*******************************************************************************
 * The API below emulates an older Diminuto implementation that found its
 * way into a lot of code. It used the older setitimer(2) system call. Unlike
 * that call, this uses POSIX timers with a rate monotonic clock. The old
 * system call uses the real-time clock, which could be jittered by, for
 * example, NTP adjustments. Most of the unit tests were originally written
 * using this older API.
 ******************************************************************************/


/**
 * Start a singleton timer for the specified number of ticks. When the
 * timer expires, the calling process will receive a SIGALRM. If a timer is
 * already running, the amount of time left in the prior timer will be returned.
 * The actual timer duration will be approximate depending on the
 * granularity of the system clock and latency in the implementation.
 * Calling with zero ticks cancels any prior timer.
 * @param ticks is the desired timer duration in ticks.
 * @param periodic if true starts a periodic timer, else a one-shot timer.
 * @return the number of ticks remaining in prior timer if a timer was already
 * running, or <0 if an error occurred.
 */
extern diminuto_sticks_t diminuto_timer_setitimer(diminuto_ticks_t ticks, int periodic);

/**
 * Start a singleton one-shot timer for the specified number of ticks. When the
 * timer expires, the calling process will receive a SIGALRM. If a timer is
 * already running, the amount of time left in the prior timer will be returned.
 * The actual timer duration will be approximate depending on the
 * granularity of the system clock and latency in the implementation.
 * The timer fires only once per call. Calling with zero ticks
 * cancels any prior timer.
 * @param ticks is the desired timer duration in ticks.
 * @return the number of ticks remaining in prior timer if a timer was already
 * running, or <0 if an error occurred.
 */
static inline diminuto_sticks_t diminuto_timer_oneshot(diminuto_ticks_t ticks)
{
    return diminuto_timer_setitimer(ticks, 0);
}

/**
 * Start a singleton periodic timer for the specified number of ticks. When
 * the timer expires, the calling process will receive a SIGALRM. If a
 * timer is already running, the amount of time left in the prior timer
 * will be returned. The actual timer period will be approximate depending
 * on the granularity of the system clock and latency in the implementation.
 * The timer fires repeatedly with a periodicity of the specified ticks
 * until it is cancelled. Calling with zero ticks cancels the timer.
 * @param ticks is the desired period interval in ticks.
 * @return the number of ticks remaining in the prior timer if a timer was
 * already running, or -1 if an error occurred.
 */
static inline diminuto_sticks_t diminuto_timer_periodic(diminuto_ticks_t ticks)
{
    return diminuto_timer_setitimer(ticks, !0);
}

#endif
