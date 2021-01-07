/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020-2021 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the unit test for the Reader Writer feature.
 *
 * Enabling DEBUG logging is enlightening.
 *
 * > COM_DIAG_DIMINUTO_LOG_MASK=0xff unittest-readerwriter
 *
 * Even more diagnostic output can be emitted to standard error by using
 * the "-d" flag on the command line for this unit test, which causes the
 * unit test to enable more debugging in the feature.
 *
 * > COM_DIAG_DIMINUTO_LOG_MASK=0xff unittest-readerwriter -d
 *
 * Turning off DEBUG and INFO logging but using the "-d" flag
 * also puts on an interesting show.
 *
 * > COM_DIAG_DIMINUTO_LOG_MASK=0xfc unittest-readerwriter -d
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_readerwriter.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_thread.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_ring.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

/******************************************************************************/

static unsigned int randy(unsigned int low, unsigned int high)
{
    unsigned long long result;
    static unsigned int seed;
    static int initialized = 0;

    if (!initialized) {
        seed = (unsigned int)diminuto_time_clock();
        initialized = !0;
    }

    /* rand_r: [0..RAND_MAX=2147483647] */

    return low + (rand_r(&seed) % (high - low + 1));
}

/******************************************************************************/

struct Context {
    int identifier;
    diminuto_readerwriter_t * rwp;
    diminuto_ticks_t latency;
    diminuto_ticks_t timeout;
    diminuto_ticks_t workload;
    int iterations;
};

static int readers = 0;
static int writers = 0;

static void * reader(void * vp)
{
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    struct Context * cp = (struct Context *)vp;
    int ii;
    int success;

    CHECKPOINT("reader[%d] rwp=%p latency=%llu workload=%llu iterations=%d\n", cp->identifier, cp->rwp, (unsigned long long)(cp->latency), (unsigned long long)(cp->workload), cp->iterations);

    for (ii = 0; ii < cp->iterations; ++ii) {
        success = 0;
        diminuto_delay(cp->latency, 0);
        DIMINUTO_READER_BEGIN(cp->rwp);
            DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
                ++readers;
            DIMINUTO_CRITICAL_SECTION_END;
            success = ((readers > 0) && (writers == 0));
            CHECKPOINT("reader[%d] readers=%d writers=%d success=%d\n", cp->identifier, readers, writers, success);
            if (success) {
                diminuto_delay(cp->workload, 0);
            }
            DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
                readers--;
            DIMINUTO_CRITICAL_SECTION_END;
        DIMINUTO_READER_END;
        if (!success) { break; }
    }

    return (void *)(intptr_t)success;
}

static void * writer(void * vp)
{
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    struct Context * cp = (struct Context *)vp;
    int ii;
    int success;

    CHECKPOINT("writer[%d] rwp=%p latency=%llu workload=%llu iterations=%d\n", cp->identifier, cp->rwp, (unsigned long long)(cp->latency), (unsigned long long)(cp->workload), cp->iterations);

    for (ii = 0; ii < cp->iterations; ++ii) {
        success = 0;
        diminuto_delay(cp->latency, 0);
        DIMINUTO_WRITER_BEGIN(cp->rwp);
            DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
                ++writers;
            DIMINUTO_CRITICAL_SECTION_END;
            success = ((readers == 0) && (writers == 1));
            CHECKPOINT("writer[%d] readers=%d writers=%d success=%d\n", cp->identifier, readers, writers, success);
            if (success) {
                diminuto_delay(cp->workload, 0);
            }
            DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
                writers--;
            DIMINUTO_CRITICAL_SECTION_END;
        DIMINUTO_WRITER_END;
        if (!success) { break; }
    }

    return (void *)(intptr_t)success;
}

static void * impatientreader(void * vp)
{
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    struct Context * cp = (struct Context *)vp;
    int ii;
    int success;

    CHECKPOINT("impatientreader[%d] rwp=%p latency=%llu timeout=%llu workload=%llu iterations=%d\n", cp->identifier, cp->rwp, (unsigned long long)(cp->latency), (unsigned long long)(cp->timeout), (unsigned long long)(cp->workload), cp->iterations);

    for (ii = 0; ii < cp->iterations; ++ii) {
        success = 0;
        diminuto_delay(cp->latency, 0);
        if (diminuto_reader_begin_timed(cp->rwp, cp->timeout) == 0) {
            pthread_cleanup_push(diminuto_reader_cleanup, cp->rwp);
            DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
                ++readers;
            DIMINUTO_CRITICAL_SECTION_END;
            success = ((readers > 0) && (writers == 0));
            CHECKPOINT("impatientreader[%d] readers=%d writers=%d success=%d\n", cp->identifier, readers, writers, success);
            if (success) {
                diminuto_delay(cp->workload, 0);
            }
            DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
                readers--;
            DIMINUTO_CRITICAL_SECTION_END;
            pthread_cleanup_pop(!0);
        } else {
            CHECKPOINT("impatientreader[%d] readers=%d writers=%d success=%d\n", cp->identifier, readers, writers, success);
        }
    }

    return (void *)(intptr_t)(!0);
}

static void * impatientwriter(void * vp)
{
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    struct Context * cp = (struct Context *)vp;
    int ii;
    int success;

    CHECKPOINT("impatientwriter[%d] rwp=%p latency=%llu timeout=%llu workload=%llu iterations=%d\n", cp->identifier, cp->rwp, (unsigned long long)(cp->latency), (unsigned long long)(cp->timeout), (unsigned long long)(cp->workload), cp->iterations);

    for (ii = 0; ii < cp->iterations; ++ii) {
        success = 0;
        diminuto_delay(cp->latency, 0);
        if (diminuto_writer_begin_timed(cp->rwp, cp->timeout) == 0) {
            pthread_cleanup_push(diminuto_writer_cleanup, cp->rwp);
            DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
                ++writers;
            DIMINUTO_CRITICAL_SECTION_END;
            success = ((readers == 0) && (writers == 1));
            CHECKPOINT("impatientwriter[%d] readers=%d writers=%d success=%d\n", cp->identifier, readers, writers, success);
            if (success) {
                diminuto_delay(cp->workload, 0);
            }
            DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
                writers--;
            DIMINUTO_CRITICAL_SECTION_END;
            pthread_cleanup_pop(!0);
        } else {
            CHECKPOINT("impatientwriter[%d] readers=%d writers=%d success=%d\n", cp->identifier, readers, writers, success);
        }
    }

    return (void *)(intptr_t)(!0);
}

/******************************************************************************/

int main(int argc, char * argv[])
{
    int debug;
    diminuto_ticks_t frequency;

    debug = (argc > 1) && (strcmp(argv[1], "-d") == 0);

    frequency = diminuto_frequency();

    SETLOGMASK();

    {
        TEST();

        ASSERT(DIMINUTO_READERWRITER_ERROR == EIO);
        ASSERT(DIMINUTO_READERWRITER_FULL == ENOSPC);
        ASSERT(DIMINUTO_READERWRITER_UNEXPECTED == EFAULT);
        ASSERT(DIMINUTO_READERWRITER_TIMEDOUT == ETIMEDOUT);
        ASSERT(DIMINUTO_READERWRITER_INTERRUPTED == EINTR);

        STATUS();
    }

    {
        diminuto_ticks_t ticks;
        diminuto_sticks_t sticks;

        TEST();

        ASSERT(DIMINUTO_READERWRITER_POLL == 0);
        memset(&ticks, 0xff, sizeof(ticks));
        ASSERT(DIMINUTO_READERWRITER_INFINITY > 0);
        ASSERT(DIMINUTO_READERWRITER_INFINITY == ticks);
        sticks = DIMINUTO_READERWRITER_INFINITY;
        ASSERT(sticks == -1);

        STATUS();
    }

    {
        diminuto_readerwriter_t rw = DIMINUTO_READERWRITER_INITIALIZER((diminuto_readerwriter_state_t *)0, 0);

        TEST();

        ASSERT(diminuto_readerwriter_debug(&rw, (FILE *)0) == (FILE *)0);
        ASSERT(diminuto_readerwriter_debug(&rw, stderr) == (FILE *)0);
        ASSERT(diminuto_readerwriter_debug(&rw, stdout) == stderr);
        ASSERT(diminuto_readerwriter_debug(&rw, (FILE *)0) == stdout);
        ASSERT(diminuto_readerwriter_debug(&rw, (FILE *)0) == (FILE *)0);

        STATUS();
    }

    {
        diminuto_readerwriter_state_t state[100];
        diminuto_readerwriter_t rw = DIMINUTO_READERWRITER_INITIALIZER(state, diminuto_countof(state));

        TEST();

        ASSERT(rw.state == state);
        ASSERT(rw.ring.capacity == diminuto_countof(state));
        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.ring.producer == 0);
        ASSERT(rw.ring.consumer == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);

        STATUS();
    }

    {
        diminuto_readerwriter_state_t state[200];
        diminuto_readerwriter_t rw;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw, state, diminuto_countof(state)) == &rw);

        ASSERT(rw.state == state);
        ASSERT(rw.ring.capacity == diminuto_countof(state));
        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.ring.producer == 0);
        ASSERT(rw.ring.consumer == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }

    {
        diminuto_readerwriter_state_t state[64];
        diminuto_readerwriter_t rw;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw, state, diminuto_countof(state)) == &rw);

        if (debug) { diminuto_readerwriter_debug(&rw, stderr); }

        ASSERT(rw.state == state);
        ASSERT(rw.ring.capacity == diminuto_countof(state));
        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.ring.producer == 0);
        ASSERT(rw.ring.consumer == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);

        { ASSERT(diminuto_reader_begin(&rw) == 0); }
            ASSERT(rw.ring.measure == 0);
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            { ASSERT(diminuto_reader_begin(&rw) == 0); }
                ASSERT(rw.ring.measure == 0);
                ASSERT(rw.reading == 2);
                ASSERT(rw.writing == 0);
            { ASSERT(diminuto_reader_end(&rw) == 0); }
            ASSERT(rw.ring.measure == 0);
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
        { ASSERT(diminuto_reader_end(&rw) == 0); }

        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }

    {
        diminuto_readerwriter_state_t state[64];
        diminuto_readerwriter_t rw;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw, state, diminuto_countof(state)) == &rw);

        if (debug) { diminuto_readerwriter_debug(&rw, stderr); }

        ASSERT(rw.state == state);
        ASSERT(rw.ring.capacity == diminuto_countof(state));
        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.ring.producer == 0);
        ASSERT(rw.ring.consumer == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);

        { ASSERT(diminuto_reader_begin_timed(&rw, frequency) == 0); }
            ASSERT(rw.ring.measure == 0);
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            { ASSERT(diminuto_reader_begin_timed(&rw, frequency) == 0); }
                ASSERT(rw.ring.measure == 0);
                ASSERT(rw.reading == 2);
                ASSERT(rw.writing == 0);
            { ASSERT(diminuto_reader_end(&rw) == 0); }
            ASSERT(rw.ring.measure == 0);
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            errno = 0;
            { ASSERT(diminuto_writer_begin_timed(&rw, frequency) < 0); }
            ASSERT(errno == ETIMEDOUT);
            ASSERT(rw.ring.measure == 1);
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            { ASSERT(diminuto_writer_begin_timed(&rw, frequency) < 0); }
            ASSERT(errno == ETIMEDOUT);
            ASSERT(rw.ring.measure == 1);
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            { ASSERT(diminuto_reader_begin_timed(&rw, frequency) == 0); }
                ASSERT(rw.ring.measure == 0);
                ASSERT(rw.reading == 2);
                ASSERT(rw.writing == 0);
            { ASSERT(diminuto_reader_end(&rw) == 0); }
            ASSERT(rw.ring.measure == 0);
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            { ASSERT(diminuto_writer_begin_timed(&rw, frequency) < 0); }
            ASSERT(errno == ETIMEDOUT);
            ASSERT(rw.ring.measure == 1);
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            { ASSERT(diminuto_reader_begin_timed(&rw, frequency) == 0); }
                ASSERT(rw.ring.measure == 0);
                ASSERT(rw.reading == 2);
                ASSERT(rw.writing == 0);
            { ASSERT(diminuto_reader_end(&rw) == 0); }
            ASSERT(rw.ring.measure == 0);
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
        { ASSERT(diminuto_reader_end(&rw) == 0); }

        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }

    {
        diminuto_readerwriter_state_t state[64];
        diminuto_readerwriter_t rw;
        int success = 0;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw, state, diminuto_countof(state)) == &rw);

        if (debug) { diminuto_readerwriter_debug(&rw, stderr); }

        ASSERT(rw.state == state);
        ASSERT(rw.ring.capacity == diminuto_countof(state));
        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.ring.producer == 0);
        ASSERT(rw.ring.consumer == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);

        { ASSERT(diminuto_writer_begin(&rw) == 0); }
            ASSERT(rw.ring.measure == 0);
            ASSERT(rw.reading == 0);
            ASSERT(rw.writing == 1);
            success = !0;
        { ASSERT(diminuto_writer_end(&rw) == 0); }

        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(success == !0);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }

    {
        diminuto_readerwriter_state_t state[64];
        diminuto_readerwriter_t rw;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw, state, diminuto_countof(state)) == &rw);

        if (debug) { diminuto_readerwriter_debug(&rw, stderr); }

        ASSERT(rw.state == state);
        ASSERT(rw.ring.capacity == diminuto_countof(state));
        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.ring.producer == 0);
        ASSERT(rw.ring.consumer == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);

        { ASSERT(diminuto_writer_begin_timed(&rw, frequency) == 0); }
            ASSERT(rw.ring.measure == 0);
            ASSERT(rw.reading == 0);
            ASSERT(rw.writing == 1);
            errno = 0;
            { ASSERT(diminuto_reader_begin_timed(&rw, 0) < 0); }
            ASSERT(errno == ETIMEDOUT);
            ASSERT(rw.ring.measure == 1);
            ASSERT(rw.reading == 0);
            ASSERT(rw.writing == 1);
            errno = 0;
            { ASSERT(diminuto_writer_begin_timed(&rw, 0) < 0); }
            ASSERT(errno == ETIMEDOUT);
            ASSERT(rw.ring.measure == 1);
            ASSERT(rw.reading == 0);
            ASSERT(rw.writing == 1);
            errno = 0;
            { ASSERT(diminuto_reader_begin_timed(&rw, frequency) < 0); }
            ASSERT(errno == ETIMEDOUT);
            ASSERT(rw.ring.measure == 1);
            ASSERT(rw.reading == 0);
            ASSERT(rw.writing == 1);
            errno = 0;
            { ASSERT(diminuto_writer_begin_timed(&rw, frequency) < 0); }
            ASSERT(errno == ETIMEDOUT);
            ASSERT(rw.ring.measure == 1);
            ASSERT(rw.reading == 0);
            ASSERT(rw.writing == 1);
        { ASSERT(diminuto_writer_end(&rw) == 0); }

        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }

    {
        diminuto_readerwriter_state_t state[64];
        diminuto_readerwriter_t rw;
        int success = 0;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw, state, diminuto_countof(state)) == &rw);

        if (debug) { diminuto_readerwriter_debug(&rw, stderr); }

        ASSERT(rw.state == state);
        ASSERT(rw.ring.capacity == diminuto_countof(state));
        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.ring.producer == 0);
        ASSERT(rw.ring.consumer == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);

        DIMINUTO_READER_BEGIN(&rw);
            ASSERT(rw.ring.measure == 0);
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            success = !0;
        DIMINUTO_READER_END;

        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(success == !0);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }

    {
        diminuto_readerwriter_state_t state[64];
        diminuto_readerwriter_t rw;
        int success = 0;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw, state, diminuto_countof(state)) == &rw);

        if (debug) { diminuto_readerwriter_debug(&rw, stderr); }

        ASSERT(rw.state == state);
        ASSERT(rw.ring.capacity == diminuto_countof(state));
        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.ring.producer == 0);
        ASSERT(rw.ring.consumer == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);

        DIMINUTO_WRITER_BEGIN(&rw);
            ASSERT(rw.ring.measure == 0);
            ASSERT(rw.reading == 0);
            ASSERT(rw.writing == 1);
            success = !0;
        DIMINUTO_WRITER_END;

        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(success == !0);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }

    {
        diminuto_readerwriter_state_t state[64];
        diminuto_readerwriter_t rw;
        int success;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw, state, diminuto_countof(state)) == &rw);

        if (debug) { diminuto_readerwriter_debug(&rw, stderr); }

        ASSERT(rw.state == state);
        ASSERT(rw.ring.capacity == diminuto_countof(state));
        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.ring.producer == 0);
        ASSERT(rw.ring.consumer == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);

        success = 0;
        DIMINUTO_READER_BEGIN(&rw);
            ASSERT(rw.ring.measure == 0);
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            success = !0;
        DIMINUTO_READER_END;

        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(success == !0);

        success = 0;
        DIMINUTO_READER_BEGIN(&rw);
            ASSERT(rw.ring.measure == 0);
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            success = !0;
        DIMINUTO_READER_END;

        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(success == !0);

        success = 0;
        DIMINUTO_READER_BEGIN(&rw);
            ASSERT(rw.ring.measure == 0);
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            success = !0;
        DIMINUTO_READER_END;

        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(success == !0);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }

    {
        diminuto_readerwriter_state_t state[64];
        diminuto_readerwriter_t rw;
        int success;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw, state, diminuto_countof(state)) == &rw);

        if (debug) { diminuto_readerwriter_debug(&rw, stderr); }

        ASSERT(rw.state == state);
        ASSERT(rw.ring.capacity == diminuto_countof(state));
        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.ring.producer == 0);
        ASSERT(rw.ring.consumer == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);

        success = 0;
        DIMINUTO_WRITER_BEGIN(&rw);
            ASSERT(rw.ring.measure == 0);
            ASSERT(rw.reading == 0);
            ASSERT(rw.writing == 1);
            success = !0;
        DIMINUTO_WRITER_END;

        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(success == !0);

        success = 0;
        DIMINUTO_WRITER_BEGIN(&rw);
            ASSERT(rw.ring.measure == 0);
            ASSERT(rw.reading == 0);
            ASSERT(rw.writing == 1);
            success = !0;
        DIMINUTO_WRITER_END;

        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(success == !0);

        success = 0;
        DIMINUTO_WRITER_BEGIN(&rw);
            ASSERT(rw.ring.measure == 0);
            ASSERT(rw.reading == 0);
            ASSERT(rw.writing == 1);
            success = !0;
        DIMINUTO_WRITER_END;

        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(success == !0);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }

    {
        diminuto_readerwriter_state_t state[64];
        diminuto_readerwriter_t rw;
        int success;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw, state, diminuto_countof(state)) == &rw);

        if (debug) { diminuto_readerwriter_debug(&rw, stderr); }

        ASSERT(rw.state == state);
        ASSERT(rw.ring.capacity == diminuto_countof(state));
        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.ring.producer == 0);
        ASSERT(rw.ring.consumer == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);

        success = 0;
        DIMINUTO_READER_BEGIN(&rw);
            ASSERT(rw.ring.measure == 0);
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            success = !0;
        DIMINUTO_READER_END;

        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(success == !0);

        success = 0;
        DIMINUTO_WRITER_BEGIN(&rw);
            ASSERT(rw.ring.measure == 0);
            ASSERT(rw.reading == 0);
            ASSERT(rw.writing == 1);
            success = !0;
        DIMINUTO_WRITER_END;

        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(success == !0);

        success = 0;
        DIMINUTO_READER_BEGIN(&rw);
            ASSERT(rw.ring.measure == 0);
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            success = !0;
            DIMINUTO_READER_BEGIN(&rw);
                ASSERT(rw.ring.measure == 0);
                ASSERT(rw.reading == 2);
                ASSERT(rw.writing == 0);
                success = success && !0;
                DIMINUTO_READER_BEGIN(&rw);
                    ASSERT(rw.ring.measure == 0);
                    ASSERT(rw.reading == 3);
                    ASSERT(rw.writing == 0);
                    success = success && !0;
            DIMINUTO_READER_END;
            DIMINUTO_READER_END;
            ASSERT(rw.ring.measure == 0);
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            success = success && !0;
        DIMINUTO_READER_END;

        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(success == !0);

        success = 0;
        DIMINUTO_WRITER_BEGIN(&rw);
            ASSERT(rw.ring.measure == 0);
            ASSERT(rw.reading == 0);
            ASSERT(rw.writing == 1);
            success = !0;
        DIMINUTO_WRITER_END;

        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(success == !0);

        success = 0;
        DIMINUTO_READER_BEGIN(&rw);
            ASSERT(rw.ring.measure == 0);
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            success = !0;
        DIMINUTO_READER_END;

        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(success == !0);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }

#if !0
    {
        diminuto_thread_t readers[3];
        struct Context reading[diminuto_countof(readers)];
        void * result[diminuto_countof(readers)];
        diminuto_readerwriter_state_t state[diminuto_countof(readers)];
        diminuto_readerwriter_t rw;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw, state, diminuto_countof(state)) == &rw);

        if (debug) { diminuto_readerwriter_debug(&rw, stderr); }

        reading[0].identifier = 0;
        reading[0].rwp = &rw;
        reading[0].latency = frequency * 0;
        reading[0].timeout = DIMINUTO_READERWRITER_INFINITY;
        reading[0].workload = frequency * 3;
        reading[0].iterations = 11;

        reading[1].identifier = 1;
        reading[1].rwp = &rw;
        reading[1].latency = frequency * 5;
        reading[1].timeout = DIMINUTO_READERWRITER_INFINITY;
        reading[1].workload = frequency * 7;
        reading[1].iterations = 11;

        reading[2].identifier = 2;
        reading[2].rwp = &rw;
        reading[2].latency = frequency * 1;
        reading[2].timeout = DIMINUTO_READERWRITER_INFINITY;
        reading[2].workload = frequency * 2;
        reading[2].iterations = 11;

        result[0] = (void *)0;
        result[1] = (void *)0;
        result[2] = (void *)0;

        ASSERT(diminuto_thread_init(&readers[0], reader) == &readers[0]);
        ASSERT(diminuto_thread_init(&readers[1], reader) == &readers[1]);
        ASSERT(diminuto_thread_init(&readers[2], reader) == &readers[2]);

        ASSERT(diminuto_thread_start(&readers[0], &reading[0]) == 0);
        ASSERT(diminuto_thread_start(&readers[1], &reading[1]) == 0);
        ASSERT(diminuto_thread_start(&readers[2], &reading[2]) == 0);

        ASSERT(diminuto_thread_join(&readers[0], &result[0]) == 0);
        ASSERT(diminuto_thread_join(&readers[1], &result[1]) == 0);
        ASSERT(diminuto_thread_join(&readers[2], &result[2]) == 0);

        CHECKPOINT("result[0]=%d\n", (int)(intptr_t)result[0]);
        CHECKPOINT("result[1]=%d\n", (int)(intptr_t)result[1]);
        CHECKPOINT("result[2]=%d\n", (int)(intptr_t)result[2]);

        ASSERT(((intptr_t)result[0]) != 0);
        ASSERT(((intptr_t)result[1]) != 0);
        ASSERT(((intptr_t)result[2]) != 0);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }
#endif

#if !0
    {
        diminuto_thread_t writers[3];
        struct Context writing[diminuto_countof(writers)];
        void * result[diminuto_countof(writers)];
        diminuto_readerwriter_state_t state[diminuto_countof(writers)];
        diminuto_readerwriter_t rw;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw, state, diminuto_countof(state)) == &rw);

        if (debug) { diminuto_readerwriter_debug(&rw, stderr); }

        writing[0].identifier = 0;
        writing[0].rwp = &rw;
        writing[0].latency = frequency * 0;
        writing[0].timeout = DIMINUTO_READERWRITER_INFINITY;
        writing[0].workload = frequency * 3;
        writing[0].iterations = 11;

        writing[1].identifier = 1;
        writing[1].rwp = &rw;
        writing[1].latency = frequency * 5;
        writing[1].timeout = DIMINUTO_READERWRITER_INFINITY;
        writing[1].workload = frequency * 7;
        writing[1].iterations = 11;

        writing[2].identifier = 2;
        writing[2].rwp = &rw;
        writing[2].latency = frequency * 1;
        writing[2].timeout = DIMINUTO_READERWRITER_INFINITY;
        writing[2].workload = frequency * 2;
        writing[2].iterations = 11;

        result[0] = (void *)0;
        result[1] = (void *)0;
        result[2] = (void *)0;

        ASSERT(diminuto_thread_init(&writers[0], writer) == &writers[0]);
        ASSERT(diminuto_thread_init(&writers[1], writer) == &writers[1]);
        ASSERT(diminuto_thread_init(&writers[2], writer) == &writers[2]);

        ASSERT(diminuto_thread_start(&writers[0], &writing[0]) == 0);
        ASSERT(diminuto_thread_start(&writers[1], &writing[1]) == 0);
        ASSERT(diminuto_thread_start(&writers[2], &writing[2]) == 0);

        ASSERT(diminuto_thread_join(&writers[0], &result[0]) == 0);
        ASSERT(diminuto_thread_join(&writers[1], &result[1]) == 0);
        ASSERT(diminuto_thread_join(&writers[2], &result[2]) == 0);

        CHECKPOINT("result[0]=%d\n", (int)(intptr_t)result[0]);
        CHECKPOINT("result[1]=%d\n", (int)(intptr_t)result[1]);
        CHECKPOINT("result[2]=%d\n", (int)(intptr_t)result[2]);

        ASSERT(((intptr_t)result[0]) != 0);
        ASSERT(((intptr_t)result[1]) != 0);
        ASSERT(((intptr_t)result[2]) != 0);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }
#endif

#if !0
    {
        diminuto_thread_t readers[3];
        diminuto_thread_t writers[3];
        struct Context reading[diminuto_countof(readers)];
        struct Context writing[diminuto_countof(writers)];
        void * reads[countof(readers)];
        void * writes[countof(writers)];
        diminuto_readerwriter_state_t state[diminuto_countof(readers) + diminuto_countof(writers)];
        diminuto_readerwriter_t rw;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw, state, diminuto_countof(state)) == &rw);

        if (debug) { diminuto_readerwriter_debug(&rw, stderr); }

        reading[0].identifier = 0;
        reading[0].rwp = &rw;
        reading[0].latency = frequency * 0;
        reading[0].timeout = DIMINUTO_READERWRITER_INFINITY;
        reading[0].workload = frequency * 3;
        reading[0].iterations = 11;

        reading[1].identifier = 1;
        reading[1].rwp = &rw;
        reading[1].latency = frequency * 5;
        reading[1].timeout = DIMINUTO_READERWRITER_INFINITY;
        reading[1].workload = frequency * 7;
        reading[1].iterations = 11;

        reading[2].identifier = 2;
        reading[2].rwp = &rw;
        reading[2].latency = frequency * 1;
        reading[2].timeout = DIMINUTO_READERWRITER_INFINITY;
        reading[2].workload = frequency * 2;
        reading[2].iterations = 11;

        reads[0] = (void *)0;
        reads[1] = (void *)0;
        reads[2] = (void *)0;

        writing[0].identifier = 3;
        writing[0].rwp = &rw;
        writing[0].latency = frequency * 0;
        writing[0].timeout = DIMINUTO_READERWRITER_INFINITY;
        writing[0].workload = frequency * 3;
        writing[0].iterations = 11;

        writing[1].identifier = 4;
        writing[1].rwp = &rw;
        writing[1].latency = frequency * 5;
        writing[1].timeout = DIMINUTO_READERWRITER_INFINITY;
        writing[1].workload = frequency * 7;
        writing[1].iterations = 11;

        writing[2].identifier = 5;
        writing[2].rwp = &rw;
        writing[2].latency = frequency * 1;
        writing[2].timeout = DIMINUTO_READERWRITER_INFINITY;
        writing[2].workload = frequency * 2;
        writing[2].iterations = 11;

        writes[0] = (void *)0;
        writes[1] = (void *)0;
        writes[2] = (void *)0;

        ASSERT(diminuto_thread_init(&readers[0], reader) == &readers[0]);
        ASSERT(diminuto_thread_init(&readers[1], reader) == &readers[1]);
        ASSERT(diminuto_thread_init(&readers[2], reader) == &readers[2]);

        ASSERT(diminuto_thread_init(&writers[0], writer) == &writers[0]);
        ASSERT(diminuto_thread_init(&writers[1], writer) == &writers[1]);
        ASSERT(diminuto_thread_init(&writers[2], writer) == &writers[2]);

        ASSERT(diminuto_thread_start(&readers[0], &reading[0]) == 0);
        ASSERT(diminuto_thread_start(&readers[1], &reading[1]) == 0);
        ASSERT(diminuto_thread_start(&readers[2], &reading[2]) == 0);

        ASSERT(diminuto_thread_start(&writers[0], &writing[0]) == 0);
        ASSERT(diminuto_thread_start(&writers[1], &writing[1]) == 0);
        ASSERT(diminuto_thread_start(&writers[2], &writing[2]) == 0);

        ASSERT(diminuto_thread_join(&readers[0], &reads[0]) == 0);
        ASSERT(diminuto_thread_join(&readers[1], &reads[1]) == 0);
        ASSERT(diminuto_thread_join(&readers[2], &reads[2]) == 0);

        ASSERT(diminuto_thread_join(&writers[0], &writes[0]) == 0);
        ASSERT(diminuto_thread_join(&writers[1], &writes[1]) == 0);
        ASSERT(diminuto_thread_join(&writers[2], &writes[2]) == 0);

        CHECKPOINT("reads[0]=%d\n", (int)(intptr_t)reads[0]);
        CHECKPOINT("reads[1]=%d\n", (int)(intptr_t)reads[1]);
        CHECKPOINT("reads[2]=%d\n", (int)(intptr_t)reads[2]);

        CHECKPOINT("writes[0]=%d\n", (int)(intptr_t)writes[0]);
        CHECKPOINT("writes[1]=%d\n", (int)(intptr_t)writes[1]);
        CHECKPOINT("writes[2]=%d\n", (int)(intptr_t)writes[2]);

        ASSERT(((intptr_t)reads[0]) != 0);
        ASSERT(((intptr_t)reads[1]) != 0);
        ASSERT(((intptr_t)reads[2]) != 0);

        ASSERT(((intptr_t)writes[0]) != 0);
        ASSERT(((intptr_t)writes[1]) != 0);
        ASSERT(((intptr_t)writes[2]) != 0);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }
#endif

#if !0
    {
        diminuto_thread_t readers[64];
        diminuto_thread_t writers[8];
        diminuto_thread_t impatientreaders[4];
        diminuto_thread_t impatientwriters[2];
        struct Context reading[diminuto_countof(readers)];
        struct Context writing[diminuto_countof(writers)];
        struct Context impatientreading[diminuto_countof(impatientreaders)];
        struct Context impatientwriting[diminuto_countof(impatientwriters)];
        void * reads[diminuto_countof(readers)];
        void * writes[diminuto_countof(writers)];
        void * impatientreads[diminuto_countof(impatientreaders)];
        void * impatientwrites[diminuto_countof(impatientwriters)];
        static const int MAXIMUM = 64;
        static const int TOTAL = 64 + 8 + 4 + 2;
        static const int EXTRA = 100;
        /*
         * When allowing for failures that place IGNORE tokens in the
         * state ring, it can be difficult to calcualte how big the
         * ring needs to be. It all depends on how many times the reader
         * or writer can retry before its IGNORE tokens are removed from
         * the head of the ring by it or another reader or writer. The
         * number below is just a SWAG based on my own testing. Because
         * the parameters for each reader and writer below are deliberately
         * random, this number might be too low. You'll know it's too low
         * when you start seeing NOSPC ("No space left on device") errors.
         */
        diminuto_readerwriter_state_t state[TOTAL + EXTRA];
        diminuto_readerwriter_t rw;
        int ii;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw, state, diminuto_countof(state)) == &rw);

        if (debug) { diminuto_readerwriter_debug(&rw, stderr); }

        for (ii = 0; ii < diminuto_countof(readers); ++ii) {
            reading[ii].identifier = ii;
            reading[ii].rwp = &rw;
            reading[ii].latency = frequency * randy(0, 3);
            reading[ii].timeout = DIMINUTO_READERWRITER_INFINITY;
            reading[ii].workload = frequency * randy(1, 5);
            reading[ii].iterations = randy(2, 13);
            reads[ii] = (void *)0;
            ASSERT(diminuto_thread_init(&readers[ii], reader) == &readers[ii]);
        }

        for (ii = 0; ii < diminuto_countof(writers); ++ii) {
            writing[ii].identifier = ii;
            writing[ii].rwp = &rw;
            writing[ii].latency = frequency * randy(0, 3);
            writing[ii].timeout = DIMINUTO_READERWRITER_INFINITY;
            writing[ii].workload = frequency * randy(1, 5);
            writing[ii].iterations = randy(2, 13);
            writes[ii] = (void *)0;
            ASSERT(diminuto_thread_init(&writers[ii], writer) == &writers[ii]);
        }

        for (ii = 0; ii < diminuto_countof(impatientreaders); ++ii) {
            impatientreading[ii].identifier = ii;
            impatientreading[ii].rwp = &rw;
            impatientreading[ii].latency = frequency * randy(0, 3);
            impatientreading[ii].timeout = (ii == 0) ? 0 : frequency * randy(1, 7);;
            impatientreading[ii].workload = frequency * randy(1, 5);
            impatientreading[ii].iterations = randy(2, 13);
            impatientreads[ii] = (void *)0;
            ASSERT(diminuto_thread_init(&impatientreaders[ii], impatientreader) == &impatientreaders[ii]);
        }

        for (ii = 0; ii < diminuto_countof(impatientwriters); ++ii) {
            impatientwriting[ii].identifier = ii;
            impatientwriting[ii].rwp = &rw;
            impatientwriting[ii].latency = frequency * randy(0, 3);
            impatientwriting[ii].timeout = (ii == 0) ? 0 : frequency * randy(1, 7);;
            impatientwriting[ii].workload = frequency * randy(1, 5);
            impatientwriting[ii].iterations = randy(2, 13);
            impatientwrites[ii] = (void *)0;
            ASSERT(diminuto_thread_init(&impatientwriters[ii], impatientwriter) == &impatientwriters[ii]);
        }

        for (ii = 0; ii < MAXIMUM; ++ii) {
            if (ii < diminuto_countof(readers)) {
                ASSERT(diminuto_thread_start(&readers[ii], &reading[ii]) == 0);
            }
            if (ii < diminuto_countof(writers)) {
                ASSERT(diminuto_thread_start(&writers[ii], &writing[ii]) == 0);
            }
            if (ii < diminuto_countof(impatientreaders)) {
                ASSERT(diminuto_thread_start(&impatientreaders[ii], &impatientreading[ii]) == 0);
            }
            if (ii < diminuto_countof(impatientwriters)) {
                ASSERT(diminuto_thread_start(&impatientwriters[ii], &impatientwriting[ii]) == 0);
            }
        }

        for (ii = 0; ii < MAXIMUM; ++ii) {
            if (ii < diminuto_countof(readers)) {
                ASSERT(diminuto_thread_join(&readers[ii], &reads[ii]) == 0);
                CHECKPOINT("reads[%d]=%d\n", ii, (int)(intptr_t)reads[ii]);
            }
            if (ii < diminuto_countof(writers)) {
                ASSERT(diminuto_thread_join(&writers[ii], &writes[ii]) == 0);
                CHECKPOINT("writes[%d]=%d\n", ii, (int)(intptr_t)writes[ii]);
            }
            if (ii < diminuto_countof(impatientreaders)) {
                ASSERT(diminuto_thread_join(&impatientreaders[ii], &impatientreads[ii]) == 0);
                CHECKPOINT("impatientreads[%d]=%d\n", ii, (int)(intptr_t)impatientreads[ii]);
            }
            if (ii < diminuto_countof(impatientwriters)) {
                ASSERT(diminuto_thread_join(&impatientwriters[ii], &impatientwrites[ii]) == 0);
                CHECKPOINT("impatientwrites[%d]=%d\n", ii, (int)(intptr_t)impatientwrites[ii]);
            }
        }

        for (ii = 0; ii < MAXIMUM; ++ii) {
            if (ii < diminuto_countof(readers)) {
                ASSERT(((intptr_t)reads[ii]) != 0);
            }
            if (ii < diminuto_countof(writers)) {
                ASSERT(((intptr_t)writes[ii]) != 0);
            }
            if (ii < diminuto_countof(impatientreaders)) {
                ASSERT(((intptr_t)impatientreads[ii]) != 0);
            }
            if (ii < diminuto_countof(impatientwriters)) {
                ASSERT(((intptr_t)impatientwrites[ii]) != 0);
            }
        }

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }
#endif

    EXIT();
}
