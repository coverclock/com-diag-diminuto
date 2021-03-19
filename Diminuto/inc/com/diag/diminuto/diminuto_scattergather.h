/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_SCATTERGATHER_
#define _H_COM_DIAG_DIMINUTO_SCATTERGATHER_
/**
 * @file
 * @copyright Copyright 2021 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This defines the Diminuto IPC Scatter/Gather API.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * The Scatter/Gather feature makes use of Diminuto Lists with the vector
 * I/O system calls to decrease the amount of data copying necessary when
 * consolidating a bunch of fields into a single contiguous data packet
 * for sending/writing, or distributing a single contiguous data packet
 * that is received/read into multiple fields. Scatter/Gather can be
 * used for interprocess communication (IPC) or for file-ish reading
 * and writing.
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

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_list.h"
#include "com/diag/diminuto/diminuto_buffer.h"
#include "com/diag/diminuto/diminuto_containerof.h"
#include "com/diag/diminuto/diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_ipc6.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <stdint.h>
#include <stdio.h>

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

enum {
    DIMINUTO_SCATTERGATHER_NODES = UIO_MAXIOV,     /* Overkill. */
    DIMINUTO_SCATTERGATHER_VECTOR = UIO_MAXIOV,    /* Maximum. */
};

/*******************************************************************************
 * TYPES
 ******************************************************************************/

typedef struct DiminutoScatterGatherBuffer {
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
} diminuto_scattergather_buffer_t;

/*
 * Everything other than a Buffer is a List node. But the different
 * type names help me keep straight the role that the List node is
 * serving in the API. Don't mix them up, otherwise wackiness will
 * ensue.
 */

typedef diminuto_list_t diminuto_scattergather_pool_t;

typedef diminuto_list_t diminuto_scattergather_segment_t;

typedef diminuto_list_t diminuto_scattergather_record_t;

/*******************************************************************************
 * GENERATORS
 ******************************************************************************/

#define DIMINUTO_SCATTERGATHER_POOL_INIT(_POINTER_) DIMINUTO_LIST_NULLINIT(_POINTER_)

#define DIMINUTO_SCATTERGATHER_SEGMENT_INIT(_POINTER_) DIMINUTO_LIST_NULLINIT(_POINTER_)

#define DIMINUTO_SCATTERGATHER_RECORD_INIT(_POINTER_) DIMINUTO_LIST_NULLINIT(_POINTER_)

#define diminuto_scattergather_bufferof(_PAYLOAD_) diminuto_containerof(diminuto_scattergather_buffer_t, payload, (_PAYLOAD_))

/*******************************************************************************
 * Pool
 ******************************************************************************/

static inline diminuto_scattergather_pool_t * diminuto_scattergather_pool_init(diminuto_scattergather_pool_t * pp) {
    return diminuto_list_nullinit(pp);
}

extern void diminuto_scattergather_pool_fini(diminuto_scattergather_pool_t * pp);

extern diminuto_list_t * diminuto_scattergather_pool_get(diminuto_scattergather_pool_t * pp);

extern void diminuto_scattergather_pool_put(diminuto_scattergather_pool_t * pp, diminuto_list_t * np);

extern diminuto_scattergather_pool_t * diminuto_scattergather_pool_populate(diminuto_scattergather_pool_t * pp, diminuto_scattergather_segment_t sa[], size_t sn);

/*******************************************************************************
 * Segment
 ******************************************************************************/

static inline diminuto_scattergather_segment_t * diminuto_scattergather_segment_init(diminuto_scattergather_segment_t * sp) {
    return diminuto_list_nullinit(sp);
}

static inline diminuto_scattergather_segment_t * diminuto_scattergather_segment_fini(diminuto_scattergather_segment_t * sp) {
    return diminuto_list_dataset(diminuto_list_remove(sp), (void *)0);
}

/*
 * The address returned by this function will be aligned on at least
 * an eight-byte boundary.
 */
static inline void * diminuto_scattergather_segment_payload_get(diminuto_scattergather_segment_t * sp) {
    return (void *)(&(((diminuto_scattergather_buffer_t *)diminuto_list_data(sp))->payload[0]));
}

static inline size_t diminuto_scattergather_segment_length_get(diminuto_scattergather_segment_t * sp) {
    return ((diminuto_scattergather_buffer_t *)diminuto_list_data(sp))->length;
}

static inline size_t diminuto_scattergather_segment_length_set(diminuto_scattergather_segment_t * sp, size_t ll) {
    return ((diminuto_scattergather_buffer_t *)diminuto_list_data(sp))->length = ll;
}

extern diminuto_scattergather_segment_t * diminuto_scattergather_segment_allocate(diminuto_scattergather_pool_t * pp, size_t size);

static inline void diminuto_scattergather_segment_free(diminuto_scattergather_pool_t * pp, diminuto_scattergather_segment_t * sp)
{
    diminuto_buffer_free(diminuto_list_data(sp));
    (void)diminuto_scattergather_pool_put(pp, diminuto_scattergather_segment_fini(sp));
}

/*******************************************************************************
 * Record
 ******************************************************************************/

static inline diminuto_scattergather_record_t * diminuto_scattergather_record_init(diminuto_scattergather_record_t * rp) {
    return diminuto_list_nullinit(rp);
}

static inline diminuto_scattergather_record_t * diminuto_scattergather_record_fini(diminuto_scattergather_record_t * rp) {
    return diminuto_list_dataset(diminuto_list_remove(rp), (void *)0);
}

static inline diminuto_scattergather_segment_t * diminuto_scattergather_record_segment_remove(diminuto_scattergather_record_t * rp /* Unused. */, diminuto_scattergather_segment_t * sp) {
    return diminuto_list_remove(sp);
}

static inline diminuto_scattergather_segment_t * diminuto_scattergather_record_segment_prepend(diminuto_scattergather_record_t * rp, diminuto_scattergather_segment_t * sp) {
    return diminuto_list_push(rp, sp);
}

static inline diminuto_scattergather_segment_t * diminuto_scattergather_record_segment_append(diminuto_scattergather_record_t * rp, diminuto_scattergather_segment_t * sp) {
    return diminuto_list_enqueue(rp, sp);
}

static inline diminuto_scattergather_segment_t * diminuto_scattergather_record_segment_insert(diminuto_scattergather_segment_t * op, diminuto_scattergather_segment_t * sp) {
    return diminuto_list_splice(op, sp);
}

static inline diminuto_scattergather_segment_t * diminuto_scattergather_record_segment_replace(diminuto_scattergather_segment_t * op, diminuto_scattergather_segment_t * sp) {
    return diminuto_list_replace(op, sp);
}

static inline diminuto_scattergather_segment_t * diminuto_scattergather_record_segment_head(diminuto_scattergather_record_t * rp) {
    return diminuto_list_head(rp);
}

static inline diminuto_scattergather_segment_t * diminuto_scattergather_record_segment_tail(diminuto_scattergather_record_t * rp) {
    return diminuto_list_tail(rp);
}

static inline diminuto_scattergather_segment_t * diminuto_scattergather_record_segment_next(diminuto_scattergather_record_t * rp, diminuto_scattergather_segment_t * sp) {
    return (diminuto_list_next(sp) == rp) ? (diminuto_scattergather_segment_t *)0 : diminuto_list_next(sp);
}

extern size_t diminuto_scattergather_record_enumerate(diminuto_scattergather_record_t * rp);

extern size_t diminuto_scattergather_record_measure(diminuto_scattergather_record_t * rp);

extern diminuto_scattergather_record_t * diminuto_scattergather_record_dump(FILE * fp, diminuto_scattergather_record_t * rp);

extern struct iovec * diminuto_scattergather_record_vectorize(diminuto_scattergather_record_t * rp, struct iovec va[], size_t nn);

static inline diminuto_scattergather_record_t * diminuto_scattergather_record_segment_free(diminuto_scattergather_pool_t * pp, diminuto_scattergather_record_t * rp, diminuto_scattergather_segment_t * sp)
{
    (void)diminuto_scattergather_segment_free(pp, diminuto_scattergather_record_segment_remove(rp, sp));
    return rp;
}

extern diminuto_scattergather_record_t * diminuto_scattergather_record_segments_free(diminuto_scattergather_pool_t * pp, diminuto_scattergather_record_t * rp);

extern diminuto_scattergather_record_t * diminuto_scattergather_record_allocate(diminuto_scattergather_pool_t * pp);

static inline void diminuto_scattergather_record_free(diminuto_scattergather_pool_t * pp, diminuto_scattergather_record_t * rp) {
    diminuto_scattergather_pool_put(pp, diminuto_scattergather_record_fini(diminuto_scattergather_record_segments_free(pp, rp)));
}

/*******************************************************************************
 * INPUT/OUTPUT
 ******************************************************************************/

/*
 * File-ish 
 */

extern ssize_t diminuto_scattergather_record_write(int fd, diminuto_scattergather_record_t * rp);

extern ssize_t diminuto_scattergather_record_read(int fd, diminuto_scattergather_record_t * rp);

/*
 * IPv4
 */

extern ssize_t diminuto_scattergather_record_send_generic(int fd, diminuto_scattergather_record_t * rp, diminuto_ipv4_t address, diminuto_port_t port);

static inline ssize_t diminuto_scattergather_record_send(int fd, diminuto_scattergather_record_t * rp) {
    return diminuto_scattergather_record_send_generic(fd, rp, DIMINUTO_IPC4_UNSPECIFIED, 0);
}

extern ssize_t diminuto_scattergather_record_receive_generic(int fd, diminuto_scattergather_record_t * rp, diminuto_ipv4_t * addressp, diminuto_port_t * portp);

static inline ssize_t diminuto_scattergather_record_receive(int fd, diminuto_scattergather_record_t * rp) {
    return diminuto_scattergather_record_receive_generic(fd, rp, (diminuto_ipv4_t *)0, (diminuto_port_t *)0);
}

/*
 * IPv6
 */

extern ssize_t diminuto_scattergather_record_send6_generic(int fd, diminuto_scattergather_record_t * rp, diminuto_ipv6_t address, diminuto_port_t port);

static inline ssize_t diminuto_scattergather_record_send6(int fd, diminuto_scattergather_record_t * rp) {
    return diminuto_scattergather_record_send6_generic(fd, rp, DIMINUTO_IPC6_UNSPECIFIED, 0);
}

extern ssize_t diminuto_scattergather_record_receive6_generic(int fd, diminuto_scattergather_record_t * rp, diminuto_ipv6_t * addressp, diminuto_port_t * portp);

static inline ssize_t diminuto_scattergather_record_receive6(int fd, diminuto_scattergather_record_t * rp) {
    return diminuto_scattergather_record_receive6_generic(fd, rp, (diminuto_ipv6_t *)0, (diminuto_port_t *)0);
}

#endif
