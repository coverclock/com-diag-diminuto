/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_IPC6_
#define _H_COM_DIAG_DIMINUTO_IPC6_

/**
 * @file
 * @copyright Copyright 2015-2021 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Provides a slightly simpler interface to stream and datagram IPv6 sockets.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * The IPC6 feature provides a slightly more abstract interface to stream
 * and datagram IPv6 sockets. It was derived from the IPC4 feature.
 *
 * Note that ALL uses of IPv6 addresses and ports are in HOST BYTE ORDER.
 * This simplifies their use in unit tests and applications.
 *
 * The ipc6 API can be used to talk to both IPv6 and IPv4 end points, the
 * latter done through IPv4-mapped IPv6 addresses. This would seem to make the
 * preferential use of the ipc6 API over the ipc4 API a no-brainer, and in
 * fact is what I would recommend, all other things being equal. However,
 * RFC 4942 points out a number of security issues with this approach, some
 * of which involve circumventing firewall rules. You can use the IPv6 Only
 * socket option supported by the ipc6 API to prohibit accepting IPv4-mapped
 * IPv6 connections. [RFC 4942, section 2.2, pp. 19-20]
 *
 * REFERENCES
 *
 * R. Graziani, IPV6 FUNDAMENTALS, Cisco Press, 2013
 *
 * P. Bieringer, LINUX IPv6 HOWTO, 2015-10-16
 *
 * R. Gilligan, S. Thomson, J. Bound, J. McCann, W. Stevens, "Basic Socket
 * Interface Extentions for IPv6", RFC 3493, February 2003
 *
 * W. Stevens, M. Thomas, E. Nordmark, T. Jinmei, "Advanced Sockets
 * Applications Program Interface (API) for IPv6", RFC 3542, May 2003
 *
 * E. Davies, S. Krishnan, P. Sovola, "IPv6 Transition/Coexistence Security
 * Considerations", RFC 4942, September 2007 
 *
 * T. Berners-Lee, R. Fielding, L. Masinter, "Uniform Resource Identifier
 * (URI) Syntax", RFC 3986, January 2005
 *
 * O.Li, T. Jimmel, K. Shima, IPV6 CORE PROTOCOLS IMPLEMENTATION, Morgan
 * Kaufmann, 2007
 */

#include "com/diag/diminuto/diminuto_ipc.h"
#include <string.h>

/*******************************************************************************
 * GLOBALS
 ******************************************************************************/

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

/*******************************************************************************
 * COMPARATORS
 ******************************************************************************/

/**
 * Compares two IPv6 addresses using memcmp(3).
 * @param address1p points to the first IPv6 address.
 * @param address2p points to the second IPv6 address.
 * @return 0 if equal, <0 if less than, >0 if greater than.
 */
static inline int diminuto_ipc6_compare(const diminuto_ipv6_t * address1p, const diminuto_ipv6_t * address2p) {
    return memcmp(address1p, address2p, sizeof(diminuto_ipv6_t));
}

/*******************************************************************************
 * CLASSIFIERS
 ******************************************************************************/

/**
 * Return true if the IPv6 address in host byte order is the unspecified
 * address, false otherwise.
 * @param addressp points to an IPv6 address.
 * @return true or false.
 */
static inline int diminuto_ipc6_is_unspecified(const diminuto_ipv6_t * addressp) {
    return (diminuto_ipc6_compare(addressp, &DIMINUTO_IPC6_UNSPECIFIED) == 0);
}

/**
 * Return true if the IPv6 address in host byte order is the loopback address,
 * false otherwise.
 * @param addressp points to an IPv6 address.
 * @return true or false.
 */
static inline int diminuto_ipc6_is_loopback(const diminuto_ipv6_t * addressp) {
    return (diminuto_ipc6_compare(addressp, &DIMINUTO_IPC6_LOOPBACK) == 0);
}

/**
 * Return true if the IPv6 address in host byte order is the IPv4-mapped
 * loopback address, false otherwise.
 * @param addressp points to an IPv6 address.
 * @return true or false.
 */
static inline int diminuto_ipc6_is_loopback4(const diminuto_ipv6_t * addressp) {
    return (diminuto_ipc6_compare(addressp, &DIMINUTO_IPC6_LOOPBACK4) == 0);
}

/**
 * Return true if the IPv6 address in host byte order is embedded IPv4, false
 * otherwise.
 * @param addressp points to an IPv6 address.
 * @return true or false.
 */
static inline int diminuto_ipc6_is_v4mapped(const diminuto_ipv6_t * addressp) {
    return (memcmp(addressp, &DIMINUTO_IPC6_LOOPBACK4, sizeof(diminuto_ipv6_t) - sizeof(diminuto_ipv4_t)) == 0);
}

/**
 * Return true if the IPv6 address in host byte order is IPv4 compatible, false
 * otherwise.
 * @param addressp points to an IPv6 address.
 * @return true or false.
 */
static inline int diminuto_ipc6_is_v4compatible(const diminuto_ipv6_t * addressp) {
    return (memcmp(addressp, &DIMINUTO_IPC6_UNSPECIFIED, sizeof(diminuto_ipv6_t) - sizeof(diminuto_ipv4_t)) == 0);
}

/**
 * Return true if the IPv6 address in host byte order is NAT64 WKP, false
 * otherwise.
 * @param addressp points to an IPv6 address.
 * @return true or false.
 */
static inline int diminuto_ipc6_is_nat64wkp(const diminuto_ipv6_t * addressp) {
    return ((addressp->u16[0] == 0x64) && (addressp->u16[1] == 0xff9b) && (addressp->u16[2] == 0) && (addressp->u16[3] == 0) && (addressp->u16[4] == 0) && (addressp->u16[5] == 0));
}

/**
 * Return true if the IPv6 address in host byte order is global unicast, false
 * otherwise.
 * @param addressp points to an IPv6 address.
 * @return true or false.
 */
static inline int diminuto_ipc6_is_unicastglobal(const diminuto_ipv6_t * addressp) {
    return ((0x2000 <= addressp->u16[0]) && (addressp->u16[0] <= 0x3fff));
}

/**
 * Return true if the IPv6 address in host byte order is an ISATAP tunnel, false
 * otherwise.
 * @param addressp points to an IPv6 address.
 * @return true or false.
 */
static inline int diminuto_ipc6_is_isatap(const diminuto_ipv6_t * addressp) {
    return (diminuto_ipc6_is_unicastglobal(addressp) && (addressp->u16[4] == 0x0000) && (addressp->u16[5] == 0x5efe));
}

/**
 * Return true if the IPv6 address in host byte order is a 6to4 tunnel, false
 * otherwise.
 * @param addressp points to an IPv6 address.
 * @return true or false.
 */
static inline int diminuto_ipc6_is_6to4(const diminuto_ipv6_t * addressp) {
    return (addressp->u16[0] == 0x2002);
}

/**
 * Return true if the IPv6 address in host byte order is unique-local, false
 * otherwise.
 * @param addressp points to an IPv6 address.
 * @return true or false.
 */
static inline int diminuto_ipc6_is_uniquelocal(const diminuto_ipv6_t * addressp) {
    return ((0xfc00 <= addressp->u16[0]) && (addressp->u16[0] <= 0xfdff));
}

/**
 * Return true if the IPv6 address in host byte order is link-local, false
 * otherwise.
 * @param addressp points to an IPv6 address.
 * @return true or false.
 */
static inline int diminuto_ipc6_is_linklocal(const diminuto_ipv6_t * addressp) {
    return ((0xfe80 <= addressp->u16[0]) && (addressp->u16[0] <= 0xfebf));
}

/**
 * Return true if the IPv6 address in host byte order is multicast, false
 * otherwise.
 * @param addressp points to an IPv6 address.
 * @return true or false.
 */
static inline int diminuto_ipc6_is_multicast(const diminuto_ipv6_t * addressp) {
    return ((0xff00 <= addressp->u16[0]) && (addressp->u16[0] <= 0xffff));
}

/*******************************************************************************
 * CONVERTORS
 ******************************************************************************/

/**
 * If an IPv6 address in host byte order encapsulates an IPv4 address, extract
 * the IPv4 address in host byte order.
 * @param address is the IPv6 address.
 * @param addressp points to an IPv4 address variable or null for no extraction.
 * @return !0 if the address was an IPv4 address, 0 otherwise.
 */
extern int diminuto_ipc6_ipv6toipv4(diminuto_ipv6_t address, diminuto_ipv4_t * addressp);

/**
 * Encapsulate an IPv4 address in host byte order into an IPv6 address in host
 * byte order.
 * @param address is the IPv4 address.
 * @param addressp points to an IPv6 address variable.
 */
extern void diminuto_ipc6_ipv4toipv6(diminuto_ipv4_t address, diminuto_ipv6_t * addressp);

/*******************************************************************************
 * RESOLVERS
 ******************************************************************************/

/**
 * Convert a hostname or an IPV6 address string in dot notation into one
 * or more IPv6 addresses in host byte order. Since a single host can map to
 * multiple addresses, this returns a list of addresses in dynamically acquired
 * memory. The last entry will be all zeros. The list must be freed by the
 * application.
 * IMPORTANT SAFETY TIP: the underlying glibc gethostbyname(3) function ONLY
 * works if the application is dynamically linked; the build will emit a
 * warning to this effect.
 * NOTE: I have seen the underlying glibc gethostbyname(3) function fail and
 * then succeed on subsequent tries with the same argument; it depends on the
 * relatively vast Domain Name System (DNS) distributed infrastructure, so
 * there are lots of points of temporary failure.
 * @param hostname points to the hostname or IP address string.
 * @return an array or NULL if no such hostname or the string is invalid.
 */
extern diminuto_ipv6_t * diminuto_ipc6_addresses(const char * hostname);

/**
 * Convert a hostname or an IPV6 address string in dot notation into an IPv6
 * address in host byte order. Since a single host can map to multiple
 * addresses, only the first address is returned.
 * IMPORTANT SAFETY TIP: the underlying glibc gethostbyname(3) function ONLY
 * works if the application is dynamically linked; the build will emit a
 * warning to this effect.
 * NOTE: I have seen the underlying glibc gethostbyname(3) function fail and
 * then succeed on subsequent tries with the same argumnet; it depends on the
 * relatively vast Domain Name System (DNS) distributed infrastructure, so
 * there are lots of points of temporary failure.
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

/*******************************************************************************
 * STRINGIFIERS
 ******************************************************************************/

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

/*******************************************************************************
 * INTERROGATORS
 ******************************************************************************/

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
extern int diminuto_ipc6_source(int fd, diminuto_ipv6_t address, diminuto_port_t port);

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
 * @return >=0 for success or <0 if an error occurred.
 */
static inline int diminuto_ipc6_close(int fd) {
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
extern int diminuto_ipc6_stream_provider_base(diminuto_ipv6_t address, diminuto_port_t port, const char * interface, int backlog, diminuto_ipc_injector_t * functionp, void * datap);

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
static inline int diminuto_ipc6_stream_provider_generic(diminuto_ipv6_t address, diminuto_port_t port, const char * interface, int backlog) {
    return diminuto_ipc6_stream_provider_base(address, port, interface, backlog, diminuto_ipc_inject_defaults, (void *)0);
}

/**
 * Create a provider-side stream socket with the maximum connection backlog.
 * The port is in host byte order. If the port is zero, an unused ephemeral
 * port is allocated; its value can be determined using the nearend function.
 * @param port is the port number at which connection requests will rendezvous.
 * @return a provider-side stream socket or <0 if an error occurred.
 */
static inline int diminuto_ipc6_stream_provider(diminuto_port_t port) {
    return diminuto_ipc6_stream_provider_generic(DIMINUTO_IPC6_UNSPECIFIED, port, (const char *)0, -1);
}

/**
 * Wait for and accept a connection request from a consumer on a provider-side
 * stream socket. Optionally return the address and port in host byte order to
 * the provider.
 * @param fd is the provider-side stream socket.
 * @param addressp if non-NULL points to where the address will be stored.
 * @param portp if non-NULL points to where the port will be stored.
 * @return a data stream socket to the provider or <0 if an error occurred.
 */
extern int diminuto_ipc6_stream_accept_generic(int fd, diminuto_ipv6_t * addressp, diminuto_port_t * portp);

/**
 * Wait for and accept a connection request from a consumer on a provider-side
 * stream socket.
 * @param fd is the provider-side stream socket.
 * @return a data stream socket to the provider or <0 if an error occurred.
 */
static inline int diminuto_ipc6_stream_accept(int fd) {
    return diminuto_ipc6_stream_accept_generic(fd, (diminuto_ipv6_t *)0, (diminuto_port_t *)0);
}

/**
 * Request a consumer-side stream socket to a provider using a specific address,
 * port, and interface on the near end. If an optional function is provided by
 * the caller, invoke it to set socket options before the connect(2) is
 * performed.
 * @param address is the provider's IPv6 address in host byte order.
 * @param port is the provider's port in host byte order.
 * @param address0 is the address to which to bind the socket, or zero.
 * @param port0 is the port to which to bind the socket, or zero
 * @param interface points to the name of the interface, or NULL.
 * @param functionp points to an optional function to set socket options.
 * @param datap is passed to the optional function.
 * @return a data stream socket to the consumer or <0 if an error occurred.
 */
extern int diminuto_ipc6_stream_consumer_base(diminuto_ipv6_t address, diminuto_port_t port, diminuto_ipv6_t address0, diminuto_port_t port0, const char * interface, diminuto_ipc_injector_t * functionp, void * datap);

/**
 * Request a consumer-side stream socket to a provider using a specific address,
 * port, and interface on the near end.
 * @param address is the provider's IPv6 address in host byte order.
 * @param port is the provider's port in host byte order.
 * @param address0 is the address to which to bind the socket, or zero.
 * @param port0 is the port to which to bind the socket, or zero
 * @param interface points to the name of the interface, or NULL.
 * @return a data stream socket to the consumer or <0 if an error occurred.
 */
static inline int diminuto_ipc6_stream_consumer_generic(diminuto_ipv6_t address, diminuto_port_t port, diminuto_ipv6_t address0, diminuto_port_t port0, const char * interface) {
    return diminuto_ipc6_stream_consumer_base(address, port, address0, port0, interface, diminuto_ipc_inject_defaults, (void *)0);
}

/**
 * Request a consumer-side stream socket to a provider. The address and port
 * are in host byte order.
 * @param address is the provider's IPv6 address.
 * @param port is the provider's port.
 * @return a data stream socket to the consumer or <0 if an error occurred.
 */
static inline int diminuto_ipc6_stream_consumer(diminuto_ipv6_t address, diminuto_port_t port) {
    return diminuto_ipc6_stream_consumer_generic(address, port, DIMINUTO_IPC6_UNSPECIFIED, 0, (const char *)0);
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
static inline ssize_t diminuto_ipc6_stream_read_generic(int fd, void * buffer, size_t min, size_t max) {
    return diminuto_ipc_stream_read_generic(fd, buffer, min, max);
}

/**
 * Read bytes from a stream socket into a buffer.
 * @param fd is an open stream socket.
 * @param buffer points to the buffer into which data is read.
 * @param size is the maximum number of bytes to be read.
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EAGAIN for non-blocking, EINTR for timer expiry).
 */
static inline ssize_t diminuto_ipc6_stream_read(int fd, void * buffer, size_t size) {
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
static inline ssize_t diminuto_ipc6_stream_write_generic(int fd, const void * buffer, size_t min, size_t max) {
    return diminuto_ipc_stream_write_generic(fd, buffer, min, max);
}

/**
 * Write bytes to a stream socket from a buffer.
 * @param fd is an open stream socket.
 * @param buffer points to the buffer from which data is written.
 * @param size is the maximum number of bytes to be written.
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EAGAIN for non-blocking, EINTR for timer expiry).
 */
static inline ssize_t diminuto_ipc6_stream_write(int fd, const void * buffer, size_t size) {
    return diminuto_ipc_stream_write(fd, buffer, size);
}

/*******************************************************************************
 * DATAGRAM SOCKETS
 ******************************************************************************/

/**
 * Request a peer datagram socket. The address and port are in host byte order.
 * If an optional function is provided by the caller, invoke it to set socket
 * options.
 * @param address is the address of the interface that will be used.
 * @param port is the port number. If the port is zero, an unused ephemeral
 * port is allocated; its value can be determined using the nearend function.
 * @param interface points to the name of the interface, or NULL.
 * @param functionp points to an optional function to set socket options.
 * @param datap is passed to the optional function.
 * @return a peer datagram socket or <0 if an error occurred.
 */
extern int diminuto_ipc6_datagram_peer_base(diminuto_ipv6_t address, diminuto_port_t port, const char * interface, diminuto_ipc_injector_t * functionp, void * datap);

/**
 * Request a peer datagram socket. The address and port are in host byte order.
 * @param address is the address of the interface that will be used.
 * @param port is the port number. If the port is zero, an unused ephemeral
 * port is allocated; its value can be determined using the nearend function.
 * @param interface points to the name of the interface, or NULL.
 * @return a peer datagram socket or <0 if an error occurred.
 */
static inline int diminuto_ipc6_datagram_peer_generic(diminuto_ipv6_t address, diminuto_port_t port, const char * interface) {
    return diminuto_ipc6_datagram_peer_base(address, port, interface, diminuto_ipc_inject_defaults, (void *)0);
}

/**
 * Request a peer datagram socket. The port is in host byte order.
 * @param port is the port number. If the port is zero, an unused ephemeral
 * port is allocated; its value can be determined using the nearend function.
 * @return a peer datagram socket or <0 if an error occurred.
 */
static inline int diminuto_ipc6_datagram_peer(diminuto_port_t port) {
    return diminuto_ipc6_datagram_peer_generic(DIMINUTO_IPC6_UNSPECIFIED, port, (const char *)0);
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
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EAGAIN for non-blocking, EINTR for timer expiry).
 */
extern ssize_t diminuto_ipc6_datagram_receive_generic(int fd, void * buffer, size_t size, diminuto_ipv6_t * addressp, diminuto_port_t * portp, int flags);

/**
 * Receive a datagram from a datagram socket using no flags.
 * Optionally return the address and port in host byte order of the sender.
 * (This function can legitimately be also used with a stream socket.)
 * @param fd is an open datagram socket.
 * @param buffer points to the buffer into which data is received.
 * @param size is the maximum number of bytes to be received.
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EAGAIN for non-blocking, EINTR for timer expiry).
 */
static inline ssize_t diminuto_ipc6_datagram_receive(int fd, void * buffer, size_t size) {
    return diminuto_ipc6_datagram_receive_generic(fd, buffer, size, (diminuto_ipv6_t *)0, (diminuto_port_t *)0, 0);
}

/**
 * Send a datagram to a datagram socket using the specified flags. The address
 * and port are in host byte order. Flags may be specified for sendto(2) such as
 * MSG_DONTWAIT, MSG_WAITALL, or MSG_OOB. (This function can legitimately be
 * also used with a stream socket by passing zero as the port number, in which
 * case the address is ignored.)
 * @param fd is an open datagram socket.
 * @param buffer points to the buffer from which data is sent.
 * @param size is the maximum number of bytes to be sent.
 * @param address is the receiver's address.
 * @param port is the receiver's port.
 * @param flags is the sendto(2) flags to be used.
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EAGAIN for non-blocking, EINTR for timer expiry).
 */
extern ssize_t diminuto_ipc6_datagram_send_generic(int fd, const void * buffer, size_t size, diminuto_ipv6_t address, diminuto_port_t port, int flags);

/**
 * Send a datagram to a datagram socket using no flags. The address and port are
 * in host byte order. (This function can legitimately be also used with a
 * stream socket by passing zero as the port number, in which case the address
 * is ignored.)
 * @param fd is an open datagram socket.
 * @param buffer points to the buffer from which data is sent.
 * @param size is the maximum number of bytes to be sent.
 * @param address is the receiver's address.
 * @param port is the receiver's port.
 * @return the number of bytes received, 0 if the far end closed, or <0 if an error occurred (errno will be EAGAIN for non-blocking, EINTR for timer expiry).
 */
static inline ssize_t diminuto_ipc6_datagram_send(int fd, const void * buffer, size_t size, diminuto_ipv6_t address, diminuto_port_t port) {
    return diminuto_ipc6_datagram_send_generic(fd, buffer, size, address, port, 0);
}

/*******************************************************************************
 * INTERFACES
 ******************************************************************************/

/**
 * Return a list of IPv6 addresses associated with the specified network
 * interface. The addresses will be in host byte order. Since a single
 * interface can map to multiple addresses, this returns a list of addresses
 * in dynamically acquired memory. The last entry will be all zeros. The
 * list must be freed by the application using free(3).
 * @param interface points to the interface name string.
 * @return an array or NULL if no such interface or the string is invalid.
 */
extern diminuto_ipv6_t * diminuto_ipc6_interface(const char * interface);

#endif
