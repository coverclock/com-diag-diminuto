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

    SETLOGMASK();

    ASSERT((sock = diminuto_ping6_datagram_peer()) >= 0);

    {
        to = diminuto_ipc6_address("google.com");
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "to=\"%s\"\n", diminuto_ipc6_address2string(to, buffer, sizeof(buffer)));
        ASSERT(memcmp(&to, &DIMINUTO_IPC6_UNSPECIFIED, sizeof(to)) != 0);

        ASSERT(diminuto_ping6_datagram_send(sock, to) > 0);

        memcpy(&from, &DIMINUTO_IPC6_UNSPECIFIED, sizeof(from));
        ASSERT((size = diminuto_ping6_datagram_recv(sock, &from)) > 0);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "from=\"%s\"\n", diminuto_ipc6_address2string(from, buffer, sizeof(buffer)));
        ASSERT(memcmp(&from, &DIMINUTO_IPC6_UNSPECIFIED, sizeof(from)) != 0);
    }

    {
        to = diminuto_ipc6_address("diag.com");
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "to=\"%s\"\n", diminuto_ipc6_address2string(to, buffer, sizeof(buffer)));
        ASSERT(memcmp(&to, &DIMINUTO_IPC6_UNSPECIFIED, sizeof(to)) != 0);

        ASSERT(diminuto_ping6_datagram_send(sock, to) > 0);

        memcpy(&from, &DIMINUTO_IPC6_UNSPECIFIED, sizeof(from));
        ASSERT((size = diminuto_ping6_datagram_recv(sock, &from)) > 0);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "from=\"%s\"\n", diminuto_ipc6_address2string(from, buffer, sizeof(buffer)));
        ASSERT(memcmp(&from, &DIMINUTO_IPC6_UNSPECIFIED, sizeof(from)) != 0);
    }

    {
        to = diminuto_ipc6_address("::1");
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "to=\"%s\"\n", diminuto_ipc6_address2string(to, buffer, sizeof(buffer)));
        ASSERT(memcmp(&to, &DIMINUTO_IPC6_UNSPECIFIED, sizeof(to)) != 0);

        ASSERT(diminuto_ping6_datagram_send(sock, to) > 0);

        memcpy(&from, &DIMINUTO_IPC6_UNSPECIFIED, sizeof(from));
        ASSERT((size = diminuto_ping6_datagram_recv(sock, &from)) > 0);
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "from=\"%s\"\n", diminuto_ipc6_address2string(from, buffer, sizeof(buffer)));
        ASSERT(memcmp(&from, &DIMINUTO_IPC6_UNSPECIFIED, sizeof(from)) != 0);
    }

    ASSERT(diminuto_ping6_close(sock) >= 0);

    EXIT();
}
