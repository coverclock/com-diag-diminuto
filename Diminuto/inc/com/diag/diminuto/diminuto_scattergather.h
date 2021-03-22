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
 * that is received/read into multiple fields. This reduces the amount of
 * buffer-to-buffer copying necessary when consolidating many fields (as in
 * a packet used by a protocol stack) to be transmitted atomically by a
 * single system call, or by distributing many fields (ditto) received
 * atomically into separate fields. Scatter/Gather can be used for
 * interprocess communication (IPC) or for reading and writing with
 * file and file-like interfaces.
 *
 * Four data structures are defined: Buffer, Segment, Record, and Pool.
 *
 * A Buffer is a Diminuto Buffer object that contains a length field in
 * bytes followed by a contiguous section of memory containing the payload.
 * The length field indicates the number of bytes in the payload, not
 * the total size of the Buffer object. (Dimunuto Buffers have their
 * own length field indicating the total length of the Buffer Object.)
 *
 * A Segment is a Diminuto List node containing a pointer to a Buffer
 * in its payload pointer field.
 *
 * A Record is a Diminuto List node containing a circular linked list
 * of used Segments. (All Diminuto Lists are circular.) Each Segment
 * points to a Buffer. When all of the Segments in the Record are
 * concatenated in the order of the List as traversed from first to
 * last, they form the total buffer that will be transmitted or
 * received.
 *
 * A Pool is a Diminuto list node containing a circular linked list of
 * unused Segments. (Diminuto Buffers have their own pool of unused
 * Buffer objects of varying lengths.)
 *
 * The API has two further sections: Record Segments (functions that
 * deal with Segments in relation to a Record), and Input/Output
 * (functions that use the underlying vector send/receive and read/write
 * system calls). The IPC section of the Input/Output API supports IPv4
 * and IPv6 stream and datagram connections.
 *
 * This feature was derived from the original Diminuto IPC Scatter/Gather
 * unit test, which still exists but is independent of this feature which
 * it predates.
 *
 * The function names are crazy long. But I'm using the same data type
 * (List) for three different objects, and it's important not to get
 * them confused. I'm hoping the function names will make it more obvious
 * if the developer has botched something up.
 *
 * SEE ALSO
 *
 * Chip Overclock, "Scatter/Gather", 2020-12-08,
 * <https://coverclock.blogspot.com/2020/12/scattergather.html>
 *
 * tst/unittest-scattergather4.c
 *
 * tst/unittest-scattergather6.c
 *
 * tst/unittest-ipc-scattergather.c
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

/**
 * This enumeration defines some useful constants about underlying
 * platform limitations like the maximum number of fields you can
 * have in a single I/O vector.
 */
enum DiminutoScatterGatherConstants {
    DIMINUTO_SCATTERGATHER_MAXIMUM = UIO_MAXIOV,    /* Nominally 1024. */
};

/*******************************************************************************
 * TYPES
 ******************************************************************************/

/**
 * This type is a header for a scatter/gather Buffer. The Buffer has a length
 * field followed by a contiguous payload field. The length field is the number
 * of bytes of data payload, not the entire length of the payload field.
 */
typedef struct DiminutoScatterGatherBuffer {
    uint64_t alignment[0]; /* This will cause -pedantic warnings. */
    /**
     * This is the length of the data in the payload field, not the
     * length of the payload field.
     * On some platforms (particularly those with 32-bit kernels),
     * size_t is 32 bits, and on others (64-bit kernels) it is 64
     * bits. So there may or may not be some slack in this structure.
     * But as Bob Dobbs reminds us, we all need slack.
     */
    size_t length; /* This is the data length, not the buffer length. */
    /**
     * This is the beginning of the payload field.
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

/**
 * This is the head of a List of Pool of unused List nodes. List nodes
 * may be used as Segments, or as Records.
 */
typedef diminuto_list_t diminuto_scattergather_pool_t;

/**
 * This is a List node used as a Segment. A Segment has a pointer
 * to a Buffer in its data field.
 */
typedef diminuto_list_t diminuto_scattergather_segment_t;

/**
 * This is a List used node as a Record. A Record is the head of
 * a List of Segments. The data read (received) or written (send)
 * is the concatenation of the payload of the Buffer in each Segment
 * in the Record, traversed from head to tail.
 */
typedef diminuto_list_t diminuto_scattergather_record_t;

/*******************************************************************************
 * GENERATORS
 ******************************************************************************/

/**
 * @def DIMINUTO_SCATTERGATHER_POOL_INIT
 * This is a static initializer for a Pool.
 */
#define DIMINUTO_SCATTERGATHER_POOL_INIT(_POINTER_) DIMINUTO_LIST_NULLINIT(_POINTER_)

/**
 * @def DIMINUTO_SCATTERGATHER_SEGMENT_INIT
 * This is a static initializer for a Segment.
 */
#define DIMINUTO_SCATTERGATHER_SEGMENT_INIT(_POINTER_) DIMINUTO_LIST_NULLINIT(_POINTER_)

/**
 * @def DIMINUTO_SCATTERGATHER_RECORD_INIT
 * This is a static initializer for a Record.
 */
#define DIMINUTO_SCATTERGATHER_RECORD_INIT(_POINTER_) DIMINUTO_LIST_NULLINIT(_POINTER_)

/**
 * @def diminuto_scattergather_bufferof
 * This operator returns the address of the Buffer when given the address
 * of the Buffer's payload as an argument.
 */
#define diminuto_scattergather_bufferof(_PAYLOAD_) diminuto_containerof(diminuto_scattergather_buffer_t, payload, (_PAYLOAD_))

/*******************************************************************************
 * POOL
 ******************************************************************************/

/**
 * Initialize a Pool to the empty state.
 * @param pp points to the Pool.
 * @return a pointer to the Pool.
 */
static inline diminuto_scattergather_pool_t * diminuto_scattergather_pool_init(diminuto_scattergather_pool_t * pp) {
    return diminuto_list_nullinit(pp);
}

/**
 * Deinitialize a Pool to the empty state. Any List nodes on the Pool are
 * removed.
 * @param pp points to the Pool.
 */
extern void diminuto_scattergather_pool_fini(diminuto_scattergather_pool_t * pp);

/**
 * Get a List node from the Pool. If the Pool is empty, NULL is returned.
 * Otherwise a pointer to the initialized List node is returned. The List
 * node may be used as a Segment or as a Record.
 * @param pp points to the Pool.
 * @return a pointer to the List node or NULL.
 */
extern diminuto_list_t * diminuto_scattergather_pool_get(diminuto_scattergather_pool_t * pp);

/**
 * Put a List node in the Pool. The List node may be placed anywhere in the
 * Pool. The data field of the List node is zeroed out. The List node may
 * be a Segment or a Record (or, indeed, any other kind of List node).
 * @param pp points to the Pool.
 * @param np points to the List node.
 */
extern void diminuto_scattergather_pool_put(diminuto_scattergather_pool_t * pp, diminuto_list_t * np);

/**
 * Populate a Pool with an array of List nodes. The data field of each List
 * node is zeroed out. Ths List nodes may be used as a Segment or as a Record.
 * @param pp points to the Pool.
 * @param sa is an array of List nodes.
 * @param sn is the number of List ndoes in the array.
 */
extern diminuto_scattergather_pool_t * diminuto_scattergather_pool_populate(diminuto_scattergather_pool_t * pp, diminuto_list_t sa[], size_t sn);

/*******************************************************************************
 * SEGMENT
 ******************************************************************************/

/**
 * Initialize a Segment. The data field of the Segment is zeroed out.
 * @param sp points to the Segment.
 * @return a pointer to the Segment.
 */
static inline diminuto_scattergather_segment_t * diminuto_scattergather_segment_init(diminuto_scattergather_segment_t * sp) {
    return diminuto_list_nullinit(sp);
}

/**
 * Deinitialize a Segment. The Segment is removed from any Record it may be
 * on. The data field of the Segment is zeroed out.
 * @param sp points to the Segment.
 * @return a pointer to the Segment.
 */
static inline diminuto_scattergather_segment_t * diminuto_scattergather_segment_fini(diminuto_scattergather_segment_t * sp) {
    return diminuto_list_dataset(diminuto_list_remove(sp), (void *)0);
}

/**
 * Get the pointer to the payload field in the Buffer pointed to by a Segment.
 * The address returned by this function will be aligned on at least
 * an eight-byte boundary.
 * @param sp points to the Segment.
 * @return a pointer to the payload field.
 */
static inline void * diminuto_scattergather_segment_payload_get(const diminuto_scattergather_segment_t * sp) {
    return (void *)(&(((diminuto_scattergather_buffer_t *)diminuto_list_data(sp))->payload[0]));
}

/**
 * Get the length field in the Buffer pointed to by a Segment. This is the
 * length in bytes of the data in the payload field, not the total length
 * of the Buffer (some or all of which may be unused).
 * @param sp points to the Segment.
 * @return the length field.
 */
static inline size_t diminuto_scattergather_segment_length_get(const diminuto_scattergather_segment_t * sp) {
    return ((diminuto_scattergather_buffer_t *)diminuto_list_data(sp))->length;
}

/**
 * Set the length field in the Buffer pointed to by a Segment. This is the
 * length in bytes of the data in the payload field, not the total length
 * of the Buffer (some or all of which may be unused).
 * @param sp points to the Segment.
 * @param ll is the payload length in bytes.
 * @return the length field.
 */
static inline size_t diminuto_scattergather_segment_length_set(diminuto_scattergather_segment_t * sp, size_t ll) {
    return ((diminuto_scattergather_buffer_t *)diminuto_list_data(sp))->length = ll;
}

/**
 * Get a Segment from a Pool. A Buffer of the specified size is allocated.
 * The data field of the Segment is set to the Buffer. For convenience,
 * the length field of the Buffer is set to the specified size (which the
 * caller may change).
 * @param pp points to the Pool.
 * @param size is the requested size in bytes.
 * @return a pointer to the Segment.
 */
extern diminuto_scattergather_segment_t * diminuto_scattergather_segment_allocate(diminuto_scattergather_pool_t * pp, size_t size);

/**
 * Free a Segment. The Buffer associated with the Segment is freed. The
 * Segment is deinitialized (which removes it from any Record it was on).
 * The Segment is returned to the Pool.
 * @param pp points to the Pool.
 * @param sp points to the Segment.
 */
static inline void diminuto_scattergather_segment_free(diminuto_scattergather_pool_t * pp, diminuto_scattergather_segment_t * sp)
{
    diminuto_buffer_free(diminuto_list_data(sp));
    (void)diminuto_scattergather_pool_put(pp, diminuto_scattergather_segment_fini(sp));
}

/*******************************************************************************
 * RECORD
 ******************************************************************************/

/**
 * Initialize a Record to the empty state. The data field is zeroed out.
 * @param rp points to the Record.
 * @return a pointer to the Record.
 */
static inline diminuto_scattergather_record_t * diminuto_scattergather_record_init(diminuto_scattergather_record_t * rp) {
    return diminuto_list_nullinit(rp);
}

/**
 * Deinitialize a Record. The Record is removed from any List it is on. The
 * data field is zeroed out.
 * @param rp points to the Record.
 * @return a pointer to the Record.
 */
static inline diminuto_scattergather_record_t * diminuto_scattergather_record_fini(diminuto_scattergather_record_t * rp) {
    return diminuto_list_dataset(diminuto_list_remove(rp), (void *)0);
}

/**
 * Allocate a Record from a Pool. The Record is initially empty.
 * @param pp points to the Pool.
 * @return a pointer to the Record or NULL if the Pool is empty.
 */
extern diminuto_scattergather_record_t * diminuto_scattergather_record_allocate(diminuto_scattergather_pool_t * pp);

/**
 * Free all Segments contained in a Record. Each Segment is removed from
 * the Record. The Buffer associated with each Record is freed. The data
 * field if each segment is zeroed out. Each Segment is returned to the
 * Pool. The Record itself is NOT returned to the Pool.
 * @param pp points to the Pool.
 * @param rp points to the Record.
 * @return a pointer to the Record.
 */
extern diminuto_scattergather_record_t * diminuto_scattergather_record_segments_free(diminuto_scattergather_pool_t * pp, diminuto_scattergather_record_t * rp);

/**
 * Free a Record. All of the Segments associated with the Record are freed.
 * The Record is deinitialized and put in the Pool.
 * @param pp points to the Pool.
 * @param rp points to the Record.
 */
static inline void diminuto_scattergather_record_free(diminuto_scattergather_pool_t * pp, diminuto_scattergather_record_t * rp) {
    diminuto_scattergather_pool_put(pp, diminuto_scattergather_record_fini(diminuto_scattergather_record_segments_free(pp, rp)));
}

/**
 * Return the number of Segments associated with a Record.
 * @param rp points to the Record.
 * @return the number of Segments.
 */
extern size_t diminuto_scattergather_record_enumerate(const diminuto_scattergather_record_t * rp);

/**
 * Return the total number of bytes of payload in all Buffers of all Segments
 * associated with a Record.
 * @param rp points to the Record.
 * @return the total number of bytes of payload.
 */
extern size_t diminuto_scattergather_record_measure(const diminuto_scattergather_record_t * rp);

/**
 * Dump a report of useful information about a Record to the specified FILE
 * (e.g. stderr).
 * @param fp points to the FILE.
 * @param rp points to the Record.
 * @return a pointer to the Record.
 */
extern const diminuto_scattergather_record_t * diminuto_scattergather_record_dump(FILE * fp, const diminuto_scattergather_record_t * rp);

/**
 * Vectorize a Record by populating an I/O vector of a specified size with
 * a pointer to, and the size of, the payload of each Buffer in each Segment
 * of the Record. Note that having a vector that is too small is not an error
 * (so no error message is emitted), but NULL is returned in such cases.
 * @param rp points to the Record.
 * @param va points to the I/O vector (an array).
 * @param nn is the number of slots in the I/O vector.
 * @return a pointer to the I/O vector or NULL if it is too small.
 */
extern struct iovec * diminuto_scattergather_record_vectorize(const diminuto_scattergather_record_t * rp, struct iovec va[], size_t nn);

/*******************************************************************************
 * RECORD SEGMENTS
 ******************************************************************************/

/**
 * Remove a Segment from a Record. The Segment is removed from the Record it is
 * on (if any).
 * @param sp points to the Segment.
 * @return a pointer to the Segment.
 */
static inline diminuto_scattergather_segment_t * diminuto_scattergather_record_segment_remove(diminuto_scattergather_segment_t * sp) {
    return diminuto_list_remove(sp);
}

/**
 * Prepend a Segment onto a Record, which places it at the head of the Record.
 * @param rp points to the Record.
 * @param sp points to the Segment.
 * @return a pointer to the Segment.
 */
static inline diminuto_scattergather_segment_t * diminuto_scattergather_record_segment_prepend(diminuto_scattergather_record_t * rp, diminuto_scattergather_segment_t * sp) {
    return diminuto_list_push(rp, sp);
}

/**
 * Append a Segment onto a Record, which places it at the tail of the Record.
 * @param rp points to the Record.
 * @param sp points to the Segment.
 * @return a pointer to the Segment.
 */
static inline diminuto_scattergather_segment_t * diminuto_scattergather_record_segment_append(diminuto_scattergather_record_t * rp, diminuto_scattergather_segment_t * sp) {
    return diminuto_list_enqueue(rp, sp);
}

/**
 * Insert a Segment onto a Record after an existing Segment. The inserted
 * Segment becomes associated with whatever Record the existing Segment is
 * associated with.
 * @param op points to the existing Segment on a Record.
 * @param sp points to the Segment to be inserted.
 * @return a pointer to the inserted Segment.
 */
static inline diminuto_scattergather_segment_t * diminuto_scattergather_record_segment_insert(diminuto_scattergather_segment_t * op, diminuto_scattergather_segment_t * sp) {
    return diminuto_list_splice(op, sp);
}

/**
 * Replace a Segment on a Record with another Segment.
 * @param op points to the existing Segment to be replaced on a Record.
 * @param sp points to the new Segment.
 * @return a pointer to the Segment that was replaced.
 */
static inline diminuto_scattergather_segment_t * diminuto_scattergather_record_segment_replace(diminuto_scattergather_segment_t * op, diminuto_scattergather_segment_t * sp) {
    return diminuto_list_replace(op, sp);
}

/**
 * Return a pointer to the first Segment on a Record.
 * @param rp points to the Record.
 * @return a pointer to the head Segment or NULL if the Record is empty.
 */
static inline diminuto_scattergather_segment_t * diminuto_scattergather_record_segment_head(const diminuto_scattergather_record_t * rp) {
    return diminuto_list_head(rp);
}

/**
 * Return a pointer to the last Segment on a Record.
 * @param rp points to the Record.
 * @return a pointer to the tail Segment or NULL if the Record is empty.
 */
static inline diminuto_scattergather_segment_t * diminuto_scattergather_record_segment_tail(const diminuto_scattergather_record_t * rp) {
    return diminuto_list_tail(rp);
}

/**
 * Return a pointer to the next Segment after a specified Segment on a Record.
 * @param sp points to a Segment on a Record.
 * @return a pointer to the following Segment or NULL is none.
 */
static inline diminuto_scattergather_segment_t * diminuto_scattergather_record_segment_next(const diminuto_scattergather_segment_t * sp) {
    return (diminuto_list_next(sp) == diminuto_list_root(sp)) ? (diminuto_scattergather_segment_t *)0 : diminuto_list_next(sp);
}

/**
 * Return a pointer to the previous Segment before a specified Segment on
 * a Record.
 * @param sp points to a Segment on a Record.
 * @return a pointer to the preceeding Segment or NULL is none.
 */
static inline diminuto_scattergather_segment_t * diminuto_scattergather_record_segment_previous(const diminuto_scattergather_segment_t * sp) {
    return (diminuto_list_prev(sp) == diminuto_list_root(sp)) ? (diminuto_scattergather_segment_t *)0 : diminuto_list_prev(sp);
}

/**
 * Free a Segment that is on a Record. The Segment is removed from the Record.
 * The Segment is deinitialized and returned to the Pool.
 * @param pp points to the Pool.
 * @param sp points to the Segment.
 */
static inline void diminuto_scattergather_record_segment_free(diminuto_scattergather_pool_t * pp, diminuto_scattergather_segment_t * sp)
{
    (void)diminuto_scattergather_segment_free(pp, diminuto_scattergather_record_segment_remove(sp));
}

/*******************************************************************************
 * INPUT/OUTPUT
 ******************************************************************************/

/*
 * On a stream socket, you can send/send6 to a read, and write to a
 * receive/receive6, although the current unit tests don't exercise this.
 */

/*
 * File-like
 */

/**
 * Write a Record to a file descriptor. The descriptor may be a stream socket,
 * a file, or some other file-like data sink.
 * @param fd is open file descriptor.
 * @param rp points to the record.
 * @return the total number of bytes written, or <0 for an error.
 */
extern ssize_t diminuto_scattergather_record_write(int fd, const diminuto_scattergather_record_t * rp);

/**
 * Read a Record from a file descriptor. The descriptor may be a stream socket,
 * a file, or some other file-like data source.
 * @param fd is an open file descriptor.
 * @param rp points to the Record.
 * @return the total number of bytes read, 0 for EOF, or <0 for an error.
 */
extern ssize_t diminuto_scattergather_record_read(int fd, diminuto_scattergather_record_t * rp);

/*
 * IPC
 */

/**
 * Send a Record to a file descriptor. If the file descriptor is a socket,
 * it must be a stream socket.
 * @param fd is an open file descriptor.
 * @param rp points to the Record.
 * @return the total number of bytes sent, or <0 for an error.
 */
extern ssize_t diminuto_scattergather_record_ipc_stream_send(int fd, const diminuto_scattergather_record_t * rp);

/**
 * Receive a Record from a file descriptor. If the file descriptor is a socket,
 * it must be a stream socket.
 * @param fd is an open file descriptor.
 * @param rp points to the Record.
 * @return the total number of bytes sent, 0 for EOF, or <0 for an error.
 */
extern ssize_t diminuto_scattergather_record_ipc_stream_receive(int fd, diminuto_scattergather_record_t * rp);

/*
 * IPC4
 */

/**
 * Send a Record to a datagram socket using a IPv4 address and a port number.
 * @param fd is an open file descriptor.
 * @param rp points to the Record.
 * @param address is an IPv4 address.
 * @param port is a port number.
 * @return the total number of bytes sent, or <0 for an error.
 */
extern ssize_t diminuto_scattergather_record_ipc4_datagram_send(int fd, const diminuto_scattergather_record_t * rp, diminuto_ipv4_t address, diminuto_port_t port);

/**
 * Receive a Record from a datagram socket. Optionally return the IPv4 address
 * and port number of the sender.
 * @param fd is an open file descriptor.
 * @param rp points to the Record.
 * @param addressp points to an IPv4 address variable, or NULL.
 * @param portp points to a port number variable, or NULL.
 * @return the total number of bytes sent, 0 for EOF, or <0 for an error.
 */
extern ssize_t diminuto_scattergather_record_ipc4_datagram_receive(int fd, diminuto_scattergather_record_t * rp, diminuto_ipv4_t * addressp, diminuto_port_t * portp);

/*
 * IPC6
 */

/**
 * Send a Record to a datagram socket using a IPv6 address and a port number.
 * @param fd is an open file descriptor.
 * @param rp points to the Record.
 * @param address is an IPv6 address.
 * @param port is a port number.
 * @return the total number of bytes sent, or <0 for an error.
 */
extern ssize_t diminuto_scattergather_record_ipc6_datagram_send(int fd, const diminuto_scattergather_record_t * rp, diminuto_ipv6_t address, diminuto_port_t port);

/**
 * Receive a Record from a datagram socket. Optionally return the IPv6 address
 * and port number of the sender.
 * @param fd is an open file descriptor.
 * @param rp points to the Record.
 * @param addressp points to an IPv6 address variable, or NULL.
 * @param portp points to a port number variable, or NULL.
 * @return the total number of bytes sent, 0 for EOF, or <0 for an error.
 */
extern ssize_t diminuto_scattergather_record_ipc6_datagram_receive(int fd, diminuto_scattergather_record_t * rp, diminuto_ipv6_t * addressp, diminuto_port_t * portp);

#endif
