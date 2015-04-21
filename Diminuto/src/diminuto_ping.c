/* vi: set ts=4 expandtab shiftwidth=4: */
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

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include "diminuto_ping.h"
#include "diminuto_ipc.h"
#include "com/diag/diminuto/diminuto_ping.h"
#include "com/diag/diminuto/diminuto_ipc.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_widthof.h"
#include "com/diag/diminuto/diminuto_offsetof.h"
#include "com/diag/diminuto/diminuto_memberof.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_dump.h"

int diminuto_ping_debug = 0; /* Not part of the public API. */

uint16_t diminuto_inet_checksum(void * buffer, size_t size)
{
    uint16_t datum;
    uint32_t accumulator = 0;
    uint8_t * here;

    here = (uint8_t *)buffer;

    while (size > 1) {
        datum = *(here++);
        datum = (datum << widthof(uint8_t)) | *(here++);
        accumulator += datum;
        size -= sizeof(uint16_t);
    }

    if (size > 0) {
        datum = *(here++);
        accumulator += datum;
    }

    while ((datum = accumulator >> widthof(uint16_t))) {
        accumulator = (accumulator & (uint16_t)~0) + datum;
    }

    datum = ~accumulator;

    return datum;
}

int diminuto_ping_datagram_peer(void)
{
    int fd;

    if ((fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) >= 0) {
        /* Do nothing. */
    } else if (errno == EPERM) {
        diminuto_perror("diminuto_ping_datagram_peer: must be root");
    } else {
        diminuto_perror("diminuto_ping_datagram_peer: socket");
    }

    return fd;
}

typedef union {
    struct {
        struct icmp icmp;
    } header;
    char payload[8 + 56];
} icmp_echo_request_datagram_t;

ssize_t diminuto_ping_datagram_send(int fd, diminuto_ipv4_t address, uint16_t id, uint16_t seq)
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
    icmpp->icmp_id = htons(id);
    icmpp->icmp_seq = htons(seq);
    memcpy(icmpp->icmp_data, &now, sizeof(diminuto_ticks_t));
    icmpp->icmp_cksum = htons(diminuto_inet_checksum(&buffer, sizeof(buffer)));

    if (diminuto_ping_debug) { diminuto_dump(stderr, &buffer, sizeof(buffer)); }

    if ((total = sendto(fd, &buffer, sizeof(buffer), 0, (struct sockaddr *)&sa, sizeof(sa))) == 0) {
        /* Do nothing: not sure what this means. */
    } else if (total > 0) {
        /* Do nothing: nominal case. */
    } else if ((errno != EINTR) && (errno != EAGAIN) && (errno != EWOULDBLOCK)) {
        diminuto_perror("diminuto_ipc_datagram_send_flags: sendto");
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

ssize_t diminuto_ping_datagram_recv(int fd, diminuto_ipv4_t * addressp, uint16_t * idp, uint16_t * seqp, diminuto_ticks_t * elapsedp)
{
    ssize_t total;
    icmp_echo_reply_datagram_t buffer = { 0 };
    struct sockaddr_in sa = { 0 };
    socklen_t length = sizeof(sa);
    struct iphdr * ipp;
    struct icmp * icmpp;
    uint16_t checksum = 0;
    diminuto_ticks_t now;
    diminuto_ticks_t then;

    while (!0) {
        if ((total = recvfrom(fd, &buffer, sizeof(buffer), 0, (struct sockaddr *)&sa, &length)) >= 0) {
            if (diminuto_ping_debug) { diminuto_dump(stderr, &buffer, total); }
            if (total < sizeof(buffer)) {
                total = 0;
                break; /* Too small to be a legitimate reply. */
            } else {
                ipp = (struct iphdr *)(&buffer);
                icmpp = (struct icmp *)((&(buffer.payload[0])) + (ipp->ihl << 2));
                if (icmpp->icmp_type != ICMP_ECHOREPLY) {
                    total = 0;
                    break; /* This was not the response we wanted. */
                } else {
                    checksum = htons(diminuto_inet_checksum(icmpp, total - ((char *)icmpp - (char *)ipp)));
                    /* The checksum will be zero if everything is correct. */
                    diminuto_ipc_identify((struct sockaddr *)&sa, addressp, (diminuto_port_t *)0);
                    if (idp != (uint16_t *)0) { *idp = ntohs(icmpp->icmp_id); }
                    if (seqp != (uint16_t *)0) { *seqp = ntohs(icmpp->icmp_seq); }
                    if (elapsedp != (diminuto_ticks_t *)0) {
                        now = diminuto_time_clock();
                        memcpy(&then, icmpp->icmp_data, sizeof(diminuto_ticks_t));
                        *elapsedp = now - then;
                    }
                    break; /* Nominal. */
                }
            }
            break;
        } else if ((errno != EINTR) && (errno != EAGAIN) && (errno != EWOULDBLOCK)) {
            diminuto_perror("diminuto_ping_datagram_recv: recvfrom");
            break; /* Error! */
        } else {
            continue; /* Interrupted; try again. */
        }
    }

    return total;
}
