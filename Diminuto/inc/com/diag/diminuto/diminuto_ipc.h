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
 * @param name is the name of the network interface.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_interface(int fd, const char * name);

/**
 * Set or clear a mask in the file descriptor or socket status.
 * @param fd is an open socket of any type.
 * @param enable is !0 to set the mask, 0 to clear the mask.
 * @param mask is the bit mask.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_status(int fd, int enable, long mask);

/**
 * Enable or disable a socket option.
 * @param fd is an open socket of any type.
 * @param enable is !0 to enable an option, 0 to disable an option.
 * @param option is option name.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_option(int fd, int enable, int option);

/**
 * Enable or disable the non-blocking status.
 * @param fd is an open socket of any type.
 * @param enable is !0 to enable non-blocking, 0 to disable non-blocking.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_nonblocking(int fd, int enable);

/**
 * Enable or disable the address reuse option.
 * @param fd is an open socket of any type.
 * @param enable is !0 to enable address reuse, 0 to disable address reuse.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_reuseaddress(int fd, int enable);

/**
 * Enable or disable the keep alive option.
 * @param fd is an open socket of any type.
 * @param enable is !0 to enable keep alive, 0 to disable keep alive.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_keepalive(int fd, int enable);

/**
 * Enable or disable the debug option (only available to root on most systems).
 * @param fd is an open socket of any type.
 * @param enable is !0 to enable debug, 0 to disable debug.
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

/* (Many other options are possible, but these are the ones I have used.) */

/**
 * Read bytes from a stream socket into a buffer until at least a minimum
 * number of bytes are read and no more than a maximum number of bytes are
 * read. Less than the minimum can still be read if an error occurs. A
 * minimum of zero should always be used for file descriptors for which
 * zero indicates end of file (this is not the case for non-blocking sockets).
 * @param fd is an open stream socket.
 * @param buffer points to the buffer into which data is read.
 * @param min is the minimum number of bytes to be read.
 * @param max is the maximum number of bytes to be read.
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EGAIN for non-blocking, EINTR for timer expiry).
 */
static inline ssize_t diminuto_ipc_stream_read(int fd, void * buffer, size_t min, size_t max)
{
    return diminuto_fd_read(fd, buffer, min, max);
}

/**
 * Write bytes to a stream socketfrom a buffer until at least a minimum
 * number of bytes are written and no more than a maximum number of bytes are
 * written. Less than the minimum can still be written if an error occurs. A
 * minimum of zero should always be used for file descriptors for which
 * zero indicates end of file (this is not the case for non-blocking sockets).
 * @param fd is an open stream socket.
 * @param buffer points to the buffer from which data is written.
 * @param min is the minimum number of bytes to be written.
 * @param max is the maximum number of bytes to be written.
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EGAIN for non-blocking, EINTR for timer expiry).
 */
static inline ssize_t diminuto_ipc_stream_write(int fd, const void * buffer, size_t min, size_t max)
{
    return diminuto_fd_write(fd, buffer, min, max);
}

#endif
