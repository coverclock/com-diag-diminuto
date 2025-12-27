/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2016-2025 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the IPC feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the IPC feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_ipc.h"
#include "com/diag/diminuto/diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_minmaxof.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include "../src/diminuto_ipc.h"

int main(int argc, char * argv[])
{
    SETLOGMASK();

    {
        diminuto_ipc_type_t type = DIMINUTO_IPC_TYPE_UNSPECIFIED;

        TEST();

        switch (type) {
        case DIMINUTO_IPC_TYPE_UNSPECIFIED: break;
        case DIMINUTO_IPC_TYPE_IPV4:        break;
        case DIMINUTO_IPC_TYPE_IPV6:        break;
        case DIMINUTO_IPC_TYPE_LOCAL:       break;
        }

        STATUS();
    }

    {
        diminuto_ipc_preference_t preference = DIMINUTO_IPC_PREFERENCE_NONE;

        TEST();

        switch (preference) {
        case DIMINUTO_IPC_PREFERENCE_IPV4:  break;
        case DIMINUTO_IPC_PREFERENCE_IPV6:  break;
        case DIMINUTO_IPC_PREFERENCE_NONE:  break;
        }

        STATUS();
    }

    {
        int sock;
        int rc;

        TEST();

        sock = socket(AF_INET, SOCK_STREAM, 0);
        ASSERT(sock >= 0);

        rc = diminuto_ipc_set_nonblocking(sock, !0);
        EXPECT(rc >= 0);
        rc = diminuto_ipc_set_nonblocking(sock, 0);
        EXPECT(rc >= 0);

        rc = diminuto_ipc_set_reuseaddress(sock, !0);
        EXPECT(rc >= 0);
        rc = diminuto_ipc_set_reuseaddress(sock, 0);
        EXPECT(rc >= 0);

        rc = diminuto_ipc_set_reuseport(sock, !0);
        EXPECT(rc >= 0);
        rc = diminuto_ipc_set_reuseport(sock, 0);
        EXPECT(rc >= 0);

        rc = diminuto_ipc_set_keepalive(sock, !0);
        EXPECT(rc >= 0);
        rc = diminuto_ipc_set_keepalive(sock, 0);
        EXPECT(rc >= 0);

        rc = diminuto_ipc_set_timestamp(sock, !0);
        EXPECT(rc >= 0);
        rc = diminuto_ipc_set_timestamp(sock, 0);
        EXPECT(rc >= 0);

        if (geteuid() == 0) {
            rc = diminuto_ipc_set_debug(sock, !0);
            EXPECT(rc >= 0);
            rc = diminuto_ipc_set_debug(sock, 0);
            EXPECT(rc >= 0);
        }

        {
            diminuto_ipc_debug = !0;
        }

        rc = diminuto_ipc_set_linger(sock, diminuto_maximumof(diminuto_ticks_t));
        EXPECT(rc >= 0);
        rc = diminuto_ipc_set_linger(sock, diminuto_maximumof(diminuto_ticks_t) - 1);
        EXPECT(rc >= 0);
        rc = diminuto_ipc_set_linger(sock, diminuto_maximumof(diminuto_ticks_t) - diminuto_frequency() + 1);
        EXPECT(rc >= 0);
        rc = diminuto_ipc_set_linger(sock, diminuto_maximumof(diminuto_ticks_t) - diminuto_frequency());
        EXPECT(rc >= 0);
        rc = diminuto_ipc_set_linger(sock, diminuto_maximumof(diminuto_ticks_t) - diminuto_frequency() - 1);
        EXPECT(rc >= 0);
        rc = diminuto_ipc_set_linger(sock, ((diminuto_frequency() * diminuto_maximumof(int)) + 1) - diminuto_frequency());
        EXPECT(rc >= 0);
        rc = diminuto_ipc_set_linger(sock, ((diminuto_frequency() * diminuto_maximumof(int)) + 1) - diminuto_frequency() - 1);
        EXPECT(rc >= 0);
        rc = diminuto_ipc_set_linger(sock, diminuto_frequency() * 10);
        EXPECT(rc >= 0);
        rc = diminuto_ipc_set_linger(sock, diminuto_frequency());
        EXPECT(rc >= 0);
        rc = diminuto_ipc_set_linger(sock, 1);
        EXPECT(rc >= 0);
        rc = diminuto_ipc_set_linger(sock, 0);
        EXPECT(rc >= 0);

        {
            diminuto_ipc_debug = 0;
        }

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

        rc = diminuto_ipc_stream_get_available(sock);
        EXPECT(rc == 0);

        rc = diminuto_ipc_stream_get_pending(sock);
        EXPECT(rc == 0);

        rc = close(sock);
        EXPECT(rc >= 0);

        STATUS();
    }

    {
        diminuto_port_t port;
        diminuto_port_buffer_t buffer;
        const char * pointer;
        unsigned long number;
        char * end;

        TEST();

        port = diminuto_minimumof(diminuto_port_t);
        EXPECT(port == 0);
        pointer = diminuto_ipc_port2string(port, buffer, sizeof(buffer));
        EXPECT(pointer == &(buffer[0]));
        number = strtoul(buffer, &end, 10);
        EXPECT(end != (const char *)0);
        EXPECT(*end == '\0');
        EXPECT(number == port);

        STATUS();
    }

    {
        diminuto_port_t port;
        diminuto_port_buffer_t buffer;
        const char * pointer;
        unsigned long number;
        char * end;

        TEST();

        port = diminuto_maximumof(diminuto_port_t);
        EXPECT(port > 0);
        pointer = diminuto_ipc_port2string(port, buffer, sizeof(buffer));
        EXPECT(pointer == &(buffer[0]));
        number = strtoul(buffer, &end, 10);
        EXPECT(end != (const char *)0);
        EXPECT(*end == '\0');
        EXPECT(number == port);

        STATUS();
    }

    {
        diminuto_port_t port;
        int count;

        TEST();

        port = 0;
        EXPECT(diminuto_ipc_is_ephemeral(port));
        count = 0;
        for (port = 1; port < diminuto_maximumof(diminuto_port_t); ++port) {
            EXPECT(!diminuto_ipc_is_ephemeral(port));
            count += 1;
        }
        EXPECT(count > 0);

        STATUS();
    }

    EXIT();
}
