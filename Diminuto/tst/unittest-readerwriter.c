/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the unit test for the Reader Writer feature.
 *
 * Enabling DEBUG logging before running is enlightening.
 *
 * > export COM_DIAG_DIMINUTO_LOG_MASK=0xff
 *
 * Even more diagnostic output can be emitted to standard
 * error by using the "-d" flag on the command line for this
 * unit test, which causes the unit test to enable more
 * debugging in the feature.
 *
 * > unittest-readerwriter -d
 *
 * Turning off DEBUG and INFO logging but using the "-d" flag
 * also puts on an interesting show.
 *
 * > export COM_DIAG_DIMINUTO_LOG_MASK=0xfc
 */

#include "com/diag/diminuto/diminuto_unittest.h"
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

/******************************************************************************/

int main(int argc, char * argv[])
{
    int debug;
    diminuto_ticks_t frequency;

    debug = (argc > 1) && (strcmp(argv[1], "-d") == 0);

    frequency = diminuto_frequency();

    SETLOGMASK();

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
        reading[0].workload = frequency * 3;
        reading[0].iterations = 11;

        reading[1].identifier = 1;
        reading[1].rwp = &rw;
        reading[1].latency = frequency * 5;
        reading[1].workload = frequency * 7;
        reading[1].iterations = 11;

        reading[2].identifier = 2;
        reading[2].rwp = &rw;
        reading[2].latency = frequency * 1;
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
        writing[0].workload = frequency * 3;
        writing[0].iterations = 11;

        writing[1].identifier = 1;
        writing[1].rwp = &rw;
        writing[1].latency = frequency * 5;
        writing[1].workload = frequency * 7;
        writing[1].iterations = 11;

        writing[2].identifier = 2;
        writing[2].rwp = &rw;
        writing[2].latency = frequency * 1;
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
        reading[0].workload = frequency * 3;
        reading[0].iterations = 11;

        reading[1].identifier = 1;
        reading[1].rwp = &rw;
        reading[1].latency = frequency * 5;
        reading[1].workload = frequency * 7;
        reading[1].iterations = 11;

        reading[2].identifier = 2;
        reading[2].rwp = &rw;
        reading[2].latency = frequency * 1;
        reading[2].workload = frequency * 2;
        reading[2].iterations = 11;

        reads[0] = (void *)0;
        reads[1] = (void *)0;
        reads[2] = (void *)0;

        writing[0].identifier = 3;
        writing[0].rwp = &rw;
        writing[0].latency = frequency * 0;
        writing[0].workload = frequency * 3;
        writing[0].iterations = 11;

        writing[1].identifier = 4;
        writing[1].rwp = &rw;
        writing[1].latency = frequency * 5;
        writing[1].workload = frequency * 7;
        writing[1].iterations = 11;

        writing[2].identifier = 5;
        writing[2].rwp = &rw;
        writing[2].latency = frequency * 1;
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
        struct Context reading[diminuto_countof(readers)];
        struct Context writing[diminuto_countof(writers)];
        void * reads[countof(readers)];
        void * writes[countof(writers)];
        diminuto_readerwriter_state_t state[diminuto_countof(readers) + diminuto_countof(writers)];
        diminuto_readerwriter_t rw;
        int maximum = (diminuto_countof(readers) > diminuto_countof(writers) ? diminuto_countof(readers) : diminuto_countof(writers));
        int ii;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw, state, diminuto_countof(state)) == &rw);

        if (debug) { diminuto_readerwriter_debug(&rw, stderr); }

        for (ii = 0; ii < diminuto_countof(readers); ++ii) {
            reading[ii].identifier = ii;
            reading[ii].rwp = &rw;
            reading[ii].latency = frequency * randy(0, 3);
            reading[ii].workload = frequency * randy(1, 5);
            reading[ii].iterations = randy(2, 13);
            reads[ii] = (void *)0;
            ASSERT(diminuto_thread_init(&readers[ii], reader) == &readers[ii]);
        }

        for (ii = 0; ii < diminuto_countof(writers); ++ii) {
            writing[ii].identifier = ii;
            writing[ii].rwp = &rw;
            writing[ii].latency = frequency * randy(0, 3);
            writing[ii].workload = frequency * randy(1, 5);
            writing[ii].iterations = randy(2, 13);
            writes[ii] = (void *)0;
            ASSERT(diminuto_thread_init(&writers[ii], writer) == &writers[ii]);
        }

        for (ii = 0; ii < maximum; ++ii) {
            if (ii < diminuto_countof(readers)) {
                ASSERT(diminuto_thread_start(&readers[ii], &reading[ii]) == 0);
            }
            if (ii < diminuto_countof(writers)) {
                ASSERT(diminuto_thread_start(&writers[ii], &writing[ii]) == 0);
            }
        }

        for (ii = 0; ii < maximum; ++ii) {
            if (ii < diminuto_countof(readers)) {
                ASSERT(diminuto_thread_join(&readers[ii], &reads[ii]) == 0);
                CHECKPOINT("reads[%d]=%d\n", ii, (int)(intptr_t)reads[ii]);
            }
            if (ii < diminuto_countof(writers)) {
                ASSERT(diminuto_thread_join(&writers[ii], &writes[ii]) == 0);
                CHECKPOINT("writes[%d]=%d\n", ii, (int)(intptr_t)writes[ii]);
            }
        }

        for (ii = 0; ii < maximum; ++ii) {
            if (ii < diminuto_countof(readers)) {
                ASSERT(((intptr_t)reads[ii]) != 0);
            }
            if (ii < diminuto_countof(writers)) {
                ASSERT(((intptr_t)writes[ii]) != 0);
            }
        }

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }
#endif

    EXIT();
}
