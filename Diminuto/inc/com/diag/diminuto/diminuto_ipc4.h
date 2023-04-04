/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_IPC4_
#define _H_COM_DIAG_DIMINUTO_IPC4_

/**
 * @file
 * @copyright Copyright 2010-2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Provides a slightly simpler interface to stream and datagram IPv4 sockets.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The IPC4 feature provides a slightly more abstract interface to stream
 * and datagram IPv4 sockets. It was ported from the Digital Aggregates
 * Grandote C++ library.
 *
 * Note that ALL uses of IPv4 addresses and ports are in HOST BYTE ORDER.
 * This simplifies their use in unit tests and applications.
 */

#include "com/diag/diminuto/diminuto_ipc.h"

/*******************************************************************************
 * CODE GENERATORS
 ******************************************************************************/

/**
 * @def DIMINUTO_IPC4_UNSPECIFIED_INIT
 * Generate the code for a static initializer for an IPv4 unspecified address.
 */
#define DIMINUTO_IPC4_UNSPECIFIED_INIT (0x00000000UL)

/**
 * @def DIMINUTO_IPC4_LOOPBACK_INIT
 * Generate the code for a static initializer for an IPv4 loopback address.
 */
#define DIMINUTO_IPC4_LOOPBACK_INIT (0x7f000001UL)

/**
 * @def DIMINUTO_IPC4_LOOPBACK2_INIT
 * Generate the code for a static initializer for a second IPv4 loopback
 * address.
 */
#define DIMINUTO_IPC4_LOOPBACK2_INIT (0x7f000002UL)

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

/**
 * This is the Diminuto binary IPv4 address in host byte order for "0.0.0.0",
 * the IPv4 address of all zeros. Helpfully, the unspecified IPv4 address is
 * the same in either host or network byte order.
 */
static const diminuto_ipv4_t DIMINUTO_IPC4_UNSPECIFIED = DIMINUTO_IPC4_UNSPECIFIED_INIT;

/**
 * This is the Diminuto binary IPv4 address in host byte order for "127.0.0.1",
 * the IPv4 loopback address.
 */
static const diminuto_ipv4_t DIMINUTO_IPC4_LOOPBACK = DIMINUTO_IPC4_LOOPBACK_INIT;

/**
 * This is the Diminuto binary IPv4 address in host byte order for "127.0.0.2",
 * the second IPv4 loopback address used in some circumstances.
 */
static const diminuto_ipv4_t DIMINUTO_IPC4_LOOPBACK2 = DIMINUTO_IPC4_LOOPBACK2_INIT;

/*******************************************************************************
 * COMPARATORS
 ******************************************************************************/

/**
 * Compares two IPv4 addresses in a manner similar to diminuto_ipc6_compare().
 * @param address1p points to the first IPv4 address.
 * @param address2p points to the second IPv4 address.
 * @return 0 if equal, <0 if less than, >0 if greater than.
 */
static inline int diminuto_ipc4_compare(const diminuto_ipv4_t * address1p, const diminuto_ipv4_t * address2p) {
    return (*address1p < *address2p) ? -1 : (*address1p > *address2p) ? 1 : 0;
}

/*******************************************************************************
 * CLASSIFIERS
 ******************************************************************************/

/**
 * Return true if the IPv4 address in host byte order is unspecified.
 * @param addressp points to an IPv4 address.
 * @return true or false.
 */
static inline int diminuto_ipc4_is_unspecified(const diminuto_ipv4_t * addressp) {
    return (*addressp == 0x00000000);
}

/**
 * Return true if the IPv4 address in host byte order is limited broadcast.
 * @param addressp points to an IPv4 address.
 * @return true or false.
 */
static inline int diminuto_ipc4_is_limitedbroadcast(const diminuto_ipv4_t * addressp) {
    return (*addressp == 0xffffffff);
}

/**
 * Return true if the IPv4 address in host byte order is loopback.
 * @param addressp points to an IPv4 address.
 * @return true or false.
 */
static inline int diminuto_ipc4_is_loopback(const diminuto_ipv4_t * addressp) {
    return ((0x7f000001 <= *addressp) && (*addressp <= 0x7fffffff));
}

/**
 * Return true if the IPv4 address in host byte order is private.
 * @param addressp points to an IPv4 address.
 * @return true or false.
 */
static inline int diminuto_ipc4_is_private(const diminuto_ipv4_t * addressp) {
    return ((0xc0a80000 <= *addressp) && (*addressp <= 0xc0a8ffff)) || /* /16 */
           ((0xac100000 <= *addressp) && (*addressp <= 0xac1fffff)) || /* /12 */
           ((0x0a000000 <= *addressp) && (*addressp <= 0x0affffff));   /* /24 */
}

/**
 * Return true if the IPv4 address in host byte order is multicast.
 * @param addressp points to an IPv4 address.
 * @return true or false.
 */
static inline int diminuto_ipc4_is_multicast(const diminuto_ipv4_t * addressp) {
    return ((0xe0000000 <= *addressp) && (*addressp <= 0xefffffff));
}

/*******************************************************************************
 * RESOLVERS
 ******************************************************************************/

/**
 * Convert a hostname or an IPv4 address string in dot notation into one
 * or more IPv4 addresses in host byte order. Since a single host can map to
 * multiple addresses, this returns a list of addresses in dynamically acquired
 * memory. The last entry will be all zeros. The list must be freed by the
 * application.
 * IMPORTANT SAFETY TIP: the underlying glibc gethostbyname(3) function ONLY
 * works if the application is dynamically linked; the build will emit a
 * warning to this effect if statically linked.
 * NOTE: I have seen the underlying glibc gethostbyname(3) function fail and
 * then succeed on subsequent tries with the same argument; it depends on the
 * relatively vast Domain Name System (DNS) distributed infrastructure, so
 * there are lots of points of temporary failure.
 * @param hostname points to the hostname or IP address string.
 * @return an array or NULL if no such hostname or the string is invalid.
 */
extern diminuto_ipv4_t * diminuto_ipc4_addresses(const char * hostname);

/**
 * Convert a hostname or an IPv4 address string in dot notation into an IPv4
 * address in host byte order. Since a single host can map to multiple
 * addresses, only the first address is returned.
 * IMPORTANT SAFETY TIP: the underlying glibc gethostbyname() function ONLY
 * works if the application is dynamically linked; the build will emit a
 * warning to this effect if statically linked.
 * NOTE: I have seen the underlying glibc gethostbyname(3) function fail and
 * then succeed on subsequent tries with the same argumnet; it depends on the
 * relatively vast Domain Name System (DNS) distributed infrastructure, so
 * there are lots of points of temporary failure.
 * @param hostname points to the hostname or IP address string.
 * @return the IPv4 address or 0 if no such hostname or the string is invalid.
 */
extern diminuto_ipv4_t diminuto_ipc4_address(const char * hostname);

/**
 * Convert a service name or a port number string into a port in host byte
 * order.
 * @param service points to the service name or port number string.
 * @param protocol points to the protocol name (e.g. "udp" or "tcp").
 * @return the port number or 0 if no such service exists for the protocol.
 */
static inline diminuto_port_t diminuto_ipc4_port(const char * service, const char * protocol) {
    return diminuto_ipc_port(service, protocol);
}

/*******************************************************************************
 * STRINGIFIERS
 ******************************************************************************/

/**
 * Convert an IPv4 address in host byte order into a printable IP address
 * string in dot notation.
 * @param address is the IPv4 address in host byte order.
 * @param buffer points to the buffer into to which the string is stored.
 * @param length is the length of the buffer in bytes.
 * @return a pointer to the buffer.
 */
extern char * diminuto_ipc4_dotnotation(diminuto_ipv4_t address, void * buffer, size_t length);

/**
 * Convert an IPv4 address in host byte order into a printable IP address
 * string in dot notation.
 * @param address is the IPv4 address in host byte order.
 * @param buffer points to the buffer into to which the string is stored.
 * @param length is the length of the buffer in bytes.
 * @return a pointer to the buffer.
 */
static inline char * diminuto_ipc4_address2string(diminuto_ipv4_t address, void * buffer, size_t length) {
    return diminuto_ipc4_dotnotation(address, buffer, length);
}

/**
 * Given an IPv4 address in host byte order, return a string describing the
 * type of address it is.
 * @param address is the IPv4 address in host byte order.
 * @return a pointer to a string describing what type of address it is.
 */
extern const char * diminuto_ipc4_address2type(diminuto_ipv4_t address);

/*******************************************************************************
 * INTERROGATORS
 ******************************************************************************/

/**
 * Return the address and port of the near end of the socket if it can be
 * determined. If it cannot be determined, the address and port variables
 * will remain unchanged.
 * @param fd is a socket.
 * @param addressp if non-NULL points to where the address will be stored
 * in host byte order.
 * @param portp if non-NULL points to where the port will be stored
 * in host byte order.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc4_nearend(int fd, diminuto_ipv4_t * addressp, diminuto_port_t * portp);

/**
 * Return the address and port of the far end of the socket if it can be
 * determined. If it cannot be determined, the address and port variables
 * will remain unchanged.
 * @param fd is a socket.
 * @param addressp if non-NULL points to where the address will be stored
 * in host byte order.
 * @param portp if non-NULL points to where the port will be stored
 * in host byte order.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc4_farend(int fd, diminuto_ipv4_t * addressp, diminuto_port_t * portp);

/*******************************************************************************
 * SOCKETS
 ******************************************************************************/

/**
 * Bind an existing socket to a specific address and port. The address and port
 * are in host byte order. If the address is zero, the socket will be bound to
 * any appropriate interface. If the port is zero, an unused ephemeral port is
 * allocated.
 * @param fd is the socket.
 * @param address is the address to which to bind.
 * @param port is the port to which to bind.
 * @return >=0 for success or <0 if an error occurred.
 */
extern int diminuto_ipc4_source(int fd, diminuto_ipv4_t address, diminuto_port_t port);

/**
 * Shutdown a socket. This eliminates the transmission of any pending data.
 * @param fd is an open socket of any type.
 * @return 0 for success or <0 if an error occurred.
 */
static inline int diminuto_ipc4_shutdown(int fd) {
    return diminuto_ipc_shutdown(fd);
}

/**
 * Close a socket. Unless the socket has been shutdown, pending data will
 * still be transmitted.
 * @param fd is an open socket of any type.
 * @return >=0 for success or <0 if an error occurred.
 */
static inline int diminuto_ipc4_close(int fd) {
    return diminuto_ipc_close(fd);
}

/*******************************************************************************
 * STREAM SOCKETS
 ******************************************************************************/

/**
 * Create a provider-side stream socket bound to a specific address and with
 * a specific connection backlog. The address and port are in host byte order.
 * If the address is zero, the socket will be bound to any appropriate
 * interface. If the port is zero, an unused ephemeral port is allocated;
 * its value can be determined using the nearend function. If an optional
 * function is provided by the caller, invoke it to set socket options before
 * the listen(2) is performed.
 * @param address is the address of the interface that will be used.
 * @param port is the port number at which connection requests will rendezvous.
 * @param interface points to the interface name, or NULL.
 * @param backlog is the limit to how many incoming connections may be queued, <0 for the default.
 * @param functionp points to an optional function to set socket options.
 * @param datap is passed to the optional function.
 * @return a provider-side stream socket or <0 if an error occurred.
 */
extern int diminuto_ipc4_stream_provider_base(diminuto_ipv4_t address, diminuto_port_t port, const char * interface, int backlog, diminuto_ipc_injector_t * functionp, void * datap);

/**
 * Create a provider-side stream socket bound to a specific address and with
 * a specific connection backlog. The address and port are in host byte order.
 * If the address is zero, the socket will be bound to any appropriate
 * interface. If the port is zero, an unused ephemeral port is allocated;
 * its value can be determined using the nearend function.
 * @param address is the address of the interface that will be used.
 * @param port is the port number at which connection requests will rendezvous.
 * @param interface points to the interface name, or NULL.
 * @param backlog is the limit to how many incoming connections may be queued, <0 for the default.
 * @return a provider-side stream socket or <0 if an error occurred.
 */
static inline int diminuto_ipc4_stream_provider_generic(diminuto_ipv4_t address, diminuto_port_t port, const char * interface, int backlog) {
    return diminuto_ipc4_stream_provider_base(address, port, interface, backlog, diminuto_ipc_inject_defaults, (void *)0);
}

/**
 * Create a provider-side stream socket with the maximum connection backlog.
 * The port is in host byte order. If the port is zero, an unused ephemeral port
 * is allocated; its value can be determined using the nearend function.
 * @param port is the port number at which connection requests will rendezvous.
 * @return a provider-side stream socket or <0 if an error occurred.
 */
static inline int diminuto_ipc4_stream_provider(diminuto_port_t port) {
    return diminuto_ipc4_stream_provider_generic(DIMINUTO_IPC4_UNSPECIFIED, port, (const char *)0, -1);
}

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
extern int diminuto_ipc4_stream_accept_generic(int fd, diminuto_ipv4_t * addressp, diminuto_port_t * portp);

/**
 * Wait for and accept a connection request from a consumer on a provider-side
 * stream socket.
 * @param fd is the provider-side stream socket.
 * @return a data stream socket to the requestor or <0 if an error occurred.
 */
static inline int diminuto_ipc4_stream_accept(int fd) {
    return diminuto_ipc4_stream_accept_generic(fd, (diminuto_ipv4_t *)0, (diminuto_port_t *)0);
}

/**
 * Request a consumer-side stream socket to a provider using a specific address,
 * port, and interface on the near end. If an optional function is provided by
 * the caller, invoke it to set socket options before the connect(2) is
 * performed.
 * @param address is the provider's IPv4 address in host byte order.
 * @param port is the provider's port in host byte order.
 * @param address0 is the address to which to bind the socket, or zero.
 * @param port0 is the port to which to bind the socket, or zero
 * @param interface points to the name of the interface, or NULL.
 * @param functionp points to an optional function to set socket options.
 * @param datap is passed to the optional function.
 * @return a data stream socket to the consumer or <0 if an error occurred.
 */
extern int diminuto_ipc4_stream_consumer_base(diminuto_ipv4_t address, diminuto_port_t port, diminuto_ipv4_t address0, diminuto_port_t port0, const char * interface, diminuto_ipc_injector_t * functionp, void * datap);

/**
 * Request a consumer-side stream socket to a provider using a specific address,
 * port, and interface on the near end.
 * @param address is the provider's IPv4 address in host byte order.
 * @param port is the provider's port in host byte order.
 * @param address0 is the address to which to bind the socket, or zero.
 * @param port0 is the port to which to bind the socket, or zero
 * @param interface points to the name of the interface, or NULL.
 * @return a data stream socket to the consumer or <0 if an error occurred.
 */
static inline int diminuto_ipc4_stream_consumer_generic(diminuto_ipv4_t address, diminuto_port_t port, diminuto_ipv4_t address0, diminuto_port_t port0, const char * interface) {
    return diminuto_ipc4_stream_consumer_base(address, port, address0, port0, interface, diminuto_ipc_inject_defaults, (void *)0);
}

/**
 * Request a consumer-side stream socket to a provider.
 * @param address is the provider's IPv4 address in host byte order.
 * @param port is the provider's port in host byte order.
 * @return a data stream socket to the consumer or <0 if an error occurred.
 */
static inline int diminuto_ipc4_stream_consumer(diminuto_ipv4_t address, diminuto_port_t port) {
    return diminuto_ipc4_stream_consumer_generic(address, port, DIMINUTO_IPC4_UNSPECIFIED, 0, (const char *)0);
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
static inline ssize_t diminuto_ipc4_stream_read_generic(int fd, void * buffer, size_t min, size_t max) {
    return diminuto_ipc_stream_read_generic(fd, buffer, min, max);
}

/**
 * Read bytes from a stream socket into a buffer.
 * @param fd is an open stream socket.
 * @param buffer points to the buffer into which data is read.
 * @param size is the maximum number of bytes to be read.
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EAGAIN for non-blocking, EINTR for timer expiry).
 */
static inline ssize_t diminuto_ipc4_stream_read(int fd, void * buffer, size_t size) {
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
static inline ssize_t diminuto_ipc4_stream_write_generic(int fd, const void * buffer, size_t min, size_t max) {
    return diminuto_ipc_stream_write_generic(fd, buffer, min, max);
}

/**
 * Write bytes to a stream socket from a buffer.
 * @param fd is an open stream socket.
 * @param buffer points to the buffer from which data is written.
 * @param size is the number of bytes to be written.
 * @return the number of bytes sent, 0 if the far end closed, or <0 if an error occurred (errno will be EAGAIN for non-blocking, EINTR for timer expiry).
 */
static inline ssize_t diminuto_ipc4_stream_write(int fd, const void * buffer, size_t size) {
    return diminuto_ipc_stream_write(fd, buffer, size);
}

/*******************************************************************************
 * DATAGRAM SOCKETS
 ******************************************************************************/

/**
 * Request a peer datagram socket. The address and port are in host byte order.
 * If the port is zero, an unused ephemeral port is allocated; its value can
 * be determined using the nearend function. If an optional function is
 * provided by the caller, invoke it to set socket options.
 * @param address is the IPv4 address of the interface to use.
 * @param port is the port number.
 * @param interface points to the name of the interface, or NULL.
 * @param functionp points to an optional function to set socket options.
 * @param datap is passed to the optional function.
 * @return a peer datagram socket or <0 if an error occurred.
 */
extern int diminuto_ipc4_datagram_peer_base(diminuto_ipv4_t address, diminuto_port_t port, const char * interface, diminuto_ipc_injector_t * functionp, void * datap);

/**
 * Request a peer datagram socket. The address and port are in host byte order.
 * If the port is zero, an unused ephemeral port is allocated; its value can
 * be determined using the nearend function.
 * @param address is the IPv4 address of the interface to use.
 * @param port is the port number.
 * @param interface points to the name of the interface, or NULL.
 * @return a peer datagram socket or <0 if an error occurred.
 */
static inline int diminuto_ipc4_datagram_peer_generic(diminuto_ipv4_t address, diminuto_port_t port, const char * interface) {
    return diminuto_ipc4_datagram_peer_base(address, port, interface, diminuto_ipc_inject_defaults, (void *)0);
}

/**
 * Request a peer datagram socket. The port is in host byte order. If the port
 * is zero, an unused ephemeral port is allocated; its value can be determined
 * using the nearend function.
 * @param port is the port number.
 * @return a peer datagram socket or <0 if an error occurred.
 */
static inline int diminuto_ipc4_datagram_peer(diminuto_port_t port) {
    return diminuto_ipc4_datagram_peer_generic(DIMINUTO_IPC4_UNSPECIFIED, port, (const char *)0);
}

/**
 * Receive a datagram from a datagram socket using the specified flags.
 * Optionally return the address and port of the sender. Flags may be specified
 * for recvfrom(2) such as MSG_DONTWAIT, MSG_WAITALL, or MSG_OOB. (This function
 * can legitimately be also used with a stream socket.)
 * @param fd is an open datagram socket.
 * @param buffer points to the buffer into which data is received.
 * @param size is the maximum number of bytes to be received.
 * @param addressp if non-NULL points to where the address will be stored
 * in host byte order.
 * @param portp if non-NULL points to where the port will be stored
 * in host byte order.
 * @param flags is the recvfrom(2) flags to be used.
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EAGAIN for non-blocking, EINTR for timer expiry).
 */
extern ssize_t diminuto_ipc4_datagram_receive_generic(int fd, void * buffer, size_t size, diminuto_ipv4_t * addressp, diminuto_port_t * portp, int flags);

/**
 * Receive a datagram from a datagram socket using no flags. Optionally return
 * the address and port of the sender. (This function can legitimately be also
 * used with a stream socket.)
 * @param fd is an open datagram socket.
 * @param buffer points to the buffer into which data is received.
 * @param size is the maximum number of bytes to be received.
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EAGAIN for non-blocking, EINTR for timer expiry).
 */
static inline ssize_t diminuto_ipc4_datagram_receive(int fd, void * buffer, size_t size) {
    return diminuto_ipc4_datagram_receive_generic(fd, buffer, size, (diminuto_ipv4_t *)0, (diminuto_port_t *)0, 0);
}

/**
 * Send a datagram to a datagram socket using the specified flags. Flags may be
 * specified for sendto(2) such as MSG_DONTWAIT, MSG_WAITALL, or MSG_OOB. (This
 * function can legitimately be also used with a stream socket by passing zero
 * as the port number, in which case the address is ignored.)
 * @param fd is an open datagram socket.
 * @param buffer points to the buffer from which data is sent.
 * @param size is the maximum number of bytes to be sent.
 * @param address is the receiver's address.
 * @param port is the receiver's port.
 * @param flags is the sendto(2) flags to be used.
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EAGAIN for non-blocking, EINTR for timer expiry).
 */
extern ssize_t diminuto_ipc4_datagram_send_generic(int fd, const void * buffer, size_t size, diminuto_ipv4_t address, diminuto_port_t port, int flags);

/**
 * Send a datagram to a datagram socket using no flags. (This function can
 * legitimately be also used with a stream socket by passing zero as the port
 * number, in which case the address is ignored.)
 * @param fd is an open datagram socket.
 * @param buffer points to the buffer from which data is sent.
 * @param size is the maximum number of bytes to be sent.
 * @param address is the receiver's address.
 * @param port is the receiver's port.
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EAGAIN for non-blocking, EINTR for timer expiry).
 */
static inline ssize_t diminuto_ipc4_datagram_send(int fd, const void * buffer, size_t size, diminuto_ipv4_t address, diminuto_port_t port) {
    return diminuto_ipc4_datagram_send_generic(fd, buffer, size, address, port, 0);
}

/*******************************************************************************
 * INTERFACES
 ******************************************************************************/

/**
 * Return a list of IPv4 addresses associated with the specified network
 * interface. The addresses will be in host byte order. Since a single
 * interface can map to multiple addresses, this returns a list of addresses
 * in dynamically acquired memory. The last entry will be all zeros. The
 * list must be freed by the application using free(3).
 * @param interface points to the interface name string.
 * @return an array or NULL if no such interface or the string is invalid.
 */
extern diminuto_ipv4_t * diminuto_ipc4_interface(const char * interface);

#endif
