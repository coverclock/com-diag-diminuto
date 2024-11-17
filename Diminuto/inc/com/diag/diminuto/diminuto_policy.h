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
 * Defines the possible Diminuto scheduler policies in a type-safe way.
 */
typedef enum DiminutoPolicyScheduler {
    DIMINUTO_POLICY_SCHEDULER_OTHER     = SCHED_OTHER,
    DIMINUTO_POLICY_SCHEDULER_FIFO      = SCHED_FIFO,
    DIMINUTO_POLICY_SCHEDULER_RR        = SCHED_RR,
    DIMINUTO_POLICY_SCHEDULER_BATCH     = SCHED_BATCH,
    DIMINUTO_POLICY_SCHEDULER_ISO       = SCHED_ISO,
    DIMINUTO_POLICY_SCHEDULER_IDLE      = SCHED_IDLE,
    DIMINUTO_POLICY_SCHEDULER_DEADLINE  = SCHED_DEADLINE,
    /*
     * DEFAULTS
     */
    DIMINUTO_POLICY_SCHEDULER_TIMER     = SCHED_RR,
    DIMINUTO_POLICU_SCHEDULER_THREAD    = SCHED_RR,
} diminuto_policy_scheduler_t;

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
    DIMINUTO_POLICY_CLOCK_TIME_CLOCK            = CLOCK_REALTIME,
    DIMINUTO_POLICY_CLOCK_TIME_LOGGING          = CLOCK_REALTIME,
    DIMINUTO_POLICY_CLOCK_TIME_ATOMIC           = CLOCK_TAI,
    DIMINUTO_POLICY_CLOCK_TIME_ELAPSED          = CLOCK_BOOTTIME,
    DIMINUTO_POLICY_CLOCK_TIME_PROCESS          = CLOCK_PROCESS_CPUTIME_ID,
    DIMINUTO_POLICY_CLOCK_TIME_THREAD           = CLOCK_THREAD_CPUTIME_ID,
    DIMINUTO_POLICY_CLOCK_TIMER                 = CLOCK_MONOTONIC,
} diminuto_policy_clock_t;

/**
 * Defines the possible Diminuto priority policies in a type-safe way.
 */
typedef enum DiminutoPolicyPriority {
    /*
     * DEFAULTS
     */
    DIMINUTO_POLICY_PRIORITY_TIMER              = 49,
    DIMINUTO_POLICY_PRIORITY_THREAD             = 49,
} diminuto_policy_priority_t;

#endif
