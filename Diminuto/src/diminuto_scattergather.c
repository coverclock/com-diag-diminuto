/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2021-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This implements the Diminuto Scatter/Gather feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 */

#include "com/diag/diminuto/diminuto_scattergather.h"
#include "com/diag/diminuto/diminuto_criticalsection.h"
#include "com/diag/diminuto/diminuto_dump.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include "../src/diminuto_ipc4.h"
#include "../src/diminuto_ipc6.h"

/*******************************************************************************
 * GLOBALS
 ******************************************************************************/

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/*******************************************************************************
 * POOL
 ******************************************************************************/

void diminuto_scattergather_pool_fini(diminuto_scattergather_pool_t * pp)
{
    diminuto_list_t * np = (diminuto_list_t *)0;

    while ((np = diminuto_list_head(pp)) != (diminuto_list_t *)0) {
        (void)diminuto_list_remove(np);
    }
}

diminuto_list_t * diminuto_scattergather_pool_get(diminuto_scattergather_pool_t * pp)
{
    diminuto_list_t * np = (diminuto_list_t *)0;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
        np = diminuto_list_dequeue(pp);
    DIMINUTO_CRITICAL_SECTION_END;

    return np;
}

void diminuto_scattergather_pool_put(diminuto_scattergather_pool_t * pp, diminuto_list_t * np)
{
    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
        (void)diminuto_list_enqueue(pp, diminuto_list_nullinit(np));
    DIMINUTO_CRITICAL_SECTION_END;
}

diminuto_scattergather_pool_t * diminuto_scattergather_pool_populate(diminuto_scattergather_pool_t * pp, diminuto_list_t sa[], size_t sn)
{
    int ii = 0;

    DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex);
        for (ii = 0; ii < sn; ++ii) {
            (void)diminuto_list_enqueue(pp, diminuto_list_nullinit(&sa[ii]));
        }
    DIMINUTO_CRITICAL_SECTION_END;

    return pp;
}

/*******************************************************************************
 * SEGMENT
 ******************************************************************************/

diminuto_scattergather_segment_t * diminuto_scattergather_segment_allocate(diminuto_scattergather_pool_t * pp, size_t size)
{
    diminuto_scattergather_buffer_t * bp = (diminuto_scattergather_buffer_t *)0;
    diminuto_scattergather_segment_t * sp = (diminuto_scattergather_segment_t *)0;

    /*
     * This function returns a buffer whose beginning address is
     * aligned on at least an eight-byte boundary. The header we
     * put before the payload portion is eight bytes long.
     */
    if ((bp = (diminuto_scattergather_buffer_t *)diminuto_buffer_malloc(sizeof(diminuto_scattergather_buffer_t) + size)) == (void *)0) {
        /* Do nothing. */
    } else if ((sp = diminuto_scattergather_pool_get(pp)) == (diminuto_scattergather_segment_t *)0) {
        diminuto_buffer_free(bp);
        errno = ENOMEM;
        diminuto_perror("diminuto_scattergather_segment_allocate");
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

/*******************************************************************************
 * RECORD
 ******************************************************************************/

size_t diminuto_scattergather_record_enumerate(const diminuto_scattergather_record_t * rp)
{
    size_t nn = 0;
    const diminuto_scattergather_segment_t * sp = (const diminuto_scattergather_segment_t *)0;

    for (sp = diminuto_scattergather_record_segment_head(rp); sp != (diminuto_scattergather_segment_t *)0; sp = diminuto_scattergather_record_segment_next(sp)) {
        nn += 1;
    }

    return nn;    
}

size_t diminuto_scattergather_record_measure(const diminuto_scattergather_record_t * rp)
{
    size_t ll = 0;
    const diminuto_scattergather_segment_t * sp = (const diminuto_scattergather_segment_t *)0;

    for (sp = diminuto_scattergather_record_segment_head(rp); sp != (diminuto_scattergather_segment_t *)0; sp = diminuto_scattergather_record_segment_next(sp)) {
        if (diminuto_list_data(sp) != (void *)0) {
            ll += diminuto_scattergather_segment_length_get(sp);
        }
    }

    return ll;    
}

const diminuto_scattergather_record_t * diminuto_scattergather_record_dump(const diminuto_scattergather_record_t * rp)
{
    const diminuto_scattergather_segment_t * sp = (const diminuto_scattergather_segment_t *)0;
    void * pp = 0;
    size_t ii = 0;
    size_t ll = 0;
    size_t tt = 0;
    FILE * fp = (FILE *)0;

    diminuto_log_emit("RECORD %p:\n", rp);
    fp = diminuto_log_stream();
    for (sp = diminuto_scattergather_record_segment_head(rp); sp != (diminuto_scattergather_segment_t *)0; sp = diminuto_scattergather_record_segment_next(sp)) {
        diminuto_log_emit("  SEGMENT %p: #%zu\n", sp, ii++);
        pp = diminuto_scattergather_segment_payload_get(sp);
        ll = diminuto_scattergather_segment_length_get(sp);
        tt += ll;
        diminuto_log_emit("    PAYLOAD %p: [%zu]\n", pp, ll);
        if (fp != (FILE *)0) {
            diminuto_dump_general(fp, pp, ll, 0, '.', 0, 0, 6);
        }
    }
    diminuto_log_emit("  TOTAL %p: #%zu [%zu]\n", rp, ii, tt);

    return rp;    
}

struct iovec * diminuto_scattergather_record_vectorize(const diminuto_scattergather_record_t * rp, struct iovec va[], size_t nn)
{
    const diminuto_scattergather_segment_t * sp = (const diminuto_scattergather_segment_t *)0;
    size_t ii = 0;

    for (sp = diminuto_scattergather_record_segment_head(rp); sp != (diminuto_scattergather_segment_t *)0; sp = diminuto_scattergather_record_segment_next(sp)) {
        if (ii < nn) {
            va[ii].iov_base = diminuto_scattergather_segment_payload_get(sp);
            va[ii].iov_len = diminuto_scattergather_segment_length_get(sp);
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

diminuto_scattergather_record_t * diminuto_scattergather_record_segments_free(diminuto_scattergather_pool_t * pp, diminuto_scattergather_record_t * rp)
{
    diminuto_scattergather_segment_t * sp = (diminuto_scattergather_segment_t *)0;

    while ((sp = diminuto_scattergather_record_segment_head(rp)) != (diminuto_scattergather_segment_t *)0) {
        diminuto_scattergather_record_segment_free(pp, sp);
    }

    return rp;    
}

diminuto_scattergather_record_t * diminuto_scattergather_record_allocate(diminuto_scattergather_pool_t * pp)
{
    diminuto_scattergather_record_t * rp = (diminuto_scattergather_record_t *)0;

    if ((rp = diminuto_scattergather_pool_get(pp)) == (diminuto_scattergather_record_t *)0) {
        errno = ENOMEM;
        diminuto_perror("diminuto_scattergather_record_allocate");
    }

    return rp;
}

/*******************************************************************************
 * INPUT/OUTPUT
 ******************************************************************************/

ssize_t diminuto_scattergather_record_write(int fd, const diminuto_scattergather_record_t * rp)
{
    ssize_t total = -1;
    size_t nn = 0;
    struct iovec * vp = (struct iovec *)0;

    nn = diminuto_scattergather_record_enumerate(rp);

    if (!((0 < nn) && (nn <= DIMINUTO_SCATTERGATHER_MAXIMUM))) {
        errno = EINVAL;
        diminuto_perror("diminuto_scattergather_record_write: enumeration");
    } else if ((vp = (struct iovec *)alloca(nn * sizeof(*vp))) == (struct iovec *)0) {
        diminuto_perror("diminuto_scattergather_record_write: alloca");
    } else if (diminuto_scattergather_record_vectorize(rp, vp, nn) == (struct iovec *)0) {
        diminuto_perror("diminuto_scattergather_record_write: diminuto_scattergather_record_vectorize");
    } else if ((total = writev(fd, vp, nn)) < 0) {
        diminuto_perror("diminuto_scattergather_record_write: writev");
    } else {
        /* Do nothing. */
    }

    return total;
}

ssize_t diminuto_scattergather_record_read(int fd, diminuto_scattergather_record_t * rp)
{
    ssize_t total = -1;
    size_t nn = 0;
    struct iovec * vp = (struct iovec *)0;

    nn = diminuto_scattergather_record_enumerate(rp);

    if (!((0 < nn) && (nn <= DIMINUTO_SCATTERGATHER_MAXIMUM))) {
        errno = EINVAL;
        diminuto_perror("diminuto_scattergather_record_read: enumeration");
    } else if ((vp = (struct iovec *)alloca(nn * sizeof(*vp))) == (struct iovec *)0) {
        diminuto_perror("diminuto_scattergather_record_write: alloca");
    } else if (diminuto_scattergather_record_vectorize(rp, vp, nn) == (struct iovec *)0) {
        diminuto_perror("diminuto_scattergather_record_read: diminuto_scattergather_record_vectorize");
    } else if ((total = readv(fd, vp, nn)) < 0) {
        diminuto_perror("diminuto_scattergather_record_write: writev");
    } else {
        /* Do nothing. */
    }

    return total;
}

static ssize_t diminuto_scattergather_record_send_generic(int fd, const diminuto_scattergather_record_t * rp, struct sockaddr * sap, socklen_t length)
{
    ssize_t total = 0;
    struct iovec * vp = (struct iovec *)0;
    struct msghdr message = { 0, };

    message.msg_name = sap;
    message.msg_namelen = length;
    message.msg_iovlen = diminuto_scattergather_record_enumerate(rp);

    if (!((0 < message.msg_iovlen) && (message.msg_iovlen <= DIMINUTO_SCATTERGATHER_MAXIMUM))) {
        errno = EINVAL;
        diminuto_perror("diminuto_scattergather_record_send_generic: enumeration");
    } else if ((vp = (struct iovec *)alloca(message.msg_iovlen * sizeof(*message.msg_iov))) == (struct iovec *)0) {
        diminuto_perror("diminuto_scattergather_record_send_generic: alloca");
    } else if ((message.msg_iov = diminuto_scattergather_record_vectorize(rp, vp, message.msg_iovlen)) == (struct iovec *)0) {
        diminuto_perror("diminuto_scattergather_record_send_generic: diminuto_scattergather_record_vectorize");
    } else if ((total = sendmsg(fd, &message, 0)) < 0) {
        diminuto_perror("diminuto_scattergather_record_send_generic: sendmsg");
    } else {
        /* Do nothing. */
    }

    return total;
}

ssize_t diminuto_scattergather_record_ipc_stream_send(int fd, const diminuto_scattergather_record_t * rp)
{
    return diminuto_scattergather_record_send_generic(fd, rp, (struct sockaddr *)0, 0);
}

ssize_t diminuto_scattergather_record_ipc4_datagram_send(int fd, const diminuto_scattergather_record_t * rp, diminuto_ipv4_t address, diminuto_port_t port)
{
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

    return diminuto_scattergather_record_send_generic(fd, rp, sap, length);
}

ssize_t diminuto_scattergather_record_ipc6_datagram_send(int fd, const diminuto_scattergather_record_t * rp, diminuto_ipv6_t address, diminuto_port_t port)
{
    struct sockaddr_in6 sa = { 0, };
    struct sockaddr * sap = (struct sockaddr *)0;
    socklen_t length = 0;

    if (port > 0) {
        length = sizeof(sa);
        sa.sin6_family = AF_INET6;
        diminuto_ipc6_hton6(&address);
        memcpy(&sa.sin6_addr.s6_addr, address.u16, sizeof(sa.sin6_addr.s6_addr));
        sa.sin6_port = htons(port);
        sap = (struct sockaddr *)&sa;
    }

    return diminuto_scattergather_record_send_generic(fd, rp, sap, length);
}

static ssize_t diminuto_scattergather_record_receive_generic(int fd, diminuto_scattergather_record_t * rp, struct sockaddr * sap, socklen_t length)
{
    ssize_t total = 0;
    struct iovec * vp = (struct iovec *)0;
    struct msghdr message = { 0, };

    message.msg_name = sap;
    message.msg_namelen = length;
    message.msg_iovlen = diminuto_scattergather_record_enumerate(rp);

    if (!((0 < message.msg_iovlen) && (message.msg_iovlen <= DIMINUTO_SCATTERGATHER_MAXIMUM))) {
        errno = EINVAL;
        diminuto_perror("diminuto_scattergather_record_receive_generic: enumeration");
    } else if ((vp = (struct iovec *)alloca(message.msg_iovlen * sizeof(*message.msg_iov))) == (struct iovec *)0) {
        diminuto_perror("diminuto_scattergather_record_receive_generic: alloca");
    } else if ((message.msg_iov = diminuto_scattergather_record_vectorize(rp, vp, message.msg_iovlen)) == (struct iovec *)0) {
        diminuto_perror("diminuto_scattergather_record_receive_generic: diminuto_scattergather_record_vectorize");
    } else if ((total = recvmsg(fd, &message, 0)) < 0) {
        diminuto_perror("diminuto_scattergather_record_receive_generic: sendmsg");
    } else {
        /* Do nothing. */
    }

    return total;
}

ssize_t diminuto_scattergather_record_ipc_stream_receive(int fd, diminuto_scattergather_record_t * rp)
{
    return diminuto_scattergather_record_receive_generic(fd, rp, (struct sockaddr *)0, 0);
}

ssize_t diminuto_scattergather_record_ipc4_datagram_receive(int fd, diminuto_scattergather_record_t * rp, diminuto_ipv4_t * addressp, diminuto_port_t * portp)
{
    ssize_t total = 0;
    struct sockaddr_in sa = { 0, };

    total = diminuto_scattergather_record_receive_generic(fd, rp, (struct sockaddr *)&sa, sizeof(sa));

    diminuto_ipc4_identify((struct sockaddr *)&sa, addressp, portp);

    return total;
}

ssize_t diminuto_scattergather_record_ipc6_datagram_receive(int fd, diminuto_scattergather_record_t * rp, diminuto_ipv6_t * addressp, diminuto_port_t * portp)
{
    ssize_t total = 0;
    struct sockaddr_in6 sa = { 0, };

    total = diminuto_scattergather_record_receive_generic(fd, rp, (struct sockaddr *)&sa, sizeof(sa));

    diminuto_ipc6_identify((struct sockaddr *)&sa, addressp, portp);

    return total;
}
