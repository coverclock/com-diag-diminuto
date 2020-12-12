/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the scatter/gather socket capability.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test to evaluate whether Diminuto Lists might be used
 * to implement the gather portion of the scatter/gather capability of
 * the vector I/O system calls referenced below and minimize the amount
 * of data copying necessary to consolidate a bunch of fields in a layered
 * protocol into a continguous data packet. This approach is, IMO, mostly
 * useful on the gather side where writev(2) and sendmsg(2) are used. It
 * was also an excuse to try out the different I/O system calls that
 * implement scatter/gather.
 *
 * I find the command
 *
 * valgrind --leak-check=full --show-leak-kinds=all unittest-ipc-scattergather
 *
 * useful for insuring I've covered all the memory allocation and freeing
 * correctly.
 *
 * SEE ALSO
 *
 * tst/unittest-ipc-ancillary.c
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
 *
 * POSIX 1003.1-2001 (readv, writev)
 *
 * Single UNIX Specification version 4 (POSIX:2008), Issue 7 (sendmsg, recvmsg)
 *
 * <linux/uio.h> 5.4.0: UIO_MAXIOV=1024 UIO_FASTIOV=8
 *
 * POSIX 1003.1g 5.4.1.1: "UIO_MAXIOV shall be at least 16"
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_list.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_containerof.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_dump.h"
#include "com/diag/diminuto/diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_buffer.h"
#include "com/diag/diminuto/diminuto_fletcher.h"
#include "com/diag/diminuto/diminuto_reaper.h"
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
    NODES = UIO_MAXIOV,     /* Overkill. */
    VECTOR = UIO_MAXIOV,    /* Maximum. */
};

/*******************************************************************************
 * TYPES
 ******************************************************************************/

typedef struct Buffer {
    /*
     * The type of the length field is way overkill as is uint32_t
     * (although uint16_t is too small). But it insures the caller
     * of the alignment. uint64_t will be double the size of size_t
     * on some ARM platforms (especially those with 32-bit kernels)
     * but the same as size_t on 64-bit ARMs and all x86_64s.
     */
    uint64_t length; /* This is the data length, not the buffer length. */
    /*
     * Insure that the payload portion of the buffer is 8-byte aligned.
     * This guarantees that the caller can use the payload field to
     * store anything requiring 8-byte alignment or less.
     */
    uint64_t payload[0]; /* This will cause -pendantic warnings. */
} buffer_t;

/*
 * Everything other than a Buffer is a List node. But the different
 * type names help me keep straight the role that the List node is
 * serving in the API. Don't mix them up, otherwise wackiness will
 * ensue.
 */

typedef diminuto_list_t pool_t;

typedef diminuto_list_t segment_t;

typedef diminuto_list_t record_t;

/*******************************************************************************
 * GENERATORS
 ******************************************************************************/

#define POOL_INIT(_POINTER_) DIMINUTO_LIST_NULLINIT(_POINTER_)

#define SEGMENT_INIT(_POINTER_) DIMINUTO_LIST_NULLINIT(_POINTER_)

#define RECORD_INIT(_POINTER_) DIMINUTO_LIST_NULLINIT(_POINTER_)

/*******************************************************************************
 * Pool
 ******************************************************************************/

/*
 * This test program isn't multi-threaded, but I wanted to make the
 * pool thread safe for when/if I promote this to a mainstream feature
 * in Diminuto.
 */
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static inline pool_t * pool_init(pool_t * pp) {
    return diminuto_list_nullinit(pp);
}

static void pool_fini(pool_t * pp)
{
    diminuto_list_t * np = (diminuto_list_t *)0;

    while ((np = diminuto_list_head(pp)) != (diminuto_list_t *)0) {
        diminuto_list_remove(np);
    }
}

static diminuto_list_t * pool_get(pool_t * pp)
{
    diminuto_list_t * np = (diminuto_list_t *)0;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
        np = diminuto_list_dataset(diminuto_list_dequeue(pp), (void *)0);
    DIMINUTO_CRITICAL_SECTION_END;

    return np;
}

static void pool_put(pool_t * pp, diminuto_list_t * np)
{
    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
        diminuto_list_enqueue(pp, diminuto_list_dataset(np, (void *)0));
    DIMINUTO_CRITICAL_SECTION_END;
}

static pool_t * pool_populate(pool_t * pp, segment_t sa[], size_t sn)
{
    int ii = 0;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
        for (ii = 0; ii < sn; ++ii) {
            diminuto_list_enqueue(pp, diminuto_list_nullinit(&sa[ii]));
        }
    DIMINUTO_CRITICAL_SECTION_END;

    return pp;
}

/*******************************************************************************
 * Segment
 ******************************************************************************/

static inline segment_t * segment_init(segment_t * sp) {
    return diminuto_list_nullinit(sp);
}

static inline segment_t * segment_fini(segment_t * sp) {
    return sp;
}

/*
 * The address returned by this function will be aligned on at least
 * an eight-byte boundary.
 */
static inline void * segment_payload_get(segment_t * sp) {
    return (void *)(&(((buffer_t *)diminuto_list_data(sp))->payload[0]));
}

static inline size_t segment_length_get(segment_t * sp) {
    return ((buffer_t *)diminuto_list_data(sp))->length;
}

static inline size_t segment_length_set(segment_t * sp, size_t ll) {
    return ((buffer_t *)diminuto_list_data(sp))->length = ll;
}

static segment_t * segment_allocate(pool_t * pp, size_t size)
{
    buffer_t * bp = (buffer_t *)0;
    segment_t * sp = (segment_t *)0;

    /*
     * This function returns a buffer whose beginning address is
     * aligned on at least an eight-byte boundary. The header we
     * put before the payload portion is eight bytes long.
     */
    if ((bp = (buffer_t *)diminuto_buffer_malloc(sizeof(buffer_t) + size)) == (void *)0) {
        /* Do nothing. */
    } else if ((sp = pool_get(pp)) == (segment_t *)0) {
        diminuto_buffer_free(bp);
        errno = ENOMEM;
        diminuto_perror("segment_allocate");
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

static void segment_free(pool_t * pp, segment_t * sp)
{
    diminuto_buffer_free(diminuto_list_data(sp));
    pool_put(pp, sp);
}

/*******************************************************************************
 * Record
 ******************************************************************************/

static inline record_t * record_init(record_t * rp) {
    return diminuto_list_nullinit(rp);
}

static inline record_t * record_fini(record_t * rp) {
    return rp;
}

static inline segment_t * record_segment_remove(record_t * rp /* Unused. */, segment_t * sp) {
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
        if (diminuto_list_data(sp) != (void *)0) {
            ll += segment_length_get(sp);
        }
    }

    return ll;    
}

static record_t * record_dump(FILE * fp, record_t * rp)
{
    segment_t * sp = (segment_t *)0;
    void * pp = 0;
    size_t ii = 0;
    size_t ll = 0;
    size_t tt = 0;

    fprintf(fp, "RECORD %p:\n", rp);
    for (sp = record_segment_head(rp); sp != (segment_t *)0; sp = record_segment_next(rp, sp)) {
        fprintf(fp, "  SEGMENT %p: #%zu\n", sp, ii++);
        pp = segment_payload_get(sp);
        ll = segment_length_get(sp);
        tt += ll;
        fprintf(fp, "    PAYLOAD %p: [%zu]\n", pp, ll);
        diminuto_dump_general(fp, pp, ll, 0, '.', 0, 0, 6);
    }
    fprintf(fp, "  TOTAL %p: #%zu [%zu]\n", rp, ii, tt);

    return rp;    
}

static struct iovec * record_vectorize(record_t * rp, struct iovec va[], size_t nn)
{
    segment_t * sp = (segment_t *)0;
    size_t ii = 0;

    for (sp = record_segment_head(rp); sp != (segment_t *)0; sp = record_segment_next(rp, sp)) {
        if (ii < nn) {
            va[ii].iov_base = segment_payload_get(sp);
            va[ii].iov_len = segment_length_get(sp);
            ii += 1;
        } else {
            /*
             * It is not an error to provide a vector that
             * is too short. It's probably a bug, but might
             * be intentional. So we return an error, but
             * we don't emit an error message.
             */
            errno = ETOOMANYREFS;
            return (struct iovec *)0;
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

    if (!((0 < nn) && (nn <= VECTOR))) {
        errno = EINVAL;
        diminuto_perror("record_write: enumeration");
    } else if ((vp = (struct iovec *)alloca(nn * sizeof(*vp))) == (struct iovec *)0) {
        diminuto_perror("record_write: alloca");
    } else if (record_vectorize(rp, vp, nn) == (struct iovec *)0) {
        diminuto_perror("record_write: record_vectorize");
    } else if ((total = writev(fd, vp, nn)) < 0) {
        diminuto_perror("record_write: writev");
    } else {
        /* Do nothing. */
    }

    return total;
}

static ssize_t record_read(int fd, record_t * rp)
{
    ssize_t total = -1;
    size_t nn = 0;
    struct iovec * vp = (struct iovec *)0;

    nn = record_enumerate(rp);

    if (!((0 < nn) && (nn <= VECTOR))) {
        errno = EINVAL;
        diminuto_perror("record_read: enumeration");
    } else if ((vp = (struct iovec *)alloca(nn * sizeof(*vp))) == (struct iovec *)0) {
        diminuto_perror("record_write: alloca");
    } else if (record_vectorize(rp, vp, nn) == (struct iovec *)0) {
        diminuto_perror("record_read: record_vectorize");
    } else if ((total = readv(fd, vp, nn)) < 0) {
        diminuto_perror("record_write: writev");
    } else {
        /* Do nothing. */
    }

    return total;
}

static ssize_t record_send(int fd, record_t * rp, diminuto_ipv4_t address, diminuto_port_t port)
{
    ssize_t total = 0;
    struct iovec * vp = (struct iovec *)0;
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

    if (!((0 < message.msg_iovlen) && (message.msg_iovlen <= VECTOR))) {
        errno = EINVAL;
        diminuto_perror("record_send: enumeration");
    } else if ((vp = (struct iovec *)alloca(message.msg_iovlen * sizeof(*message.msg_iov))) == (struct iovec *)0) {
        diminuto_perror("record_send: alloca");
    } else if ((message.msg_iov = record_vectorize(rp, vp, message.msg_iovlen)) == (struct iovec *)0) {
        diminuto_perror("record_send: record_vectorize");
    } else if ((total = sendmsg(fd, &message, 0)) < 0) {
        diminuto_perror("record_send: sendmsg");
    } else {
        /* Do nothing. */
    }

    return total;
}

static ssize_t record_receive(int fd, record_t * rp)
{
    ssize_t total = 0;
    struct iovec * vp = (struct iovec *)0;
    struct msghdr message = { 0, };

    message.msg_name = (struct sockaddr *)0;
    message.msg_namelen = 0;
    message.msg_iovlen = record_enumerate(rp);

    if (!((0 < message.msg_iovlen) && (message.msg_iovlen <= VECTOR))) {
        errno = EINVAL;
        diminuto_perror("record_receive: enumeration");
    } else if ((vp = (struct iovec *)alloca(message.msg_iovlen * sizeof(*message.msg_iov))) == (struct iovec *)0) {
        diminuto_perror("record_receive: alloca");
    } else if ((message.msg_iov = record_vectorize(rp, vp, message.msg_iovlen)) == (struct iovec *)0) {
        diminuto_perror("record_receive: record_vectorize");
    } else if ((total = recvmsg(fd, &message, 0)) < 0) {
        diminuto_perror("record_receive: sendmsg");
    } else {
        /* Do nothing. */
    }

    return total;
}

static inline record_t * record_segment_free(pool_t * pp, record_t * rp, segment_t * sp)
{
    (void)segment_free(pp, record_segment_remove(rp, sp));
    return rp;
}

static record_t * record_segments_free(pool_t * pp, record_t * rp)
{
    segment_t * sp = (segment_t *)0;

    while ((sp = record_segment_head(rp)) != (segment_t *)0) {
        (void)record_segment_free(pp, rp, sp);
    }

    return rp;    
}

static record_t * record_allocate(pool_t * pp)
{
    record_t * rp = (record_t *)0;

    if ((rp = pool_get(pp)) == (record_t *)0) {
        errno = ENOMEM;
        diminuto_perror("record_allocate");
    }

    return rp;
}

static void record_free(pool_t * pp, record_t * rp)
{
    pool_put(pp, record_segments_free(pp, rp));
}

/*******************************************************************************
 * GLOBALS
 ******************************************************************************/

static pool_t pool;

static segment_t segments[NODES];

/*******************************************************************************
 * CATCHERS
 ******************************************************************************/

/*
 * In any real application, we would be passing the binary fields in
 * network byte order, and the field lengths would be consistent. Here,
 * we pass everything in host byte order, and the length field is eight
 * bytes on the x86_64 and four bytes on an ARM with a 32-bit kernel. We
 * would also not assume below that the entire packet had been read or
 * received in a single system call. I didn't make any effort to optimize
 * or minimize the error leg code paths like I would in production code.
 * I try several different methods to receive and parse data streams that
 * have been sent using writev(2) or sendmsg(2), the gather system calls.
 * In all cases, the data packet on the wire has the following format.
 *
 * PACKET: ADDRESS[4], PORT[2], LENGTH[4 or 8], DATA[LENGTH], CHECKSUM[2]
 */

enum Offsets {
    ADDRESS = 0,
    PORT    = ADDRESS   + sizeof(diminuto_ipv4_t),
    LENGTH  = PORT      + sizeof(diminuto_port_t),
    DATA    = LENGTH    + sizeof(size_t),
};

enum Lengths {
    HEADER  = DATA,
    MINIMUM = HEADER   + sizeof(uint16_t), /* Zero length payload. */
    MAXIMUM = MINIMUM   + 256, /* Arbitrary. */
};

/*
 * Stream Socket Server
 */
int streamserver(int listensocket)
{
    int result = 1;
    int streamsocket;
    uint8_t * bp;
    ssize_t total;
    diminuto_ipv4_t address;
    diminuto_port_t port;
    size_t length;
    uint16_t checksum;
    diminuto_ipv4_buffer_t printable;
    uint8_t a;
    uint8_t b;
    uint16_t expected;
    struct iovec vector[5];
    size_t minimum;
    record_t * rp;
    segment_t * sp;
    int ii;

    do {

        if ((streamsocket = diminuto_ipc4_stream_accept(listensocket)) < 0) {
            break;
        }

        /*
         * This approach reads the packet a field at a time. Reading multiple
         * fields at a time, such as the front matter that preceeds the payload,
         * into a structure isn't as easy as it sounds. Structs can have holes
         * due to the memory alignment requirements of their individial fields,
         * while bits on the wire have no such gaps.
         */

        fprintf(stderr, "streamserver: ONE\n");

        fprintf(stderr, "READ:\n");

        if ((total = diminuto_ipc4_stream_read_generic(streamsocket, &address, sizeof(address), sizeof(address))) != sizeof(address)) {
            errno = EINVAL;
            diminuto_perror("short");
            (void)diminuto_ipc_close(streamsocket);
            break;
        }
        fprintf(stderr, "  ADDRESS: %s\n", diminuto_ipc4_address2string(address, printable, sizeof(printable)));

        if ((total = diminuto_ipc4_stream_read_generic(streamsocket, &port, sizeof(port), sizeof(port))) != sizeof(port)) {
            errno = EINVAL;
            diminuto_perror("short");
            (void)diminuto_ipc_close(streamsocket);
            break;
        }
        fprintf(stderr, "  PORT: %d\n", port);

        if ((total = diminuto_ipc4_stream_read_generic(streamsocket, &length, sizeof(length), sizeof(length))) != sizeof(length)) {
            errno = EINVAL;
            diminuto_perror("short");
            (void)diminuto_ipc_close(streamsocket);
            break;
        }
        fprintf(stderr, "  LENGTH: %zu\n", length);

        if (length > (MAXIMUM - MINIMUM)) {
            errno = EINVAL;
            diminuto_perror("length");
            (void)diminuto_ipc_close(streamsocket);
            break;
        }

        if (length > 0) {

            bp = (uint8_t *)diminuto_buffer_malloc(length);
            if (bp == (uint8_t *)0) {
                (void)diminuto_ipc_close(streamsocket);
                break;
            }

            if ((total = diminuto_ipc4_stream_read_generic(streamsocket, bp, length, length)) != length) {
                errno = EINVAL;
                diminuto_perror("short");
                (void)diminuto_ipc_close(streamsocket);
                break;
            }
            fprintf(stderr, "  DATA:\n");
            diminuto_dump_general(stderr, bp, total, 0, '.', 0, 0, 4);

        }

        if ((total = diminuto_ipc4_stream_read_generic(streamsocket, &checksum, sizeof(checksum), sizeof(checksum))) != sizeof(checksum)) {
            errno = EINVAL;
            diminuto_perror("short");
            if (length > 0) {
                diminuto_buffer_free(bp);
            }
            break;
        }
        fprintf(stderr, "  CHECKSUM: 0x%x\n", checksum);

        a = b = 0;
        expected = diminuto_fletcher_16(bp, length, &a, &b);
        fprintf(stderr, "  EXPECTED: 0x%x\n", expected);
        if (checksum != expected) {
            errno = EINVAL;
            diminuto_perror("checksum");
            if (length > 0) {
                diminuto_buffer_free(bp);
            }
            break;
        }

        if (length > 0) {
            diminuto_buffer_free(bp);
        }

        /*
         * This approach uses readv(2) to read the first three fields and
         * scatter them into their variables. Then it uses readv(2) again to
         * read the payload (now that we know how long it is) and the
         * checksum. I don't think this will work with datagrams, which are
         * treated as one record.
         */

        minimum = 0;

        vector[0].iov_base = &address;
        vector[0].iov_len = sizeof(address);
        minimum += vector[0].iov_len;

        vector[1].iov_base = &port;
        vector[1].iov_len = sizeof(port);
        minimum += vector[1].iov_len;

        vector[2].iov_base = &length;
        vector[2].iov_len = sizeof(length);
        minimum += vector[2].iov_len;

        if ((total = readv(streamsocket, &vector[0], 3)) < minimum) {
            errno = EINVAL;
            diminuto_perror("short");
            (void)diminuto_ipc_close(streamsocket);
            break;
        }

        fprintf(stderr, "streamserver: TWO\n");

        fprintf(stderr, "READ [%zd]:\n", total);

        fprintf(stderr, "  ADDRESS: %s\n", diminuto_ipc4_address2string(address, printable, sizeof(printable)));

        fprintf(stderr, "  PORT: %d\n", port);

        fprintf(stderr, "  LENGTH: %zu\n", length);

        minimum = 0;
        ii = 0;

        if (length > 0) {

            bp = (uint8_t *)diminuto_buffer_malloc(length);
            if (bp == (uint8_t *)0) {
                (void)diminuto_ipc_close(streamsocket);
                break;
            }
            vector[ii].iov_base = bp;
            vector[ii].iov_len = length;
            minimum += vector[ii].iov_len;
            ii += 1;
 
        }

        vector[ii].iov_base = &checksum;
        vector[ii].iov_len = sizeof(checksum);
        minimum += vector[ii].iov_len;
        ii += 1;

        if ((total = readv(streamsocket, &vector[0], ii)) < minimum) {
            errno = EINVAL;
            diminuto_perror("short");
            if (length > 0) {
                diminuto_buffer_free(bp);
            }
            break;
        }

        fprintf(stderr, "READ [%zd]:\n", total);

        fprintf(stderr, "  DATA:\n");
        diminuto_dump_general(stderr, bp, length, 0, '.', 0, 0, 4);

        fprintf(stderr, "  CHECKSUM: 0x%x\n", checksum);

        a = b = 0;
        expected = diminuto_fletcher_16(bp, length, &a, &b);
        fprintf(stderr, "  EXPECTED: 0x%x\n", expected);
        if (expected != checksum) {
            errno = EINVAL;
            diminuto_perror("checksum");
            if (length > 0) {
                diminuto_buffer_free(bp);
            }
            (void)diminuto_ipc_close(streamsocket);
            break;
        }

        if (length > 0) {
            diminuto_buffer_free(bp);
        }

        /*
         * This approach uses the record and segment facility just like
         * the sender, with the complication that we have to deal with
         * zero-length payloads. (This test program doesn't actually
         * send zero length payloads, so that path has not actually been
         * tested). This would only work for stream sockets, since we
         * read the incoming packet in two parts: the header and the
         * payload plus checksum.
         */

        if ((rp = record_allocate(&pool)) == (record_t *)0) {
            (void)diminuto_ipc_close(streamsocket);
            break;
        }

        if ((sp = segment_allocate(&pool, sizeof(diminuto_ipv4_t))) == (segment_t *)0) {
            record_free(&pool, rp);
            (void)diminuto_ipc_close(streamsocket);
            break;
        }
        record_segment_append(rp, sp);

        if ((sp = segment_allocate(&pool, sizeof(diminuto_port_t))) == (segment_t *)0) {
            record_free(&pool, rp);
            (void)diminuto_ipc_close(streamsocket);
            break;
        }
        record_segment_append(rp, sp);

        if ((sp = segment_allocate(&pool, sizeof(size_t))) == (segment_t *)0) {
            record_free(&pool, rp);
            (void)diminuto_ipc_close(streamsocket);
            break;
        }
        record_segment_append(rp, sp);

        if ((total = record_read(streamsocket, rp)) != HEADER) {
            errno = EINVAL;
            diminuto_perror("short");
            record_free(&pool, rp);
            (void)diminuto_ipc_close(streamsocket);
            break;
        }

        fprintf(stderr, "streamserver: THREE\n");

        record_dump(stderr, rp);
        fprintf(stderr, "READ [%zd]:\n", total);

        address = *(diminuto_ipv4_t *)segment_payload_get(sp = record_segment_head(rp));
        fprintf(stderr, "  ADDRESS: %s\n", diminuto_ipc4_address2string(address, printable, sizeof(printable)));
        record_segment_free(&pool, rp, sp);

        port = *(diminuto_port_t *)segment_payload_get(sp = record_segment_head(rp));
        fprintf(stderr, "  PORT: %d\n", port);
        record_segment_free(&pool, rp, sp);

        length = *(size_t *)segment_payload_get(sp = record_segment_head(rp));
        fprintf(stderr, "  LENGTH: %zu\n", length);
        record_segment_free(&pool, rp, sp);

        if (length <= 0) {
            /* Do nothing. */
        } else if ((sp = segment_allocate(&pool, length)) == (segment_t *)0) {
            record_free(&pool, rp);
            (void)diminuto_ipc_close(streamsocket);
            break;
        } else {
            record_segment_append(rp, sp);
        }

        if ((sp = segment_allocate(&pool, sizeof(uint16_t))) == (segment_t *)0) {
            record_free(&pool, rp);
            (void)diminuto_ipc_close(streamsocket);
            break;
        }
        record_segment_append(rp, sp);

        if ((total = record_read(streamsocket, rp)) != (length + sizeof(uint16_t))) {
            errno = EINVAL;
            diminuto_perror("short");
            record_free(&pool, rp);
            (void)diminuto_ipc_close(streamsocket);
            break;
        }

        record_dump(stderr, rp);
        fprintf(stderr, "READ [%zd]:\n", total);

        if (length > 0) {
            fprintf(stderr, "  DATA:\n");
            bp = (uint8_t *)segment_payload_get(sp = record_segment_head(rp));
            diminuto_dump_general(stderr, bp, length, 0, '.', 0, 0, 4);
            record_segment_free(&pool, rp, sp);
        } else {
            bp = (uint8_t *)0;
        }

        checksum = *(uint16_t *)segment_payload_get(sp = record_segment_head(rp));
        fprintf(stderr, "  CHECKSUM: 0x%x\n", checksum);
        record_segment_free(&pool, rp, sp);

        a = b = 0;
        expected = diminuto_fletcher_16(bp, length, &a, &b);
        fprintf(stderr, "  EXPECTED: 0x%x\n", expected);
        if (expected != checksum) {
            errno = EINVAL;
            diminuto_perror("checksum");
            record_free(&pool, rp);
            (void)diminuto_ipc_close(streamsocket);
            break;
        }

        record_free(&pool, rp);

        /*
         * This approach just reads the entire packet into a single
         * contiguous buffer. Since the payload is of variable length,
         * if we were receiving multiple messages using read(2) we
         * would have to effectively parse the input stream since
         * read(2) might return fewer or more bytes than were written
         * in a single write(2) call from the far end. That's why it's
         * called a stream versus a datagram. And why this approach has
         * to be the last one; otherwise we'd would consume part or all
         * of the following packet.
         */

        bp = (uint8_t *)diminuto_buffer_malloc(MAXIMUM);
        if (bp == (uint8_t *)0) {
            (void)diminuto_ipc_close(streamsocket);
            break;
        }

        if ((total = diminuto_ipc4_stream_read(streamsocket, bp, MAXIMUM)) < MINIMUM) {
            errno = EINVAL;
            diminuto_perror("short");
            diminuto_buffer_free(bp);
            (void)diminuto_ipc_close(streamsocket);
            break;
        }

        fprintf(stderr, "streamserver: FOUR\n");

        fprintf(stderr, "READ [%zd]:\n", total);
        diminuto_dump_general(stderr, bp, total, 0, '.', 0, 0, 2);

        memcpy(&address, &bp[ADDRESS], sizeof(address));
        fprintf(stderr, "    ADDRESS: %s\n", diminuto_ipc4_address2string(address, printable, sizeof(printable)));

        memcpy(&port, &bp[PORT], sizeof(port));
        fprintf(stderr, "    PORT: %d\n", port);

        memcpy(&length, &bp[LENGTH], sizeof(length));
        fprintf(stderr, "    LENGTH: %zu\n", length);
        if ((MINIMUM + length) != total) {
            errno = EINVAL;
            diminuto_perror("length");
            diminuto_buffer_free(bp);
            (void)diminuto_ipc_close(streamsocket);
            break;
        }

        memcpy(&checksum, &bp[DATA + length], sizeof(checksum));
        fprintf(stderr, "    CHECKSUM: 0x%x\n", checksum);

        a = b = 0;
        expected = diminuto_fletcher_16(&bp[DATA], length, &a, &b);
        fprintf(stderr, "    EXPECTED: 0x%x\n", expected);
        if (checksum != expected) {
            errno = EINVAL;
            diminuto_perror("checksum");
            diminuto_buffer_free(bp);
            (void)diminuto_ipc_close(streamsocket);
            break;
        }

        diminuto_buffer_free(bp);

        /*
         * Done.
         */

        if (diminuto_ipc_close(streamsocket) < 0) {
            break;
        }

        result = 0;

    } while (0);

    return result; 
}

/*
 * Datagram Peer
 */
int datagrampeer(int datagramsocket)
{
    int result = 1;
    ssize_t total;
    diminuto_ipv4_t address;
    diminuto_port_t port;
    size_t length;
    uint8_t * bp;
    uint16_t checksum;
    diminuto_ipv4_buffer_t printable;
    uint8_t a;
    uint8_t b;
    uint16_t expected;
    struct msghdr message;
    struct iovec vector[4];
    size_t maximum;
    record_t * rp;
    segment_t * sp;
    int ii;

    do {

        /*
         * This approach receives the first datagram all in one piece and
         * parses it apart.
         */

        bp = (uint8_t *)diminuto_buffer_malloc(MAXIMUM);
        if (bp == (uint8_t *)0) {
            break;
        }

        if ((total = diminuto_ipc4_datagram_receive(datagramsocket, bp, MAXIMUM)) < MINIMUM) {
            errno = EINVAL;
            diminuto_perror("short");
            diminuto_buffer_free(bp);
            break;
        }

        fprintf(stderr, "datagrampeer: ONE\n");

        fprintf(stderr, "RECEIVE [%zd]:\n", total);
        diminuto_dump_general(stderr, bp, total, 0, '.', 0, 0, 2);

        memcpy(&address, &bp[ADDRESS], sizeof(address));
        fprintf(stderr, "    ADDRESS: %s\n", diminuto_ipc4_address2string(address, printable, sizeof(printable)));

        memcpy(&port, &bp[PORT], sizeof(port));
        fprintf(stderr, "    PORT: %d\n", port);

        memcpy(&length, &bp[LENGTH], sizeof(length));
        fprintf(stderr, "    LENGTH: %zu\n", length);
        if ((length + MINIMUM) != total) {
            errno = EINVAL;
            diminuto_perror("length");
            diminuto_buffer_free(bp);
            break;
        }
    
        memcpy(&checksum, &bp[DATA +  length], sizeof(checksum));
        fprintf(stderr, "    CHECKSUM: 0x%x\n", checksum);

        a = b = 0;
        expected = diminuto_fletcher_16(&bp[DATA], length, &a, &b);
        fprintf(stderr, "    EXPECTED: 0x%x\n", expected);
        if (expected != checksum) {
            errno = EINVAL;
            diminuto_perror("checksum");
            diminuto_buffer_free(bp);
            break;
        }

        diminuto_buffer_free(bp);

        /*
         * This approach receives the second datagram and use recvmsg(2) to
         * scatter the fields. Because we don't know the length of the variable
         * length payload, we have to receive the payload and checksum
         * into a single buffer and extract the checksum afterwards.
         */

        message.msg_name = (void *)0;
        message.msg_namelen = 0;
        message.msg_iov = vector;
        message.msg_iovlen = diminuto_countof(vector);
        message.msg_control = (void *)0;
        message.msg_controllen = 0;
        message.msg_flags = 0;

        vector[0].iov_base = &address;
        vector[0].iov_len = sizeof(address);

        vector[1].iov_base = &port;
        vector[1].iov_len = sizeof(port);

        vector[2].iov_base = &length;
        vector[2].iov_len = sizeof(length);

        maximum = MAXIMUM - MINIMUM + sizeof(uint16_t);
        bp = (uint8_t *)diminuto_buffer_malloc(maximum);
        if (bp == (uint8_t *)0) {
            break;
        }
        vector[3].iov_base = bp;
        vector[3].iov_len = maximum;

        if ((total = recvmsg(datagramsocket, &message, 0)) < MINIMUM) {
            errno = EINVAL;
            diminuto_perror("short");
            diminuto_buffer_free(bp);
            break;
        }

        fprintf(stderr, "datagrampeer: TWO\n");

        fprintf(stderr, "RECEIVE [%zd]:\n", total);

        fprintf(stderr, "  ADDRESS: %s\n", diminuto_ipc4_address2string(address, printable, sizeof(printable)));

        fprintf(stderr, "  PORT: %d\n", port);

        fprintf(stderr, "  LENGTH: %zu\n", length);
        if ((length + MINIMUM) != total) {
            errno = EINVAL;
            diminuto_perror("length");
            diminuto_buffer_free(bp);
            break;
        }

        fprintf(stderr, "  DATA:\n");
        diminuto_dump_general(stderr, bp, length, 0, '.', 0, 0, 4);

        memcpy(&checksum, &bp[length], sizeof(checksum));
        fprintf(stderr, "  CHECKSUM: 0x%x\n", checksum);

        a = b = 0;
        expected = diminuto_fletcher_16(bp, length, &a, &b);
        fprintf(stderr, "  EXPECTED: 0x%x\n", expected);
        if (expected != checksum) {
            errno = EINVAL;
            diminuto_perror("checksum");
            diminuto_buffer_free(bp);
            break;
        }

        diminuto_buffer_free(bp);

        /*
         * This approach is similar to the one above except it uses the
         * record and segment paradigm which leaves the checksum as the
         * only field we have to handle as a special case. Because we
         * receive the entire datagram all at once (that's how datagrams
         * work) we don't know the length of the last segment that
         * contains both the payload and the checksum until we interrogate
         * the length segment.
         */

        if ((rp = record_allocate(&pool)) == (record_t *)0) {
            break;
        }

        if ((sp = segment_allocate(&pool, sizeof(diminuto_ipv4_t))) == (segment_t *)0) {
            record_free(&pool, rp);
            break;
        }
        record_segment_append(rp, sp);

        if ((sp = segment_allocate(&pool, sizeof(diminuto_port_t))) == (segment_t *)0) {
            record_free(&pool, rp);
            break;
        }
        record_segment_append(rp, sp);

        if ((sp = segment_allocate(&pool, sizeof(size_t))) == (segment_t *)0) {
            record_free(&pool, rp);
            break;
        }
        record_segment_append(rp, sp);

        maximum = MAXIMUM - MINIMUM + sizeof(uint16_t);
        if ((sp = segment_allocate(&pool, maximum)) == (segment_t *)0) {
            record_free(&pool, rp);
            break;
        }
        record_segment_append(rp, sp);

        if ((total = record_receive(datagramsocket, rp)) < MINIMUM) {
            errno = EINVAL;
            diminuto_perror("short");
            record_free(&pool, rp);
            break;
        }

        fprintf(stderr, "datagrampeer: THREE\n");

        record_dump(stderr, rp);
        fprintf(stderr, "RECEIVE [%zd]:\n", total);

        address = *(diminuto_ipv4_t *)segment_payload_get(sp = record_segment_head(rp));
        fprintf(stderr, "  ADDRESS: %s\n", diminuto_ipc4_address2string(address, printable, sizeof(printable)));
        record_segment_free(&pool, rp, sp);

        port = *(diminuto_port_t *)segment_payload_get(sp = record_segment_head(rp));
        fprintf(stderr, "  PORT: %d\n", port);
        record_segment_free(&pool, rp, sp);

        length = *(size_t *)segment_payload_get(sp = record_segment_head(rp));
        fprintf(stderr, "  LENGTH: %zu\n", length);
        record_segment_free(&pool, rp, sp);
        if ((HEADER + length + sizeof(uint16_t)) != total) {
            errno = EINVAL;
            diminuto_perror("length");
            record_free(&pool, rp);
            break;
        }

        /* Kinda overkill to use a segment here but stick with the paradigm. */

        segment_length_set(record_segment_head(rp), length);

        if ((sp = segment_allocate(&pool, sizeof(uint16_t))) == (segment_t *)0) {
            record_free(&pool, rp);
            break;
        }
        bp = (uint8_t *)segment_payload_get(record_segment_head(rp));
        memcpy(segment_payload_get(sp), &bp[length], segment_length_get(sp));
        record_segment_append(rp, sp);

        /* There's a checksum segment now and the data length is correct. */

        if (length > 0) {
            fprintf(stderr, "  DATA:\n");
            bp = (uint8_t *)segment_payload_get(sp = record_segment_head(rp));
            diminuto_dump_general(stderr, bp, length, 0, '.', 0, 0, 4);
            record_segment_free(&pool, rp, sp);
        } else {
            bp = (uint8_t *)0;
        }

        checksum = *(uint16_t *)segment_payload_get(sp = record_segment_head(rp));
        fprintf(stderr, "  CHECKSUM: 0x%x\n", checksum);

        a = b = 0;
        expected = diminuto_fletcher_16(bp, length, &a, &b);
        fprintf(stderr, "  EXPECTED: 0x%x\n", expected);
        if (expected != checksum) {
            errno = EINVAL;
            diminuto_perror("checksum");
            record_free(&pool, rp);
            break;
        }

        record_free(&pool, rp);

        /*
         * Done.
         */

        result = 0;

    } while (0);

    return result;
}

/*******************************************************************************
 * MAIN
 ******************************************************************************/

int main(void)
{
    record_t * rp;
    diminuto_ipv4_t address;
    diminuto_port_t streamport;
    diminuto_port_t datagramport;
    int listensocket;
    int datagramsocket;
    pid_t streampid;
    pid_t datagrampid;
    static const char DATA[] = "Now is the time for all good men to come to the aid of their country.";

    SETLOGMASK();

    {
        segment_t segment = SEGMENT_INIT(&segment);

        TEST();

        /* Everything is a List node. */
        ASSERT(record_enumerate(&segment) == 0);
        ASSERT(record_measure(&segment) == 0);

        STATUS();
    }

    {
        segment_t segment;

        TEST();

        ASSERT(segment_init(&segment) == &segment);
        /* Everything is a List node. */
        ASSERT(record_enumerate(&segment) == 0);
        ASSERT(record_measure(&segment) == 0);
        segment_fini(&segment);

        STATUS();
    }

    {
        record_t record = RECORD_INIT(&record);

        TEST();

        ASSERT(record_enumerate(&record) == 0);
        ASSERT(record_measure(&record) == 0);
        ASSERT(record_dump(stderr, &record) == &record);

        STATUS();
    }

    {
        record_t record;

        TEST();

        ASSERT(record_init(&record) == &record);
        ASSERT(record_enumerate(&record) == 0);
        ASSERT(record_measure(&record) == 0);
        ASSERT(record_dump(stderr, &record) == &record);
        record_fini(&record);

        STATUS();
    }

    {
        pool_t pool = POOL_INIT(&pool); /* Private. */
        segment_t segments[7]; /* Private. */

        TEST();

        /* Everything is a List node. */
        ASSERT(record_enumerate(&pool) == 0);
        ASSERT(record_measure(&pool) == 0);
        ASSERT(pool_populate(&pool, segments, countof(segments)) == &pool);
        /* Everything is a List node. */
        ASSERT(record_enumerate(&pool) == countof(segments));
        ASSERT(record_measure(&pool) == 0);
        pool_fini(&pool);
        ASSERT(record_enumerate(&pool) == 0);
        ASSERT(record_measure(&pool) == 0);

        STATUS();
    }

    {
        pool_t pool; /* Private. */
        segment_t segments[11]; /* Private. */

        TEST();

        ASSERT(pool_init(&pool) == &pool);
        /* Everything is a List node. */
        ASSERT(record_enumerate(&pool) == 0);
        ASSERT(record_measure(&pool) == 0);
        ASSERT(pool_populate(&pool, segments, countof(segments)) == &pool);
        /* Everything is a List node. */
        ASSERT(record_enumerate(&pool) == countof(segments));
        ASSERT(record_measure(&pool) == 0);
        pool_fini(&pool);
        ASSERT(record_enumerate(&pool) == 0);
        ASSERT(record_measure(&pool) == 0);

        STATUS();
    }

    {
        TEST();

        ASSERT(pool_init(&pool) == &pool);
        /* Everything is a List node. */
        ASSERT(record_enumerate(&pool) == 0);
        ASSERT(pool_populate(&pool, segments, countof(segments)) == &pool);
        /* Everything is a List node. */
        ASSERT(record_enumerate(&pool) == countof(segments));
        ASSERT(record_measure(&pool) == 0);

        STATUS();
    }

    {
        segment_t * sp[3];
        void * dp;

        TEST();

        /* Everything is a List node. */
        ASSERT(record_enumerate(&pool) == (NODES - 0));
        ASSERT(diminuto_buffer_log() >= 0);

        ASSERT((sp[0] = segment_allocate(&pool, sizeof(size_t))) != (segment_t *)0);
        /* Everything is a List node. */
        ASSERT(record_enumerate(&pool) == (NODES - 1));
        ASSERT((dp = segment_payload_get(sp[0])) != (void *)0);
        ASSERT((((uintptr_t)dp) & 0x7) == 0);
        ASSERT(segment_length_get(sp[0]) == sizeof(size_t));
        ASSERT(segment_length_set(sp[0], 0) == 0);
        ASSERT(segment_length_get(sp[0]) == 0);
        ASSERT(diminuto_buffer_log() >= 0);

        ASSERT((sp[1] = segment_allocate(&pool, sizeof(uint64_t))) != (segment_t *)0);
        /* Everything is a List node. */
        ASSERT(record_enumerate(&pool) == (NODES - 2));
        ASSERT((dp = segment_payload_get(sp[1])) != (void *)0);
        ASSERT((((uintptr_t)dp) & 0x7) == 0);
        ASSERT(segment_length_get(sp[1]) == sizeof(uint64_t));
        ASSERT(segment_length_set(sp[1], 0) == 0);
        ASSERT(segment_length_get(sp[1]) == 0);
        ASSERT(diminuto_buffer_log() >= 0);

        ASSERT((sp[2] = segment_allocate(&pool, 64)) != (segment_t *)0);
        /* Everything is a List node. */
        ASSERT(record_enumerate(&pool) == (NODES - 3));
        ASSERT((dp = segment_payload_get(sp[2])) != (void *)0);
        ASSERT((((uintptr_t)dp) & 0x7) == 0);
        ASSERT(segment_length_get(sp[2]) == 64);
        ASSERT(segment_length_set(sp[2], 0) == 0);
        ASSERT(segment_length_get(sp[2]) == 0);
        ASSERT(diminuto_buffer_log() >= 0);

        /* Order is deliberate. */

        segment_free(&pool, sp[0]);
        /* Everything is a List node. */
        ASSERT(record_enumerate(&pool) == (NODES - 2));
        ASSERT(diminuto_buffer_log() >= 0);

        segment_free(&pool, sp[2]);
        /* Everything is a List node. */
        ASSERT(record_enumerate(&pool) == (NODES - 1));
        ASSERT(diminuto_buffer_log() >= 0);

        segment_free(&pool, sp[1]);
        /* Everything is a List node. */
        ASSERT(record_enumerate(&pool) == (NODES - 0));
        ASSERT(diminuto_buffer_log() >= 0);

        STATUS();
    }

    {
        TEST();

        ASSERT((rp = record_allocate(&pool)) != (record_t *)0);
        ASSERT(record_enumerate(rp) == 0);
        ASSERT(record_measure(rp) == 0);
        ASSERT(record_dump(stderr, rp) == rp);

        STATUS();
    }

    {
        segment_t * sp;
        uint8_t * bp;
        size_t ll;

        /* Payload */

        TEST();

        ASSERT((sp = segment_allocate(&pool, MAXIMUM)) != (segment_t *)0);
        ASSERT(segment_length_get(sp) == MAXIMUM);
        ASSERT((bp = (uint8_t *)segment_payload_get(sp)) != (uint8_t *)0);
        ASSERT((ll = (strlen(DATA) + 1 /* Including NUL. */)) > 0);
        strncpy(bp, DATA, ll);
        ASSERT(segment_length_set(sp, ll) == ll);
        ASSERT(segment_length_get(sp) == ll);
        ASSERT(record_segment_append(rp, sp) == sp);

        ASSERT(record_enumerate(rp) == 1);
        ASSERT(record_measure(rp) > 0);
        ASSERT(record_dump(stderr, rp) == rp);

        STATUS();
    }

    {
        segment_t * sp;
        uint8_t * bp;
        size_t length;
        uint8_t a;
        uint8_t b;
        uint16_t checksum;

        /* Length Payload Checksum */

        TEST();

        ASSERT((sp = record_segment_head(rp)) != (segment_t *)0);
        ASSERT((bp = (uint8_t *)segment_payload_get(sp)) != (uint8_t *)0);
        ASSERT((length = segment_length_get(sp)) > 0);
        a = b = 0;
        checksum = diminuto_fletcher_16(bp, length, &a, &b);

        ASSERT((sp = segment_allocate(&pool, sizeof(length))) != (segment_t *)0);
        ASSERT(segment_length_get(sp) == sizeof(length));
        ASSERT((bp = (uint8_t *)segment_payload_get(sp)) != (uint8_t *)0);
        memcpy(bp, &length, sizeof(length));
        ASSERT(record_segment_prepend(rp, sp) == sp);
 
        ASSERT((sp = segment_allocate(&pool, sizeof(checksum))) != (segment_t *)0);
        ASSERT(segment_length_get(sp) == sizeof(checksum));
        ASSERT((bp = (uint8_t *)segment_payload_get(sp)) != (uint8_t *)0);
        memcpy(bp, &checksum, sizeof(checksum));
        ASSERT(record_segment_append(rp, sp) == sp);
    
        ASSERT(record_enumerate(rp) == 3);
        ASSERT(record_measure(rp) > 0);
        ASSERT(record_dump(stderr, rp) == rp);

        STATUS();
    }

    {
        fflush(stderr);
    }

    {
        TEST();

        address = diminuto_ipc4_address("localhost");
        ASSERT(!diminuto_ipc4_is_unspecified(&address));

        ASSERT((listensocket = diminuto_ipc4_stream_provider(0)) >= 0);
        ASSERT(diminuto_ipc4_nearend(listensocket, (diminuto_ipv4_t *)0, &streamport) >= 0);

        ASSERT((datagramsocket = diminuto_ipc4_datagram_peer(0)) >= 0);
        ASSERT(diminuto_ipc4_nearend(datagramsocket, (diminuto_ipv4_t *)0, &datagramport) >= 0);

        if ((streampid = fork()) == 0) {
            segment_t * sp;
            int xc;
            ASSERT(record_segments_free(&pool, rp) == rp);
            ASSERT(record_enumerate(rp) == 0);
            ASSERT(record_measure(rp) == 0);
            record_free(&pool, rp);
            xc = streamserver(listensocket);
            ASSERT(diminuto_ipc_close(listensocket) >= 0);
            /* To make valgrind(1) happy. */
            while ((sp = diminuto_list_head(&pool)) != (segment_t *)0) {
                ASSERT(sp->data == (void *)0);
                diminuto_list_remove(sp);
            }
            diminuto_buffer_log();
            diminuto_buffer_fini();
            diminuto_buffer_log();
            exit(xc);
        }
        ASSERT(streampid > 0);
        ASSERT(diminuto_ipc_close(listensocket) >= 0);

        if ((datagrampid = fork()) == 0) {
            segment_t * sp;
            int xc;
            ASSERT(record_segments_free(&pool, rp) == rp);
            ASSERT(record_enumerate(rp) == 0);
            ASSERT(record_measure(rp) == 0);
            record_free(&pool, rp);
            xc = datagrampeer(datagramsocket);
            ASSERT(diminuto_ipc_close(datagramsocket) >= 0);
            /* TO make valgrind(1) happy. */
            while ((sp = diminuto_list_head(&pool)) != (segment_t *)0) {
                ASSERT(sp->data == (void *)0);
                diminuto_list_remove(sp);
            }
            diminuto_buffer_log();
            diminuto_buffer_fini();
            diminuto_buffer_log();
            exit(xc);
        }
        ASSERT(datagrampid  > 0);
        ASSERT(diminuto_ipc_close(datagramsocket) >= 0);

        STATUS();
    }

    {
        uint8_t * bp;
        segment_t * sp;
        segment_t * tp;
        ssize_t length;

        /* Address StreamPort Length Payload Checksum */

        TEST();

        ASSERT((sp = segment_allocate(&pool, sizeof(address))) != (segment_t *)0);
        ASSERT(segment_length_get(sp) == sizeof(address));
        ASSERT((bp = (uint8_t *)segment_payload_get(sp)) != (uint8_t *)0);
        memcpy(bp, &address, sizeof(address));
        ASSERT(record_segment_prepend(rp, sp) == sp);
        tp = sp;
 
        ASSERT((sp = segment_allocate(&pool, sizeof(streamport))) != (segment_t *)0);
        ASSERT(segment_length_get(sp) == sizeof(streamport));
        ASSERT((bp = (uint8_t *)segment_payload_get(sp)) != (uint8_t *)0);
        memcpy(bp, &streamport, sizeof(streamport));
        ASSERT(record_segment_insert(tp, sp) == sp);
    
        ASSERT(record_enumerate(rp) == 5);
        ASSERT(record_measure(rp) > 0);
        ASSERT(record_dump(stderr, rp) == rp);

        STATUS();
    }

    {
        fflush(stderr);
    }

    {
        segment_t * sp;
        diminuto_ipv4_t * addressp;
        diminuto_port_t * portp;
        size_t total;
        int socket;
        ssize_t length;
        int status;

        /* Write Stream (more than once) */

        TEST();

        ASSERT((sp = record_segment_head(rp)) != (segment_t *)0);
        ASSERT(segment_length_get(sp) == sizeof(*addressp));
        ASSERT((addressp = (diminuto_ipv4_t *)segment_payload_get(sp)) != (diminuto_ipv4_t *)0);

        ASSERT((sp = record_segment_next(rp, sp)) != (segment_t *)0);
        ASSERT(segment_length_get(sp) == sizeof(*portp));
        ASSERT((portp = (diminuto_port_t *)segment_payload_get(sp)) != (diminuto_port_t *)0);

        ASSERT((total = record_measure(rp)) > 0);
        ASSERT((socket = diminuto_ipc4_stream_consumer(*addressp, *portp)) >= 0);
        ASSERT((length = record_write(socket, rp)) == total);
        ASSERT((length = record_write(socket, rp)) == total);
        ASSERT((length = record_write(socket, rp)) == total);
        ASSERT((length = record_write(socket, rp)) == total);
        ASSERT(diminuto_ipc_close(socket) >= 0);

        status = 2;
        ASSERT(diminuto_reaper_reap_generic(streampid, &status, 0) == streampid);
        ASSERT(WIFEXITED(status));
        ASSERT(WEXITSTATUS(status) == 0);

        STATUS();
    }

    {
        uint8_t * bp;
        segment_t * sp;
        segment_t * tp;
        diminuto_ipv4_t address;
        diminuto_port_t port;
        ssize_t length;

        /* Address DatagramPort Length Payload Checksum */

        TEST();

        ASSERT((sp = segment_allocate(&pool, sizeof(datagramport))) != (segment_t *)0);
        ASSERT(segment_length_get(sp) == sizeof(datagramport));
        ASSERT((bp = (uint8_t *)segment_payload_get(sp)) != (uint8_t *)0);
        memcpy(bp, &datagramport, sizeof(streamport));
        ASSERT((tp = record_segment_head(rp)) != (segment_t *)0);
        ASSERT((tp = record_segment_next(rp, tp)) != (segment_t *)0);
        ASSERT(record_segment_replace(tp, sp) == tp);
        segment_free(&pool, tp);
    
        ASSERT(record_enumerate(rp) == 5);
        ASSERT(record_measure(rp) > 0);
        ASSERT(record_dump(stderr, rp) == rp);

        STATUS();
    }

    {
        fflush(stderr);
    }

    {
        segment_t * sp;
        diminuto_ipv4_t * addressp;
        diminuto_port_t * portp;
        size_t total;
        int socket;
        ssize_t length;
        int status;

        /* Send Datagram (more than once) */

        TEST();

        ASSERT((sp = record_segment_head(rp)) != (segment_t *)0);
        ASSERT(segment_length_get(sp) == sizeof(*addressp));
        ASSERT((addressp = (diminuto_ipv4_t *)segment_payload_get(sp)) != (diminuto_ipv4_t *)0);

        ASSERT((sp = record_segment_next(rp, sp)) != (segment_t *)0);
        ASSERT(segment_length_get(sp) == sizeof(*portp));
        ASSERT((portp = (diminuto_port_t *)segment_payload_get(sp)) != (diminuto_port_t *)0);

        ASSERT((total = record_measure(rp)) > 0);
        ASSERT((socket = diminuto_ipc4_datagram_peer(0)) >= 0);
        ASSERT((length = record_send(socket, rp, *addressp, *portp)) == total);
        ASSERT((length = record_send(socket, rp, *addressp, *portp)) == total);
        ASSERT((length = record_send(socket, rp, *addressp, *portp)) == total);
        ASSERT(diminuto_ipc_close(socket) >= 0);

        status = 3;
        ASSERT(diminuto_reaper_reap_generic(datagrampid, &status, 0) == datagrampid);
        ASSERT(WIFEXITED(status));
        ASSERT(WEXITSTATUS(status) == 0);

        STATUS();
    }

    {
        segment_t * sp;

        TEST();

        record_free(&pool, rp);
        ASSERT(record_enumerate(&pool) > 0);
        ASSERT(record_measure(&pool) == 0);
        pool_fini(&pool);
        ASSERT(record_enumerate(&pool) == 0);
        ASSERT(record_measure(&pool) == 0);
        ASSERT(diminuto_buffer_log() > 0);
        diminuto_buffer_fini();
        ASSERT(diminuto_buffer_log() == 0);

        STATUS();
    }

    EXIT();
}
