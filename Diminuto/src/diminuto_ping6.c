/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/icmp6.h>
#include "diminuto_ipc6.h"
#include "com/diag/diminuto/diminuto_ping6.h"
#include "com/diag/diminuto/diminuto_ipc6.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_time.h"

int diminuto_ping6_datagram_peer(void)
{
    int fd;

    if ((fd = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6)) >= 0) {
        /* Do nothing. */
    } else if (errno == EPERM) {
        diminuto_perror("diminuto_ping6_datagram_peer: must be root");
    } else {
        diminuto_perror("diminuto_ping6_datagram_peer: socket");
    }

    return fd;
}

typedef union {
    struct icmp6_hdr header;
    char payload[8 + 56];
} icmp6_datagram_t;

ssize_t diminuto_ping6_datagram_send(int fd, diminuto_ipv6_t address, uint16_t id, uint16_t seq)
{
    ssize_t total;
    icmp6_datagram_t buffer = { 0 };
    struct sockaddr_in6 sa = { 0 };
    struct icmp6_hdr * icmpp;
    diminuto_ticks_t now;
    int option;

    now = diminuto_time_clock();

    sa.sin6_family = AF_INET6;
    diminuto_ipc6_hton6(&address);
    memcpy(sa.sin6_addr.s6_addr, address.u16, sizeof(sa.sin6_addr.s6_addr));
    sa.sin6_port = 0;

    icmpp = (struct icmp6_hdr *)(&buffer);
    icmpp->icmp6_type = ICMP6_ECHO_REQUEST;
    icmpp->icmp6_id = id;
    icmpp->icmp6_seq = seq;
    memcpy(&buffer.payload[sizeof(buffer.header)], &now, sizeof(diminuto_ticks_t));
    option = offsetof(struct icmp6_hdr, icmp6_cksum);
    setsockopt(fd, SOL_RAW, IPV6_CHECKSUM, &option, sizeof(option));

    if ((total = sendto(fd, &buffer, sizeof(buffer), 0, (struct sockaddr *)&sa, sizeof(sa))) == 0) {
        /* Do nothing: not sure what this means. */
    } else if (total > 0) {
        /* Do nothing: nominal case. */
    } else if ((errno != EINTR) && (errno != EAGAIN) && (errno != EWOULDBLOCK)) {
        diminuto_perror("diminuto_ipc6_datagram_send_flags: sendto");
    } else {
        /* Do nothing: timeout or poll. */
    }

    return total;

}

ssize_t diminuto_ping6_datagram_recv(int fd, diminuto_ipv6_t * addressp, uint8_t * typep, uint16_t * idp, uint16_t * seqp, diminuto_ticks_t * elapsedp)
{
    ssize_t total;
    icmp6_datagram_t buffer = { 0 };
    diminuto_ipc6_sockaddr_t sa = { 0 };
    socklen_t length = sizeof(sa);
    struct icmp6_hdr * icmpp;
    diminuto_ticks_t now;
    diminuto_ticks_t then;

    if ((total = recvfrom(fd, &buffer, sizeof(buffer), 0, (struct sockaddr *)&sa, &length)) > 0) {
        diminuto_ipc6_identify((struct sockaddr *)&sa, addressp, (diminuto_port_t *)0);
        if (total < sizeof(struct icmp6_hdr)) {
            total = 0; /* Too small to be a reply. */
        } else {
            icmpp = (struct icmp6_hdr *)(&buffer);
            if (typep != (uint8_t *)0) {
                *typep = icmpp->icmp6_type;
            }
            if (icmpp->icmp6_type != ICMP6_ECHO_REPLY) {
                total = 0; /* This was not a reply. */
            } else {
                if (idp != (uint16_t *)0) {
                    *idp = icmpp->icmp6_id;
                }
                if (seqp != (uint16_t *)0) {
                    *seqp = icmpp->icmp6_seq;
                }
                if (total < sizeof(buffer)) {
                    total = 0; /* This was not our reply. */
                }
                if (elapsedp != (diminuto_ticks_t *)0) {
                    now = diminuto_time_clock();
                    memcpy(&then, &buffer.payload[sizeof(buffer.header)], sizeof(diminuto_ticks_t));
                    *elapsedp = now - then;
                }
            }
        }
    } else if (total == 0) {
        /* Not sure what this means in the context of a connectionless socket. */
    } else if ((errno != EINTR) && (errno != EAGAIN) && (errno != EWOULDBLOCK)) {
        diminuto_perror("diminuto_ping6_datagram_recv: recvfrom");
    } else {
        /* Interrupted. */
    }

    return total;
}
