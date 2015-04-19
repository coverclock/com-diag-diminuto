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

#include <string.h>
#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_ping6.h"

int main(int argc, char * argv[])
{
    int sock;
    char buffer[sizeof("XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX")];
    diminuto_ipv6_t to;
    diminuto_ipv6_t from;
    ssize_t size;
    uint16_t id;
    uint16_t seq;

    SETLOGMASK();

    ASSERT((sock = diminuto_ping6_datagram_peer()) >= 0);

    {
        to = diminuto_ipc6_address("::1");
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "to=\"%s\"\n", diminuto_ipc6_address2string(to, buffer, sizeof(buffer)));
        ASSERT(memcmp(&to, &DIMINUTO_IPC6_UNSPECIFIED, sizeof(to)) != 0);

        ASSERT(diminuto_ping6_datagram_send(sock, to, 0xdead, 1) > 0);

        memcpy(&from, &DIMINUTO_IPC6_UNSPECIFIED, sizeof(from));
        /*
         * Remarkably, the first datagram we get back when we ping ourselves
         * is our own ICMP ECHO REQUEST. The ping feature recognizes this and
         * returns a zero, indicating we didn't get an ICMP ECHO REPLY back,
         * but we did get something, and it wasn't an error. It is up to the
         * caller to decide what to do. The unit test just tries again.
         */
        ASSERT((size = diminuto_ping6_datagram_recv(sock, &from, (uint16_t *)0, (uint16_t *)0)) == 0);
        ASSERT((size = diminuto_ping6_datagram_recv(sock, &from, &id, &seq)) > 0);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "from=\"%s\" id=0x%x seq=0x%x\n", diminuto_ipc6_address2string(from, buffer, sizeof(buffer)), id, seq);
        ASSERT(memcmp(&from, &DIMINUTO_IPC6_UNSPECIFIED, sizeof(from)) != 0);
        ASSERT(id == 0xdead);
        ASSERT(seq == 1);
    }

    ASSERT(diminuto_ping6_close(sock) >= 0);

    EXIT();
}
