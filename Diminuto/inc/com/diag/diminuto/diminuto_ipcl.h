/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_IPCL_
#define _H_COM_DIAG_DIMINUTO_IPCL_

/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Provides a interface to stream, datagram, and packet Local sockets.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The IPCL features offers an IPC-style API to UNIX (or Local) Domain sockets
 * (AF_UNIX a.k.a. AF_LOCAL).
 *
 * REFERENCES
 *
 * C. Sridharan, "File Descriptor Transfer over Unix Domain Sockets",
 * CopyConstruct, August 2020
 */

#include "com/diag/diminuto/diminuto_ipc.h"

/*******************************************************************************
 * COMPARATORS
 ******************************************************************************/

/**
 * Compares two Local paths. This is not as simple as doing a string
 * comparison: all of the soft links and special directory characters in
 * each path is resolved before the comparison. This means Local paths
 * that look quite different may in fact resolve to the same UNIX domain socket.
 * @param path1p points to the first Local path.
 * @param path2p points to the second Local path.
 * @return 0 if equal, <0 if less than, >0 if greater than.
 */
extern int diminuto_ipcl_compare(const char * path1p, const char * path2p);

/*******************************************************************************
 * STRINGIFIERS
 ******************************************************************************/

/**
 * Convert an Local path in host byte order into a printable Local path
 * string in dot notation. This has the side effect of resolving all the soft
 * links and special directory characters in the path in the output buffer,
 * and can be used specifically for that purpose.
 * @param path is the Local path in host byte order.
 * @param buffer points to the buffer into to which the string is stored.
 * @param length is the length of the buffer in bytes.
 */
extern const char * diminuto_ipcl_path2string(const char * path, char * buffer, size_t length);

/*******************************************************************************
 * SOCKETS
 ******************************************************************************/

/**
 * Bind an existing socket to a specific path.
 * @param fd is the socket.
 * @param path is the path to which to bind.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipcl_source(int fd, const char * path);

/**
 * Shutdown a socket. This eliminates the transmission of any pending data.
 * @param fd is an open socket of any type.
 * @return 0 for success or <0 if an error occurred.
 */
static inline int diminuto_ipcl_shutdown(int fd) {
    return diminuto_ipc_shutdown(fd);
}

/**
 * Close a socket. Unless the socket has been shutdown, pending data will
 * still be transmitted.
 * @param fd is an open socket of any type.
 * @return >=0 for success or <0 if an error occurred.
 */
static inline int diminuto_ipcl_close(int fd) {
    return diminuto_ipc_close(fd);
}

/*******************************************************************************
 * STREAM SOCKETS
 ******************************************************************************/

/**
 * Create a provider-side stream socket bound to a specific path and with
 * a specific connection backlog. If an optional function is provided by the
 * caller, invoke it to set socket options before the listen(2) is performed.
 * @param path is the path of the interface that will be used.
 * @param backlog is the limit to how many incoming connections may be queued, <0 for the default.
 * @param functionp points to an optional function to set socket options.
 * @param datap is passed to the optional function.
 * @return a provider-side stream socket or <0 if an error occurred.
 */
extern int diminuto_ipcl_stream_provider_base(const char * path, int backlog, diminuto_ipc_injector_t * functionp, void * datap);

/**
 * Create a provider-side stream socket bound to a specific path and with
 * a specific connection backlog.
 * @param path is the path of the interface that will be used.
 * @param backlog is the limit to how many incoming connections may be queued, <0 for the default.
 * @return a provider-side stream socket or <0 if an error occurred.
 */
static inline int diminuto_ipcl_stream_provider_generic(const char * path, int backlog) {
    return diminuto_ipcl_stream_provider_base(path, backlog, diminuto_ipc_inject_defaults, (void *)0);
}

/**
 * Create a provider-side stream socket with the maximum connection backlog.
 * @param port is the port number at which connection requests will rendezvous.
 * @return a provider-side stream socket or <0 if an error occurred.
 */
static inline int diminuto_ipcl_stream_provider(const char * path) {
    return diminuto_ipcl_stream_provider_generic(path, -1);
}

/**
 * Wait for and accept a connection request from a consumer on a provider-side
 * stream socket.
 * @param fd is the provider-side stream socket.
 * @return a data stream socket to the requestor or <0 if an error occurred.
 */
extern int diminuto_ipcl_stream_accept_generic(int fd);

/**
 * Wait for and accept a connection request from a consumer on a provider-side
 * stream socket.
 * @param fd is the provider-side stream socket.
 * @return a data stream socket to the requestor or <0 if an error occurred.
 */
static inline int diminuto_ipcl_stream_accept(int fd) {
    return diminuto_ipcl_stream_accept_generic(fd);
}

/**
 * Request a consumer-side stream socket to a provider using a specific path.
 * If an optional function is provided by the caller, invoke it to set socket
 * options before the connect(2) is performed.
 * @param path is the provider's Local path.
 * @param functionp points to an optional function to set socket options.
 * @param datap is passed to the optional function.
 * @return a data stream socket to the provider or <0 if an error occurred.
 */
extern int diminuto_ipcl_stream_consumer_base(const char * path, diminuto_ipc_injector_t * functionp, void * datap);

/**
 * Request a consumer-side stream socket to a provider using a specific path.
 * @param path is the provider's Local path.
 * @return a data stream socket to the provider or <0 if an error occurred.
 */
static inline int diminuto_ipcl_stream_consumer_generic(const char * path) {
    return diminuto_ipcl_stream_consumer_base(path, (diminuto_ipc_injector_t *)0, (void *)0);
}

/**
 * Request a consumer-side stream socket to a provider.
 * @param path is the provider's Local path in host byte order.
 * @return a data stream socket to the provider or <0 if an error occurred.
 */
static inline int diminuto_ipcl_stream_consumer(const char * path) {
    return diminuto_ipcl_stream_consumer_generic(path);
}

/**
 * Read bytes from a stream socket into a buffer until at least a minimum
 * number of bytes are read and no more than a maximum number of bytes are
 * read. Less than the minimum can still be read if an error occurs.
 * @param fd is an open stream socket.
 * @param buffer points to the buffer into which data is read.
 * @param min is the minimum number of bytes to be read.
 * @param max is the maximum number of bytes to be read.
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EAGAIN for non-blocking, EINTR for timer expiry).
 */
static inline ssize_t diminuto_ipcl_stream_read_generic(int fd, void * buffer, size_t min, size_t max) {
    return diminuto_ipc_stream_read_generic(fd, buffer, min, max);
}

/**
 * Read bytes from a stream socket into a buffer.
 * @param fd is an open stream socket.
 * @param buffer points to the buffer into which data is read.
 * @param size is the maximum number of bytes to be read.
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EAGAIN for non-blocking, EINTR for timer expiry).
 */
static inline ssize_t diminuto_ipcl_stream_read(int fd, void * buffer, size_t size) {
    return diminuto_ipc_stream_read(fd, buffer, size);
}

/**
 * Write bytes to a stream socket from a buffer until at least a minimum
 * number of bytes are written and no more than a maximum number of bytes are
 * written. Less than the minimum can still be written if an error occurs.
 * @param fd is an open stream socket.
 * @param buffer points to the buffer from which data is written.
 * @param min is the minimum number of bytes to be written.
 * @param max is the maximum number of bytes to be written.
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EAGAIN for non-blocking, EINTR for timer expiry).
 */
static inline ssize_t diminuto_ipcl_stream_write_generic(int fd, const void * buffer, size_t min, size_t max) {
    return diminuto_ipc_stream_write_generic(fd, buffer, min, max);
}

/**
 * Write bytes to a stream socket from a buffer.
 * @param fd is an open stream socket.
 * @param buffer points to the buffer from which data is written.
 * @param size is the number of bytes to be written.
 * @return the number of bytes sent, 0 if the far end closed, or <0 if an error occurred (errno will be EAGAIN for non-blocking, EINTR for timer expiry).
 */
static inline ssize_t diminuto_ipcl_stream_write(int fd, const void * buffer, size_t size) {
    return diminuto_ipc_stream_write(fd, buffer, size);
}

/*******************************************************************************
 * DATAGRAM SOCKETS
 ******************************************************************************/

/**
 * Request a peer datagram socket. If an optional function is provided by the
 * caller, invoke it to set socket options.
 * @param path is the Local path of the interface to use.
 * @param functionp points to an optional function to set socket options.
 * @param datap is passed to the optional function.
 * @return a peer datagram socket or <0 if an error occurred.
 */
extern int diminuto_ipcl_datagram_peer_base(const char * path, diminuto_ipc_injector_t * functionp, void * datap);

/**
 * Request a peer datagram socket.
 * @param path is the Local path of the interface to use.
 * @return a peer datagram socket or <0 if an error occurred.
 */
static inline int diminuto_ipcl_datagram_peer_generic(const char * path) {
    return diminuto_ipcl_datagram_peer_base(path, diminuto_ipc_inject_defaults, (void *)0);
}

/**
 * Request a peer datagram socket.
 * @param path is the Local path of the interface to use.
 * @return a peer datagram socket or <0 if an error occurred.
 */
static inline int diminuto_ipcl_datagram_peer(const char * path) {
    return diminuto_ipcl_datagram_peer_generic(path);
}

/**
 * Receive a datagram from a datagram socket using the specified flags.
 * Flags may be specified for recvfrom(2) such as MSG_DONTWAIT, MSG_WAITALL,
 * or MSG_OOB. (This function can legitimately be also used with a stream
 * socket.)
 * @param fd is an open datagram socket.
 * @param buffer points to the buffer into which data is received.
 * @param size is the maximum number of bytes to be received.
 * @param flags is the recvfrom(2) flags to be used.
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EAGAIN for non-blocking, EINTR for timer expiry).
 */
extern ssize_t diminuto_ipcl_datagram_receive_generic(int fd, void * buffer, size_t size, int flags);

/**
 * Receive a datagram from a datagram socket using no flags. (This function can
 * legitimately be also
 * used with a stream socket.)
 * @param fd is an open datagram socket.
 * @param buffer points to the buffer into which data is received.
 * @param size is the maximum number of bytes to be received.
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EAGAIN for non-blocking, EINTR for timer expiry).
 */
static inline ssize_t diminuto_ipcl_datagram_receive(int fd, void * buffer, size_t size) {
    return diminuto_ipcl_datagram_receive_generic(fd, buffer, size, 0);
}

/**
 * Send a datagram to a datagram socket using the specified flags. Flags may be
 * specified for sendto(2) such as MSG_DONTWAIT, MSG_WAITALL, or MSG_OOB. (This
 * function can legitimately be also used with a stream socket by passing zero
 * as the port number, in which case the path is ignored.)
 * @param fd is an open datagram socket.
 * @param buffer points to the buffer from which data is send.
 * @param size is the maximum number of bytes to be sent.
 * @param path is the receiver's path.
 * @param flags is the sendto(2) flags to be used.
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EAGAIN for non-blocking, EINTR for timer expiry).
 */
extern ssize_t diminuto_ipcl_datagram_send_generic(int fd, const void * buffer, size_t size, const char * path, int flags);

/**
 * Send a datagram to a datagram socket using no flags. (This function can
 * legitimately be also used with a stream socket by passing zero as the port
 * number, in which case the path is ignored.)
 * @param fd is an open datagram socket.
 * @param buffer points to the buffer from which data is send.
 * @param size is the maximum number of bytes to be sent.
 * @param path is the receiver's path.
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EAGAIN for non-blocking, EINTR for timer expiry).
 */
static inline ssize_t diminuto_ipcl_datagram_send(int fd, const void * buffer, size_t size, const char * path) {
    return diminuto_ipcl_datagram_send_generic(fd, buffer, size, path, 0);
}

#endif
