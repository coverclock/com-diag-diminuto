/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the sendmsg(2) ancillary data capability.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test to evaluate whether Diminuto Lists and the
 * vector read/write/send/recv system calls can be used to implement
 * a scatter/gather I/O like one might use in a protocol stack.
 *
 * As with some of the functions in the IPC Ancillary unit test, some
 * of the functions in this unit test may eventually be promoted to
 * full Diminuto features if I find them useful enough.
 *
 * REFERENCES
 *
 * unix(7) man page
 *
 * socket(7) man page
 *
 * sendmsg(2) man page
 *
 * recvmsg(2) man page
 *
 * cmsg(3) man page
 *
 * readv(2) man page
 *
 * writev(2) man page
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_list.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_containerof.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_dump.h"
#include "com/diag/diminuto/diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_buffer.h"
#include "com/diag/diminuto/diminuto_fletcher.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

enum {
    NODES = 64,
    /*
     * <linux/uio.h> 5.4.0:
     * UIO_MAXIOV=1024
     * UIO_FASTIOV=8
     * POSIX 1003.1g 5.4.1.1 "UIO_MAXIOV shall be at least 16"
     */
    VECTOR = UIO_MAXIOV,
};

/*******************************************************************************
 * CLASSES
 ******************************************************************************/

typedef struct Buffer {
    size_t length; /* This is the data length, not the buffer length. */
    uint8_t payload[0]; /* This will cause -pendantic warnings. */
} buffer_t;

/******************************************************************************/

typedef diminuto_list_t segment_t;

static inline void * segment_payload_get(segment_t * sp) {
    return (void *)(&(((buffer_t *)diminuto_list_data(sp))->payload[0]));
}

static inline size_t segment_length_get(segment_t * sp) {
    return ((buffer_t *)diminuto_list_data(sp))->length;
}

static inline size_t segment_length_set(segment_t * sp, size_t ll) {
    return ((buffer_t *)diminuto_list_data(sp))->length = ll;
}

/******************************************************************************/

typedef diminuto_list_t record_t;

#define RECORD_INIT(_POINTER_) DIMINUTO_LIST_NULLINIT(_POINTER_)

static inline segment_t * record_segment_remove(segment_t * sp) {
    return diminuto_list_remove(sp);
}

static inline segment_t * record_segment_prepend(record_t * rp, segment_t * sp) {
    return diminuto_list_push(rp, sp);
}

static inline segment_t * record_segment_append(record_t * rp, segment_t * sp) {
    return diminuto_list_enqueue(rp, sp);
}

static inline segment_t * record_segment_insert(segment_t * op, segment_t * sp) {
    return diminuto_list_splice(op, sp);
}

static inline segment_t * record_segment_replace(segment_t * op, segment_t * sp) {
    return diminuto_list_replace(op, sp);
}

static inline segment_t * record_segment_head(record_t * rp) {
    return diminuto_list_head(rp);
}

static inline segment_t * record_segment_tail(record_t * rp) {
    return diminuto_list_tail(rp);
}

static inline segment_t * record_segment_next(record_t * rp, segment_t * sp) {
    return (diminuto_list_next(sp) == rp) ? (segment_t *)0 : diminuto_list_next(sp);
}

static size_t record_enumerate(record_t * rp)
{
    size_t nn = 0;
    segment_t * sp = (segment_t *)0;

    for (sp = record_segment_head(rp); sp != (segment_t *)0; sp = record_segment_next(rp, sp)) {
        nn += 1;
    }

    return nn;    
}

static size_t record_measure(record_t * rp)
{
    size_t ll = 0;
    segment_t * sp = (segment_t *)0;

    for (sp = record_segment_head(rp); sp != (segment_t *)0; sp = record_segment_next(rp, sp)) {
        ll += segment_length_get(sp);
    }

    return ll;    
}

/******************************************************************************/

typedef diminuto_list_t pool_t;

static segment_t * pool_segment_allocate(pool_t * pp, size_t size)
{
    buffer_t * bp = (buffer_t *)0;
    segment_t * sp = (segment_t *)0;

    if ((bp = (buffer_t *)diminuto_buffer_malloc(sizeof(buffer_t) + size)) == (void *)0) {
        /* Do nothing. */
    } else if ((sp = diminuto_list_dequeue(pp)) == (segment_t *)0) {
        diminuto_buffer_free(bp);
    } else {
        /*
         * The caller can always change this length, but most of the time
         * it is allocating a segment for a fixed length field or header
         * and this will save a lot of time and code.
         */
        bp->length = size;
        diminuto_list_dataset(sp, bp);
    }

    return sp;
}

static segment_t * pool_segment_free(pool_t * pp, segment_t * sp)
{
    diminuto_buffer_free(diminuto_list_data(sp));
    diminuto_list_dataset(sp, (void *)0);
    diminuto_list_enqueue(pp, sp);
    return (segment_t *)0;
}

/******************************************************************************/

static record_t * record_dump(FILE * fp, record_t * rp)
{
    segment_t * sp = (segment_t *)0;
    void * pp = 0;
    size_t ll = 0;
    size_t ii = 0;

    fprintf(fp, "RECORD %p:\n", rp);
    for (sp = record_segment_head(rp); sp != (segment_t *)0; sp = record_segment_next(rp, sp)) {
        pp = segment_payload_get(sp);
        ll = segment_length_get(sp);
        fprintf(fp, "  SEGMENT %p #%zu:\n", sp, ii++);
        pp = segment_payload_get(sp);
        ll = segment_length_get(sp);
        fprintf(fp, "    PAYLOAD %p [%zu]:\n", pp, ll);
        diminuto_dump_general(fp, pp, ll, 0, '.', 0, 0, 6);
    }

    return rp;    
}

static struct iovec * record_gather(record_t * rp, struct iovec va[], size_t nn)
{
    segment_t * sp = (segment_t *)0;
    size_t ii = 0;

    if (va != (struct iovec *)0) {
        for (sp = record_segment_head(rp); sp != (segment_t *)0; sp = record_segment_next(rp, sp)) {
            if (ii < nn) {
                va[ii].iov_base = segment_payload_get(sp);
                va[ii].iov_len = segment_length_get(sp);
                ii += 1;
            }
        }
    }

    return va;    
}

static ssize_t record_write(int fd, record_t * rp)
{
    ssize_t total = -1;
    size_t nn = 0;
    struct iovec * vp = (struct iovec *)0;

    nn = record_enumerate(rp);

    if (!((0 < nn) && (nn <= UIO_MAXIOV))) {
        errno = EINVAL;
        diminuto_perror("record_write: UIO_MAXIOV");
    } else if ((vp = record_gather(rp, (struct iovec *)alloca(nn * sizeof(*vp)), nn)) == (struct iovec *)0) {
        diminuto_perror("record_write: alloca");
    } else if ((total = writev(fd, record_gather(rp, vp, nn), nn)) < 0) {
        diminuto_perror("record_write: writev");
    } else {
        /* Do nothing. */
    }

    return total;
}

static int record_send(int fd, record_t * rp, diminuto_ipv4_t address, diminuto_port_t port)
{
    ssize_t total = 0;
    struct iovec * vector = (struct iovec *)0;
    struct msghdr message = { 0, };
    struct sockaddr_in sa = { 0, };
    struct sockaddr * sap = (struct sockaddr *)0;
    socklen_t length = 0;

    if (port > 0) {
        length = sizeof(sa);
        sa.sin_family = AF_INET;
        sa.sin_addr.s_addr = htonl(address);
        sa.sin_port = htons(port);
        sap = (struct sockaddr *)&sa;
    }

    message.msg_name = sap;
    message.msg_namelen = length;
    message.msg_iovlen = record_enumerate(rp);

    if (!((0 < message.msg_iovlen) && (message.msg_iovlen <= UIO_MAXIOV))) {
        errno = EINVAL;
        diminuto_perror("record_write: UIO_MAXIOV");
    } else if ((message.msg_iov = record_gather(rp, (struct iovec *)alloca(message.msg_iovlen * sizeof(*message.msg_iov)), message.msg_iovlen)) == (struct iovec *)0) {
        diminuto_perror("record_write: alloca");
    } else if ((total = sendmsg(fd, &message, 0)) < 0) {
        diminuto_perror("record_send: sendmsg");
    } else {
        /* Do nothing. */
    }

    return total;
}

/*******************************************************************************
 * HELPERS
 ******************************************************************************/

/*
 * This is just like the record enumerator but without the extra cruft
 * like the types and the List inlines that abstract out the inner workings
 * of Lists. I made it separate just to keep the proposed class stuff distinct
 * from the unit test stuff.
 */
static size_t enumerate(diminuto_list_t * lp) {
    size_t nn = 0;
    diminuto_list_t * np;
    for (np = lp->next; np != lp; np = np->next) { ++nn; }
    return nn;
}

/*******************************************************************************
 * MAIN
 ******************************************************************************/

int main(void)
{
    pool_t pool = DIMINUTO_LIST_NULLINIT(&pool);
    segment_t segments[NODES];

    SETLOGMASK();

    {
        int ii = 0;

        TEST();

        ASSERT(enumerate(&pool) == 0);
        for (ii = 0; ii < countof(segments); ++ii) {
            ASSERT(diminuto_list_enqueue(&pool, diminuto_list_init(&segments[ii])) == &segments[ii]);
        }
        ASSERT(enumerate(&pool) == NODES);

        STATUS();
    }

    {
        segment_t * sp[3];

        TEST();

        ASSERT(enumerate(&pool) == (NODES - 0));
        ASSERT(diminuto_buffer_log() >= 0);

        ASSERT((sp[0] = pool_segment_allocate(&pool, sizeof(size_t))) != (segment_t *)0);
        ASSERT(enumerate(&pool) == (NODES - 1));
        ASSERT(segment_payload_get(sp[0]) != (void *)0);
        ASSERT(segment_length_get(sp[0]) == sizeof(size_t));
        ASSERT(segment_length_set(sp[0], 0) == 0);
        ASSERT(segment_length_get(sp[0]) == 0);
        ASSERT(diminuto_buffer_log() >= 0);

        ASSERT((sp[1] = pool_segment_allocate(&pool, sizeof(uint64_t))) != (segment_t *)0);
        ASSERT(enumerate(&pool) == (NODES - 2));
        ASSERT(segment_payload_get(sp[1]) != (void *)0);
        ASSERT(segment_length_get(sp[1]) == sizeof(uint64_t));
        ASSERT(segment_length_set(sp[1], 0) == 0);
        ASSERT(segment_length_get(sp[1]) == 0);
        ASSERT(diminuto_buffer_log() >= 0);

        ASSERT((sp[2] = pool_segment_allocate(&pool, 64)) != (segment_t *)0);
        ASSERT(enumerate(&pool) == (NODES - 3));
        ASSERT(segment_payload_get(sp[2]) != (void *)0);
        ASSERT(segment_length_get(sp[2]) == 64);
        ASSERT(segment_length_set(sp[2], 0) == 0);
        ASSERT(segment_length_get(sp[2]) == 0);
        ASSERT(diminuto_buffer_log() >= 0);

        /* Order is deliberate. */

        ASSERT(pool_segment_free(&pool, sp[0]) == (segment_t *)0);
        ASSERT(enumerate(&pool) == (NODES - 2));
        ASSERT(diminuto_buffer_log() >= 0);

        ASSERT(pool_segment_free(&pool, sp[2]) == (segment_t *)0);
        ASSERT(enumerate(&pool) == (NODES - 1));
        ASSERT(diminuto_buffer_log() >= 0);

        ASSERT(pool_segment_free(&pool, sp[1]) == (segment_t *)0);
        ASSERT(enumerate(&pool) == (NODES - 0));
        ASSERT(diminuto_buffer_log() >= 0);

        STATUS();
    }

    {
        record_t record = RECORD_INIT(&record);
        static const char PAYLOAD[] = "Now is the time for all good men to come to the aid of their country.";

        {
            TEST();

            ASSERT(record_enumerate(&record) == 0);
            ASSERT(record_measure(&record) == 0);

            STATUS();
        }

        /* Payload */

        {
            segment_t * sp;
            segment_t * tp;
            char * bp;
            size_t ll;

            TEST();

            ASSERT((sp = pool_segment_allocate(&pool, sizeof(PAYLOAD) * 2 /* Arbitary. */)) != (segment_t *)0);
            ASSERT(segment_length_get(sp) == (sizeof(PAYLOAD) * 2));
            ASSERT((bp = (char *)segment_payload_get(sp)) != (char *)0);
            strncpy(bp, PAYLOAD, sizeof(PAYLOAD));
            ASSERT((ll = (strlen(bp) + 1 /* Including NUL. */)) > 0);
            ASSERT(segment_length_set(sp, ll) == ll);
            ASSERT(segment_length_get(sp) == ll);
            ASSERT(record_segment_append(&record, sp) == sp);

            ASSERT(record_enumerate(&record) == 1);
            ASSERT(record_measure(&record) > 0);
            ASSERT(record_dump(stderr, &record) == &record);

            STATUS();
        }

        /* Length Payload Checksum */

        {
            char * bp;
            size_t length;
            uint8_t a;
            uint8_t b;
            uint16_t checksum;
            segment_t * sp;

            TEST();

            ASSERT((sp = record_segment_head(&record)) != (segment_t *)0);
            ASSERT((bp = (char *)segment_payload_get(sp)) != (char *)0);
            ASSERT((length = segment_length_get(sp)) > 0);
            a = b = 0;
            checksum = diminuto_fletcher_16(bp, length, &a, &b);

            ASSERT((sp = pool_segment_allocate(&pool, sizeof(length))) != (segment_t *)0);
            ASSERT(segment_length_get(sp) == sizeof(length));
            ASSERT((bp = (char *)segment_payload_get(sp)) != (char *)0);
            memcpy(bp, &length, sizeof(length));
            ASSERT(record_segment_prepend(&record, sp) == sp);
 
            ASSERT((sp = pool_segment_allocate(&pool, sizeof(checksum))) != (segment_t *)0);
            ASSERT(segment_length_get(sp) == sizeof(checksum));
            ASSERT((bp = (char *)segment_payload_get(sp)) != (char *)0);
            memcpy(bp, &checksum, sizeof(checksum));
            ASSERT(record_segment_append(&record, sp) == sp);
        
            ASSERT(record_enumerate(&record) == 3);
            ASSERT(record_measure(&record) > 0);
            ASSERT(record_dump(stderr, &record) == &record);

        }

        STATUS();
    }

    EXIT();
}
