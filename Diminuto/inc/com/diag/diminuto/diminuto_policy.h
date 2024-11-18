/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_POLICY
#define _H_COM_DIAG_DIMINUTO_POLICY

/**
 * @file
 * @copyright Copyright 2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Defines constants for various Diminuto default policy choices.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * Defines constants for various Diminuto default policy choices.
 */

#include <sched.h>
#include <time.h>

/**
 * Defines the possible Diminuto clock policies in a type-safe way.
 */
typedef enum DiminutoPolicyClock {
    DIMINUTO_POLICY_CLOCK_REALTIME              = CLOCK_REALTIME,
    DIMINUTO_POLICY_CLOCK_MONOTONIC             = CLOCK_MONOTONIC,
    DIMINUTO_POLICY_CLOCK_PROCESS_CPUTIME_ID    = CLOCK_PROCESS_CPUTIME_ID,
    DIMINUTO_POLICY_CLOCK_THREAD_CPUTIME_ID     = CLOCK_THREAD_CPUTIME_ID,
    DIMINUTO_POLICY_CLOCK_MONOTONIC_RAW         = CLOCK_MONOTONIC_RAW,
    DIMINUTO_POLICY_CLOCK_REALTIME_COARSE       = CLOCK_REALTIME_COARSE,
    DIMINUTO_POLICY_CLOCK_MONOTONIC_COARSE      = CLOCK_MONOTONIC_COARSE,
    DIMINUTO_POLICY_CLOCK_BOOTTIME              = CLOCK_BOOTTIME,
    DIMINUTO_POLICY_CLOCK_REALTIME_ALARM        = CLOCK_REALTIME_ALARM,
    DIMINUTO_POLICY_CLOCK_BOOTTIME_ALARM        = CLOCK_BOOTTIME_ALARM,
    DIMINUTO_POLICY_CLOCK_TAI                   = CLOCK_TAI,
    /*
     * DEFAULTS
     */
    DIMINUTO_POLICY_CLOCK_TIME_CLOCK            = DIMINUTO_POLICY_CLOCK_REALTIME,
    DIMINUTO_POLICY_CLOCK_TIME_LOGGING          = DIMINUTO_POLICY_CLOCK_REALTIME,
    DIMINUTO_POLICY_CLOCK_TIME_ATOMIC           = DIMINUTO_POLICY_CLOCK_TAI,
    DIMINUTO_POLICY_CLOCK_TIME_ELAPSED          = DIMINUTO_POLICY_CLOCK_BOOTTIME,
    DIMINUTO_POLICY_CLOCK_TIME_PROCESS          = DIMINUTO_POLICY_CLOCK_PROCESS_CPUTIME_ID,
    DIMINUTO_POLICY_CLOCK_TIME_THREAD           = DIMINUTO_POLICY_CLOCK_THREAD_CPUTIME_ID,
    DIMINUTO_POLICY_CLOCK_TIMER                 = DIMINUTO_POLICY_CLOCK_MONOTONIC,
} diminuto_policy_clock_t;

/**
 * Defines the possible Diminuto scheduler policies in a type-safe way.
 */
typedef enum DiminutoPolicyScheduler {
    DIMINUTO_POLICY_SCHEDULER_OTHER             = SCHED_OTHER,
    DIMINUTO_POLICY_SCHEDULER_FIFO              = SCHED_FIFO,
    DIMINUTO_POLICY_SCHEDULER_RR                = SCHED_RR,
    DIMINUTO_POLICY_SCHEDULER_BATCH             = SCHED_BATCH,
    DIMINUTO_POLICY_SCHEDULER_ISO               = SCHED_ISO,
    DIMINUTO_POLICY_SCHEDULER_IDLE              = SCHED_IDLE,
    DIMINUTO_POLICY_SCHEDULER_DEADLINE          = SCHED_DEADLINE,
    /*
     * DEFAULTS
     */
    DIMINUTO_POLICY_SCHEDULER_THREAD            = DIMINUTO_POLICY_SCHEDULER_RR,
    DIMINUTO_POLICY_SCHEDULER_TIMER             = DIMINUTO_POLICY_SCHEDULER_FIFO,
} diminuto_policy_scheduler_t;

/**
 * Defines the possible Diminuto priority policies in a type-safe way.
 * The minimum and maximum apply to Round Robin (RR) and First In First
 * Out (FIFO) schedulers. In the code I write, threads exist to do I/O
 * in the background, and hence are I/O bound, but they may have real-time
 * response requirements when, for example, a select(2) fires. Kernel and
 * other critical services typically run in the range of 50..99.
 */
typedef enum DiminutoPolicyPriority {
    DIMINUTO_POLICY_PRIORITY_MINIMUM            = 0,
    DIMINUTO_POLICY_PRIORITY_LOW                = 1,
    DIMINUTO_POLICY_PRIORITY_HIGH               = 49,
    DIMINUTO_POLICY_PRIORITY_MAXIMUM            = 99,
    /*
     * DEFAULTS
     */
    DIMINUTO_POLICY_PRIORITY_THREAD             = 48,
    DIMINUTO_POLICY_PRIORITY_TIMER              = 49,
} diminuto_policy_priority_t;

#endif
