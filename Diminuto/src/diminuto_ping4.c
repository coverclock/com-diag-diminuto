/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015-2016 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include "diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_ping.h"
#include "com/diag/diminuto/diminuto_inet.h"
#include "com/diag/diminuto/diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_widthof.h"
#include "com/diag/diminuto/diminuto_offsetof.h"
#include "com/diag/diminuto/diminuto_memberof.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_dump.h"

int diminuto_ping4_debug = 0; /* Not part of the public API. */

typedef union {
    uint16_t word;
    uint8_t byte[sizeof(uint16_t)];
} inet_checksum_t;

uint16_t diminuto_inet_checksum(void * buffer, size_t size)
{
    inet_checksum_t datum;
    uint32_t accumulator = 0;
    uint8_t * here;

    here = (uint8_t *)buffer;

    while (size > 1) {
        datum.byte[0] = *(here++);
        datum.byte[1] = *(here++);
        accumulator += datum.word;
        size -= sizeof(uint16_t);
    }

    if (size > 0) {
        datum.byte[0] = *(here++);
        datum.byte[1] = 0;
        accumulator += datum.word;
    }

    while ((datum.word = accumulator >> widthof(uint16_t))) {
        accumulator = (accumulator & (uint16_t)~0) + datum.word;
    }

    return ~accumulator;
}

int diminuto_ping4_datagram_peer(void)
{
    int fd;

    if ((fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) >= 0) {
        /* Do nothing. */
    } else if (errno == EPERM) {
        diminuto_perror("diminuto_ping4_datagram_peer: must be root");
    } else {
        diminuto_perror("diminuto_ping4_datagram_peer: socket");
    }

    return fd;
}

int diminuto_ping4_interface(int fd, const char * ifname)
{
    int rc;
    struct ifreq iface = { 0 };

    strncpy(iface.ifr_name, ifname, sizeof(iface.ifr_name));
    iface.ifr_ifrn.ifrn_name[sizeof(iface.ifr_name) - 1] = '\0';

    if ((rc = setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, &iface, sizeof(iface))) < 0) {
        diminuto_perror("diminuto_ping4_interface: setsockopt");
    }

    return rc;
}

int diminuto_ping4_address(int fd, diminuto_ipv4_t address, diminuto_port_t port)
{
    int rc;
    struct sockaddr_in sa = { 0 };

    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(address);
    sa.sin_port = htons(port);

    if ((rc = bind(fd, &sa, sizeof(sa))) < 0) {
        diminuto_perror("diminuto_ping4_address: bind");
    }

    return rc;
}

typedef union {
    struct {
        struct icmp icmp;
    } header;
    char payload[8 + 56];
} icmp_echo_request_datagram_t;

ssize_t diminuto_ping4_datagram_send(int fd, diminuto_ipv4_t address, uint16_t id, uint16_t seq)
{
    ssize_t total;
    icmp_echo_request_datagram_t buffer = { 0 };
    struct sockaddr_in sa = { 0 };
    struct icmp * icmpp;
    diminuto_ticks_t now;

    now = diminuto_time_clock();

    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(address);

    icmpp = (struct icmp *)(&buffer);
    icmpp->icmp_type = ICMP_ECHO;
    icmpp->icmp_id = id;
    icmpp->icmp_seq = seq;
    memcpy(icmpp->icmp_data, &now, sizeof(diminuto_ticks_t));
    icmpp->icmp_cksum = diminuto_inet_checksum(&buffer, sizeof(buffer));

    if (diminuto_ping4_debug) { diminuto_dump(stderr, &buffer, sizeof(buffer)); }

    if ((total = sendto(fd, &buffer, sizeof(buffer), 0, (struct sockaddr *)&sa, sizeof(sa))) == 0) {
        /* Do nothing: not sure what this means. */
    } else if (total > 0) {
        /* Do nothing: nominal case. */
    } else if ((errno != EINTR) && (errno != EAGAIN) && (errno != EWOULDBLOCK)) {
        diminuto_perror("diminuto_ipc4_datagram_send_flags: sendto");
    } else {
        /* Do nothing: timeout or poll. */
    }

    return total;

}

typedef union {
    struct {
        struct iphdr ip;
        struct icmp icmp;
    } header;
    char payload[20 + 8 + 56];
} icmp_echo_reply_datagram_t;

ssize_t diminuto_ping4_datagram_recv(int fd, diminuto_ipv4_t * addressp, uint8_t * typep, uint16_t * idp, uint16_t * seqp, uint8_t * ttlp, diminuto_ticks_t * elapsedp)
{
    ssize_t total;
    icmp_echo_reply_datagram_t buffer = { 0 };
    struct sockaddr_in sa = { 0 };
    socklen_t length = sizeof(sa);
    struct iphdr * ipp;
    struct icmp * icmpp;
    diminuto_ticks_t now;
    diminuto_ticks_t then;

    if ((total = recvfrom(fd, &buffer, sizeof(buffer), 0, (struct sockaddr *)&sa, &length)) > 0) {
        if (diminuto_ping4_debug) { diminuto_dump(stderr, &buffer, total); }
        diminuto_ipc4_identify((struct sockaddr *)&sa, addressp, (diminuto_port_t *)0);
        if (total < (sizeof(struct iphdr) + ICMP_MINLEN)) {
            total = 0; /* Too small to be an ICMP datagram. */
        } else {
            ipp = (struct iphdr *)(&buffer);
            icmpp = (struct icmp *)((&(buffer.payload[0])) + (ipp->ihl << 2));
            if (diminuto_inet_checksum(icmpp, total - ((char *)icmpp - (char *)ipp)) != 0) {
                total = 0; /* Should never happen. */
            } else {
                if (typep != (uint8_t *)0) {
                    *typep = icmpp->icmp_type;
                }
                if (icmpp->icmp_type != ICMP_ECHOREPLY) {
                    total = 0; /* This was not an echo reply. */
                } else {
                    if (idp != (uint16_t *)0) {
                        *idp = icmpp->icmp_id;
                    }
                    if (seqp != (uint16_t *)0) {
                        *seqp = icmpp->icmp_seq;
                    }
                    if (ttlp != (uint8_t *)0) {
                        *ttlp = ipp->ttl;
                    }
                    if (total < sizeof(buffer)) {
                        total = 0; /* This was not our reply. */
                    } else {
                        if (elapsedp != (diminuto_ticks_t *)0) {
                            now = diminuto_time_clock();
                            memcpy(&then, icmpp->icmp_data, sizeof(diminuto_ticks_t));
                            *elapsedp = now - then;
                        }
                        total -= sizeof(struct iphdr); /* Nominal. */
                    }
                }
            }
        }
    } else if (total == 0) {
        /* Not sure what this means for a connectionless socket. */
    } else if ((errno != EINTR) && (errno != EAGAIN) && (errno != EWOULDBLOCK)) {
        diminuto_perror("diminuto_ping4_datagram_recv: recvfrom");
    } else {
        /* Interrupted. */
    }

    return total;
}
