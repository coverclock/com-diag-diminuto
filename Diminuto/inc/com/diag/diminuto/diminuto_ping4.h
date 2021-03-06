/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_PING4_
#define _H_COM_DIAG_DIMINUTO_PING4_

/**
 * @file
 * @copyright Copyright 2015-2016 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements a function to send an IPv4 ICMP ECHO REQUEST and receive a response.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Ping4 feature is a programmatic version of ping(1): send an ICMP ECHO
 * REQUEST to an IPv4 address and (hopefully) receive an ICMP ECHO REPLY back.
 * If so, compute the latency in ticks between the REQUEST and the REPLY.
 * On most platforms, this must run with elevated privileges.
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_ipc4.h"

/**
 * Create a raw socket over which an ICMP ECHO REQUEST datagram can be sent
 * and an ICMP ECHO REPLY datagram can be received. This is a privileged
 * operation. This socket can be used with the mux feature.
 * @return >=0 for a raw socket, or <0 if an error occurred.
 */
extern int diminuto_ping4_datagram_peer(void);

/**
 * Sent an ICMP ECHO REQUEST datagram to the specified IPv4 address.
 * @param fd is the raw socket.
 * @param address is the IPv4 address.
 * @param id is the ECHO REQUEST id assigned by the caller.
 * @param seq is the ECHO REQUEST sequence number assigned by the caller.
 * @return >0 if successful, or <0 if an error occurred.
 */
extern ssize_t diminuto_ping4_datagram_send(int fd, diminuto_ipv4_t address, uint16_t id, uint16_t seq);

/**
 * Receive an ICMP ECHO REPLY reply to the ECHO REQUEST datagram. The response could be
 * an error (<0), an ICMP ECHO REPLY datagram (>0), or some other ICMP datagram
 * (0). It is up to the caller to match the identifier and sequence number.
 * @param fd is the raw socket. If any datagram is received, the sending address
 * is returned. If an ICMP header is present, the type is returned. If the
 * datagram is an ECHO REPLY, the id, sequence number, and time to live is
 * returned. If the packet is long enough to be the one that was sent, the
 * elapsed time is returned.
 * @param addressp points to a variable into which the replying address is returned, or NULL.
 * @param typep points to a variable into which the ICMP message type is returned, or NULL.
 * @param codep points to a variable into which the ICMP message code is returned, or NULL.
 * @param idp points to a variable into which the ECHO REPLY id is returned, or NULL.
 * @param seqp points to a variable into which the ECHO REPLY sequence number is returned, or NULL.
 * @param ttlp points to a variable into which the IP Time To Live hop count is returned, or NULL.
 * @param elapsedp points to a variable into which the elapsed ticks is returned, or NULL.
 * @return >0 for a reply, 0 for some other datagram, or <0 if an error occurred.
 */
extern ssize_t diminuto_ping4_datagram_receive(int fd, diminuto_ipv4_t * addressp, uint8_t * typep, uint8_t * codep, uint16_t * idp, uint16_t * seqp, uint8_t * ttlp, diminuto_ticks_t * elapsedp);

/**
 * Close the raw socket.
 * @param fd is the raw socket.
 * @return >=0 for success, <0 for failure.
 */
static inline int diminuto_ping4_close(int fd) {
    return diminuto_ipc4_close(fd);
}

#endif
