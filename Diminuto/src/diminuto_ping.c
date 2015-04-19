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
#include "com/diag/diminuto/diminuto_memberof.h"
#include "com/diag/diminuto/diminuto_time.h"

#define DATA_LENGTH 56

uint16_t diminuto_inet_checksum(void * buffer, size_t size)
{
    uint16_t datum;
    uint32_t accumulator = 0;
    uint8_t * here;

    here = (uint8_t *)buffer;

    while (size > 1) {
        datum = *(here++);
        datum = (datum << widthof(uint8_t)) | *(here++);
        accumulator += htons(datum);
        size -= sizeof(uint16_t);
    }

    if (size > 0) {
        datum = *(here++);
        datum = (datum << widthof(uint8_t)) /* | 0 */;
        accumulator += htons(datum);
    }

    datum = accumulator >> widthof(uint16_t);
    accumulator &= ~(uint16_t)0;
    accumulator += datum;

    datum = accumulator >> widthof(uint16_t);
    accumulator &= ~(uint16_t)0;
    accumulator += datum;

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

ssize_t diminuto_ping_datagram_send(int fd, diminuto_ipv4_t address, uint16_t id, uint16_t seq)
{
    ssize_t total;
    struct { char payload[sizeof(struct icmp) - sizeof(memberof(struct icmp, icmp_data)) + sizeof(diminuto_ticks_t)]; } buffer = { 0 };
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
    //memcpy(icmpp->icmp_data, &now, sizeof(diminuto_ticks_t));
    icmpp->icmp_cksum = diminuto_inet_checksum(&buffer, sizeof(buffer));

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

ssize_t diminuto_ping_datagram_recv(int fd, diminuto_ipv4_t * addressp, uint16_t * idp, uint16_t * seqp)
{
    ssize_t total;
    struct { char payload[sizeof(struct iphdr) + sizeof(struct icmp) - sizeof(memberof(struct icmp, icmp_data)) + sizeof(diminuto_ticks_t)]; } buffer = { 0 };
    struct sockaddr_in sa = { 0 };
    socklen_t length = sizeof(sa);
    struct iphdr * ipp;
    struct icmp * icmpp;
    diminuto_ticks_t now;
    diminuto_ticks_t then;

    while (!0) {
        if ((total = recvfrom(fd, &buffer, sizeof(buffer), 0, (struct sockaddr *)&sa, &length)) >= 0) {
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
                    diminuto_ipc_identify((struct sockaddr *)&sa, addressp, (diminuto_port_t *)0);
                    if (idp != (uint16_t *)0) { *idp = icmpp->icmp_id; }
                    if (seqp != (uint16_t *)0) { *seqp = icmpp->icmp_seq; }
                    now = diminuto_time_clock();
                    //memcpy(&then, icmpp->icmp_data, sizeof(diminuto_ticks_t));
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
