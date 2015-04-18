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
#include "com/diag/diminuto/diminuto_endianess.h"
#include "com/diag/diminuto/diminuto_widthof.h"

#define DATA_LENGTH 56

static const int IPV6_ICMP = 58;

int diminuto_ping6_datagram_peer(void)
{
    int fd;

    if ((fd = socket(AF_INET6, SOCK_RAW, IPV6_ICMP)) >= 0) {
        /* Do nothing. */
    } else if (errno == EPERM) {
        diminuto_perror("diminuto_ping6_datagram_peer: must be root");
    } else {
        diminuto_perror("diminuto_ping6_datagram_peer: socket");
    }

    return fd;
}

ssize_t diminuto_ping6_datagram_send(int fd, diminuto_ipv6_t address)
{
    ssize_t total;
    union { char payload[sizeof(struct icmp6_hdr) + DATA_LENGTH]; uint16_t alignment[0]; } buffer = { { 0 } };
    struct sockaddr_in6 sa = { 0 };
    struct icmp6_hdr * icmpp;
    int option;

    sa.sin6_family = AF_INET6;
    diminuto_ipc6_hton6(&address);
    memcpy(sa.sin6_addr.s6_addr, address.u16, sizeof(sa.sin6_addr.s6_addr));
    sa.sin6_port = 0;

    icmpp = (struct icmp6_hdr *)(&buffer);
    icmpp->icmp6_type = ICMP6_ECHO_REQUEST;
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

ssize_t diminuto_ping6_datagram_recv(int fd, diminuto_ipv6_t * addressp)
{
    ssize_t total;
    union { char payload[sizeof(struct icmp6_hdr) + DATA_LENGTH]; uint16_t alignment[0]; } buffer = { { 0 } };
    diminuto_ipc6_sockaddr_t sa = { 0 };
    socklen_t length = sizeof(sa);
    struct icmp6_hdr * icmpp;

    while (!0) {
        if ((total = recvfrom(fd, &buffer, sizeof(buffer), 0, (struct sockaddr *)&sa, &length)) >= 0) {
            diminuto_ipc6_identify((struct sockaddr *)&sa, addressp, (diminuto_port_t *)0);
            break;
        } else if ((errno != EINTR) && (errno != EAGAIN) && (errno != EWOULDBLOCK)) {
            diminuto_perror("diminuto_ping6_datagram_recv: recvfrom");
        } else {
            continue;
        }
    }

    if (total < sizeof(struct icmp6_hdr)) {
        total = 0; /* Too small to be a reply. */
    } else {
        icmpp = (struct icmp6_hdr *)(&buffer);
        if (icmpp->icmp6_type != ICMP6_ECHO_REPLY) {
            total = 0; /* This was not the reply we wanted. */
        }
    }

    return total;
}
