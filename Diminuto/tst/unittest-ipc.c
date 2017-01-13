/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2016-2017 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_ipc.h"
#include "com/diag/diminuto/diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_ipc6.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

int main(int argc, char * argv[])
{
    SETLOGMASK();

    {
        int sock;
        int rc;

        TEST();

        sock = socket(AF_INET6, SOCK_STREAM, 0);
        ASSERT(sock >= 0);

        rc = diminuto_ipc_set_nonblocking(sock, !0);
        EXPECT(rc >= 0);
        rc = diminuto_ipc_set_nonblocking(sock, 0);
        EXPECT(rc >= 0);

        rc = diminuto_ipc_set_reuseaddress(sock, !0);
        EXPECT(rc >= 0);
        rc = diminuto_ipc_set_reuseaddress(sock, 0);
        EXPECT(rc >= 0);

        rc = diminuto_ipc_set_keepalive(sock, !0);
        EXPECT(rc >= 0);
        rc = diminuto_ipc_set_keepalive(sock, 0);
        EXPECT(rc >= 0);

        if (geteuid() == 0) {
            rc = diminuto_ipc_set_debug(sock, !0);
            EXPECT(rc >= 0);
            rc = diminuto_ipc_set_debug(sock, 0);
            EXPECT(rc >= 0);
        }

        rc = diminuto_ipc_set_linger(sock, diminuto_frequency());
        EXPECT(rc >= 0);
        rc = diminuto_ipc_set_debug(sock, 0);
        EXPECT(rc >= 0);

        rc = diminuto_ipc_set_nodelay(sock, !0);
        EXPECT(rc >= 0);
        rc = diminuto_ipc_set_nodelay(sock, 0);
        EXPECT(rc >= 0);

        rc = diminuto_ipc_set_quickack(sock, !0);
        EXPECT(rc >= 0);
        rc = diminuto_ipc_set_quickack(sock, 0);
        EXPECT(rc >= 0);

        rc = diminuto_ipc_set_send(sock, 512);
        EXPECT(rc >= 0);

        rc = diminuto_ipc_set_receive(sock, 512);
        EXPECT(rc >= 0);

        rc = close(sock);
        EXPECT(rc >= 0);

        STATUS();
    }

    {
        int sock;
        int rc;
        int value;

        TEST();

        sock = socket(AF_INET6, SOCK_STREAM, 0);
        ASSERT(sock >= 0);

        value = !0;
        rc = setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &value, sizeof(value));
        if (rc < 0) { diminuto_perror("setsockopt: IPPROTO_IPV6 IPV6_V6ONLY"); }
        EXPECT(rc >= 0);

        rc = close(sock);
        EXPECT(rc >= 0);

        STATUS();
    }

    {
        int sock;
        int rc;
        int value;

        TEST();

        sock = socket(AF_INET6, SOCK_STREAM, 0);
        ASSERT(sock >= 0);

        value = !0;
        rc = setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &value, sizeof(value));
        if (rc < 0) { diminuto_perror("setsockopt: IPPROTO_IP IPV6_V6ONLY"); }
        EXPECT(rc >= 0);

        rc = close(sock);
        EXPECT(rc >= 0);

        STATUS();
    }

    {
        int sock;
        int rc;
        int value;

        TEST();

        sock = socket(AF_INET6, SOCK_STREAM, 0);
        ASSERT(sock >= 0);

        value = AF_INET;
        rc = setsockopt(sock, IPPROTO_TCP, IPV6_ADDRFORM, &value, sizeof(value));
        if (rc < 0) { diminuto_perror("setsockopt: IPPROTO_TCP IPV6_ADDRFORM"); }
        EXPECT(rc >= 0);

        rc = close(sock);
        EXPECT(rc >= 0);

        STATUS();
    }

    {
        int sock;
        int rc;
        int value;

        TEST();

        sock = socket(AF_INET6, SOCK_DGRAM, 0);
        ASSERT(sock >= 0);

        value = AF_INET;
        rc = setsockopt(sock, IPPROTO_UDP, IPV6_ADDRFORM, &value, sizeof(value));
        if (rc < 0) { diminuto_perror("setsockopt: IPPROTO_UDP IPV6_ADDRFORM"); }
        EXPECT(rc >= 0);

        rc = close(sock);
        EXPECT(rc >= 0);

        STATUS();
    }

    {
        int sock;
        int rc;

        TEST();

        sock = socket(AF_INET6, SOCK_STREAM, 0);
        ASSERT(sock >= 0);
        rc = diminuto_ipc6_set_ipv6only(sock, !0);
        EXPECT(rc >= 0);
        rc = diminuto_ipc6_set_ipv6only(sock, 0);
        EXPECT(rc >= 0);
        rc = close(sock);
        EXPECT(rc >= 0);

        STATUS();
    }

    {
        int sock;
        int rc;

        TEST();

        sock = socket(AF_INET6, SOCK_DGRAM, 0);
        ASSERT(sock >= 0);
        rc = diminuto_ipc6_set_ipv6only(sock, !0);
        EXPECT(rc >= 0);
        rc = diminuto_ipc6_set_ipv6only(sock, 0);
        EXPECT(rc >= 0);
        rc = close(sock);
        EXPECT(rc >= 0);

        STATUS();
    }

    {
        int sock;
        int rc;

        TEST();

        sock = socket(AF_INET6, SOCK_STREAM, 0);
        ASSERT(sock >= 0);
        rc = diminuto_ipc6_set_stream_ipv6toipv4(sock);
        EXPECT(rc >= 0);
        rc = close(sock);
        EXPECT(rc >= 0);

        STATUS();
    }

    {
        int sock;
        int rc;

        TEST();

        sock = socket(AF_INET6, SOCK_DGRAM, 0);
        ASSERT(sock >= 0);
        rc = diminuto_ipc6_set_datagram_ipv6toipv4(sock);
        EXPECT(rc >= 0);
        rc = close(sock);
        EXPECT(rc >= 0);

        STATUS();
    }

    EXIT();
}
