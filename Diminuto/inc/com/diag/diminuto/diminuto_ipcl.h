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
 * The IPCL features offers an IPC-style API to Local (UNIX domain) sockets
 * (AF_UNIX a.k.a. AF_LOCAL). Local sockets use paths in the file system
 * instead of IP addresses and ports to identify the endpoints of the
 * connection. They are only useful for peers, providers, or consumers
 * that are running in the same process environment and file system.
 *
 * REFERENCES
 *
 * C. Sridharan, "File Descriptor Transfer over Unix Domain Sockets",
 * CopyConstruct, August 2020
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_ipc.h"
#include <string.h>
#include <sys/socket.h>

/*******************************************************************************
 * GLOBALS
 ******************************************************************************/

/**
 * This is the Diminuto Local path that is unnamed.
 */
extern const char DIMINUTO_IPCL_UNNAMED[];

/*******************************************************************************
 * COMPARATORS
 ******************************************************************************/

/**
 * Compares two Local paths. This does little except perform a string
 * comparison. The paths should be canonicalized beforehand or the result
 * may be unexpected (e.g. different softlinks may ultimately point to the
 * same file). Does a NULL pointer check so that the canonicalize function
 * (which can return NULL) can be used directly as an argument. NULL pointers
 * are not considered equal.
 * @param path1p points to the first Local path.
 * @param path2p points to the second Local path.
 * @return 0 if equal, >0 if greater than, or <0 if less than.
 */
static inline int diminuto_ipcl_compare(const char * path1p, const char * path2p)
{
    return (path1p == (const char *)0)
        ? (((int)1) << ((sizeof(int) * 8) - 1))
        : (path2p == (const char *)0) 
            ? (~(((int)1) << ((sizeof(int) * 8) - 1)))
            : strncmp(path1p, path2p, sizeof(diminuto_path_t));
}

/*******************************************************************************
 * CLASSIFIERS
 ******************************************************************************/

/**
 * Return true if the Local path is unnamed.
 * @param pathp points to a Local path.
 * @return true or false.
 */
static inline int diminuto_ipcl_is_unnamed(const char * pathp) {
    return ((pathp != (const char *)0) && (pathp[0] == '\0'));
}

/*******************************************************************************
 * RESOLVERS
 ******************************************************************************/

/**
 * Expand, canonicalize, and validate a Local path string. This resolves all
 * of the softlinks, expands all of the path meta characters, and verifies that
 * all but the last path component exists.
 * @param path points to the Local path string.
 * @param buffer points to the buffer where the canonicalized string is stored.
 * @param size is the size of the output buffer in bytes.
 * @return a pointer to the output buffer or NULL if an error occurred.
 */
extern char * diminuto_ipcl_path(const char * path, char * buffer, size_t size);

/*******************************************************************************
 * STRINGIFIERS
 ******************************************************************************/

/**
 * Convert an Local path into a printable Local path string. This does little
 * but return a pointer to the Local path. For best results the path should
 * be canonicalized. Includes a NULL pointer check so that the canonicalize
 * function (which can return NULL) can be used directly as an argument; a
 * NULL pointer prints as an asterisk, which is not a valid character in a
 * canonical path, and which distinguishes it from the empty string, which is
 * also a valid path that indicates an unnamed Local socket.
 * @param path is the Local path.
 * @return a pointer to the output buffer.
 */
static inline const char * diminuto_ipcl_path2string(const char * path)
{
    return (path == (const char *)0) ? "*" : path;
}

/*******************************************************************************
 * INTERROGATORS
 ******************************************************************************/

/**
 * Return the address and port of the near end of the socket if it can be
 * determined. If it cannot be determined, the path variable will remain
 * unchanged.
 * @param fd is a socket.
 * @param pathp if non-NULL points to where the path will be stored.
 * @param psize is the size of the path variable in bytes.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipcl_nearend(int fd, char * pathp, size_t psize);

/**
 * Return the address and port of the far end of the socket if it can be
 * determined. If it cannot be determined, the path variable will remain
 * unchanged.
 * @param fd is a socket.
 * @param pathp if non-NULL points to where the path will be stored.
 * @param psize isa the size of th epath variable in bytes.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipcl_farend(int fd, char * pathp, size_t psize);

/*******************************************************************************
 * SOCKETS
 ******************************************************************************/

/**
 * Bind an existing socket to a specific path. For best results, the
 * path should be canonicalized. An empty string ("") suppresses the
 * bind and is not otherwise an error. A NULL pointer is an error, and
 * is checked for explicitly so that the output of the canonicalize
 * function can be used directly as an argument.
 * @param fd is the socket.
 * @param path is the path to which to bind or "" for an unnamed socket.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipcl_source(int fd, const char * path);

/**
 * Shutdown a socket. This eliminates the transmission of any pending data.
e* @param fd is an open socket of any type.
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

/**
 * Remove the name in the file system associated with a Local
 * socket. For best results, the path should be canonicalized. This
 * does not happen automatically when the associated socket is closed
 * but the file system name can be removed while the file descriptor
 * remains open; this is really only useful when using stream (connection-
 * oriented) sockets.  A NULL pointer is an error, and is checked for
 * explicitly so that the output of the canonicalize function can be
 * used directly as an argument.
 * @param path is the name to be removed.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipcl_remove(const char * path);

/*******************************************************************************
 * STREAM SOCKETS
 ******************************************************************************/

/**
 * Create a provider-side stream socket bound to a specific Local path and with
 * a specific connection backlog. If an optional function is provided by the
 * caller, invoke it to set socket options before the listen(2) is performed.
 * @param path is the path in the file system or "" for an unnamed socket.
 * @param backlog is the limit to how many incoming connections may be queued, <0 for the default.
 * @param functionp points to an optional function to set socket options.
 * @param datap is passed to the optional function.
 * @return a provider-side stream socket or <0 if an error occurred.
 */
extern int diminuto_ipcl_stream_provider_base(const char * path, int backlog, diminuto_ipc_injector_t * functionp, void * datap);

/**
 * Create a provider-side stream socket bound to a specific path and with
 * a specific connection backlog.
 * @param path is the path in the file system or "" for an unnamed socket.
 * @param backlog is the limit to how many incoming connections may be queued, <0 for the default.
 * @return a provider-side stream socket or <0 if an error occurred.
 */
static inline int diminuto_ipcl_stream_provider_generic(const char * path, int backlog) {
    return diminuto_ipcl_stream_provider_base(path, backlog, diminuto_ipc_inject_defaults, (void *)0);
}

/**
 * Create a provider-side stream socket with the maximum connection backlog.
 * @param path is the path in the file system or "" for an unnamed socket.
 * @return a provider-side stream socket or <0 if an error occurred.
 */
static inline int diminuto_ipcl_stream_provider(const char * path) {
    return diminuto_ipcl_stream_provider_generic(path, -1);
}

/**
 * Wait for and accept a connection request from a consumer on a provider-side
 * stream socket.
 * @param fd is the provider-side stream socket.
 * @param pathp if non-NULL points to where the path will be stored.
 * @param psize is the size of the path variable in bytes.
 * @return a data stream socket to the requestor or <0 if an error occurred.
 */
extern int diminuto_ipcl_stream_accept_generic(int fd, char * pathp, size_t psize);

/**
 * Wait for and accept a connection request from a consumer on a provider-side
 * stream socket.
 * @param fd is the provider-side stream socket.
 * @return a data stream socket to the requestor or <0 if an error occurred.
 */
static inline int diminuto_ipcl_stream_accept(int fd) {
    return diminuto_ipcl_stream_accept_generic(fd, (char *)0, 0);
}

/**
 * Request a consumer-side stream socket to a provider using a specific path.
 * If an optional function is provided by the caller, invoke it to set socket
 * options before the connect(2) is performed.
 * @param path is the provider path in the file system.
 * @param path0 is the consumer path in the file system or "" for unnamed.
 * @param functionp points to an optional function to set socket options.
 * @param datap is passed to the optional function.
 * @return a data stream socket to the provider or <0 if an error occurred.
 */
extern int diminuto_ipcl_stream_consumer_base(const char * path, const char * path0, diminuto_ipc_injector_t * functionp, void * datap);

/**
 * Request a consumer-side stream socket to a provider using a specific path.
 * @param path is the provider path in the file system.
 * @param path0 is the consumer path in the file system or "" for unnamed.
 * @return a data stream socket to the provider or <0 if an error occurred.
 */
static inline int diminuto_ipcl_stream_consumer_generic(const char * path, const char * path0) {
    return diminuto_ipcl_stream_consumer_base(path, path0, (diminuto_ipc_injector_t *)0, (void *)0);
}

/**
 * Request a consumer-side stream socket to a provider.
 * @param path is the provider path in the file system.
 * @return a data stream socket to the provider or <0 if an error occurred.
 */
static inline int diminuto_ipcl_stream_consumer(const char * path) {
    return diminuto_ipcl_stream_consumer_generic(path, "");
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
 * @param path is the path in the file system or "" for an unnamed socket.
 * @param functionp points to an optional function to set socket options.
 * @param datap is passed to the optional function.
 * @return a peer datagram socket or <0 if an error occurred.
 */
extern int diminuto_ipcl_datagram_peer_base(const char * path, diminuto_ipc_injector_t * functionp, void * datap);

/**
 * Request a peer datagram socket.
 * @param path is the path in the file system or "" for an unnamed socket.
 * @return a peer datagram socket or <0 if an error occurred.
 */
static inline int diminuto_ipcl_datagram_peer_generic(const char * path) {
    return diminuto_ipcl_datagram_peer_base(path, diminuto_ipc_inject_defaults, (void *)0);
}

/**
 * Request a peer datagram socket.
 * @param path is the path in the file system or "" for an unnamed socket.
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
 * @param pathp if non-NULL points to where sending the path will be stored.
 * @param psize is the size of the path variable in bytes.
 * @param flags is the recvfrom(2) flags to be used.
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EAGAIN for non-blocking, EINTR for timer expiry).
 */
extern ssize_t diminuto_ipcl_datagram_receive_generic(int fd, void * buffer, size_t size, char * pathp, size_t psize, int flags);

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
    return diminuto_ipcl_datagram_receive_generic(fd, buffer, size, (char *)0, 0, 0);
}

/**
 * Send a datagram to a datagram socket using the specified flags. Flags may be
 * specified for sendto(2) such as MSG_DONTWAIT, MSG_WAITALL, or MSG_OOB. (This
 * function can legitimately be also used with a stream socket by passing zero
 * as the port number, in which case the path is ignored.)
 * @param fd is an open datagram socket.
 * @param buffer points to the buffer from which data is send.
 * @param size is the maximum number of bytes to be sent.
 * @param path is the receiver's file system path.
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
 * @param path is the receiver's file system path.
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EAGAIN for non-blocking, EINTR for timer expiry).
 */
static inline ssize_t diminuto_ipcl_datagram_send(int fd, const void * buffer, size_t size, const char * path) {
    return diminuto_ipcl_datagram_send_generic(fd, buffer, size, path, 0);
}

#endif
