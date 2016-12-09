/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_PING6_
#define _H_COM_DIAG_DIMINUTO_PING6_

/**
 * @file
 *
 * Copyright 2015-2016 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * This feature is a programmatic version of ping(1): send an ICMP6 ECHO REQUEST
 * to an IPv6 address and (hopefully) receive an ICMP6 ECHO REPLY back. If so,
 * compute the latency in ticks between the REQUEST and the REPLY.
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_ipc6.h"

/**
 * Create a raw socket over which an ICMP6 ECHO REQUEST datagram can be sent
 * and an ICMP6 ECHO REPLY datagram can be received. This is a privileged
 * operation.
 * @return >=0 for a raw socket, or <0 if an error occurred.
 */
extern int diminuto_ping6_datagram_peer(void);

/**
 * Optionally bind raw socket to a specific interface. This forces the ICMP6
 * ECHO REQUEST datagram send and the receive for the ICMP6 ECHO REPLY datagram
 * to only use the specified interface.
 * @param fd is the raw socket.
 * @param ifname is the interface name, e.g. "eth1".
 * @return >=0 for success, or <0 if an error occurred.
 */
static inline int diminuto_ping6_interface(int fd, const char * ifname) {
    return diminuto_ipc6_set_interface(fd, ifname);
}

/**
 * Optionally bind raw socket to a specific source address and port. This isn't
 * necessary for this feature to work, but does make commands that display the
 * local address being used (like netstat(1)) display something other than
 * zeros for the source address. (The kernel requires that the address be that
 * of the interface being used.)
 * @param fd is the raw socket.
 * @param address is the IPv6 address.
 * @param port is the port number.
 * @return >= 0 for success, or <0 of an error occurred.
 */
static inline int diminuto_ping6_source(int fd, diminuto_ipv6_t address, diminuto_port_t port) {
    return diminuto_ipc6_source(fd, address, port);
}

/**
 * Sent an ICMP6 ECHO REQUEST datagram to the specified IPv6 address.
 * @param fd is the raw socket.
 * @param address is the IPv4 address.
 * @param id is the ECHO REQUEST id assigned by the caller.
 * @param seq is the ECHO REQUEST sequence number assigned by the caller.
 * @return >0 if successful, or <0 if an error occurred.
 */
extern ssize_t diminuto_ping6_datagram_send(int fd, diminuto_ipv6_t address, uint16_t id, uint16_t seq);

/**
 * Receive a response to the ICMP6 ECHO REQUEST datagram. The response could be
 * an error (<0), an ICMP6 ECHO REPLY datagram (>0), or some other datagram (0)
 * that was legitimate but not an ICMP6 ECHO REPLY. In the latter case, the
 * caller must decide whether to retry. If any datagram is received, the sending
 * address is returned. If an ICMP header is present, the type is returned. If
 * the datagram is an ECHO REPLY, the id, sequence number, and time to live is
 * returned. If the packet is long enough to be the one that was sent, the
 * elapsed time is returned.
 * @param fd is the raw socket.
 * @param addressp points to a variable into which the replying address is returned or NULL.
 * @param typep points to a variable into which the ICMP message type is returned, or NULL.
 * @param idp points to a variable into which the ECHO REPLY id is returned, or NULL.
 * @param seqp points to a variable into which the ECHO REPLY sequence number is returned, or NULL
 * @param elapsedp points to a variable into which the elapsed ticks is returned, or NULL.
 * @return >0 for a reply, 0 for some other datagram, or <0 if an error occurred.
 */
extern ssize_t diminuto_ping6_datagram_recv(int fd, diminuto_ipv6_t * addressp, uint8_t * typep, uint16_t * idp, uint16_t * seqp, diminuto_ticks_t * elapsedp);

/**
 * Close the raw socket.
 * @param fd is the raw socket.
 * @return >=0 for success, <0 for failure.
 */
static inline int diminuto_ping6_close(int fd) {
    return diminuto_ipc6_close(fd);
}

#endif
