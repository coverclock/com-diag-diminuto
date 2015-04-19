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

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_ping.h"

int main(int argc, char * argv[])
{
    int sock;
    char buffer[sizeof("255.255.255.255")];
    diminuto_ipv4_t to;
    diminuto_ipv4_t from;
    ssize_t size;
    uint16_t id;
    uint16_t seq;

    SETLOGMASK();

    ASSERT((sock = diminuto_ping_datagram_peer()) >= 0);

    {
        to = diminuto_ipc_address("google.com");
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "to=\"%s\"\n", diminuto_ipc_address2string(to, buffer, sizeof(buffer)));
        ASSERT(to != 0);

        ASSERT(diminuto_ping_datagram_send(sock, to, 0xcafe, 1) > 0);

        from = 0;
        id = 0;
        seq = 0;
        ASSERT((size = diminuto_ping_datagram_recv(sock, &from, &id, &seq)) > 0);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "from=\"%s\" id=0x%x seq=0x%x\n", diminuto_ipc_address2string(from, buffer, sizeof(buffer)), id, seq);
        ASSERT(from != 0);
        ASSERT(id == 0xcafe);
        ASSERT(seq == 1);
    }

    {
        to = diminuto_ipc_address("diag.com");
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "to=\"%s\"\n", diminuto_ipc_address2string(to, buffer, sizeof(buffer)));
        ASSERT(to != 0);

        ASSERT(diminuto_ping_datagram_send(sock, to, 0xbabe, 2) > 0);

        from = 0;
        id = 0;
        seq = 0;
        ASSERT((size = diminuto_ping_datagram_recv(sock, &from, &id, &seq)) > 0);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "from=\"%s\" id=0x%x seq=0x%x\n", diminuto_ipc_address2string(from, buffer, sizeof(buffer)), id, seq);
        ASSERT(from != 0);
        ASSERT(id == 0xbabe);
        ASSERT(seq == 2);
    }

    {
        to = diminuto_ipc_address("localhost");
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "to=\"%s\"\n", diminuto_ipc_address2string(to, buffer, sizeof(buffer)));
        ASSERT(to != 0);

        ASSERT(diminuto_ping_datagram_send(sock, to, 0xbeef, 3) > 0);

        from = 0;
        id = 0;
        seq = 0;
        /*
         * Remarkably, the first datagram we get back when we ping ourselves
         * is our own ICMP ECHO REQUEST. The ping feature recognizes this and
         * returns a zero, indicating we didn't get an ICMP ECHO REPLY back,
         * but we did get something, and it wasn't an error. It is up to the
         * caller to decide what to do. The unit test just tries again.
         */
        ASSERT((size = diminuto_ping_datagram_recv(sock, &from, (uint16_t *)0, (uint16_t *)0)) == 0);
        ASSERT((size = diminuto_ping_datagram_recv(sock, &from, &id, &seq)) > 0);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "from=\"%s\" id=0x%x seq=0x%x\n", diminuto_ipc_address2string(from, buffer, sizeof(buffer)), id, seq);
        ASSERT(from != 0);
    }

    ASSERT(diminuto_ping_close(sock) >= 0);

    EXIT();
}
