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

#define DATA_LENGTH 56

uint16_t diminuto_inet_checksum(void * payload, size_t size)
{
    uint16_t datum;
    uint32_t accumulator = 0;
    uint8_t * here;

    here = (uint8_t *)payload;

    while (size > 1) {
        datum = *(here++);
        datum = (datum << widthof(uint8_t)) | *(here++);
        accumulator += htons(datum);
        size -= sizeof(uint16_t);
    }

    if (size > 0) {
        datum = *(here++);
        datum <<= widthof(uint8_t);
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

ssize_t diminuto_ping_datagram_send(int fd, diminuto_ipv4_t address)
{
    ssize_t total;
    union { char payload[sizeof(struct icmp) + DATA_LENGTH - 1]; uint16_t alignment[0]; } buffer = { { 0 } };
    struct sockaddr_in sa = { 0 };
    struct icmp * icmpp;

    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = htonl(address);

    icmpp = (struct icmp *)(&buffer);
    icmpp->icmp_type = ICMP_ECHO;
    icmpp->icmp_cksum = diminuto_inet_checksum(&(buffer.alignment[0]), sizeof(buffer));

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

ssize_t diminuto_ping_datagram_recv(int fd, diminuto_ipv4_t * addressp)
{
    ssize_t total;
    union { char payload[sizeof(struct iphdr) + sizeof(struct icmp) + DATA_LENGTH]; uint16_t alignment[0]; } buffer = { { 0 } };
    struct sockaddr_in sa = { 0 };
    socklen_t length = sizeof(sa);
    struct iphdr * ipp;
    struct icmp * icmpp;

    while (!0) {
        if ((total = recvfrom(fd, &buffer, sizeof(buffer), 0, (struct sockaddr *)&sa, &length)) >= 0) {
            diminuto_ipc_identify((struct sockaddr *)&sa, addressp, (diminuto_port_t *)0);
            if (total < (sizeof(struct ip) + sizeof(struct icmp))) {
                total = 0;
                break; /* Too small to be a legitimate reply. */
            } else {
                ipp = (struct iphdr *)(&buffer);
                icmpp = (struct icmp *)((&(buffer.payload[0])) + (ipp->ihl << 2));
                if (icmpp->icmp_type == ICMP_ECHO) {
                    total = 0;
                    break; /* This is likely to be our own ICMP ECHO REQUEST to localhost. */
                } else if (icmpp->icmp_type == ICMP_ECHOREPLY) {
                    break; /* Nominal. */
                } else {
                    total = 0;
                    break; /* This was not the response we wanted. */
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
