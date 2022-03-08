/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the unit test for the Reader Writer feature.
 *
 * Enabling DEBUG logging is enlightening.
 *
 * COM_DIAG_DIMINUTO_LOG_MASK=0xff unittest-readerwriter
 *
 * Even more diagnostic output can be emitted to standard error by using
 * the "-d" flag on the command line for this unit test, which causes the
 * unit test to enable more debugging in the feature.
 *
 * COM_DIAG_DIMINUTO_LOG_MASK=0xff unittest-readerwriter -d
 *
 * Turning off DEBUG and INFO logging but using the "-d" flag
 * also puts on an interesting show.
 *
 * COM_DIAG_DIMINUTO_LOG_MASK=0xfc unittest-readerwriter -d
 *
 * Using the "-f" flag or the "-c" flag (see below) is required to
 * get the final (long) test sequence to run.
 *
 * Using the "-c" flag on the command line results in the final test
 * (the one with six different thread behaviors all operating
 * concurrently) to configure each worker thread for the maximum
 * possible iterations (MAXINT). For all practical purposes, this
 * test will not complete.
 *
 * unittest-readerwriter -c
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_readerwriter.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_thread.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_list.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_minmaxof.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include "../src/diminuto_readerwriter.h"

/******************************************************************************/

static unsigned int seed;

static unsigned int randy(unsigned int low, unsigned int high)
{
    unsigned int result;

    result = rand_r(&seed); /* rand_r: [0..RAND_MAX=2147483647] */
    result = low + (result % (high - low + 1));

    ASSERT((low <= result) && (result <= high));

    return result;
}

/******************************************************************************/

static int database = 0;

struct Context {
    int identifier;
    diminuto_readerwriter_t * rwp;
    diminuto_ticks_t latency;
    diminuto_ticks_t timeout;
    diminuto_ticks_t workload;
    int iterations;
};

static void * reader(void * vp)
{
    int count = 0;
    struct Context * cp = (struct Context *)vp;
    int ii;
    bool check;

    CHECKPOINT("reader[%d] rwp=%p latency=%llu workload=%llu iterations=%d\n", cp->identifier, cp->rwp, (unsigned long long)(cp->latency), (unsigned long long)(cp->workload), cp->iterations);

    for (ii = 0; ii < cp->iterations; ++ii) {
        diminuto_delay(cp->latency, 0);
        check = false;
        CHECKPOINT("reader[%d] requesting.\n", cp->identifier);
        DIMINUTO_READER_BEGIN(cp->rwp);
            count += 1;
            CHECKPOINT("reader[%d] running %d.\n", cp->identifier, count);
            check = true;
            ASSERT(database == 0);
            diminuto_delay(cp->workload, 0);
            ASSERT(database == 0);
        DIMINUTO_READER_END;
        CHECKPOINT("reader[%d] relinquished.\n", cp->identifier);
        ASSERT(check);
    }

    return (void *)(intptr_t)(count);
}

static void * writer(void * vp)
{
    int count = 0;
    struct Context * cp = (struct Context *)vp;
    int ii;
    bool check;

    CHECKPOINT("writer[%d] rwp=%p latency=%llu workload=%llu iterations=%d\n", cp->identifier, cp->rwp, (unsigned long long)(cp->latency), (unsigned long long)(cp->workload), cp->iterations);

    for (ii = 0; ii < cp->iterations; ++ii) {
        diminuto_delay(cp->latency, 0);
        check = false;
        CHECKPOINT("writer[%d] requesting.\n", cp->identifier);
        DIMINUTO_WRITER_BEGIN(cp->rwp);
            count += 1;
            CHECKPOINT("writer[%d] running %d.\n", cp->identifier, count);
            check = true;
            database += 1;
            ASSERT(database == 1);
            diminuto_delay(cp->workload, 0);
            database -= 1;
            ASSERT(database == 0);
        DIMINUTO_WRITER_END;
        CHECKPOINT("writer[%d] relinquished.\n", cp->identifier);
        ASSERT(check);
    }

    return (void *)(intptr_t)(count);
}

static void * impatientreader(void * vp)
{
    int count = 0;
    struct Context * cp = (struct Context *)vp;
    int ii;

    CHECKPOINT("impatientreader[%d] rwp=%p latency=%llu timeout=%llu workload=%llu iterations=%d\n", cp->identifier, cp->rwp, (unsigned long long)(cp->latency), (unsigned long long)(cp->timeout), (unsigned long long)(cp->workload), cp->iterations);

    for (ii = 0; ii < cp->iterations; ++ii) {
        diminuto_delay(cp->latency, 0);
        CHECKPOINT("impatientreader[%d] requesting.\n", cp->identifier);
        if (diminuto_reader_begin_timed(cp->rwp, cp->timeout) == 0) {
            pthread_cleanup_push(diminuto_reader_cleanup, cp->rwp);
                count += 1;
                CHECKPOINT("impatientreader[%d] running %d.\n", cp->identifier, count);
                ASSERT(database == 0);
                diminuto_delay(cp->workload, 0);
                ASSERT(database == 0);
            pthread_cleanup_pop(!0);
            CHECKPOINT("impatientreader[%d] relinquished.\n", cp->identifier);
        } else {
            ASSERT(errno == DIMINUTO_READERWRITER_TIMEDOUT);
            CHECKPOINT("impatientreader[%d] timedout!\n", cp->identifier);
        }
    }

    return (void *)(intptr_t)(count);
}

static void * impatientwriter(void * vp)
{
    int count = 0;
    struct Context * cp = (struct Context *)vp;
    int ii;

    CHECKPOINT("impatientwriter[%d] rwp=%p latency=%llu timeout=%llu workload=%llu iterations=%d\n", cp->identifier, cp->rwp, (unsigned long long)(cp->latency), (unsigned long long)(cp->timeout), (unsigned long long)(cp->workload), cp->iterations);

    for (ii = 0; ii < cp->iterations; ++ii) {
        diminuto_delay(cp->latency, 0);
        CHECKPOINT("impatientwriter[%d] requesting.\n", cp->identifier);
        if (diminuto_writer_begin_timed(cp->rwp, cp->timeout) == 0) {
            pthread_cleanup_push(diminuto_writer_cleanup, cp->rwp);
                count += 1;
                CHECKPOINT("impatientwriter[%d] running %d.\n", cp->identifier, count);
                database += 1;
                ASSERT(database == 1);
                diminuto_delay(cp->workload, 0);
                database -= 1;
                ASSERT(database == 0);
            pthread_cleanup_pop(!0);
            CHECKPOINT("impatientwriter[%d] relinquished.\n", cp->identifier);
        } else {
            ASSERT(errno == DIMINUTO_READERWRITER_TIMEDOUT);
            CHECKPOINT("impatientwriter[%d] timedout!\n", cp->identifier);
        }
    }

    return (void *)(intptr_t)(count);
}

static void * aggressivereader(void * vp)
{
    int count = 0;
    struct Context * cp = (struct Context *)vp;
    int ii;

    CHECKPOINT("aggressivereader[%d] rwp=%p latency=%llu workload=%llu iterations=%d\n", cp->identifier, cp->rwp, (unsigned long long)(cp->latency), (unsigned long long)(cp->workload), cp->iterations);

    for (ii = 0; ii < cp->iterations; ++ii) {
        diminuto_delay(cp->latency, 0);
        CHECKPOINT("aggressivereader[%d] requesting.\n", cp->identifier);
        ASSERT(diminuto_reader_begin_priority(cp->rwp) == 0);
            pthread_cleanup_push(diminuto_reader_cleanup, cp->rwp);
                count += 1;
                CHECKPOINT("aggressivereader[%d] running %d.\n", cp->identifier, count);
                ASSERT(database == 0);
                diminuto_delay(cp->workload, 0);
                ASSERT(database == 0);
            pthread_cleanup_pop(!0);
        CHECKPOINT("aggressivereader[%d] relinquished.\n", cp->identifier);
    }

    return (void *)(intptr_t)(count);
}

static void * aggressivewriter(void * vp)
{
    int count = 0;
    struct Context * cp = (struct Context *)vp;
    int ii;

    CHECKPOINT("aggressivewriter[%d] rwp=%p latency=%llu workload=%llu iterations=%d\n", cp->identifier, cp->rwp, (unsigned long long)(cp->latency), (unsigned long long)(cp->workload), cp->iterations);

    for (ii = 0; ii < cp->iterations; ++ii) {
        diminuto_delay(cp->latency, 0);
        CHECKPOINT("aggressivewriter[%d] requested.\n", cp->identifier);
        ASSERT(diminuto_writer_begin_priority(cp->rwp) == 0);
            pthread_cleanup_push(diminuto_writer_cleanup, cp->rwp);
                count += 1;
                CHECKPOINT("aggressivewriter[%d] running %d.\n", cp->identifier, count);
                database += 1;
                ASSERT(database == 1);
                diminuto_delay(cp->workload, 0);
                database -= 1;
                ASSERT(database == 0);
            pthread_cleanup_pop(!0);
        CHECKPOINT("aggressivewriter[%d] relinquished.\n", cp->identifier);
    }

    return (void *)(intptr_t)(count);
}

/******************************************************************************/

int main(int argc, char * argv[])
{
    int debug = 0;
    int full = 0;
    int continuous = 0;
    int ii;
    diminuto_ticks_t frequency;

    SETLOGMASK();

    for (ii = 1; ii < argc; ++ii) {
        if      (strcmp(argv[ii], "-c") == 0) { continuous = !0; }
        else if (strcmp(argv[ii], "-d") == 0) { debug = !0; }
        else if (strcmp(argv[ii], "-f") == 0) { full = !0; }
        else                                  { /* Do nothing. */ }
    }

    CHECKPOINT("debug=%d full=%d continuous=%d\n", debug, full, continuous);

    frequency = diminuto_frequency();

    seed = (unsigned int)diminuto_time_atomic();

    {
        TEST();

        ASSERT(DIMINUTO_READERWRITER_ERROR == EIO);
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
        diminuto_readerwriter_t rw = DIMINUTO_READERWRITER_INITIALIZER;

        TEST();

        ASSERT(!diminuto_readerwriter_debug(&rw, 0));
        ASSERT(!diminuto_readerwriter_debug(&rw, !0));
        ASSERT(diminuto_readerwriter_debug(&rw, !0));
        ASSERT(diminuto_readerwriter_debug(&rw, 0));
        ASSERT(!diminuto_readerwriter_debug(&rw, 0));

        STATUS();
    }

    {
        diminuto_readerwriter_t rw = DIMINUTO_READERWRITER_INITIALIZER;

        TEST();

        ASSERT(rw.list.next == (diminuto_list_t *)0);
        ASSERT(rw.list.prev == (diminuto_list_t *)0);
        ASSERT(rw.list.root == (diminuto_list_t *)0);
        ASSERT(rw.list.data == (void *)0);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);

        STATUS();
    }

    {
        diminuto_readerwriter_t rw;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw) == &rw);

        ASSERT(rw.list.next == &(rw.list));
        ASSERT(rw.list.prev == &(rw.list));
        ASSERT(rw.list.root == &(rw.list));
        ASSERT(rw.list.data == (void *)&rw);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }

    {
        diminuto_readerwriter_t rw;
        diminuto_list_t node1;
        diminuto_list_t node2;
        diminuto_list_t node3;
        diminuto_list_t node4;

        /* This unit test uses the private API just for unit testing. */

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw) == &rw);
        ASSERT(rw.waiting == 0);

        ASSERT(diminuto_readerwriter_front(&rw) == &rw.list);

        ASSERT(diminuto_readerwriter_head(&rw) == (diminuto_list_t *)0);

        /* NODE 1 (READABLE or WRITABLE) */

        ASSERT(diminuto_list_init(&node1) == &node1);
        diminuto_list_dataset(&node1, (void *)READABLE);

        /* We will never enqueue a READABLE outside of unit test. */

        diminuto_readerwriter_enqueue(&rw, &node1, 0);
        ASSERT(rw.waiting == 1);
        ASSERT(diminuto_list_head(&rw.list) == &node1);
        ASSERT(diminuto_list_next(&node1) == &rw.list);

        ASSERT(diminuto_readerwriter_ready(&rw, &node1, READABLE));
        ASSERT(!diminuto_readerwriter_ready(&rw, &node1, WRITABLE));

        ASSERT(diminuto_readerwriter_front(&rw) == &rw.list);

        ASSERT(diminuto_readerwriter_head(&rw) == &node1);

        /* We will never enqueue a WRITABLE outside of unit test. */

        diminuto_list_dataset(&node1, (void *)WRITABLE);

        ASSERT(!diminuto_readerwriter_ready(&rw, &node1, READABLE));
        ASSERT(diminuto_readerwriter_ready(&rw, &node1, WRITABLE));

        ASSERT(diminuto_readerwriter_front(&rw) == &rw.list);

        ASSERT(diminuto_readerwriter_head(&rw) == &node1);

        /* NODE 2 (FAILED) */

        ASSERT(diminuto_list_init(&node2) == &node2);
        diminuto_list_dataset(&node2, (void *)FAILED);

        /* We will never enqueue a FAILED outside of unit test. */

        diminuto_readerwriter_enqueue(&rw, &node2, 0);
        ASSERT(rw.waiting == 2);
        ASSERT(diminuto_list_head(&rw.list) == &node1);
        ASSERT(diminuto_list_next(&node1) == &node2);
        ASSERT(diminuto_list_next(&node2) == &rw.list);

        ASSERT(!diminuto_readerwriter_ready(&rw, &node1, READABLE));
        ASSERT(diminuto_readerwriter_ready(&rw, &node1, WRITABLE));
        ASSERT(!diminuto_readerwriter_ready(&rw, &node2, READABLE));
        ASSERT(!diminuto_readerwriter_ready(&rw, &node2, WRITABLE));

        ASSERT(diminuto_readerwriter_front(&rw) == &node2);

        ASSERT(diminuto_readerwriter_head(&rw) == &node1);

        /* NODE 3 (READER) */

        ASSERT(diminuto_list_init(&node3) == &node3);
        diminuto_list_dataset(&node3, (void *)READER);

        diminuto_readerwriter_enqueue(&rw, &node3, !0);
        ASSERT(rw.waiting == 3);
        ASSERT(diminuto_list_head(&rw.list) == &node1);
        ASSERT(diminuto_list_next(&node1) == &node3);
        ASSERT(diminuto_list_next(&node3) == &node2);
        ASSERT(diminuto_list_next(&node2) == &rw.list);

        ASSERT(!diminuto_readerwriter_ready(&rw, &node1, READABLE));
        ASSERT(diminuto_readerwriter_ready(&rw, &node1, WRITABLE));
        ASSERT(!diminuto_readerwriter_ready(&rw, &node2, READABLE));
        ASSERT(!diminuto_readerwriter_ready(&rw, &node2, WRITABLE));
        ASSERT(!diminuto_readerwriter_ready(&rw, &node3, READABLE));
        ASSERT(!diminuto_readerwriter_ready(&rw, &node3, WRITABLE));

        ASSERT(diminuto_readerwriter_front(&rw) == &node3);

        ASSERT(diminuto_readerwriter_head(&rw) == &node1);

        /* NODE 4 (WRITER) */

        ASSERT(diminuto_list_init(&node4) == &node4);
        diminuto_list_dataset(&node4, (void *)WRITER);

        diminuto_readerwriter_enqueue(&rw, &node4, 0);
        ASSERT(rw.waiting == 4);
        ASSERT(diminuto_list_head(&rw.list) == &node1);
        ASSERT(diminuto_list_next(&node1) == &node3);
        ASSERT(diminuto_list_next(&node3) == &node2);
        ASSERT(diminuto_list_next(&node2) == &node4);
        ASSERT(diminuto_list_next(&node4) == &rw.list);

        ASSERT(!diminuto_readerwriter_ready(&rw, &node1, READABLE));
        ASSERT(diminuto_readerwriter_ready(&rw, &node1, WRITABLE));
        ASSERT(!diminuto_readerwriter_ready(&rw, &node3, READABLE));
        ASSERT(!diminuto_readerwriter_ready(&rw, &node3, WRITABLE));
        ASSERT(!diminuto_readerwriter_ready(&rw, &node2, READABLE));
        ASSERT(!diminuto_readerwriter_ready(&rw, &node2, WRITABLE));
        ASSERT(!diminuto_readerwriter_ready(&rw, &node4, READABLE));
        ASSERT(!diminuto_readerwriter_ready(&rw, &node4, WRITABLE));

        ASSERT(diminuto_readerwriter_front(&rw) == &node3);

        ASSERT(diminuto_readerwriter_head(&rw) == &node1);

        /* Not empty. */

        ASSERT(diminuto_readerwriter_fini(&rw) == &rw);

        /* NODE 1 */

        diminuto_readerwriter_dequeue(&rw, &node1);
        ASSERT(rw.waiting == 3);
        ASSERT(diminuto_list_head(&rw.list) == &node3);
        ASSERT(diminuto_list_next(&node3) == &node2);
        ASSERT(diminuto_list_next(&node2) == &node4);
        ASSERT(diminuto_list_next(&node4) == &rw.list);

        ASSERT(!diminuto_readerwriter_ready(&rw, &node3, READABLE));
        ASSERT(!diminuto_readerwriter_ready(&rw, &node3, WRITABLE));
        ASSERT(!diminuto_readerwriter_ready(&rw, &node2, READABLE));
        ASSERT(!diminuto_readerwriter_ready(&rw, &node2, WRITABLE));
        ASSERT(!diminuto_readerwriter_ready(&rw, &node4, READABLE));
        ASSERT(!diminuto_readerwriter_ready(&rw, &node4, WRITABLE));

        ASSERT(diminuto_readerwriter_front(&rw) == &node3);

        ASSERT(diminuto_readerwriter_head(&rw) == &node3);

        diminuto_list_dataset(&node3, (void *)READABLE);

        ASSERT(diminuto_readerwriter_ready(&rw, &node3, READABLE));
        ASSERT(!diminuto_readerwriter_ready(&rw, &node3, WRITABLE));
        ASSERT(!diminuto_readerwriter_ready(&rw, &node2, READABLE));
        ASSERT(!diminuto_readerwriter_ready(&rw, &node2, WRITABLE));
        ASSERT(!diminuto_readerwriter_ready(&rw, &node4, READABLE));
        ASSERT(!diminuto_readerwriter_ready(&rw, &node4, WRITABLE));

        ASSERT(diminuto_readerwriter_front(&rw) == &node2);

        ASSERT(diminuto_readerwriter_head(&rw) == &node3);

        /* NODE 3 */

        diminuto_readerwriter_dequeue(&rw, &node3);
        ASSERT(rw.waiting == 2);
        ASSERT(diminuto_list_head(&rw.list) == &node2);
        ASSERT(diminuto_list_next(&node2) == &node4);
        ASSERT(diminuto_list_next(&node4) == &rw.list);

        ASSERT(!diminuto_readerwriter_ready(&rw, &node2, READABLE));
        ASSERT(!diminuto_readerwriter_ready(&rw, &node2, WRITABLE));
        ASSERT(!diminuto_readerwriter_ready(&rw, &node4, READABLE));
        ASSERT(!diminuto_readerwriter_ready(&rw, &node4, WRITABLE));

        /* NODE 2 removed as a side effect of ready/head. */

        ASSERT(rw.waiting == 1);
        ASSERT(diminuto_list_head(&rw.list) == &node4);
        ASSERT(diminuto_list_next(&node4) == &rw.list);

        ASSERT(!diminuto_readerwriter_ready(&rw, &node4, READABLE));
        ASSERT(!diminuto_readerwriter_ready(&rw, &node4, WRITABLE));

        ASSERT(diminuto_readerwriter_front(&rw) == &node4);

        ASSERT(diminuto_readerwriter_head(&rw) == &node4);

        diminuto_list_dataset(&node4, (void *)WRITABLE);

        ASSERT(!diminuto_readerwriter_ready(&rw, &node4, READABLE));
        ASSERT(diminuto_readerwriter_ready(&rw, &node4, WRITABLE));

        /* NODE 4 */

        diminuto_readerwriter_dequeue(&rw, &node4);
        ASSERT(rw.waiting == 0);
        ASSERT(diminuto_list_head(&rw.list) == (diminuto_list_t *)0);

        /* Finish. */

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }

    {
        diminuto_readerwriter_t rw;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw) == &rw);

        diminuto_readerwriter_debug(&rw, debug);

        ASSERT(rw.list.next == &(rw.list));
        ASSERT(rw.list.prev == &(rw.list));
        ASSERT(rw.list.root == &(rw.list));
        ASSERT(rw.list.data == (void *)&rw);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);

        { ASSERT(diminuto_reader_begin(&rw) == 0); }
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            ASSERT(rw.waiting == 0);
            { ASSERT(diminuto_reader_begin(&rw) == 0); }
                ASSERT(rw.reading == 2);
                ASSERT(rw.writing == 0);
                ASSERT(rw.waiting == 0);
            { ASSERT(diminuto_reader_end(&rw) == 0); }
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            ASSERT(rw.waiting == 0);
        { ASSERT(diminuto_reader_end(&rw) == 0); }

        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }

    {
        diminuto_readerwriter_t rw;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw) == &rw);

        diminuto_readerwriter_debug(&rw, debug);

        ASSERT(rw.list.next == &(rw.list));
        ASSERT(rw.list.prev == &(rw.list));
        ASSERT(rw.list.root == &(rw.list));
        ASSERT(rw.list.data == (void *)&rw);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);

        { ASSERT(diminuto_reader_begin_timed(&rw, frequency) == 0); }
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            ASSERT(rw.waiting == 0);
            { ASSERT(diminuto_reader_begin_timed(&rw, frequency) == 0); }
                ASSERT(rw.reading == 2);
                ASSERT(rw.writing == 0);
                ASSERT(rw.waiting == 0);
            { ASSERT(diminuto_reader_end(&rw) == 0); }
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            ASSERT(rw.waiting == 0);
            errno = 0;
            { ASSERT(diminuto_writer_begin_timed(&rw, 0) < 0); }
            diminuto_perror("diminuto_writer_begin_timed");
            ASSERT(errno == ETIMEDOUT);
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            ASSERT(rw.waiting == 0);
            errno = 0;
            { ASSERT(diminuto_writer_begin_timed(&rw, frequency) < 0); }
            diminuto_perror("diminuto_writer_begin_timed");
            ASSERT(errno == ETIMEDOUT);
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            ASSERT(rw.waiting == 0);
            errno = 0;
            { ASSERT(diminuto_writer_begin_timed(&rw, 0) < 0); }
            ASSERT(errno == ETIMEDOUT);
            diminuto_perror("diminuto_writer_begin_timed");
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            ASSERT(rw.waiting == 0);
            { ASSERT(diminuto_reader_begin_timed(&rw, frequency) == 0); }
                ASSERT(rw.reading == 2);
                ASSERT(rw.writing == 0);
                ASSERT(rw.waiting == 0);
            { ASSERT(diminuto_reader_end(&rw) == 0); }
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            ASSERT(rw.waiting == 0);
            errno = 0;
            { ASSERT(diminuto_writer_begin_timed(&rw, frequency) < 0); }
            diminuto_perror("diminuto_writer_begin_timed");
            ASSERT(errno == ETIMEDOUT);
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            ASSERT(rw.waiting == 0);
            errno = 0;
            { ASSERT(diminuto_writer_begin_timed(&rw, 0) < 0); }
            diminuto_perror("diminuto_writer_begin_timed");
            ASSERT(errno == ETIMEDOUT);
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            ASSERT(rw.waiting == 0);
            { ASSERT(diminuto_reader_begin_timed(&rw, frequency) == 0); }
                ASSERT(rw.reading == 2);
                ASSERT(rw.writing == 0);
                ASSERT(rw.waiting == 0);
            { ASSERT(diminuto_reader_end(&rw) == 0); }
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            ASSERT(rw.waiting == 0);
        { ASSERT(diminuto_reader_end(&rw) == 0); }

        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }

    {
        diminuto_readerwriter_t rw;
        int success = 0;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw) == &rw);

        diminuto_readerwriter_debug(&rw, debug);

        ASSERT(rw.list.next == &(rw.list));
        ASSERT(rw.list.prev == &(rw.list));
        ASSERT(rw.list.root == &(rw.list));
        ASSERT(rw.list.data == (void *)&rw);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);

        { ASSERT(diminuto_writer_begin(&rw) == 0); }
            ASSERT(rw.reading == 0);
            ASSERT(rw.writing == 1);
            ASSERT(rw.waiting == 0);
            success = !0;
        { ASSERT(diminuto_writer_end(&rw) == 0); }

        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);
        ASSERT(success == !0);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }

    {
        diminuto_readerwriter_t rw;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw) == &rw);

        diminuto_readerwriter_debug(&rw, debug);

        ASSERT(rw.list.next == &(rw.list));
        ASSERT(rw.list.prev == &(rw.list));
        ASSERT(rw.list.root == &(rw.list));
        ASSERT(rw.list.data == (void *)&rw);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);

        { ASSERT(diminuto_writer_begin_timed(&rw, frequency) == 0); }
            ASSERT(rw.reading == 0);
            ASSERT(rw.writing == 1);
            ASSERT(rw.waiting == 0);
            errno = 0;
            { ASSERT(diminuto_reader_begin_timed(&rw, 0) < 0); }
            diminuto_perror("diminuto_reader_begin_timed");
            ASSERT(errno == ETIMEDOUT);
            ASSERT(rw.reading == 0);
            ASSERT(rw.writing == 1);
            ASSERT(rw.waiting == 0);
            errno = 0;
            { ASSERT(diminuto_writer_begin_timed(&rw, 0) < 0); }
            diminuto_perror("diminuto_writer_begin_timed");
            ASSERT(errno == ETIMEDOUT);
            ASSERT(rw.reading == 0);
            ASSERT(rw.writing == 1);
            ASSERT(rw.waiting == 0);
            errno = 0;
            { ASSERT(diminuto_reader_begin_timed(&rw, frequency) < 0); }
            diminuto_perror("diminuto_reader_begin_timed");
            ASSERT(errno == ETIMEDOUT);
            ASSERT(rw.reading == 0);
            ASSERT(rw.writing == 1);
            ASSERT(rw.waiting == 0);
            errno = 0;
            { ASSERT(diminuto_writer_begin_timed(&rw, frequency) < 0); }
            diminuto_perror("diminuto_writer_begin_timed");
            ASSERT(errno == ETIMEDOUT);
            ASSERT(rw.reading == 0);
            ASSERT(rw.writing == 1);
            ASSERT(rw.waiting == 0);
        { ASSERT(diminuto_writer_end(&rw) == 0); }

        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }

    {
        diminuto_readerwriter_t rw;
        int success = 0;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw) == &rw);

        diminuto_readerwriter_debug(&rw, debug);

        ASSERT(rw.list.next == &(rw.list));
        ASSERT(rw.list.prev == &(rw.list));
        ASSERT(rw.list.root == &(rw.list));
        ASSERT(rw.list.data == (void *)&rw);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);

        DIMINUTO_READER_BEGIN(&rw);
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            ASSERT(rw.waiting == 0);
            success = !0;
        DIMINUTO_READER_END;

        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);
        ASSERT(success == !0);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }

    {
        diminuto_readerwriter_t rw;
        int success = 0;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw) == &rw);

        diminuto_readerwriter_debug(&rw, debug);

        ASSERT(rw.list.next == &(rw.list));
        ASSERT(rw.list.prev == &(rw.list));
        ASSERT(rw.list.root == &(rw.list));
        ASSERT(rw.list.data == (void *)&rw);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);

        DIMINUTO_WRITER_BEGIN(&rw);
            ASSERT(rw.reading == 0);
            ASSERT(rw.writing == 1);
            ASSERT(rw.waiting == 0);
            success = !0;
        DIMINUTO_WRITER_END;

        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);
        ASSERT(success == !0);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }

    {
        diminuto_readerwriter_t rw;
        int success;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw) == &rw);

        diminuto_readerwriter_debug(&rw, debug);

        ASSERT(rw.list.next == &(rw.list));
        ASSERT(rw.list.prev == &(rw.list));
        ASSERT(rw.list.root == &(rw.list));
        ASSERT(rw.list.data == (void *)&rw);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);

        success = 0;
        DIMINUTO_READER_BEGIN(&rw);
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            ASSERT(rw.waiting == 0);
            success = !0;
        DIMINUTO_READER_END;

        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);
        ASSERT(success == !0);

        success = 0;
        DIMINUTO_READER_BEGIN(&rw);
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            ASSERT(rw.waiting == 0);
            success = !0;
        DIMINUTO_READER_END;

        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);
        ASSERT(success == !0);

        success = 0;
        DIMINUTO_READER_BEGIN(&rw);
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            ASSERT(rw.waiting == 0);
            success = !0;
        DIMINUTO_READER_END;

        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);
        ASSERT(success == !0);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }

    {
        diminuto_readerwriter_t rw;
        int success;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw) == &rw);

        diminuto_readerwriter_debug(&rw, debug);

        ASSERT(rw.list.next == &(rw.list));
        ASSERT(rw.list.prev == &(rw.list));
        ASSERT(rw.list.root == &(rw.list));
        ASSERT(rw.list.data == (void *)&rw);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);

        success = 0;
        DIMINUTO_WRITER_BEGIN(&rw);
            ASSERT(rw.reading == 0);
            ASSERT(rw.writing == 1);
            ASSERT(rw.waiting == 0);
            success = !0;
        DIMINUTO_WRITER_END;

        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);
        ASSERT(success == !0);

        success = 0;
        DIMINUTO_WRITER_BEGIN(&rw);
            ASSERT(rw.reading == 0);
            ASSERT(rw.writing == 1);
            ASSERT(rw.waiting == 0);
            success = !0;
        DIMINUTO_WRITER_END;

        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);
        ASSERT(success == !0);

        success = 0;
        DIMINUTO_WRITER_BEGIN(&rw);
            ASSERT(rw.reading == 0);
            ASSERT(rw.writing == 1);
            ASSERT(rw.waiting == 0);
            success = !0;
        DIMINUTO_WRITER_END;

        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);
        ASSERT(success == !0);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }

    {
        diminuto_readerwriter_t rw;
        int success;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw) == &rw);

        diminuto_readerwriter_debug(&rw, debug);

        ASSERT(rw.list.next == &(rw.list));
        ASSERT(rw.list.prev == &(rw.list));
        ASSERT(rw.list.root == &(rw.list));
        ASSERT(rw.list.data == (void *)&rw);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);

        success = 0;
        DIMINUTO_READER_BEGIN(&rw);
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            ASSERT(rw.waiting == 0);
            success = !0;
        DIMINUTO_READER_END;

        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);
        ASSERT(success == !0);

        success = 0;
        DIMINUTO_WRITER_BEGIN(&rw);
            ASSERT(rw.reading == 0);
            ASSERT(rw.writing == 1);
            ASSERT(rw.waiting == 0);
            success = !0;
        DIMINUTO_WRITER_END;

        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);
        ASSERT(success == !0);

        success = 0;
        DIMINUTO_READER_BEGIN(&rw);
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            ASSERT(rw.waiting == 0);
            success = !0;
            DIMINUTO_READER_BEGIN(&rw);
                ASSERT(rw.reading == 2);
                ASSERT(rw.writing == 0);
                ASSERT(rw.waiting == 0);
                success = success && !0;
                DIMINUTO_READER_BEGIN(&rw);
                    ASSERT(rw.reading == 3);
                    ASSERT(rw.writing == 0);
                    ASSERT(rw.waiting == 0);
                    success = success && !0;
                DIMINUTO_READER_END;
            DIMINUTO_READER_END;
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            ASSERT(rw.waiting == 0);
            success = success && !0;
        DIMINUTO_READER_END;

        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);
        ASSERT(success == !0);

        success = 0;
        DIMINUTO_WRITER_BEGIN(&rw);
            ASSERT(rw.reading == 0);
            ASSERT(rw.writing == 1);
            ASSERT(rw.waiting == 0);
            success = !0;
        DIMINUTO_WRITER_END;

        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);
        ASSERT(success == !0);

        success = 0;
        DIMINUTO_READER_BEGIN(&rw);
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            ASSERT(rw.waiting == 0);
            success = !0;
        DIMINUTO_READER_END;

        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);
        ASSERT(success == !0);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }

    {
        diminuto_readerwriter_t rw;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw) == &rw);

        diminuto_readerwriter_debug(&rw, debug);

        ASSERT(rw.list.next == &(rw.list));
        ASSERT(rw.list.prev == &(rw.list));
        ASSERT(rw.list.root == &(rw.list));
        ASSERT(rw.list.data == (void *)&rw);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);

        { ASSERT(diminuto_reader_begin_priority(&rw) == 0); }
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            ASSERT(rw.waiting == 0);
            { ASSERT(diminuto_reader_begin_priority(&rw) == 0); }
                ASSERT(rw.reading == 2);
                ASSERT(rw.writing == 0);
                ASSERT(rw.waiting == 0);
            { ASSERT(diminuto_reader_end(&rw) == 0); }
            ASSERT(rw.reading == 1);
            ASSERT(rw.writing == 0);
            ASSERT(rw.waiting == 0);
        { ASSERT(diminuto_reader_end(&rw) == 0); }

        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }

    {
        diminuto_readerwriter_t rw;
        int success = 0;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw) == &rw);

        diminuto_readerwriter_debug(&rw, debug);

        ASSERT(rw.list.next == &(rw.list));
        ASSERT(rw.list.prev == &(rw.list));
        ASSERT(rw.list.root == &(rw.list));
        ASSERT(rw.list.data == (void *)&rw);
        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);

        { ASSERT(diminuto_writer_begin_priority(&rw) == 0); }
            ASSERT(rw.reading == 0);
            ASSERT(rw.writing == 1);
            ASSERT(rw.waiting == 0);
            success = !0;
        { ASSERT(diminuto_writer_end(&rw) == 0); }

        ASSERT(rw.reading == 0);
        ASSERT(rw.writing == 0);
        ASSERT(rw.waiting == 0);
        ASSERT(success == !0);

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }

    {
        diminuto_thread_t readers[3];
        struct Context reading[diminuto_countof(readers)];
        void * result[diminuto_countof(readers)];
        diminuto_readerwriter_t rw;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw) == &rw);

        diminuto_readerwriter_debug(&rw, debug);

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

    {
        diminuto_thread_t writers[3];
        struct Context writing[diminuto_countof(writers)];
        void * result[diminuto_countof(writers)];
        diminuto_readerwriter_t rw;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw) == &rw);

        diminuto_readerwriter_debug(&rw, debug);

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

    {
        diminuto_thread_t readers[3];
        diminuto_thread_t writers[3];
        struct Context reading[diminuto_countof(readers)];
        struct Context writing[diminuto_countof(writers)];
        void * reads[countof(readers)];
        void * writes[countof(writers)];
        diminuto_readerwriter_t rw;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw) == &rw);

        diminuto_readerwriter_debug(&rw, debug);

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

    if ((!continuous) && (!full)) { goto complete; }

    {
        diminuto_thread_t readers[64];
        diminuto_thread_t writers[8];
        diminuto_thread_t impatientreaders[4];
        diminuto_thread_t impatientwriters[2];
        diminuto_thread_t aggressivereaders[1];
        diminuto_thread_t aggressivewriters[1];
        struct Context reading[diminuto_countof(readers)];
        struct Context writing[diminuto_countof(writers)];
        struct Context impatientreading[diminuto_countof(impatientreaders)];
        struct Context impatientwriting[diminuto_countof(impatientwriters)];
        struct Context aggressivereading[diminuto_countof(aggressivereaders)];
        struct Context aggressivewriting[diminuto_countof(aggressivewriters)];
        void * reads[diminuto_countof(readers)];
        void * writes[diminuto_countof(writers)];
        void * impatientreads[diminuto_countof(impatientreaders)];
        void * impatientwrites[diminuto_countof(impatientwriters)];
        void * aggressivereads[diminuto_countof(aggressivereaders)];
        void * aggressivewrites[diminuto_countof(aggressivewriters)];
        static const int MAXIMUM = countof(readers);
        static const int TOTAL = countof(readers) + countof(writers) + countof(impatientreaders) + countof(impatientwriters) + countof(aggressivereaders) + countof(aggressivewriters);
        size_t successfulimpatientreaders = 0;
        size_t successfulimpatientwriters = 0;
        static const int LIMIT = maximumof(int);
        diminuto_readerwriter_t rw;
        int ii;

        TEST();

        ASSERT(diminuto_readerwriter_init(&rw) == &rw);

        diminuto_readerwriter_debug(&rw, debug);

        for (ii = 0; ii < diminuto_countof(readers); ++ii) {
            reading[ii].identifier = ii;
            reading[ii].rwp = &rw;
            reading[ii].latency = frequency * randy(1, 3);
            reading[ii].timeout = DIMINUTO_READERWRITER_INFINITY;
            reading[ii].workload = frequency * randy(1, 5);
            reading[ii].iterations = continuous ? LIMIT : randy(2, 13);
            reads[ii] = (void *)0;
            ASSERT(diminuto_thread_init(&readers[ii], reader) == &readers[ii]);
        }

        for (ii = 0; ii < diminuto_countof(writers); ++ii) {
            writing[ii].identifier = ii;
            writing[ii].rwp = &rw;
            writing[ii].latency = frequency * randy(1, 3);
            writing[ii].timeout = DIMINUTO_READERWRITER_INFINITY;
            writing[ii].workload = frequency * randy(1, 5);
            writing[ii].iterations = continuous ? LIMIT : randy(2, 13);
            writes[ii] = (void *)0;
            ASSERT(diminuto_thread_init(&writers[ii], writer) == &writers[ii]);
        }

        for (ii = 0; ii < diminuto_countof(impatientreaders); ++ii) {
            impatientreading[ii].identifier = ii;
            impatientreading[ii].rwp = &rw;
            impatientreading[ii].latency = frequency * randy(3, 5);
            impatientreading[ii].timeout = (ii == 0) ? 0 : frequency * randy(5, 60);
            impatientreading[ii].workload = frequency * randy(1, 5);
            impatientreading[ii].iterations = continuous ? LIMIT : randy(2, 13);
            impatientreads[ii] = (void *)0;
            ASSERT(diminuto_thread_init(&impatientreaders[ii], impatientreader) == &impatientreaders[ii]);
        }

        for (ii = 0; ii < diminuto_countof(impatientwriters); ++ii) {
            impatientwriting[ii].identifier = ii;
            impatientwriting[ii].rwp = &rw;
            impatientwriting[ii].latency = frequency * randy(3, 5);
            impatientwriting[ii].timeout = (ii == 0) ? 0 : frequency * randy(5, 300);
            impatientwriting[ii].workload = frequency * randy(1, 5);
            impatientwriting[ii].iterations = continuous ? LIMIT : randy(2, 13);
            impatientwrites[ii] = (void *)0;
            ASSERT(diminuto_thread_init(&impatientwriters[ii], impatientwriter) == &impatientwriters[ii]);
        }

        for (ii = 0; ii < diminuto_countof(aggressivereaders); ++ii) {
            aggressivereading[ii].identifier = ii;
            aggressivereading[ii].rwp = &rw;
            aggressivereading[ii].latency = frequency * randy(5, 7);
            aggressivereading[ii].timeout = DIMINUTO_READERWRITER_INFINITY;
            aggressivereading[ii].workload = frequency * randy(1, 5);
            aggressivereading[ii].iterations = continuous ? LIMIT : randy(2, 13);
            aggressivereads[ii] = (void *)0;
            ASSERT(diminuto_thread_init(&aggressivereaders[ii], aggressivereader) == &aggressivereaders[ii]);
        }

        for (ii = 0; ii < diminuto_countof(aggressivewriters); ++ii) {
            aggressivewriting[ii].identifier = ii;
            aggressivewriting[ii].rwp = &rw;
            aggressivewriting[ii].latency = frequency * randy(5, 7);
            aggressivewriting[ii].timeout = DIMINUTO_READERWRITER_INFINITY;
            aggressivewriting[ii].workload = frequency * randy(1, 5);
            aggressivewriting[ii].iterations = continuous ? LIMIT : randy(2, 13);
            aggressivewrites[ii] = (void *)0;
            ASSERT(diminuto_thread_init(&aggressivewriters[ii], aggressivewriter) == &aggressivewriters[ii]);
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
            if (ii < diminuto_countof(aggressivereaders)) {
                ASSERT(diminuto_thread_start(&aggressivereaders[ii], &aggressivereading[ii]) == 0);
            }
            if (ii < diminuto_countof(aggressivewriters)) {
                ASSERT(diminuto_thread_start(&aggressivewriters[ii], &aggressivewriting[ii]) == 0);
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
            if (ii < diminuto_countof(aggressivereaders)) {
                ASSERT(diminuto_thread_join(&aggressivereaders[ii], &aggressivereads[ii]) == 0);
                CHECKPOINT("aggressivereads[%d]=%d\n", ii, (int)(intptr_t)aggressivereads[ii]);
            }
            if (ii < diminuto_countof(aggressivewriters)) {
                ASSERT(diminuto_thread_join(&aggressivewriters[ii], &aggressivewrites[ii]) == 0);
                CHECKPOINT("aggressivewrites[%d]=%d\n", ii, (int)(intptr_t)aggressivewrites[ii]);
            }
        }

        for (ii = 0; ii < MAXIMUM; ++ii) {
            if (ii < diminuto_countof(readers)) {
                ASSERT(((intptr_t)reads[ii]) > 0);
            }
            if (ii < diminuto_countof(writers)) {
                ASSERT(((intptr_t)writes[ii]) > 0);
            }
            if (ii < diminuto_countof(aggressivereaders)) {
                ASSERT(((intptr_t)aggressivereads[ii]) > 0);
            }
            if (ii < diminuto_countof(aggressivewriters)) {
                ASSERT(((intptr_t)aggressivewrites[ii]) > 0);
            }
        }

        for (ii = 0; ii < diminuto_countof(impatientreaders); ++ii) {
            if ((intptr_t)impatientreads[ii] > 0) {
                successfulimpatientreaders += 1;
            }
        }
        NOTIFY("%zu of %zu impatientreaders were successful.\n", successfulimpatientreaders, diminuto_countof(impatientreaders));
        ADVISE(successfulimpatientreaders != diminuto_countof(impatientreaders));

        for (ii = 0; ii < diminuto_countof(impatientwriters); ++ii) {
            if ((intptr_t)impatientwrites[ii] > 0) {
                successfulimpatientwriters += 1;
            }
        }
        NOTIFY("%zu of %zu impatientwriters were successful.\n", successfulimpatientwriters, diminuto_countof(impatientwriters));
        ADVISE(successfulimpatientwriters != diminuto_countof(impatientwriters));

        ASSERT(diminuto_readerwriter_fini(&rw) == (diminuto_readerwriter_t *)0);

        STATUS();
    }

complete:

    EXIT();
}
