/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_IPC_
#define _H_COM_DIAG_DIMINUTO_IPC_

/**
 * @file
 *
 * Copyright 2010-2016 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * Provides socket and other interprocess/interprocessor communication
 * capabilities that are agnostic as to whether IPv4 or IPv6 is being used.
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_fd.h"
#include <bits/sockaddr.h>
#include <linux/limits.h>

/**
 * Defines the prototype of a function used for dependency injection.
 */
typedef int (diminuto_ipc_injector_t)(int fd, void *);

/**
 * Convert a service name or a port number string into a port in host byte
 * order.
 * @param service points to the service name or port number string.
 * @param protocol points to the protocol name (e.g. "udp" or "tcp").
 * @return the port number or 0 if no such service exists for the protocol.
 */
extern diminuto_port_t diminuto_ipc_port(const char * service, const char * protocol);

/**
 * Shutdown a socket. This eliminates the transmission of any pending data.
 * @param fd is an open socket of any type.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_shutdown(int fd);

/**
 * Close a socket. Unless the socket has been shutdown, pending data will
 * still be transmitted.
 * @param fd is an open socket of any type.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_close(int fd);

/**
 * Bind a socket to a particular interface identified by name, e.g. "eth0"
 * etc. Only data received by this interface will be processed by the socket.
 * If the name is an empty (zero length) string, the socket will be
 * disassociated with any interface with which it was previously been bound.
 * (Caller probably needs to have root or other privileges.)
 * @param fd is an open socket that is not of type packet.
 * @param interface is the name of the network interface.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_interface(int fd, const char * interface);

/**
 * Enable or disable the non-blocking status.
 * @param fd is an open socket of any type.
 * @param enable is !0 to enable, 0 to disable.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_nonblocking(int fd, int enable);

/**
 * Enable or disable the address reuse option.
 * @param fd is an open socket of any type.
 * @param enable is !0 to enable, 0 to disable.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_reuseaddress(int fd, int enable);

/**
 * Enable or disable the keep alive option.
 * @param fd is an open socket of any type.
 * @param enable is !0 to enable, 0 to disable.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_keepalive(int fd, int enable);

/**
 * Enable or disable the debug option (only available to root on most systems).
 * @param fd is an open socket of any type.
 * @param enable is !0 to enable, 0 to disable.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_debug(int fd, int enable);

/**
 * Enable or disable the linger option.
 * @param fd is an open socket of any type.
 * @param ticks is the number of ticks to linger (although
 * lingering has granularity of seconds), or 0 for no lingering.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_linger(int fd, diminuto_ticks_t ticks);

/**
 * Enable or disable the TCP No Delay option. (Useful for reducing latency
 * when using small packets.)
 * @param fd is an open socket.
 * @param enable is !0 to enable, 0 to disable.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_nodelay(int fd, int enable);

/**
 * Enable or disable the TCP Quick Acknowledgement option. (Useful for
 * reducing latency when using small packets.)
 * @param fd is an open strean socket.
 * @param enable is !0 to enable, 0 to disable.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_quickack(int fd, int enable);

/**
 * Change the send TCP buffer size. (Useful for setting to the bandwidth-
 * delay product for long latency paths to achieve more throughput.)
 * @param fd is an open socket.
 * @param size is the buffer size in bytes or <0 for no change.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_send(int fd, ssize_t size);

/**
 * Change the receive TCP buffer size. (Useful for settig to the bandwidth-
 * delay product for long latency paths to achieve more throughput.)
 * @param fd is an open socket.
 * @param size is the buffer size in bytes or <0 for no change.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_receive(int fd, ssize_t size);

/**
 * Enable or disable the ability to handle IPv4 packets on an ipc6 socket
 * via the IPV6 Only option.
 * @param fd is an open ipc6 socket.
 * @param enable is !0 to enable, 0 to disable.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc6_set_ipv6only(int fd, int enable);

/*
 * (Many other options are possible, but these are the ones I have used.)
 */

/**
 * Dependency injector used to set the the socket options in the
 * ipc4 and ipc6 APIs.
 */
extern diminuto_ipc_injector_t diminuto_ipc_inject_defaults;

/**
 * Read bytes from a stream socket into a buffer until at least a minimum
 * number of bytes are read and no more than a maximum number of bytes are
 * read. Less than the minimum can still be read if an error occurs.
 * @param fd is an open stream socket.
 * @param buffer points to the buffer into which data is read.
 * @param min is the minimum number of bytes to be read.
 * @param max is the maximum number of bytes to be read.
 * @return the number of bytes received, 0 if the far end closed,
 * or <0 if an error occurred (errno will be EGAIN for non-blocking,
 * EINTR for timer expiry).
 */
static inline ssize_t diminuto_ipc_stream_read_generic(int fd, void * buffer, size_t min, size_t max) {
    return diminuto_fd_read_generic(fd, buffer, min, max);
}

/**
 * Read bytes from a stream socket into a buffer.
 * @param fd is an open stream socket.
 * @param buffer points to the buffer into which data is read.
 * @param size is the maximum number of bytes to be read.
 * @return the number of bytes received, 0 if the far end closed,
 * or <0 if an error occurred (errno will be EGAIN for non-blocking,
 * EINTR for timer expiry).
 */
static inline ssize_t diminuto_ipc_stream_read(int fd, void * buffer, size_t size) {
    return diminuto_fd_read(fd, buffer, size);
}

/**
 * Write bytes to a stream socket from a buffer until at least a minimum
 * number of bytes are written and no more than a maximum number of bytes are
 * written. Less than the minimum can still be written if an error occurs.
 * @param fd is an open stream socket.
 * @param buffer points to the buffer from which data is written.
 * @param min is the minimum number of bytes to be written.
 * @param max is the maximum number of bytes to be written.
 * @return the number of bytes received, 0 if the far end closed,
 * or <0 if an error occurred (errno will be EGAIN for non-blocking,
 * EINTR for timer expiry).
 */
static inline ssize_t diminuto_ipc_stream_write_generic(int fd, const void * buffer, size_t min, size_t max) {
    return diminuto_fd_write_generic(fd, buffer, min, max);
}

/**
 * Write bytes to a stream socket from a buffer.
 * @param fd is an open stream socket.
 * @param buffer points to the buffer from which data is written.
 * @param size is the maximum number of bytes to be written.
 * @return the number of bytes received, 0 if the far end closed,
 * or <0 if an error occurred (errno will be EGAIN for non-blocking,
 * EINTR for timer expiry).
 */
static inline ssize_t diminuto_ipc_stream_write(int fd, const void * buffer, size_t size) {
    return diminuto_fd_write(fd, buffer, size);
}

/**
 * Return an array of strings containing a list of the network interfaces
 * available on the host. This is a single block of dynamically allocated
 * memory that should be eventually deallocated by the application using
 * free(3).
 * @return a list of available network interfaces or NULL if an error occurred.
 */
extern char ** diminuto_ipc_interfaces(void);

#endif
