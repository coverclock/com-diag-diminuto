/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_inet.h"
#include "com/diag/diminuto/diminuto_ping.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_frequency.h"

int main(int argc, char * argv[])
{
    int sock;
    char buffer[sizeof("255.255.255.255")];
    diminuto_ipv4_t to;
    diminuto_ipv4_t from;
    ssize_t size;
    uint16_t id;
    uint16_t seq;
    uint8_t ttl;
    uint8_t type;
    diminuto_ticks_t elapsed;

    SETLOGMASK();

    {
        uint8_t buffer[] = { 0x11, 0x11, 0x22, 0x22, 0x44, 0x44, 0x88, 0x88 };
        uint16_t checksum;
        checksum = diminuto_inet_checksum(buffer, sizeof(buffer));
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "checksum=0x%4.4x\n", checksum);
        ASSERT(checksum == 0x0000);
    }

    {
        uint8_t buffer[] = { 0x11, 0x11, 0x22, 0x22, 0x44, 0x44, 0x88, 0x88, 0x11 };
        uint16_t checksum;
        checksum = diminuto_inet_checksum(buffer, sizeof(buffer));
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "checksum=0x%4.4x\n", checksum);
        ASSERT(checksum == 0xffee);
    }

    {
        uint8_t buffer[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
        uint16_t checksum;
        checksum = diminuto_inet_checksum(buffer, sizeof(buffer));
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "checksum=0x%4.4x\n", checksum);
        ASSERT(checksum == 0xff00);
    }

    {
        uint8_t buffer[] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };
        uint16_t checksum;
        checksum = diminuto_inet_checksum(buffer, sizeof(buffer));
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "checksum=0x%4.4x\n", checksum);
        ASSERT(checksum == 0xda61);
    }

    ASSERT((sock = diminuto_ping_datagram_peer()) >= 0);

    {
        to = diminuto_ipc_address("google.com");
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "to=\"%s\"\n", diminuto_ipc_address2string(to, buffer, sizeof(buffer)));
        ASSERT(to != 0);

        ASSERT(diminuto_ping_datagram_send(sock, to, 0xcafe, 1) > 0);

        from = 0;
        type = ~0;
        id = 0;
        seq = 0;
        elapsed = 0;
        ASSERT((size = diminuto_ping_datagram_recv(sock, &from, &type, &id, &seq, &ttl, &elapsed)) > 0);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "from=\"%s\" size=%zu type=0x%x id=0x%x seq=%u ttl=%u elapsed=%lldticks\n", diminuto_ipc_address2string(from, buffer, sizeof(buffer)), size, type, id, seq, ttl, elapsed);
        ASSERT(from != 0);
        ASSERT(type != ~0);
        ASSERT(id == 0xcafe);
        ASSERT(seq == 1);
        ASSERT(elapsed > 0);
    }

    {
        to = diminuto_ipc_address("diag.com");
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "to=\"%s\"\n", diminuto_ipc_address2string(to, buffer, sizeof(buffer)));
        ASSERT(to != 0);

        ASSERT(diminuto_ping_datagram_send(sock, to, 0xbabe, 2) > 0);

        from = 0;
        type = ~0;
        id = 0;
        seq = 0;
        elapsed = 0;
        ASSERT((size = diminuto_ping_datagram_recv(sock, &from, &type, &id, &seq, &ttl, &elapsed)) > 0);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "from=\"%s\" size=%zu type=0x%x id=0x%x seq=%u ttl=%u elapsed=%lldticks\n", diminuto_ipc_address2string(from, buffer, sizeof(buffer)), size, type, id, seq, ttl, elapsed);
        ASSERT(from != 0);
        ASSERT(type != ~0);
        ASSERT(id == 0xbabe);
        ASSERT(seq == 2);
        ASSERT(elapsed > 0);
    }

    {
        to = diminuto_ipc_address("localhost");
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "to=\"%s\"\n", diminuto_ipc_address2string(to, buffer, sizeof(buffer)));
        ASSERT(to != 0);

        ASSERT(diminuto_ping_datagram_send(sock, to, 0xbeef, 3) > 0);

        from = 0;
        type = ~0;
        id = 0;
        seq = 0;
        elapsed = 0;
        /*
         * Remarkably, the first datagram we get back when we ping ourselves
         * is our own ICMP ECHO REQUEST. The ping feature recognizes this and
         * returns a zero, indicating we didn't get an ICMP ECHO REPLY back,
         * but we did get something, and it wasn't an error. It is up to the
         * caller to decide what to do. The unit test just tries again.
         */
        ASSERT((size = diminuto_ping_datagram_recv(sock, &from, (uint8_t *)0, (uint16_t *)0, (uint16_t *)0, (uint8_t *)0, (diminuto_ticks_t *)0)) == 0);
        ASSERT((size = diminuto_ping_datagram_recv(sock, &from, &type, &id, &seq, &ttl, &elapsed)) > 0);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "from=\"%s\" size=%zu type=0x%x id=0x%x seq=%u ttl=%u elapsed=%lldticks\n", diminuto_ipc_address2string(from, buffer, sizeof(buffer)), size, type, id, seq, ttl, elapsed);
        ASSERT(from != 0);
        ASSERT(type != ~0);
        ASSERT(elapsed > 0);
        ASSERT(seq == 3);
        ASSERT(elapsed > 0);
    }

    {
        diminuto_ticks_t delay;
        uint16_t ss;

        delay = diminuto_frequency();

        to = diminuto_ipc_address("youtube.com");
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "to=\"%s\"\n", diminuto_ipc_address2string(to, buffer, sizeof(buffer)));
        ASSERT(to != 0);

        for (ss = 0; ss < 10; ++ss) {
            ASSERT(diminuto_ping_datagram_send(sock, to, 0xdead, ss) > 0);
            from = 0;
            type = ~0;
            id = 0;
            seq = 0;
            elapsed = 0;
            ASSERT((size = diminuto_ping_datagram_recv(sock, &from, &type, &id, &seq, &ttl, &elapsed)) > 0);
            DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "from=\"%s\" size=%zu type=0x%x id=0x%x seq=%u ttl=%u elapsed=%lldticks\n", diminuto_ipc_address2string(from, buffer, sizeof(buffer)), size, type, id, seq, ttl, elapsed);
            ASSERT(from != 0);
            ASSERT(type != ~0);
            ASSERT(id == 0xdead);
            ASSERT(seq == ss);
            ASSERT(elapsed > 0);
            diminuto_delay(delay, 0);
        }
    }
    ASSERT(diminuto_ping_close(sock) >= 0);

    EXIT();
}
