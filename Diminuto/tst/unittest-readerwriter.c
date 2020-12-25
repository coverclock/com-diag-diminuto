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
 * Enabling DEBUG logging before running is enlightening:
 *
 * > export COM_DIAG_DIMINUTO_LOG_MASK=0xff
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_readerwriter.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_thread.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_ring.h"

static void ringing(diminuto_readerwriter_t * rwp)
{
    unsigned int used = 0;
    unsigned int count = 0;
    int index = -1;

    if ((used = diminuto_ring_used(&(rwp->ring))) > 0) {
        CHECKPOINT("ReaderWriter@%p: ring\n", rwp);
        for (count = 0, index = diminuto_ring_consumer_peek(&(rwp->ring)); count < used; count += 1, index = diminuto_ring_next(&(rwp->ring), index)) {
            CHECKPOINT("    <%d> [%d] {%c}\n", count, index, rwp->state[index]);
        }
    }
}

struct Context {
    int identifier;
    diminuto_readerwriter_t * rwp;
    diminuto_ticks_t latency;
    diminuto_ticks_t workload;
    int iterations;
};

static void * reader(void * vp)
{
    struct Context * cp = (struct Context *)vp;
    int ii;
    int success;

    CHECKPOINT("reader[%d] rwp=%p latency=%llu workload=%llu iterations=%d\n", cp->identifier, cp->rwp, (unsigned long long)(cp->latency), (unsigned long long)(cp->workload), cp->iterations);

    for (ii = 0; ii < cp->iterations; ++ii) {
        success = 0;
        diminuto_delay(cp->latency, 0);
        DIMINUTO_READER_BEGIN(cp->rwp);
            success = ((cp->rwp->reading > 0) && (cp->rwp->writing == 0));
            CHECKPOINT("reader[%d] reading=%d writing=%d waiting=%d success=%d\n", cp->identifier, cp->rwp->reading, cp->rwp->writing, diminuto_ring_used(&(cp->rwp->ring)), success);
            ringing(cp->rwp);
            if (success) {
                diminuto_delay(cp->workload, 0);
            }
        DIMINUTO_READER_END;
        if (!success) { break; }
    }

    return (void *)(intptr_t)success;
}

static void * writer(void * vp)
{
    struct Context * cp = (struct Context *)vp;
    int ii;
    int success;

    CHECKPOINT("writer[%d] rwp=%p latency=%llu workload=%llu iterations=%d\n", cp->identifier, cp->rwp, (unsigned long long)(cp->latency), (unsigned long long)(cp->workload), cp->iterations);

    for (ii = 0; ii < cp->iterations; ++ii) {
        success = 0;
        diminuto_delay(cp->latency, 0);
        DIMINUTO_WRITER_BEGIN(cp->rwp);
            success = ((cp->rwp->reading == 0) && (cp->rwp->writing == 1));
            CHECKPOINT("writer[%d] reading=%d writing=%d waiting=%d success=%d\n", cp->identifier, cp->rwp->reading, cp->rwp->writing, diminuto_ring_used(&(cp->rwp->ring)), success);
            ringing(cp->rwp);
            if (success) {
                diminuto_delay(cp->workload, 0);
            }
        DIMINUTO_WRITER_END;
        if (!success) { break; }
    }

    return (void *)(intptr_t)success;
}

int main(void)
{
    diminuto_ticks_t frequency;

    frequency = diminuto_frequency();

    SETLOGMASK();

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

        ringing(&rw);
            
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

        ringing(&rw);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }

    {
        diminuto_readerwriter_state_t state[64];
        diminuto_readerwriter_t rw;
        int success = 0;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw, state, diminuto_countof(state)) == &rw);

        ASSERT(rw.state == state);
        ASSERT(rw.ring.capacity == diminuto_countof(state));
        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.ring.producer == 0);
        ASSERT(rw.ring.consumer == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);

        ringing(&rw);

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

        ringing(&rw);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }

    {
        diminuto_readerwriter_state_t state[64];
        diminuto_readerwriter_t rw;
        int success = 0;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw, state, diminuto_countof(state)) == &rw);

        ASSERT(rw.state == state);
        ASSERT(rw.ring.capacity == diminuto_countof(state));
        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.ring.producer == 0);
        ASSERT(rw.ring.consumer == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);

        ringing(&rw);

        DIMINUTO_WRITER_BEGIN(&rw);
            ASSERT(rw.ring.measure == 0);
            ASSERT(rw.reading == 0);
            ASSERT(rw.writing == 1);
            success = !0;
        DIMINUTO_WRITER_END;

        ringing(&rw);

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

        ASSERT(rw.state == state);
        ASSERT(rw.ring.capacity == diminuto_countof(state));
        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.ring.producer == 0);
        ASSERT(rw.ring.consumer == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);

        ringing(&rw);

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

        ringing(&rw);

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

        ringing(&rw);

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

        ringing(&rw);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }

    {
        diminuto_readerwriter_state_t state[64];
        diminuto_readerwriter_t rw;
        int success;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw, state, diminuto_countof(state)) == &rw);

        ASSERT(rw.state == state);
        ASSERT(rw.ring.capacity == diminuto_countof(state));
        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.ring.producer == 0);
        ASSERT(rw.ring.consumer == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);

        ringing(&rw);

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

        ringing(&rw);

        success = 0;
        DIMINUTO_WRITER_BEGIN(&rw);
            ASSERT(rw.ring.measure == 0);
            ASSERT(rw.reading == 0);
            ASSERT(rw.writing == 1);
            success = !0;
        DIMINUTO_WRITER_END;

        ringing(&rw);

        ASSERT(rw.ring.measure == 0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(success == !0);

        ringing(&rw);

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

        ringing(&rw);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }

#if !0
    {
        diminuto_thread_t readers[3];
        struct Context reading[diminuto_countof(readers)];
        void * result[countof(readers)];
        diminuto_readerwriter_state_t state[diminuto_countof(readers)];
        diminuto_readerwriter_t rw;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw, state, diminuto_countof(readers)) == &rw);

        ringing(&rw);

        reading[0].identifier = 0;
        reading[0].rwp = &rw;
        reading[0].latency = frequency * 0;
        reading[0].workload = frequency * 3;
        reading[0].iterations = 11;

        reading[1].identifier = 0;
        reading[1].rwp = &rw;
        reading[1].latency = frequency * 5;
        reading[1].workload = frequency * 7;
        reading[1].iterations = 11;

        reading[2].identifier = 0;
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

        ringing(&rw);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }
#endif

#if !0
    {
        diminuto_thread_t writers[3];
        struct Context writing[diminuto_countof(writers)];
        void * result[countof(writers)];
        diminuto_readerwriter_state_t state[diminuto_countof(writers)];
        diminuto_readerwriter_t rw;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw, state, diminuto_countof(writers)) == &rw);

        ringing(&rw);

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

        ringing(&rw);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }
#endif

    EXIT();
}
