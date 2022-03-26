/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_IPC_
#define _H_COM_DIAG_DIMINUTO_IPC_

/**
 * @file
 * @copyright Copyright 2010-2021 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Provides capabilities common to IPv4, IPv6, and local sockets.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The IPC feature provides socket and other interprocess/interprocessor
 * communication capabilities using IPv4, IPv6, and UNIX Domain (local)
 * sockets.
 *
 * REFERENCES
 *
 * Wind River Systems, *VxWorks Reference Manual*, 5.4
 *
 * V. Jacobson et al., “TCP Extensions for High Performance”, RFC1323, 1992
 *
 * V. Welch, “A User’s Guide to TCP Windows”, NCSA, 1996
 * 
 * J. Mahdavi, “Enabling High Performance Data Transfers on Hosts”,
 * Pittsburgh Supercomputing Center, 1996
 *
 * W. Stevens, *TCP/IP Illustrated Volume 1: The Protocols*,
 * Addison-Wesley, 1994
 * 
 * G. Wright et al., *TCP/IP Illustrated Volume 2: The Implementation*,
 * Addison-Wesley, 1995
 *
 * M. Vaner, "40 millisecond bug", 2020-11-06,
 * <https://vorner.github.io/2020/11/06/40-ms-bug.html>
 *
 * W. Sewell, "Ephemeral port exhaustion and how to avoid it", 2018-05-22,
 * <https://making.pusher.com/ephemeral-port-exhaustion-and-how-to-avoid-it/>
 *
 * C. Sridharan, "File Descriptor Transfer over Unix Domain Sockets",
 * CopyConstruct, August 2020
 *
 * U. Naseer et al., "Zero Downtime Release: Disruption-free Load
 * Balancing of a Multi-Billion User Website", ACM SIGCOMM '20,
 * August 2020
 *
 * K. Kumar, "Linux TCP SO_REUSEPORT - Usage and implementation",
 * 2019-08-19,
 * <https://tech.flipkart.com/linux-tcp-so-reuseport-usage-and-implementation-6bfbf642885a>
 *
 * B. Hubert, "The ultimate SO_LINGER page, or: why is my tcp not
 * reliable", 2009-01-18,
 * <https://blog.netherlabs.nl/articles/2009/01/18/the-ultimate-so_linger-page-or-why-is-my-tcp-not-reliable>
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_fd.h"
#include <stdio.h> /* For snprintf(3). */
#include <sys/socket.h> /* For AF_UNSPEC, AF_UNIX, AF_INET, struct msghdr. */
#include <netinet/in.h> /* IPPROTO_TCP, IPPROTO_UDP, IPPROTO_IPV6. */
#include <netinet/tcp.h> /* for TCP_NODELAY, TCP_QUICKACK. */
#include <fcntl.h> /* for O_NONBLOCK. */
#include <linux/sockios.h> /* For SIOCINQ, SIOCOUTQ. */
#include <asm/ioctls.h> /* For FIONREAD, TIOCOUTQ. */


/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

/**
 * This is the Diminuto binary port number that will cause an ephemeral port
 * to be allocated. Helpfully, the this port number is the same value whether
 * in network or host byte order. (It is perfectly okay if you want to just
 * use the constant 0; this value is not going to change. But using this
 * constant makes it easier to search for uses in the source code.)
 */
static const diminuto_port_t DIMINUTO_IPC_EPHEMERAL = 0;

/*******************************************************************************
 * TYPES
 ******************************************************************************/

/**
 * Defines the prototype of a function used for dependency injection.
 */
typedef int (diminuto_ipc_injector_t)(int fd, void *);

/*******************************************************************************
 * RESOLVERS
 ******************************************************************************/

/**
 * Convert a service name or a port number string into a port in host byte
 * order. Note that legitimate port numbers are unsigned and must be >0, so
 * the value returned for an error is 0, not -1. This happens to also be the
 * value used to indicate that an ephemeral port should be allocated, which
 * could be a useful coincidence.
 * @param service points to the service name or port number string.
 * @param protocol points to the protocol name (e.g. "udp" or "tcp").
 * @return the port number or 0 if no such service exists for the protocol.
 */
extern diminuto_port_t diminuto_ipc_port(const char * service, const char * protocol);

/*******************************************************************************
 * CLASSIFIERS
 ******************************************************************************/

/**
 * Return true if the specified port when bound to a socket will cause an
 * ephemeral port to be allocated and used instead, false otherwise. Note
 * that allocated emphemeral ports WILL have non-zero values; this just
 * indicates that the specified port will cause an ephemeral port to be
 * allocated.
 * @param port is the port number.
 * @return true if the port number selects an ephemeral port, false otherwise.
 */
static inline int diminuto_ipc_is_ephemeral(diminuto_port_t port)
{
    return (port == DIMINUTO_IPC_EPHEMERAL);
}

/*******************************************************************************
 * OPTIONS
 ******************************************************************************/

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
 * Enable or disable a file control option (fcntl(2)) on a socket.
 * @param fd is an open socket file descriptor.
 * @param enable is true to enable, false to disable.
 * @param mask is a bit mask indication the option to enable or disable.
 * @return >=0 for success, <0 for error.
 */
extern int diminuto_ipc_set_status(int fd, int enable, long mask);

/**
 * Set a socket option (setsockopt(2)) on a socket.
 * @param fd is an open socket file descriptor.
 * @param level is the level in the protocol stack at which to set.
 * @param option is the option to set.
 * @param value is the value (if any) to apply to the option.
 * @return >=0 for success, <0 for error.
 */
extern int diminuto_ipc_set_socket(int fd, int level, int option, int value);

/**
 * Enable or disable the non-blocking status.
 * @param fd is an open socket of any type.
 * @param enable is !0 to enable, 0 to disable.
 * @return >=0 for success or <0 if an error occurred.
 */
static inline int diminuto_ipc_set_nonblocking(int fd, int enable) {
    return diminuto_ipc_set_status(fd, enable, O_NONBLOCK);
}

/**
 * Enable or disable the address reuse option.
 * @param fd is an open socket of any type.
 * @param enable is !0 to enable, 0 to disable.
 * @return >=0 for success or <0 if an error occurred.
 */
static inline int diminuto_ipc_set_reuseaddress(int fd, int enable) {
    return diminuto_ipc_set_socket(fd, SOL_SOCKET, SO_REUSEADDR, !!enable);
}

/**
 * Enable or disable the port reuse option.
 * @param fd is an open socket of any type.
 * @param enable is !0 to enable, 0 to disable.
 * @return >=0 for success or <0 if an error occurred.
 */
static inline int diminuto_ipc_set_reuseport(int fd, int enable) {
    return diminuto_ipc_set_socket(fd, SOL_SOCKET, SO_REUSEPORT, !!enable);
}

/**
 * Enable or disable the keep alive option.
 * @param fd is an open socket of any type.
 * @param enable is !0 to enable, 0 to disable.
 * @return >=0 for success or <0 if an error occurred.
 */
static inline int diminuto_ipc_set_keepalive(int fd, int enable) {
    return diminuto_ipc_set_socket(fd, SOL_SOCKET, SO_KEEPALIVE, !!enable);
}

/**
 * Enable or disable the debug option (only available to root on most systems).
 * @param fd is an open socket of any type.
 * @param enable is !0 to enable, 0 to disable.
 * @return >=0 for success or <0 if an error occurred.
 */
static inline int diminuto_ipc_set_debug(int fd, int enable) {
    return diminuto_ipc_set_socket(fd, SOL_SOCKET, SO_DEBUG, !!enable);
}

/**
 * Enable or disable the receiving of the timestamp control message.
 * N.B. This option can only be used on sockets which can receive control
 * messages, done via recvmsg(2).
 * N.B. Counterintuitively, this option should DISABLED is you are going to
 * use diminuto_ipc_get_timestamp(), which uses the SIOCGSTAMP ioctl(2) to
 * access the timestamp of the last received packet. They are mutually
 * exclusive.
 * @param fd is an open socket.
 * @param enable is !0 to enable, 0 to disable.
 * @return >=0 for success or <0 if an error occurred.
 */
static inline int diminuto_ipc_set_timestamp(int fd, int enable) {
    return diminuto_ipc_set_socket(fd, SOL_SOCKET, SO_TIMESTAMP, !!enable);
}

/**
 * Enable or disable the linger option.
 * @param fd is an open socket of any type.
 * @param ticks is the number of ticks to linger (although
 * the underlying socket option has granularity of seconds),
 * 0 for no lingering, or ~0 for maximum lingering.
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
static inline int diminuto_ipc_set_nodelay(int fd, int enable) {
    return diminuto_ipc_set_socket(fd, IPPROTO_TCP, TCP_NODELAY, !!enable);
}

/**
 * Enable or disable the TCP Quick Acknowledgement option. (Useful for
 * reducing latency when using small packets.)
 * @param fd is an open strean socket.
 * @param enable is !0 to enable, 0 to disable.
 * @return >=0 for success or <0 if an error occurred.
 */
static inline int diminuto_ipc_set_quickack(int fd, int enable) {
    return diminuto_ipc_set_socket(fd, IPPROTO_TCP, TCP_QUICKACK, !!enable);
}

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
static inline int diminuto_ipc6_set_ipv6only(int fd, int enable) {
    return diminuto_ipc_set_socket(fd, IPPROTO_IPV6, IPV6_V6ONLY, !!enable);
}

/**
 * Convert a IPv6 stream socket into an IPv4 stream socket.
 * DEPRECATED and SKETCHY in the kernel, NOT RECOMMENDED:
 * IPV6_ADDRFORM appears to have been deprecated in the latest pertinent
 * internet standard, RFC 3943. And perusing the implementation of it in
 * the 4.2 kernel code I had some WTF moments (e.g. UDP is supported but
 * the socket has to have TCP Established set). Seems pretty sketchy to me.
 * @param fd is an open IPv6 stream socket.
 * @return >=0 for success or <0 if an error occurred.
 */
static inline int diminuto_ipc6_stream_set_ipv6toipv4(int fd) {
    return diminuto_ipc_set_socket(fd, IPPROTO_TCP, IPV6_ADDRFORM, AF_INET);
}

/**
 * Convert a IPv6 datagram socket into an IPv4 datagram socket.
 * DEPRECATED and SKETCHY in the kernel, NOT RECOMMENDED:
 * IPV6_ADDRFORM appears to have been deprecated in the latest pertinent
 * internet standard, RFC 3943. And perusing the implementation of it in
 * the 4.2 kernel code I had some WTF moments (e.g. UDP is supported but
 * the socket has to have TCP Established set). Seems pretty sketchy to me.
 * @param fd is an open IPv6 datagram socket.
 * @return >=0 for success or <0 if an error occurred.
 */
static inline int diminuto_ipc6_datagram_set_ipv6toipv4(int fd) {
    return diminuto_ipc_set_socket(fd, IPPROTO_UDP, IPV6_ADDRFORM, AF_INET);
}

/*
 * (Many other options are possible, but these are the ones I have used.)
 */

/*******************************************************************************
 * INJECTORS
 ******************************************************************************/

/**
 * Dependency injector used to set the the socket options in the
 * ipc4 and ipc6 APIs.
 */
extern diminuto_ipc_injector_t diminuto_ipc_inject_defaults;

/*******************************************************************************
 * SOCKETS
 ******************************************************************************/

/**
 * Return the Address Family for the specified socket. If created by
 * one of the Diminuto IPC functions, this value will be AF_INET for
 * IPC4, AF_INET6 for IPC6, or AF_UNIX (a.k.a. AF_LOCAL) for IPCL. But
 * if created through some other means, it may be some other value.
 * @param fd is the socket to be queried.
 * @return the Address Family value include AF_UNSPEC is an error occurred.
 */
extern int diminuto_ipc_type(int fd);

/**
 * Get an I/O control option (ioctl(2)) on a socket.
 * @param fd is an open socket file descriptor.
 * @param option is the option to get.
 * @return the value of the option, or <0 for error.
 */
extern int diminuto_ipc_get_control(int fd, int option);

/**
 * Get a timestamp in ticks indicating when the last packet was received.
 * This returns failure if data has never been received.
 * N.B. This mechanism is mutually exclusive with the
 * diminuto_ipc_set_timestamp() socket option.
 * N.B. This mechanism only works with datagrams (or raw sockets).
 * @param fd is an open socket file descriptor.
 * @return the timestamp in ticks, or <0 for error.
 */
extern diminuto_sticks_t diminuto_ipc_get_timestamp(int fd);

/**
 * Return the number of bytes available on the input queue waiting to be
 * received by the application.
 * @param fd is an open stream socket.
 * @return >=0 for success or <0 if an error occurred.
 */
static inline ssize_t diminuto_ipc_stream_get_available(int fd) {
    return diminuto_ipc_get_control(fd, SIOCINQ);
}

/**
 * Return the number of bytes pending on the output queue waiting to be
 * transmitted by the kernel. (This may include data that has been
 * sent to the far end but thus far unacknowledged and so cannot be removed
 * from the output queue.)
 * @param fd is an open stream socket.
 * @return >=0 for success or <0 if an error occurred.
 */
static inline ssize_t diminuto_ipc_stream_get_pending(int fd) {
    return diminuto_ipc_get_control(fd, SIOCOUTQ);
}

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

/*******************************************************************************
 * STREAM SOCKETS
 ******************************************************************************/

/**
 * Read bytes from a stream socket into a buffer until at least a minimum
 * number of bytes are read and no more than a maximum number of bytes are
 * read. Less than the minimum can still be read if an error occurs.
 * @param fd is an open stream socket.
 * @param buffer points to the buffer into which data is read.
 * @param min is the minimum number of bytes to be read.
 * @param max is the maximum number of bytes to be read.
 * @return the number of bytes received, 0 if the far end closed,
 * or <0 if an error occurred (errno will be EAGAIN for non-blocking,
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
 * or <0 if an error occurred (errno will be EAGAIN for non-blocking,
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
 * or <0 if an error occurred (errno will be EAGAIN for non-blocking,
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
 * or <0 if an error occurred (errno will be EAGAIN for non-blocking,
 * EINTR for timer expiry).
 */
static inline ssize_t diminuto_ipc_stream_write(int fd, const void * buffer, size_t size) {
    return diminuto_fd_write(fd, buffer, size);
}

/*******************************************************************************
 * MESSAGING
 ******************************************************************************/

/**
 * Receive a message. This operation can be performed on a stream, datagram, or
 * local (path) socket. It can be used for vector (scatter/gather) I/O,
 * kernel control messaging, and other purposes.
 * @param fd is an open socket descriptor.
 * @param message points to a socket message header.
 * @param flags is a set of bit flags.
 * @return the number of bytes received or <0 if an error occurred.
 */
extern ssize_t diminuto_ipc_message_receive_generic(int fd, struct msghdr * message, int flags);

/**
 * Send a message. This operation can be performed on a stream, datagram, or
 * local (path) socket. It can be used for vector (scatter/gather) I/O,
 * kernel control messaging, and other purposes.
 * @param fd is an open socket descriptor.
 * @param message points to a socket message header.
 * @param flags is a set of bit flags.
 * @return the number of bytes sent or <0 if an error occurred.
 */
extern ssize_t diminuto_ipc_message_send_generic(int fd, const struct msghdr * message, int flags);

/**
 * Receive a message. This operation can be performed on a stream, datagram, or
 * local (path) socket. It can be used for vector (scatter/gather) I/O,
 * kernel control messaging, and other purposes.
 * @param fd is an open socket descriptor.
 * @param message points to a socket message header.
 * @return the number of bytes received or <0 if an error occurred.
 */
static inline ssize_t diminuto_ipc_message_receive(int fd, struct msghdr * message) {
    return diminuto_ipc_message_receive_generic(fd, message, 0);
}

/**
 * Send a message. This operation can be performed on a stream, datagram, or
 * local (path) socket. It can be used for vector (scatter/gather) I/O,
 * kernel control messaging, and other purposes.
 * @param fd is an open socket descriptor.
 * @param message points to a socket message header.
 * @return the number of bytes sent or <0 if an error occurred.
 */
static inline ssize_t diminuto_ipc_message_send(int fd, const struct msghdr * message) {
    return diminuto_ipc_message_send_generic(fd, message, 0);
}

/*******************************************************************************
 * INTERFACES
 ******************************************************************************/

/**
 * Return an array of strings containing a list of the network interfaces
 * available on the host. This is a single block of dynamically allocated
 * memory that should be eventually deallocated by the application using
 * free(3).
 * @return a list of available network interfaces or NULL if an error occurred.
 */
extern char ** diminuto_ipc_interfaces(void);

/*******************************************************************************
 * STRINGIFIERS
 ******************************************************************************/

/**
 * Convert a port number in host byte order into a printable port number
 * string. (It's perfectly okay to just use a printf(3)-style function
 * to print port numbers. This is provided just for the sake of completeness.)
 * @param port is the port number in host byte order.
 * @param buffer points to the buffer into to which the string is stored.
 * @param length is the length of the buffer in bytes.
 * @return a pointer to the buffer.
 */
static inline const char * diminuto_ipc_port2string(diminuto_port_t port, char * buffer, size_t length) {
    (void)snprintf(buffer, length, "%u", port);
    buffer[length - 1] = '\0';
    return buffer;
}

/*******************************************************************************
 * ENDPOINTS
 ******************************************************************************/

/**
 * These are the different types of Interprocess Communication Systems (IPC)
 * that the IPC feature supports (or soon will support). Rather than do my
 * usual thing of making the enumerations printable characters, these use
 * the appropriate Address Family (AF) value so that they can be directly
 * applied to the socket structure.
 */
typedef enum DiminutoIpcType {
    DIMINUTO_IPC_TYPE_UNSPECIFIED  = AF_UNSPEC,
    DIMINUTO_IPC_TYPE_IPV4         = AF_INET,
    DIMINUTO_IPC_TYPE_IPV6         = AF_INET6,
    DIMINUTO_IPC_TYPE_LOCAL        = AF_UNIX,
} diminuto_ipc_type_t;

/**
 * This type defines the structure that is populated when an endpoint string
 * is parsed. The parser (such as it is) tries to fill in all of the fields.
 * For example, it is possible that a Fully Qualified Domain Name (FQDN) will
 * have both IPv4 and IPv6 addresses, in which case the type will be IPV4 if
 * the IPv6 address is merely an IPv4 address in IPv6-compatible form,
 * otherwise IPV6. The application can always check the IPv4 and IPv6 address
 * fields explicitly. Fields that are not used will be zero or zero length.
 * Although the FQDN may resolve, there is no guarantee that the IPv4 or
 * IPv6 addresses are reachable; the service may be defined, but the
 * port may not be instantiated; the Local address (UNIX domain socket path)
 * may have been specified, but the corresponding file may not exist. Port
 * values of zero can be legitimate, indicating an ephemeral (dynamically
 * assigned and temporary) port number is being requested. The type field
 * can be used directly as the Address Family (AF) value. The IP addresses
 * and port numbers will be in Host Byte Order, NOT Network Byte Order. The
 * UNIX domain path will be an absolute path with all soft links resolved
 * and all path meta-characters resolved and removed.
 */
typedef struct DiminutoIpcEndpoint {
    diminuto_ipc_type_t type;       /* Address Family */
    diminuto_ipv4_t ipv4;           /* Host Byte Order */
    diminuto_ipv6_t ipv6;           /* Host Byte Order */
    diminuto_port_t tcp;            /* Host Byte Order */
    diminuto_port_t udp;            /* Host Byte Order */
    diminuto_local_t local;         /* Absolute Path */
} diminuto_ipc_endpoint_t;

/**
 * Try to parse an endpoint string that has some combination of host name, Fully
 * Qualified Domain Name (FQDN), IPv4 address string, IPv6 address string, port
 * number string or service name, or Local path (UNIX domain path), into a
 * usable IPv4 or IPv6 binary address, binary TCP and/or UDP port numbers, or
 * an absolute path in the file system of which all but the last path component
 * must exist.  Local paths must contain a slash ("/") somewhere in them to
 * discriminate between a path name and other possible interpretations. An
 * empty string is a valid endpoint, indicating an ephemeral port on the local
 * host. Here are just a few examples of valid endpoints and their
 * interpretations:
 * "80" (port),
 * ":80" (port),
 * ":http" (service), 
 * "localhost" (host),
 * "localhost:80" (host and port),
 * "myhostname" (host),
 * "diag.com" (domain),
 * "diag.com:http" (domain and service),
 * "diag.com:80" (domain and port),
 * "172.217.1.206:http" (IPv4 and service),
 * "172.217.1.206:80" (IPv4 and port),
 * "[::ffff:172.217.1.206]:80" (IPv6 and port),
 * "[2607:f8b0:400f:805::200e]:http" (IPv6 and service),
 * "/tmp/unix.sock" (path),
 * "./unix.sock" (path),
 * "run/unix.sock" (path),
 * "[::]" (IPv6 with ephemeral),
 * "0.0.0.0" (IPv4 with ephemeral),
 * "0" (emphemeral),
 * "" (unnamed path).
 * @param string points to the endpoint string.
 * @param endpoint points to the structure in which the results are stored.
 * @return 0 if both syntactically and semantically successful, <0 otherwise.
 */
extern int diminuto_ipc_endpoint(const char * string, diminuto_ipc_endpoint_t * endpoint);

#endif
