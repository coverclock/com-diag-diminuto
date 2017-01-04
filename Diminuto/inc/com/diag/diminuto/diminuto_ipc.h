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
 * @param fd is an open socket that is not of type packet.
 * @param interface is the name of the network interface.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_interface(int fd, const char * interface);

/**
 * Set a socket integer value option.
 * @param fd is an open socket of any type.
 * @param value is the value to set.
 * @param option is option name.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_value(int fd, int value, int option);

/**
 * Enable or disable a socket boolean option.
 * @param fd is an open socket of any type.
 * @param enable is !0 to enable an option, 0 to disable an option.
 * @param option is option name.
 * @return >=0 for success or <0 if an error occurred.
 */
static inline int diminuto_ipc_set_boolean(int fd, int enable, int option) {
    return diminuto_ipc_set_value(fd, !!enable, option);
}

/**
 * Enable or disable a TCP option.
 * @param fd is an open stream socket.
 * @param value is the value of the option.
 * @param option is option name.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_tcpoption(int fd, int value, int option);

/**
 * Enable or disable a IPv6 option.
 * @param fd is an open IPv6 socket.
 * @param value is the value of the option.
 * @param option is option name.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_ipv6option(int fd, int value, int option);

/**
 * Set or clear a mask in the file descriptor or socket status.
 * @param fd is an open socket of any type.
 * @param enable is !0 to set the mask, 0 to clear the mask.
 * @param mask is the bit mask.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_status(int fd, int enable, long mask);

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
 * Enable or disable the TCP No Delay option .
 * @param fd is an open stream socket.
 * @param enable is !0 to enable, 0 to disable.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_nodelay(int fd, int enable);

/**
 * Enable or disable the TCP Quick Acknowledgement option.
 * @param fd is an open strean socket.
 * @param enable is !0 to enable, 0 to disable.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_quickack(int fd, int enable);

/**
 * Change the send TCP buffer size.
 * @param fd is an open stream socket.
 * @param size is the buffer size in bytes or <0 for no change.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_send(int fd, ssize_t size);

/**
 * Change the receive TCP buffer size.
 * @param fd is an open stream socket.
 * @param size is the buffer size in bytes or <0 for no change.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_receive(int fd, ssize_t size);

/**
 * Enable or disable the IPv6 Only option, which restricts an IPv6
 * to handling IPv6 traffic only (versus also handling IPv4 traffic
 * using IPv6-mapped addresses).
 * @param fd is an open strean socket.
 * @param enable is !0 to enable, 0 to disable.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_ipv6only(int fd, int enable);

/**
 * Enable the IPv6 Address Form option, which turns an IPv6
 * into an IPv4 socket, which henceforth will use the IPv4 API.
 * (There is currently no way to convert back to an IPv6 socket.)
 * @param fd is an open strean socket.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_ipv6toipv4(int fd);

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
