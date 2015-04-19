/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_PING_
#define _H_COM_DIAG_DIMINUTO_PING_

/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * WORK IN PROGRESS!
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_ipc.h"

/**
 * Create a raw socket over which an ICMP ECHO REQUEST datagram can be sent
 * and an ICMP ECHO REPLY datagram can be received. This is a privileged
 * operation. This socket can be used with the mux feature.
 * @return >=0 for a raw socket, or <0 if an error occurred.
 */
extern int diminuto_ping_datagram_peer(void);

/**
 * Sent an ICMP ECHO REQUEST datagram to the specified IPv4 address.
 * @param fd is the raw socket.
 * @param address is the IPv4 address.
 * @param id is the ECHO REQUEST id assigned by the caller.
 * @param seq is the ECHO REQUEST sequence number assigned by the caller.
 * @return >0 if successful, or <0 if an error occurred.
 */
extern ssize_t diminuto_ping_datagram_send(int fd, diminuto_ipv4_t address, uint16_t id, uint16_t seq);

/**
 * Receive an ICMP ECHO REPLY reply to the ECHO REQUEST datagram. The response could be
 * an error (<0), an ICMP ECHO REPLY datagram with a matching id (>0), or some
 * other datagram (0).
 * @param fd is the raw socket.
 * @param addressp points to a variable into which the replying address is returned, or NULL.
 * @param idp points to a variable into which the ECHO REPLY id is returned, or NULL.
 * @param seqp points to a variable into which the ECHO REPLY sequence number is returned, or NULL
 * @return >0 for a reply, 0 for some other datagram, or <0 if an error occurred.
 */
extern ssize_t diminuto_ping_datagram_recv(int fd, diminuto_ipv4_t * addressp, uint16_t * idp, uint16_t * seqp);

/**
 * Close the raw socket.
 * @param fd is the raw socket.
 * @return >=0 for success, <0 for failure.
 */
static inline int diminuto_ping_close(int fd) {
    return diminuto_ipc_close(fd);
}

#endif
