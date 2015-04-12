/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_IPC6_
#define _H_COM_DIAG_DIMINUTO_IPC6_

/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * This provides a slightly more abstract interface to stream and datagram
 * IPv6 sockets. It was derived from the IPv4-based ipc feature.
 *
 * Note that ALL uses of IPv6 addresses and ports are in HOST BYTE ORDER.
 * This simplifies their use in unit tests and applications.
 */

#include "com/diag/diminuto/diminuto_ipc.h"

/**
 * Remarkably, the standard sockaddr structure doesn't reserve enough space to
 * actually hold an entire IPv6 in6_addr field; it's only fourteen bytes, where
 * as an IPv6 address is 16 bytes (and even that's not enough, since there are
 * other fields in the sockaddr_in6 structure). So we use a larger one of our
 * own devising, carefully mimicing the legacy structure's format. Other than
 * the initial family field that indicates what kind of address family structure
 * this really is (IPv6, IPv4, UNIX, etc.), the rest of this structure is just
 * just buffer space. This isn't really intended to be part of the public API,
 * and in fact isn't used externally, but is exposed for unit testing.
 */
typedef struct DiminutoIpc6SocketAddress {
    unsigned short int sa_family; /* POSIX requires an unsigned short int. */
    char sa_data[256 - sizeof(unsigned short int)]; /* glibc assumes short alignment. */
} diminuto_ipc6_sockaddr_t;

/**
 * This is the Diminuto binary IPv6 address in host byte order for "::", the
 * IPv6 address of all zeros.
 */
extern const diminuto_ipv6_t DIMINUTO_IPC6_UNSPECIFIED;

/**
 * This is the Diminuto binary IPv6 address in host byte order for "::1", the
 * IPv6 loopback address.
 */
extern const diminuto_ipv6_t DIMINUTO_IPC6_LOOPBACK;

/**
 * This is the Diminuto binary IPv6 address in host byte order for
 * "::ffff:127.0.0.1", which is the IPv4 loopback address encapsulated in an
 * IPv6 address.
 */
extern const diminuto_ipv6_t DIMINUTO_IPC6_LOOPBACK4;

/**
 * If an IPv6 address in host byte order encapsulates an IPv4 address, extract
 * the IPv4 address in host byte order.
 * @param address is the IPv6 address.
 * @param addressp points to an IPv4 address variable or null for no extraction.
 * @return !0 if the address was an IPv4 address, 0 otherwise.
 */
extern int diminuto_ipc6_ipv62ipv4(diminuto_ipv6_t address, diminuto_ipv4_t * addressp);

/**
 * Encapsulate an IPv4 address in host byte order into an IPv6 address in host
 * byte order.
 * @param address is the IPv4 address.
 * @param addressp points to an IPv6 address variable.
 */
extern void diminuto_ipc6_ipv42ipv6(diminuto_ipv4_t address, diminuto_ipv6_t * addressp);

/**
 * Convert a hostname or an IPV6 address string in dot notation into one
 * or more IPv6 addresses in host byte order. Since a single host can map to
 * multiple addresses, this returns a list of addresses in dynamically acquired
 * memory. The last entry will be all zeros. The list must be freed by the
 * application. IMPORTANT SAFETY TIP: the underlying glibc gethostbyname()
 * function ONLY works if the application is dynamically linked; the build will
 * emit a warning to this effect.
 * @param hostname points to the hostname or IP address string.
 * @return an array or NULL if no such hostname or the string is invalid.
 */
extern diminuto_ipv6_t * diminuto_ipc6_addresses(const char * hostname);

/**
 * Convert a hostname or an IPV6 address string in dot notation into an IPv6
 * address in host byte order. Since a single host can map to multiple
 * addresses, only the first address is returned. IMPORTANT SAFETY TIP: the
 * underlying glibc gethostbyname() function ONLY works if the application is
 * dynamically linked; the build will emit a warning to this effect.
 * @param hostname points to the hostname or IP address string.
 * @return the IPv6 address or 0 if no such hostname or the string is invalid.
 */
extern diminuto_ipv6_t diminuto_ipc6_address(const char * hostname);

/**
 * Convert a service name or a port number string into a port in host byte
 * order.
 * @param service points to the service name or port number string.
 * @param protocol points to the protocol name (e.g. "udp" or "tcp").
 * @return the port number or 0 if no such service exists for the protocol.
 */
static inline diminuto_port_t diminuto_ipc6_port(const char * service, const char * protocol) {
    return diminuto_ipc_port(service, protocol);
}

/**
 * Convert an IPV6 address in host byte order into a printable IP address
 * string in colon notation.
 * @param address is the IPv6 address.
 * @param buffer points to the buffer into to whcih the string is stored.
 * @param length is the length of the buffer in bytes.
 */
extern const char * diminuto_ipc6_colonnotation(diminuto_ipv6_t address, char * buffer, size_t length);

/**
 * Convert an IPV6 address in host byte order into a printable IP address
 * string in colon notation.
 * @param address is the IPv6 address.
 * @param buffer points to the buffer into to whcih the string is stored.
 * @param length is the length of the buffer in bytes.
 */
static inline const char * diminuto_ipc6_address2string(diminuto_ipv6_t address, char * buffer, size_t length) {
    return diminuto_ipc6_colonnotation(address, buffer, length);
}

/**
 * Return the address and port of the near end of the socket if it can be
 * determined. If it cannot be determined, the address and port variables
 * will remain unchanged. The address and port will be returned in host byte
 * order.
 * @param fd is a socket.
 * @param addressp if non-NULL points to where the address will be stored.
 * @param portp if non-NULL points to where the port will be stored.
 * @return 0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc6_nearend(int fd, diminuto_ipv6_t * addressp, diminuto_port_t * portp);

/**
 * Return the address and port of the far end of the socket in host byte order
 * if it can be determined. If it cannot be determined, the address and port
 * variables will remain unchanged.
 * @param fd is a socket.
 * @param addressp if non-NULL points to where the address will be stored.
 * @param portp if non-NULL points to where the port will be stored.
 * @return 0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc6_farend(int fd, diminuto_ipv6_t * addressp, diminuto_port_t * portp);

/**
 * Create a provider-side stream socket with a specified connection backlog.
 * The port is in host byte order. If the port is zero, an unused ephemeral
 * port is allocated; its value can be determined using the nearend function.
 * @param port is the port number at which connection requests will rendezvous.
 * @param backlog is the limit to how many incoming connections may be queued.
 * @return a provider-side stream socket or <0 if an error occurred.
 */
extern int diminuto_ipc6_stream_provider_backlog(diminuto_port_t port, int backlog);


/**
 * Create a provider-side stream socket with the maximum connection backlog.
 * The port is in host byte order. If the port is zero, an unused ephemeral
 * port is allocated; its value can be determined using the nearend function.
 * @param port is the port number at which connection requests will rendezvous.
 * @return a provider-side stream socket or <0 if an error occurred.
 */
extern int diminuto_ipc6_stream_provider(diminuto_port_t port);

/**
 * Wait for and accept a connection request from a consumer on a provider-side
 * stream socket. Optionally return the address and port in host byte order of
 * the requestor.
 * @param fd is the provider-side stream socket.
 * @param addressp if non-NULL points to where the address will be stored.
 * @param portp if non-NULL points to where the port will be stored.
 * @return a data stream socket to the requestor or <0 if an error occurred.
 */
extern int diminuto_ipc6_stream_accept(int fd, diminuto_ipv6_t * addressp, diminuto_port_t * portp);

/**
 * Request a consumer-side stream socket to a provider. The address and port
 * are in host byte oder.
 * @param address is the provider's IPv6 address.
 * @param port is the provider's port.
 * @return a data stream socket to the provider or <0 if an error occurred.
 */
extern int diminuto_ipc6_stream_consumer(diminuto_ipv6_t address, diminuto_port_t port);

/**
 * Request a peer datagram socket. The port is in host byte order.
 * @param port is the port number. If the port is zero, an unused ephemeral
 * port is allocated; its value can be determined using the nearend function.
 * @return a peer datagram socket or <0 if an error occurred.
 */
extern int diminuto_ipc6_datagram_peer(diminuto_port_t port);

/**
 * Shutdown a socket. This eliminates the transmission of any pending data.
 * @param fd is an open socket of any type.
 * @return 0 for success or <0 if an error occurred.
 */
static inline int diminuto_ipc6_shutdown(int fd) {
    return diminuto_ipc_shutdown(fd);
}

/**
 * Close a socket. Unless the socket has been shutdown, pending data will
 * still be transmitted.
 * @param fd is an open socket of any type.
 * @return 0 for success or <0 if an error occurred.
 */
static inline int diminuto_ipc6_close(int fd) {
    return diminuto_ipc_close(fd);
}

/**
 * Set or clear a mask in the file descriptor or socket status.
 * @param fd is an open socket of any type.
 * @param enable is !0 to set the mask, 0 to clear the mask.
 * @param mask is the bit mask.
 * @return 0 for success or <0 if an error occurred.
 */
static inline int diminuto_ipc6_set_status(int fd, int enable, long mask) {
    return diminuto_ipc_set_status(fd, enable, mask);
}

/**
 * Enable or disable a socket option.
 * @param fd is an open socket of any type.
 * @param enable is !0 to enable an option, 0 to disable an option.
 * @param option is option name.
 * @return 0 for success or <0 if an error occurred.
 */
static inline int diminuto_ipc6_set_option(int fd, int enable, int option) {
    return diminuto_ipc_set_option(fd, enable, option);
}

/**
 * Enable or disable the non-blocking status.
 * @param fd is an open socket of any type.
 * @param enable is !0 to enable non-blocking, 0 to disable non-blocking.
 * @return 0 for success or <0 if an error occurred.
 */
static inline int diminuto_ipc6_set_nonblocking(int fd, int enable) {
    return diminuto_ipc_set_nonblocking(fd, enable);
}

/**
 * Enable or disable the address reuse option.
 * @param fd is an open socket of any type.
 * @param enable is !0 to enable address reuse, 0 to disable address reuse.
 * @return 0 for success or <0 if an error occurred.
 */
static inline int diminuto_ipc6_set_reuseaddress(int fd, int enable) {
    return diminuto_ipc_set_reuseaddress(fd, enable);
}

/**
 * Enable or disable the keep alive option.
 * @param fd is an open socket of any type.
 * @param enable is !0 to enable keep alive, 0 to disable keep alive.
 * @return 0 for success or <0 if an error occurred.
 */
static inline int diminuto_ipc6_set_keepalive(int fd, int enable) {
    return diminuto_ipc_set_keepalive(fd, enable);
}

/**
 * Enable or disable the debug option (only available to root on most systems).
 * @param fd is an open socket of any type.
 * @param enable is !0 to enable debug, 0 to disable debug.
 * @return 0 for success or <0 if an error occurred.
 */
static inline int diminuto_ipc6_set_debug(int fd, int enable) {
    return diminuto_ipc_set_debug(fd, enable);
}

/**
 * Enable or disable the linger option.
 * @param fd is an open socket of any type.
 * @param ticks is the number of ticks to linger (although
 * lingering has granularity of seconds), or 0 for no lingering.
 * @return 0 for success or <0 if an error occurred.
 */
static inline int diminuto_ipc6_set_linger(int fd, diminuto_ticks_t ticks) {
    return diminuto_ipc_set_linger(fd, ticks);
}

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
static inline ssize_t diminuto_ipc6_stream_read(int fd, void * buffer, size_t min, size_t max) {
    return diminuto_ipc_stream_read(fd, buffer, min, max);
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
static inline ssize_t diminuto_ipc6_stream_write(int fd, const void * buffer, size_t min, size_t max) {
    return diminuto_ipc_stream_write(fd, buffer, min, max);
}

/**
 * Receive a datagram from a datagram socket using the specified flags.
 * Optionally return the address and port in host byte order of the sender.
 * Flags may be specified for recvfrom(2) such as MSG_DONTWAIT, MSG_WAITALL,
 * or MSG_OOB. (This function can legitimately be also used with a stream
 * socket.)
 * @param fd is an open datagram socket.
 * @param buffer points to the buffer into which data is received.
 * @param size is the maximum number of bytes to be received.
 * @param addressp if non-NULL points to where the address will be stored.
 * @param portp if non-NULL points to where the port will be stored.
 * @param flags is the recvfrom(2) flags to be used.
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EGAIN for non-blocking, EINTR for timer expiry).
 */
extern ssize_t diminuto_ipc6_datagram_receive_flags(int fd, void * buffer, size_t size, diminuto_ipv6_t * addressp, diminuto_port_t * portp, int flags);

/**
 * Receive a datagram from a datagram socket using no flags.
 * Optionally return the address and port in host byte order of the sender.
 * (This function can legitimately be also used with a stream socket.)
 * @param fd is an open datagram socket.
 * @param buffer points to the buffer into which data is received.
 * @param size is the maximum number of bytes to be received.
 * @param addressp if non-NULL points to where the address will be stored
 * in host byte order.
 * @param portp if non-NULL points to where the port will be stored
 * in host byte order.
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EGAIN for non-blocking, EINTR for timer expiry).
 */
static inline ssize_t diminuto_ipc6_datagram_receive(int fd, void * buffer, size_t size, diminuto_ipv6_t * addressp, diminuto_port_t * portp) {
    return diminuto_ipc6_datagram_receive_flags(fd, buffer, size, addressp, portp, 0);
}

/**
 * Send a datagram to a datagram socket using the specified flags. The address
 * and port are in host byte order. Flags may be specified for sendto(2) such as
 * MSG_DONTWAIT, MSG_WAITALL, or MSG_OOB. (This function can legitimately be
 * also used with a stream socket by passing zero as the port number, in which
 * case the address is ignored.)
 * @param fd is an open datagram socket.
 * @param buffer points to the buffer from which data is send.
 * @param size is the maximum number of bytes to be sent.
 * @param address is the receiver's address.
 * @param port is the receiver's port.
 * @param flags is the sendto(2) flags to be used.
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EGAIN for non-blocking, EINTR for timer expiry).
 */
extern ssize_t diminuto_ipc6_datagram_send_flags(int fd, const void * buffer, size_t size, diminuto_ipv6_t address, diminuto_port_t port, int flags);

/**
 * Send a datagram to a datagram socket using no flags. The address and port are
 * in host byte order. (This function can legitimately be also used with a
 * stream socket by passing zero as the port number, in which case the address
 * is ignored.)
 * @param fd is an open datagram socket.
 * @param buffer points to the buffer from which data is send.
 * @param size is the maximum number of bytes to be sent.
 * @param address is the receiver's address.
 * @param port is the receiver's port.
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EGAIN for non-blocking, EINTR for timer expiry).
 */
static inline ssize_t diminuto_ipc6_datagram_send(int fd, const void * buffer, size_t size, diminuto_ipv6_t address, diminuto_port_t port) {
    return diminuto_ipc6_datagram_send_flags(fd, buffer, size, address, port, 0);
}

#endif