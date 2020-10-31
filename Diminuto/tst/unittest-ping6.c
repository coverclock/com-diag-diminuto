/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2015-2016 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Ping feature for IPv6.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Ping feature for IPv6.
 *
 * NOTE: Creating a raw socket such as used for ICMP is a privileged
 *       operation. Hence this unit test has to be run with root privileges.
 */

#include <unistd.h>
#include <string.h>
#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_ping6.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_frequency.h"

int main(int argc, char * argv[])
{
    int sock;
    char buffer[sizeof("XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX")];
    diminuto_ipv6_t to;
    diminuto_ipv6_t from;
    ssize_t size;
    uint8_t type;
    uint8_t code;
    uint16_t id;
    uint16_t seq;
    diminuto_port_t port;
    diminuto_ticks_t elapsed;
    const char * Address = 0;
    const char * Interface = 0;
    const char * Port = 0;
    extern char * optarg;
    extern int optind;
    extern int opterr;
    extern int optopt;
    int opt;

    SETLOGMASK();

    while ((opt = getopt(argc, argv, "a:i:p:")) >= 0) {
        switch (opt) {
        case 'a':
            /* e.g. "2001:470:4b:4e2:e79:7f1e:21f5:9355" */
            Address = optarg;
            break;
        case 'i':
            /* e.g. "eth0" */
            Interface = optarg;
            break;
        case 'p':
            /* e.g. "5555" */
            Port = optarg;
            break;
        default:
            break;
        }
    }

    {
        TEST();

        ASSERT((sock = diminuto_ping6_datagram_peer()) >= 0);

        STATUS();
    }

    {
        TEST();

        if (Interface != (const char *)0) {
            DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "interface=\"%s\"\n", Interface);
            ASSERT(diminuto_ipc_set_interface(sock, Interface) >= 0);
        }

        STATUS();
    }

    {
        TEST();

        memset(&from, 0, sizeof(from));
        if (Address != (const char *)0) {
            from = diminuto_ipc6_address(argv[2]);
            DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "address=\"%s\"=%x:%x:%x:%x:%x:%x:%x:%x=\"%s\"\n", Address, from.u16[0], from.u16[1], from.u16[2], from.u16[3], from.u16[4], from.u16[5], from.u16[6], from.u16[7], diminuto_ipc6_address2string(from, buffer, sizeof(buffer)));
        }

        port = 0;
        if (Port != (const char *)0) {
            port = strtoul(Port, (char **)0, 0);
            DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "port=\"%s\"=%u\n", Port, port);
        }

        if ((Address != (const char *)0) || (Port != (const char * )0)) {
            ASSERT(diminuto_ipc6_source(sock, from, port) >= 0);
        }

        STATUS();
    }

    {
        static const uint16_t ID = 0xcafe;
        uint16_t ss;
        diminuto_ticks_t delay;

        TEST();

        delay = diminuto_frequency();

        to = diminuto_ipc6_address("google.com");
        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "to=\"%s\"\n", diminuto_ipc6_address2string(to, buffer, sizeof(buffer)));
        ASSERT(memcmp(&to, &DIMINUTO_IPC6_UNSPECIFIED, sizeof(to)) != 0);

        for (ss = 0; ss < 10; ++ss) {
            DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "sending 0x%x %u\n", ID, ss);
            ASSERT(diminuto_ping6_datagram_send(sock, to, ID, ss) > 0);
            do {
                memcpy(&from, &DIMINUTO_IPC6_UNSPECIFIED, sizeof(from));
                type = ~0;
                code = ~0;
                id = 0;
                seq = ~0;
                elapsed = 0;
                ASSERT((size = diminuto_ping6_datagram_receive(sock, &from, &type, &code, &id, &seq, &elapsed)) >= 0);
                DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "received 0x%x 0x%x\n", type, code);
            } while (size == 0);
            DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "from=\"%s\" size=%zu type=0x%x code=0x%x id=0x%x seq=%u elapsed=%lluticks\n", diminuto_ipc6_address2string(from, buffer, sizeof(buffer)), size, type, code, id, seq, elapsed);
            ASSERT(memcmp(&from, &DIMINUTO_IPC6_UNSPECIFIED, sizeof(from)) != 0);
            EXPECT(memcmp(&from, &to, sizeof(from)) == 0);
            ASSERT(type != ~0);
            ASSERT(code != ~0);
            ASSERT(id == ID);
            ASSERT(seq == ss);
            ASSERT(elapsed > 0);
            diminuto_delay(delay, 0);
        }

        STATUS();
    }

    {
        TEST();

        ASSERT(diminuto_ping6_close(sock) >= 0);

        STATUS();
    }

    EXIT();
}
