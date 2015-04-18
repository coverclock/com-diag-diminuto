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
#include "com/diag/diminuto/diminuto_ping.h"

int main(int argc, char * argv[])
{
    int sock;
    char buffer[sizeof("255.255.255.255")];
    diminuto_ipv4_t to;
    diminuto_ipv4_t from;
    ssize_t size;

    SETLOGMASK();

    ASSERT((sock = diminuto_ping_datagram_peer()) >= 0);

    {
        to = diminuto_ipc_address("google.com");
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "to=\"%s\"\n", diminuto_ipc_address2string(to, buffer, sizeof(buffer)));
        ASSERT(to != 0);

        ASSERT(diminuto_ping_datagram_send(sock, to) > 0);

        from = 0;
        ASSERT((size = diminuto_ping_datagram_recv(sock, &from)) > 0);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "from=\"%s\"\n", diminuto_ipc_address2string(from, buffer, sizeof(buffer)));
        ASSERT(from != 0);
    }

    {
        to = diminuto_ipc_address("diag.com");
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "to=\"%s\"\n", diminuto_ipc_address2string(to, buffer, sizeof(buffer)));
        ASSERT(to != 0);

        ASSERT(diminuto_ping_datagram_send(sock, to) > 0);

        from = 0;
        ASSERT((size = diminuto_ping_datagram_recv(sock, &from)) > 0);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "from=\"%s\"\n", diminuto_ipc_address2string(from, buffer, sizeof(buffer)));
        ASSERT(from != 0);
    }

    ASSERT(diminuto_ping_close(sock) >= 0);

    EXIT();
}
