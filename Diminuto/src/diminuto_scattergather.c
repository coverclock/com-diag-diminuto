/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2021 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This implements the Diminuto IPC Scatter/Gather feature.
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
#include "../src/diminuto_ipc6.h"

/*******************************************************************************
 * GLOBALS
 ******************************************************************************/

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/*******************************************************************************
 * Pool
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
        (void)diminuto_list_enqueue(pp, np);
    DIMINUTO_CRITICAL_SECTION_END;
}

diminuto_scattergather_pool_t * diminuto_scattergather_pool_populate(diminuto_scattergather_pool_t * pp, diminuto_scattergather_segment_t sa[], size_t sn)
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
 * Segment
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
 * Record
 ******************************************************************************/

size_t diminuto_scattergather_record_enumerate(diminuto_scattergather_record_t * rp)
{
    size_t nn = 0;
    diminuto_scattergather_segment_t * sp = (diminuto_scattergather_segment_t *)0;

    for (sp = diminuto_scattergather_record_segment_head(rp); sp != (diminuto_scattergather_segment_t *)0; sp = diminuto_scattergather_record_segment_next(rp, sp)) {
        nn += 1;
    }

    return nn;    
}

size_t diminuto_scattergather_record_measure(diminuto_scattergather_record_t * rp)
{
    size_t ll = 0;
    diminuto_scattergather_segment_t * sp = (diminuto_scattergather_segment_t *)0;

    for (sp = diminuto_scattergather_record_segment_head(rp); sp != (diminuto_scattergather_segment_t *)0; sp = diminuto_scattergather_record_segment_next(rp, sp)) {
        if (diminuto_list_data(sp) != (void *)0) {
            ll += diminuto_scattergather_segment_length_get(sp);
        }
    }

    return ll;    
}

diminuto_scattergather_record_t * diminuto_scattergather_record_dump(FILE * fp, diminuto_scattergather_record_t * rp)
{
    diminuto_scattergather_segment_t * sp = (diminuto_scattergather_segment_t *)0;
    void * pp = 0;
    size_t ii = 0;
    size_t ll = 0;
    size_t tt = 0;

    fprintf(fp, "RECORD %p:\n", rp);
    for (sp = diminuto_scattergather_record_segment_head(rp); sp != (diminuto_scattergather_segment_t *)0; sp = diminuto_scattergather_record_segment_next(rp, sp)) {
        fprintf(fp, "  SEGMENT %p: #%zu\n", sp, ii++);
        pp = diminuto_scattergather_segment_payload_get(sp);
        ll = diminuto_scattergather_segment_length_get(sp);
        tt += ll;
        fprintf(fp, "    PAYLOAD %p: [%zu]\n", pp, ll);
        diminuto_dump_general(fp, pp, ll, 0, '.', 0, 0, 6);
    }
    fprintf(fp, "  TOTAL %p: #%zu [%zu]\n", rp, ii, tt);

    return rp;    
}

struct iovec * diminuto_scattergather_record_vectorize(diminuto_scattergather_record_t * rp, struct iovec va[], size_t nn)
{
    diminuto_scattergather_segment_t * sp = (diminuto_scattergather_segment_t *)0;
    size_t ii = 0;

    for (sp = diminuto_scattergather_record_segment_head(rp); sp != (diminuto_scattergather_segment_t *)0; sp = diminuto_scattergather_record_segment_next(rp, sp)) {
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
        (void)diminuto_scattergather_record_segment_free(pp, rp, sp);
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

ssize_t diminuto_scattergather_record_write(int fd, diminuto_scattergather_record_t * rp)
{
    ssize_t total = -1;
    size_t nn = 0;
    struct iovec * vp = (struct iovec *)0;

    nn = diminuto_scattergather_record_enumerate(rp);

    if (!((0 < nn) && (nn <= DIMINUTO_SCATTERGATHER_VECTOR))) {
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

    if (!((0 < nn) && (nn <= DIMINUTO_SCATTERGATHER_VECTOR))) {
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

ssize_t diminuto_scattergather_record_send_base(int fd, diminuto_scattergather_record_t * rp, struct sockaddr * sap, socklen_t length)
{
    ssize_t total = 0;
    struct iovec * vp = (struct iovec *)0;
    struct msghdr message = { 0, };

    message.msg_name = sap;
    message.msg_namelen = length;
    message.msg_iovlen = diminuto_scattergather_record_enumerate(rp);

    if (!((0 < message.msg_iovlen) && (message.msg_iovlen <= DIMINUTO_SCATTERGATHER_VECTOR))) {
        errno = EINVAL;
        diminuto_perror("diminuto_scattergather_record_send: enumeration");
    } else if ((vp = (struct iovec *)alloca(message.msg_iovlen * sizeof(*message.msg_iov))) == (struct iovec *)0) {
        diminuto_perror("diminuto_scattergather_record_send: alloca");
    } else if ((message.msg_iov = diminuto_scattergather_record_vectorize(rp, vp, message.msg_iovlen)) == (struct iovec *)0) {
        diminuto_perror("diminuto_scattergather_record_send: diminuto_scattergather_record_vectorize");
    } else if ((total = sendmsg(fd, &message, 0)) < 0) {
        diminuto_perror("diminuto_scattergather_record_send: sendmsg");
    } else {
        /* Do nothing. */
    }

    return total;
}

ssize_t diminuto_scattergather_record_send_generic(int fd, diminuto_scattergather_record_t * rp, diminuto_ipv4_t address, diminuto_port_t port)
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

    return diminuto_scattergather_record_send_base(fd, rp, sap, length);
}

ssize_t diminuto_scattergather_record_send6_generic(int fd, diminuto_scattergather_record_t * rp, diminuto_ipv6_t address, diminuto_port_t port)
{
    ssize_t total = 0;
    struct iovec * vp = (struct iovec *)0;
    struct msghdr message = { 0, };
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

    return diminuto_scattergather_record_send_base(fd, rp, sap, length);
}

ssize_t diminuto_scattergather_record_receive_base(int fd, diminuto_scattergather_record_t * rp, struct sockaddr * sap, socklen_t length)
{
    ssize_t total = 0;
    struct iovec * vp = (struct iovec *)0;
    struct msghdr message = { 0, };

    message.msg_name = sap;
    message.msg_namelen = length;
    message.msg_iovlen = diminuto_scattergather_record_enumerate(rp);

    if (!((0 < message.msg_iovlen) && (message.msg_iovlen <= DIMINUTO_SCATTERGATHER_VECTOR))) {
        errno = EINVAL;
        diminuto_perror("diminuto_scattergather_record_receive: enumeration");
    } else if ((vp = (struct iovec *)alloca(message.msg_iovlen * sizeof(*message.msg_iov))) == (struct iovec *)0) {
        diminuto_perror("diminuto_scattergather_record_receive: alloca");
    } else if ((message.msg_iov = diminuto_scattergather_record_vectorize(rp, vp, message.msg_iovlen)) == (struct iovec *)0) {
        diminuto_perror("diminuto_scattergather_record_receive: diminuto_scattergather_record_vectorize");
    } else if ((total = recvmsg(fd, &message, 0)) < 0) {
        diminuto_perror("diminuto_scattergather_record_receive: sendmsg");
    } else {
        /* Do nothing. */
    }

    return total;
}

ssize_t diminuto_scattergather_record_receive_generic(int fd, diminuto_scattergather_record_t * rp, diminuto_ipv4_t * addressp, diminuto_port_t * portp)
{
    ssize_t total = 0;
    struct sockaddr_in sa = { 0, };

    total = diminuto_scattergather_record_receive_base(fd, rp, (struct sockaddr *)&sa, sizeof(sa));

    diminuto_ipc4_identify(&sa, addressp, portp);

    return total;
}

ssize_t diminuto_scattergather_record_receive6_generic(int fd, diminuto_scattergather_record_t * rp, diminuto_ipv6_t * addressp, diminuto_port_t * portp)
{
    ssize_t total = 0;
    struct sockaddr_in6 sa = { 0, };

    total = diminuto_scattergather_record_receive_base(fd, rp, (struct sockaddr *)&sa, sizeof(sa));

    diminuto_ipc6_identify(&sa, addressp, portp);

    return total;
}
