/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_IPC_
#define _H_COM_DIAG_DIMINUTO_IPC_

/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * This provides a slightly more abstract interface to stream and datagram
 * IPV4 sockets. It was ported from the Desperado library.
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * Convert a hostname or an IPV4 address string in dot notation into an IPV4
 * address in host byte order. Since a single host can map to multiple
 * addresses, indicate which address is desired.
 * @param hostname points to the hostname or IP address string.
 * @param index indicates which IP address to return.
 * @return the IPV4 address or 0 if no such hostname or the string is invalid.
 */
extern diminuto_ipv4_t diminuto_ipc_address_index(const char * hostname, size_t index);

/**
 * Convert a hostname or an IPV4 address string in dot notation into an IPV4
 * address in host byte order. Since a single host can map to multiple
 * addresses, the first address is returned.
 * @param hostname points to the hostname or IP address string.
 * @return the IPV4 address or 0 if no such hostname or the string is invalid.
 */
extern diminuto_ipv4_t diminuto_ipc_address(const char * hostname);

/**
 * Convert a service name or a port number string into a port in host byte
 * order.
 * @param service points to the service name or port number string.
 * @param protocol points to the protocol name (e.g. "udp" or "tcp").
 * @return the port number or 0 if no such service exists for the protocol.
 */
extern diminuto_port_t diminuto_ipc_port(const char * service, const char * protocol);

/**
 * Convert an IPV4 address in host byte order into a printable IP address
 * string in dot notation.
 * @param address is the IPV4 address in host byte order.
 * @param buffer points to the buffer into to whcih the string is stored.
 * @param length is the length of the buffer in bytes.
 */
extern const char * diminuto_ipc_dotnotation(diminuto_ipv4_t address, char * buffer, size_t length);

/**
 * Return the address and port of the near end of the socket if it can be
 * determined. If it cannot be determined, the address and port variables
 * will remain unchanged.
 * @param fd is a socket.
 * @param addressp if non-NULL points to where the address will be stored
 * in host byte order.
 * @param portp if non-NULL points to where the port will be stored
 * in host byte order.
 * @return 0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_nearend(int fd, diminuto_ipv4_t * addressp, diminuto_port_t * portp);

/**
 * Return the address and port of the far end of the socket if it can be
 * determined. If it cannot be determined, the address and port variables
 * will remain unchanged.
 * @param fd is a socket.
 * @param addressp if non-NULL points to where the address will be stored
 * in host byte order.
 * @param portp if non-NULL points to where the port will be stored
 * in host byte order.
 * @return 0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_farend(int fd, diminuto_ipv4_t * addressp, diminuto_port_t * portp);

/**
 * Create a provider-side stream socket with a specified connection backlog.
 * @param port is the port number at which connection requests will rendezvous.
 * @param backlog is the limit to how many incoming connections may be queued.
 * @return a provider-side stream socket or <0 if an error occurred.
 */
extern int diminuto_ipc_stream_provider_backlog(diminuto_port_t port, int backlog);


/**
 * Create a provider-side stream socket with the maximum connection backlog.
 * @param port is the port number at which connection requests will rendezvous
 * in host byte order.
 * @return a provider-side stream socket or <0 if an error occurred.
 */
extern int diminuto_ipc_stream_provider(diminuto_port_t port);

/**
 * Wait for and accept a connection request from a consumer on a provider-side
 * stream socket. Optionally return the address and port of the requestor.
 * @param fd is the provider-side stream socket.
 * @param addressp if non-NULL points to where the address will be stored
 * in host byte order.
 * @param portp if non-NULL points to where the port will be stored
 * in host byte order.
 * @return a data stream socket to the requestor or <0 if an error occurred.
 */
extern int diminuto_ipc_stream_accept(int fd, diminuto_ipv4_t * addressp, diminuto_port_t * portp);

/**
 * Request a consumer-side stream socket to a provider.
 * @param address is the provider's IPV4 address in host byte order.
 * @param port is the provider's port in host byte order.
 * @return a data stream socket to the provider or <0 if an error occurred.
 */
extern int diminuto_ipc_stream_consumer(diminuto_ipv4_t address, diminuto_port_t port);

/**
 * Request a peer datagram socket.
 * @param port is the port number in host byte order.
 * @return a peer datagram socket or <0 if an error occurred.
 */
extern int diminuto_ipc_datagram_peer(diminuto_port_t port);

/**
 * Shutdown a socket. This eliminates the transmission of any pending data.
 * @param fd is an open socket of any type.
 * @return 0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_shutdown(int fd);

/**
 * Close a socket. Unless the socket has been shutdown, pending data will
 * still be transmitted.
 * @param fd is an open socket of any type.
 * @return 0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_close(int fd);

/**
 * Set or clear a mask in the file descriptor or socket status.
 * @param fd is an open socket of any type.
 * @param enable is !0 to set the mask, 0 to clear the mask.
 * @param mask is the bit mask.
 * @return 0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_status(int fd, int enable, long mask);

/**
 * Enable or disable a socket option.
 * @param fd is an open socket of any type.
 * @param enable is !0 to enable an option, 0 to disable an option.
 * @param option is option name.
 * @return 0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_option(int fd, int enable, int option);

/**
 * Enable or disable the non-blocking status.
 * @param fd is an open socket of any type.
 * @param enable is !0 to enable non-blocking, 0 to disable non-blocking.
 * @return 0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_nonblocking(int fd, int enable);

/**
 * Enable or disable the address reuse option.
 * @param fd is an open socket of any type.
 * @param enable is !0 to enable address reuse, 0 to disable address reuse.
 * @return 0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_reuseaddress(int fd, int enable);

/**
 * Enable or disable the keep alive option.
 * @param fd is an open socket of any type.
 * @param enable is !0 to enable keep alive, 0 to disable keep alive.
 * @return 0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_keepalive(int fd, int enable);

/**
 * Enable or disable the debug option (only available to root on most systems).
 * @param fd is an open socket of any type.
 * @param enable is !0 to enable debug, 0 to disable debug.
 * @return 0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_debug(int fd, int enable);

/**
 * Enable or disable the linger option.
 * @param fd is an open socket of any type.
 * @param microseconds is the number of microseconds to linger (although
 * lingering has granularity of seconds), or 0 for no lingering.
 * @return 0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc_set_linger(int fd, diminuto_usec_t microseconds);

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
extern ssize_t diminuto_ipc_stream_read(int fd, void * buffer, size_t min, size_t max);

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
extern ssize_t diminuto_ipc_stream_write(int fd, const void * buffer, size_t min, size_t max);

/**
 * Receive a datagram from a datagram socket using the specified flags.
 * Optionally return the address and port of the sender.
 * @param fd is an open datagram socket.
 * @param buffer points to the buffer into which data is received.
 * @param size is the maximum number of bytes to be received.
 * @param addressp if non-NULL points to where the address will be stored
 * in host byte order.
 * @param portp if non-NULL points to where the port will be stored
 * in host byte order.
 * @param flags is the recvfrom(2) flags to be used.
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EGAIN for non-blocking, EINTR for timer expiry).
 */
extern ssize_t diminuto_ipc_datagram_receive_flags(int fd, void * buffer, size_t size, diminuto_ipv4_t * addressp, diminuto_port_t * portp, int flags);

/**
 * Receive a datagram from a datagram socket using no flags.
 * Optionally return the address and port of the sender.
 * @param fd is an open datagram socket.
 * @param buffer points to the buffer into which data is received.
 * @param size is the maximum number of bytes to be received.
 * @param addressp if non-NULL points to where the address will be stored
 * in host byte order.
 * @param portp if non-NULL points to where the port will be stored
 * in host byte order.
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EGAIN for non-blocking, EINTR for timer expiry).
 */
extern ssize_t diminuto_ipc_datagram_receive(int fd, void * buffer, size_t size, diminuto_ipv4_t * addressp, diminuto_port_t * portp);

/**
 * Send a datagram to a datagram socket using the specified flags.
 * @param fd is an open datagram socket.
 * @param buffer points to the buffer from which data is send.
 * @param size is the maximum number of bytes to be sent.
 * @param address is the receiver's address.
 * @param port is the receiver's port.
 * @param flags is the sendto(2) flags to be used.
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EGAIN for non-blocking, EINTR for timer expiry).
 */
extern ssize_t diminuto_ipc_datagram_send_flags(int fd, const void * buffer, size_t size, diminuto_ipv4_t address, diminuto_port_t port, int flags);

/**
 * Send a datagram to a datagram socket using no flags.
 * @param fd is an open datagram socket.
 * @param buffer points to the buffer from which data is send.
 * @param size is the maximum number of bytes to be sent.
 * @param address is the receiver's address.
 * @param port is the receiver's port.
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EGAIN for non-blocking, EINTR for timer expiry).
 */
extern ssize_t diminuto_ipc_datagram_send(int fd, const void * buffer, size_t size, diminuto_ipv4_t address, diminuto_port_t port);

#endif
