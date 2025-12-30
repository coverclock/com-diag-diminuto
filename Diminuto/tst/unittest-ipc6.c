/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2015-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the IPC feature for IPv6.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the IPC feature for IPv6.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_ipc6.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_alarm.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_dump.h"
#include "com/diag/diminuto/diminuto_types.h"
#include "../src/diminuto_ipc6.h" /* Private API accessed for unit testing. */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

enum {
    TEST_PORT = 0xDEAD,
    TEST_WORD = 0xDEADC0DE,
};

#define TEST_INIT { { 0xDEAD, 0xDEAD, 0xDEAD, 0xDEAD, 0xDEAD, 0xDEAD, 0xDEAD, 0xDEAD, } }

static const diminuto_ipv6_t SERVER64 = { { 0, 0, 0, 0, 0, 0xffff, ((192 << 8) + 168), ((1 << 8) + 222), } };
static const diminuto_ipv4_t SERVER4 = ((((((192 << 8) + 168) << 8) + 1) << 8) + 222);
static const diminuto_ipv4_t LOCALHOST4 = ((((((127 << 8) + 0) << 8) + 0) << 8) + 1);
static const diminuto_ipv6_t TEST6 = TEST_INIT;

static const size_t LIMIT = 256;
static const size_t TOTAL = 1024 * 1024 * 100;

static struct in6_addr * convert(diminuto_ipv6_t * addressp, struct in6_addr * in6p)
{
    memcpy(&(in6p->s6_addr16), diminuto_ipc6_hton6(addressp), sizeof(in6p->s6_addr16));
    return in6p;
}

static int ipv6only(int fd, void * vp)
{
    if ((fd = diminuto_ipc_set_reuseaddress(fd, !0)) < 0) {
        /* Do nothing. */
    } else if ((fd = diminuto_ipc6_set_ipv6only(fd, (intptr_t)vp)) < 0) {
        /* Do nothing. */
    } else {
        /* Do nothing. */
    }
    return fd;
}

int main(int argc, char * argv[])
{
    diminuto_ticks_t hertz;
    const char * Host = (const char *)0;
    extern char * optarg;
    extern int optind;
    extern int opterr;
    extern int optopt;
    int opt;

    SETLOGMASK();

    while ((opt = getopt(argc, argv, "h:")) >= 0) {
        switch (opt) {
        case 'h':
            /* e.g. "www.diag.com" */
            Host = optarg;
            break;
        default:
            break;
        }
    }

    hertz = diminuto_frequency();

    {
        diminuto_ipv4_t address4;

        TEST();

        ASSERT(!diminuto_ipc6_is_v4mapped(&DIMINUTO_IPC6_LOOPBACK));
        address4 = TEST_WORD;
        ASSERT(!diminuto_ipc6_ipv6toipv4(DIMINUTO_IPC6_LOOPBACK, &address4));
        ASSERT(address4 == TEST_WORD);

        ASSERT(diminuto_ipc6_is_v4mapped(&DIMINUTO_IPC6_LOOPBACK4));
        address4 = TEST_WORD;
        ASSERT(diminuto_ipc6_ipv6toipv4(DIMINUTO_IPC6_LOOPBACK4, &address4));
        ASSERT(address4 == LOCALHOST4);

        STATUS();
    }

    {
        diminuto_ipv6_t address6 = TEST_INIT;
        diminuto_ipv4_t address4 = TEST_WORD;

        TEST();

        diminuto_ipc6_ipv4toipv6(SERVER4, &address6);
        ASSERT(diminuto_ipc6_compare(&address6, &SERVER64) == 0);
        ASSERT(diminuto_ipc6_is_v4mapped(&address6));

        ASSERT(diminuto_ipc6_ipv6toipv4(address6, &address4));
        ASSERT(address4 == SERVER4);

        STATUS();
    }

    {
        diminuto_ipv4_t address4 = TEST_WORD;
        diminuto_ipv6_t address6 = TEST_INIT;

        TEST();

        ASSERT(diminuto_ipc6_ipv6toipv4(SERVER64, &address4));
        ASSERT(address4 == SERVER4);

        diminuto_ipc6_ipv4toipv6(address4, &address6);
        ASSERT(diminuto_ipc6_compare(&address6, &SERVER64) == 0);

        STATUS();
    }

    {
        diminuto_ipv6_t address6;
        struct in6_addr in6;

        TEST();

        address6 = diminuto_ipc6_address("::");
        EXPECT(diminuto_ipc6_is_unspecified(&address6));
        EXPECT(IN6_IS_ADDR_UNSPECIFIED(convert(&address6, &in6)));

        address6 = diminuto_ipc6_address("::1");
        EXPECT(diminuto_ipc6_is_loopback(&address6));
        EXPECT(IN6_IS_ADDR_LOOPBACK(convert(&address6, &in6)));

        /*
         * IN6_IS_ADDR_V4COMPAT not present because IPv4-compatible addresses
         * have been deprecated, replaced with IPv4-mapped addresses.
         */

        address6 = diminuto_ipc6_address("::ffff:192.168.1.1");
        EXPECT(diminuto_ipc6_is_v4mapped(&address6));
        EXPECT(IN6_IS_ADDR_V4MAPPED(convert(&address6, &in6)));

        address6 = diminuto_ipc6_address("64:ff9b::192.168.1.1");
        EXPECT(diminuto_ipc6_is_nat64wkp(&address6));

        address6 = diminuto_ipc6_address("2001:1234:4678:9abc:def0:1234:5678:9abc");
        EXPECT(diminuto_ipc6_is_unicastglobal(&address6));

        address6 = diminuto_ipc6_address("2001:1234:4678:9abc:0000:5efe:192.168.1.1");
        EXPECT(diminuto_ipc6_is_isatap(&address6));

        address6 = diminuto_ipc6_address("2002:c0a8:0101:9abc:def0:1234:c0a8:0101");
        EXPECT(diminuto_ipc6_is_6to4(&address6));

        address6 = diminuto_ipc6_address("fd71:1234:4678:9abc:def0:12ff:fe78:9abc");
        EXPECT(diminuto_ipc6_is_uniquelocal(&address6));

        address6 = diminuto_ipc6_address("fe80:1234:4678:9abc:def0:12ff:fe78:9abc");
        EXPECT(diminuto_ipc6_is_linklocal(&address6));
        EXPECT(IN6_IS_ADDR_LINKLOCAL(convert(&address6, &in6)));

        /*
         * IN6_IS_ADDR_SITELOCAL not present because site-local addresses have been
         * deprecated, replaced with unique local addresses.
         */

        address6 = diminuto_ipc6_address("ff02:1:4678:9abc:def0:1234:5678:9abc");
        EXPECT(diminuto_ipc6_is_multicast(&address6));
        EXPECT(IN6_IS_ADDR_MULTICAST(convert(&address6, &in6)));

        STATUS();
    }

    {
        diminuto_ipv6_t address6;
        char buffer[sizeof("XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX")];

        TEST();

        address6 = diminuto_ipc6_address("::1");
        CHECKPOINT("\"%s\" %4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x %4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x\n", "::1", DIMINUTO_IPC6_LOOPBACK.u16[0], DIMINUTO_IPC6_LOOPBACK.u16[1], DIMINUTO_IPC6_LOOPBACK.u16[2], DIMINUTO_IPC6_LOOPBACK.u16[3], DIMINUTO_IPC6_LOOPBACK.u16[4], DIMINUTO_IPC6_LOOPBACK.u16[5], DIMINUTO_IPC6_LOOPBACK.u16[6], DIMINUTO_IPC6_LOOPBACK.u16[7], address6.u16[0], address6.u16[1], address6.u16[2], address6.u16[3], address6.u16[4], address6.u16[5], address6.u16[6], address6.u16[7]);
        EXPECT(diminuto_ipc6_is_loopback(&address6));

        EXPECT(diminuto_ipc6_colonnotation(address6, buffer, sizeof(buffer)) == buffer);
        CHECKPOINT("\"%s\" \"%s\"\n", buffer, "::1");
        EXPECT(strcmp(buffer, "::1") == 0);

        STATUS();
    }

    {
        const char * effective6;
        diminuto_ipv6_t address6;
        char buffer[sizeof("XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX")];

        TEST();

        address6 = diminuto_ipc6_address("localhost");
        CHECKPOINT("\"%s\" %4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x\n", "localhost", address6.u16[0], address6.u16[1], address6.u16[2], address6.u16[3], address6.u16[4], address6.u16[5], address6.u16[6], address6.u16[7]);
        /*
         * This next test depends on how the local host is administered with an
         * IPv6 local host address, or whether we get back a IPv4 local host
         * address encapsulated in an IPv6 address. Either is okay.
         */
        EXPECT(diminuto_ipc6_is_loopback(&address6) || diminuto_ipc6_is_loopback4(&address6));
        EXPECT(diminuto_ipc6_colonnotation(address6, buffer, sizeof(buffer)) == buffer);
        CHECKPOINT("\"%s\" \"%s\"\n", "localhost", buffer);

        effective6 = "ip6-localhost"; /* Ubuntu. */
        address6 = diminuto_ipc6_address(effective6);
        if (diminuto_ipc6_is_unspecified(&address6)) {
            effective6 = "ipv6-localhost"; /* CBL-Mariner. */
            address6 = diminuto_ipc6_address(effective6);
        }
        if (diminuto_ipc6_is_unspecified(&address6)) {
            effective6 = "localhost6"; /* Fedora. */
            address6 = diminuto_ipc6_address(effective6);
        }
        CHECKPOINT("\"%s\" %4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x\n", effective6, address6.u16[0], address6.u16[1], address6.u16[2], address6.u16[3], address6.u16[4], address6.u16[5], address6.u16[6], address6.u16[7]);
        /*
         * This next test depends on how the local host is administered with an
         * IPv6 local host address, or whether we get back a IPv4 local host
         * address encapsulated in an IPv6 address. Either is okay.
         */
        EXPECT(diminuto_ipc6_is_loopback(&address6) || diminuto_ipc6_is_loopback4(&address6));
        EXPECT(diminuto_ipc6_colonnotation(address6, buffer, sizeof(buffer)) == buffer);
        CHECKPOINT("\"%s\" \"%s\"\n", effective6, buffer);

        /*
         * Long pause here, not sure why. strace(1) suggests it's blocked in
         * a poll(2) in the resolver waiting on a socket to "127.0.1.1", which
         * is a variant of the localhost IPv4 loopback address, trying to
         * resolve "www.diag.com", my test domain. I'm seeing some long
         * delays on "copper", a RaspberryPi 3 too, that seem to be related
         * to DNS6 over the Hurricane Electric tunnel.
         */

        if (Host != (const char *)0) {
            address6 = diminuto_ipc6_address(Host);
            CHECKPOINT("\"%s\" %4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x\n", Host, address6.u16[0], address6.u16[1], address6.u16[2], address6.u16[3], address6.u16[4], address6.u16[5], address6.u16[6], address6.u16[7]);
            EXPECT(!diminuto_ipc6_is_unspecified(&address6));
            EXPECT(diminuto_ipc6_colonnotation(address6, buffer, sizeof(buffer)) == buffer);
            CHECKPOINT("\"%s\" \"%s\"\n", Host, buffer);
        }

        address6 = diminuto_ipc6_address("invalid.domain");
        CHECKPOINT("\"%s\" %4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x\n", "invalid.domain", address6.u16[0], address6.u16[1], address6.u16[2], address6.u16[3], address6.u16[4], address6.u16[5], address6.u16[6], address6.u16[7]);

        /*
         * Damned internet service providers map invalid domains to a "help"
         * page. "invalid.domain" becomes 0xd0448f32 a.k.a. 208.68.143.50
         * a.k.a. "search5.comcast.com". That's not helpful!
         */
        ADVISE(diminuto_ipc6_is_unspecified(&address6));
        EXPECT(diminuto_ipc6_colonnotation(address6, buffer, sizeof(buffer)) == buffer);
        CHECKPOINT("\"%s\" \"%s\"\n", "invalid.domain", buffer);

        STATUS();
    }

    {
        static const char HOSTNAME[] = "google.com";
        struct addrinfo hints = { 0 };
        struct addrinfo * infop = (struct addrinfo *)0;
        diminuto_ipv6_t address = { { 0x2607, 0xf8b0, 0x400f, 0x0803, 0x0000, 0x0000, 0x0000, 0x200e, } };
        diminuto_ipv6_t host;
        diminuto_ipv6_t network;
        diminuto_ipv6_t prime;
        char buffer0[sizeof("XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX")];
        char buffer1[sizeof("XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX")];
        char buffer2[sizeof("XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX")];
        char buffer3[sizeof("XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX")];
        char buffer4[sizeof("XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX")];
        int rc;

        TEST();

        ASSERT(diminuto_ipc6_colonnotation(address, buffer0, sizeof(buffer0)) == buffer0);
        CHECKPOINT("CON \"%s\" %4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x\n", buffer0, address.u16[0], address.u16[1], address.u16[2], address.u16[3], address.u16[4], address.u16[5], address.u16[6], address.u16[7]);

        hints.ai_family = AF_INET6;
        rc = getaddrinfo(HOSTNAME, (const char *)0, &hints, &infop);
        ASSERT(rc == 0);
        ASSERT(infop != (struct addrinfo *)0);
        ASSERT(infop->ai_family == AF_INET6);
#define IN6ADDR (((struct sockaddr_in6 *)(infop->ai_addr))->sin6_addr.s6_addr)
#define IN6ADDR16 (((struct sockaddr_in6 *)(infop->ai_addr))->sin6_addr.s6_addr16)
        ASSERT(inet_ntop(infop->ai_family, IN6ADDR, buffer1, sizeof(buffer1)) == buffer1);
        CHECKPOINT("RAW \"%s\" %4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x\n", buffer1, IN6ADDR16[0], IN6ADDR16[1], IN6ADDR16[2], IN6ADDR16[3], IN6ADDR16[4], IN6ADDR16[5], IN6ADDR16[6], IN6ADDR16[7]);
        freeaddrinfo(infop);

        host = diminuto_ipc6_address(HOSTNAME);
        ASSERT(diminuto_ipc6_colonnotation(host, buffer2, sizeof(buffer2)) == buffer2);
        CHECKPOINT("HBO \"%s\" %4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x\n", buffer2, host.u16[0], host.u16[1], host.u16[2], host.u16[3], host.u16[4], host.u16[5], host.u16[6], host.u16[7]);

        network = host;
        diminuto_ipc6_hton6(&network);
        ASSERT(inet_ntop(AF_INET6, &network, buffer3, sizeof(buffer3)) == buffer3);
        CHECKPOINT("NBO \"%s\" %4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x\n", buffer3, network.u16[0], network.u16[1], network.u16[2], network.u16[3], network.u16[4], network.u16[5], network.u16[6], network.u16[7]);

        prime = network;
        diminuto_ipc6_ntoh6(&prime);
        ASSERT(diminuto_ipc6_compare(&host, &prime) == 0);
        ASSERT(diminuto_ipc6_colonnotation(prime, buffer4, sizeof(buffer4)) == buffer4);
        CHECKPOINT("PRI \"%s\" %4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x\n", buffer4, prime.u16[0], prime.u16[1], prime.u16[2], prime.u16[3], prime.u16[4], prime.u16[5], prime.u16[6], prime.u16[7]);

        STATUS();
    }

    {
        diminuto_ipv6_t * addresses;
        size_t ii;
        char buffer[sizeof("XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX")];

        TEST();

        addresses = diminuto_ipc6_addresses("localhost");
        ASSERT(addresses != (diminuto_ipv6_t *)0);

        for (ii = 0; ii < LIMIT; ++ii) {
            CHECKPOINT("\"%s\" %4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x\n", "localhost", addresses[ii].u16[0], addresses[ii].u16[1], addresses[ii].u16[2], addresses[ii].u16[3], addresses[ii].u16[4], addresses[ii].u16[5], addresses[ii].u16[6], addresses[ii].u16[7]);
            if (diminuto_ipc6_is_unspecified(&addresses[ii])) {
                break;
            }
            EXPECT(diminuto_ipc6_colonnotation(addresses[ii], buffer, sizeof(buffer)) == buffer);
            CHECKPOINT("\"%s\" \"%s\"\n", "localhost", buffer);
        }
        EXPECT(ii > 0);
        EXPECT(ii < LIMIT);

        free(addresses);

        STATUS();
    }

    {
        diminuto_ipv6_t * addresses;
        size_t ii;
        char buffer[sizeof("XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX")];

        TEST();

        addresses = diminuto_ipc6_addresses("google.com");
        ASSERT(addresses != (diminuto_ipv6_t *)0);

        for (ii = 0; ii < LIMIT; ++ii) {
            CHECKPOINT("\"%s\" %4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x\n", "google.com", addresses[ii].u16[0], addresses[ii].u16[1], addresses[ii].u16[2], addresses[ii].u16[3], addresses[ii].u16[4], addresses[ii].u16[5], addresses[ii].u16[6], addresses[ii].u16[7]);
            if (diminuto_ipc6_is_unspecified(&addresses[ii])) {
                break;
            }
            EXPECT(diminuto_ipc6_colonnotation(addresses[ii], buffer, sizeof(buffer)) == buffer);
            CHECKPOINT("\"%s\" \"%s\"\n", "google.com", buffer);
        }
        /*
         * "nslookup google.com" used to resolve as multiple addresses. But
         * today it doesn't. So I added the "amazon.com" test below. So far
         * "amazon.com" still resolves as multiple addresses. HOWEVER,
         * "google.com" DOES respond with an IPv6 address,, while "amazon.com"
         * responds with IPv4 addresses. It's all good.
         */
        EXPECT(ii > 0);
        EXPECT(ii < LIMIT);

        free(addresses);

        STATUS();
    }

    {
        diminuto_ipv6_t * addresses;
        size_t ii;
        char buffer[sizeof("XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX")];

        TEST();

        addresses = diminuto_ipc6_addresses("amazon.com");
        ASSERT(addresses != (diminuto_ipv6_t *)0);

        for (ii = 0; ii < LIMIT; ++ii) {
            CHECKPOINT("\"%s\" %4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x\n", "amazon.com", addresses[ii].u16[0], addresses[ii].u16[1], addresses[ii].u16[2], addresses[ii].u16[3], addresses[ii].u16[4], addresses[ii].u16[5], addresses[ii].u16[6], addresses[ii].u16[7]);
            if (diminuto_ipc6_is_unspecified(&addresses[ii])) {
                break;
            }
            EXPECT(diminuto_ipc6_colonnotation(addresses[ii], buffer, sizeof(buffer)) == buffer);
            CHECKPOINT("\"%s\" \"%s\"\n", "amazon.com", buffer);
        }
        EXPECT(ii > 0);
        EXPECT(ii < LIMIT);

        free(addresses);

        STATUS();
    }

   {
        diminuto_ipv6_t * addresses;
        size_t ii;
        char buffer[sizeof("XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX")];

        TEST();

        if (Host != (const char *)0) {
            addresses = diminuto_ipc6_addresses(Host);
            ASSERT(addresses != (diminuto_ipv6_t *)0);

            for (ii = 0; ii < LIMIT; ++ii) {
                CHECKPOINT("\"%s\" %4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x\n", Host, addresses[ii].u16[0], addresses[ii].u16[1], addresses[ii].u16[2], addresses[ii].u16[3], addresses[ii].u16[4], addresses[ii].u16[5], addresses[ii].u16[6], addresses[ii].u16[7]);
                if (diminuto_ipc6_is_unspecified(&addresses[ii])) {
                    break;
                }
                EXPECT(diminuto_ipc6_colonnotation(addresses[ii], buffer, sizeof(buffer)) == buffer);
                CHECKPOINT("\"%s\" \"%s\"\n", Host, buffer);
            }
            EXPECT(ii > 0);
            EXPECT(ii < LIMIT);

            free(addresses);
        }

        STATUS();
    }

    {
        diminuto_port_t port;

        TEST();

        port = diminuto_ipc6_port("80", NULL);
        CHECKPOINT("\"%s\" \"%s\" %d %d\n", "80", "(null)", port, 80);
        EXPECT(port == 80);

        port = diminuto_ipc6_port("80", "tcp");
        CHECKPOINT("\"%s\" \"%s\" %d %d\n", "80", "tcp", port, 80);
        EXPECT(port == 80);

        port = diminuto_ipc6_port("80", "udp");
        CHECKPOINT("\"%s\" \"%s\" %d %d\n", "80", "udp", port, 80);
        EXPECT(port == 80);

        port = diminuto_ipc6_port("http", NULL);
        CHECKPOINT("\"%s\" \"%s\" %d %d\n", "http", "(null)", port, 80);
        EXPECT(port == 80);    

        port = diminuto_ipc6_port("http", "tcp");
        CHECKPOINT("\"%s\" \"%s\" %d %d\n", "http", "tcp", port, 80);
        EXPECT(port == 80);

        port = diminuto_ipc6_port("https", NULL);
        CHECKPOINT("\"%s\" \"%s\" %d %d\n", "https", "(null)", port, 443);
        EXPECT(port == 443);    

        port = diminuto_ipc6_port("https", "tcp");
        CHECKPOINT("\"%s\" \"%s\" %d %d\n", "https", "tcp", port, 443);
        EXPECT(port == 443);

        port = diminuto_ipc6_port("tftp", "udp");
        CHECKPOINT("\"%s\" \"%s\" %d %d\n", "tftp", "udp", port, 69);
        EXPECT(port == 69);

        port = diminuto_ipc6_port("login", NULL);
        CHECKPOINT("\"%s\" \"%s\" %d %d\n", "login", "(null)", port, 513);
        EXPECT(port == 513);

        port = diminuto_ipc6_port("login", "tcp");
        CHECKPOINT("\"%s\" \"%s\" %d %d\n", "login", "tcp", port, 513);
        EXPECT(port == 513);

        port = diminuto_ipc6_port("login", "udp");
        CHECKPOINT("\"%s\" \"%s\" %d %d\n", "login", "udp", port, 0);
        EXPECT(port == 0);

        port = diminuto_ipc6_port("who", NULL);
        CHECKPOINT("\"%s\" \"%s\" %d %d\n", "who", "(null)", port, 513);
        EXPECT(port == 513);

        port = diminuto_ipc6_port("who", "tcp");
        CHECKPOINT("\"%s\" \"%s\" %d %d\n", "who", "tcp", port, 0);
        EXPECT(port == 0);

        port = diminuto_ipc6_port("who", "udp");
        CHECKPOINT("\"%s\" \"%s\" %d %d\n", "who", "udp", port, 513);
        EXPECT(port == 513);

        port = diminuto_ipc6_port("unknown", NULL);
        CHECKPOINT("\"%s\" \"%s\" %d %d\n", "unknown", "(null)", port, 0);
        EXPECT(port == 0);

        port = diminuto_ipc6_port("unknown", "tcp");
        CHECKPOINT("\"%s\" \"%s\" %d %d\n", "unknown", "tcp", port, 0);
        EXPECT(port == 0);

        port = diminuto_ipc6_port("unknown", "udp");
        CHECKPOINT("\"%s\" \"%s\" %d %d\n", "unknown", "udp", port, 0);
        EXPECT(port == 0);

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

        sock = socket(AF_INET6, SOCK_DGRAM, 0);
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
        rc = diminuto_ipc6_stream_set_ipv6toipv4(sock);
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
        rc = diminuto_ipc6_datagram_set_ipv6toipv4(sock);
        EXPECT(rc >= 0);
        rc = close(sock);
        EXPECT(rc >= 0);

        STATUS();
    }

    {
        int fd;
        diminuto_ipv6_t address6 = TEST_INIT;
        diminuto_port_t port = TEST_PORT;
        char buffer[sizeof("XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX")];

        TEST();

        EXPECT((fd = diminuto_ipc6_datagram_peer(0)) >= 0);
        EXPECT(diminuto_ipc_type(fd) == AF_INET6);
        EXPECT(diminuto_ipc6_nearend(fd, &address6, &port) == 0);
        EXPECT(diminuto_ipc6_compare(&address6, &TEST6) != 0);
        EXPECT(port != TEST_PORT);
        EXPECT(diminuto_ipc6_close(fd) >= 0);
        EXPECT(diminuto_ipc6_colonnotation(address6, buffer, sizeof(buffer)) == buffer);
        CHECKPOINT("\"%s\" %u\n", buffer, port);

        STATUS();
    }

    {
        int fd;
        diminuto_ipv6_t address6 = TEST_INIT;
        diminuto_port_t port = TEST_PORT;
        char buffer[sizeof("XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX")];

        TEST();

        EXPECT((fd = diminuto_ipc6_stream_provider(0)) >= 0);
        EXPECT(diminuto_ipc_type(fd) == AF_INET6);
        EXPECT(diminuto_ipc6_nearend(fd, &address6, &port) == 0);
        EXPECT(diminuto_ipc6_compare(&address6, &TEST6) != 0);
        EXPECT(port != TEST_PORT);
        EXPECT(port != 0);
        EXPECT(diminuto_ipc6_close(fd) >= 0);
        EXPECT(diminuto_ipc6_colonnotation(address6, buffer, sizeof(buffer)) == buffer);
        CHECKPOINT("\"%s\" %u\n", buffer, port);

        STATUS();
    }

    {
        int fd;
        const char MSG[] = "Chip Overclock";
        char buffer[sizeof(MSG) * 2];
        diminuto_ipv6_t server;
        diminuto_port_t rendezvous = TEST_PORT;
        diminuto_ipv6_t address = TEST_INIT;
        diminuto_port_t port = TEST_PORT;

        TEST();

        server = diminuto_ipc6_address("::1");
        EXPECT(!diminuto_ipc6_is_unspecified(&server));
        EXPECT((fd = diminuto_ipc6_datagram_peer(0)) >= 0);
        EXPECT(diminuto_ipc6_nearend(fd, (diminuto_ipv6_t *)0, &rendezvous) == 0);
        EXPECT(rendezvous != TEST_PORT);
        EXPECT(rendezvous != 0);

        /* This only works because the kernel buffers socket data. */

        EXPECT((diminuto_ipc6_datagram_send(fd, MSG, sizeof(MSG), server, rendezvous)) == sizeof(MSG));
        EXPECT((diminuto_ipc6_datagram_receive_generic(fd, buffer, sizeof(buffer), &address, &port, 0)) == sizeof(MSG));
        EXPECT(strcmp(buffer, MSG) == 0);
        ADVISE(diminuto_ipc6_compare(&address, &server) == 0);
        EXPECT(port == rendezvous);

        EXPECT(diminuto_ipc6_close(fd) >= 0);

        STATUS();
    }

    {
        int fd;
        const char MSG[] = "Chip Overclock";
        char buffer[sizeof(MSG) * 2];
        diminuto_ipv6_t server;
        diminuto_ipv6_t binding = TEST_INIT;
        diminuto_port_t rendezvous = TEST_PORT;
        diminuto_ipv6_t address = TEST_INIT;
        diminuto_port_t port = TEST_PORT;

        TEST();

        server = diminuto_ipc6_address("::1");
        EXPECT(!diminuto_ipc6_is_unspecified(&server));
        EXPECT((fd = diminuto_ipc6_datagram_peer_generic(server, 0, (const char *)0)) >= 0);
        EXPECT(diminuto_ipc6_nearend(fd, &binding, &rendezvous) == 0);
        EXPECT(diminuto_ipc6_compare(&binding, &server) == 0);
        EXPECT(rendezvous != 0);

        /* This only works because the kernel buffers socket data. */

        EXPECT((diminuto_ipc6_datagram_send(fd, MSG, sizeof(MSG), server, rendezvous)) == sizeof(MSG));
        EXPECT((diminuto_ipc6_datagram_receive_generic(fd, buffer, sizeof(buffer), &address, &port, 0)) == sizeof(MSG));
        EXPECT(strcmp(buffer, MSG) == 0);
        EXPECT(diminuto_ipc6_compare(&address, &server) == 0);
        EXPECT(port != TEST_PORT);
        EXPECT(port != 0);

        EXPECT(diminuto_ipc6_close(fd) >= 0);

        STATUS();
    }

    {
        int fd;
        const char MSG[] = "Chip Overclock";
        char buffer[sizeof(MSG) * 2];
        diminuto_ipv6_t server;
        diminuto_port_t rendezvous = TEST_PORT;
        diminuto_ipv6_t address = TEST_INIT;
        diminuto_port_t port = TEST_PORT;

        TEST();

        server = diminuto_ipc6_address("::ffff:127.0.0.1");
        EXPECT(!diminuto_ipc6_is_unspecified(&server));
        EXPECT((fd = diminuto_ipc6_datagram_peer(0)) >= 0);
        EXPECT(diminuto_ipc6_nearend(fd, (diminuto_ipv6_t *)0, &rendezvous) == 0);
        EXPECT(rendezvous != 0);

        /* This only works because the kernel buffers socket data. */

        EXPECT((diminuto_ipc6_datagram_send(fd, MSG, sizeof(MSG), server, rendezvous)) == sizeof(MSG));
        EXPECT((diminuto_ipc6_datagram_receive_generic(fd, buffer, sizeof(buffer), &address, &port, 0)) == sizeof(MSG));
        EXPECT(strcmp(buffer, MSG) == 0);
        ADVISE(diminuto_ipc6_compare(&address, &server) == 0);
        EXPECT(port == rendezvous);

        EXPECT(diminuto_ipc6_close(fd) >= 0);

        STATUS();
    }

    {
        int fd;
        const char MSG[] = "Chip Overclock";
        char buffer[sizeof(MSG) * 2];
        diminuto_ipv6_t server;
        diminuto_port_t rendezvous = TEST_PORT;
        diminuto_ipv6_t address = TEST_INIT;
        diminuto_port_t port = TEST_PORT;
        diminuto_sticks_t timestamp = 0;
        int year = 0;
        int month = 0;
        int day = 0;
        int hour = 0;
        int minute = 0;
        int second = 0;
        diminuto_ticks_t ticks = 0;

        TEST();

        server = diminuto_ipc6_address("::1");
        EXPECT(!diminuto_ipc6_is_unspecified(&server));
        EXPECT((fd = diminuto_ipc6_datagram_peer(0)) >= 0);
        EXPECT(diminuto_ipc_set_timestamp(fd, 0) == fd);
        EXPECT(diminuto_ipc6_nearend(fd, (diminuto_ipv6_t *)0, &rendezvous) == 0);
        EXPECT(rendezvous != TEST_PORT);
        EXPECT(rendezvous != 0);

        /* This only works because the kernel buffers socket data. */

        EXPECT((diminuto_ipc6_datagram_send(fd, MSG, sizeof(MSG), server, rendezvous)) == sizeof(MSG));
        EXPECT((diminuto_ipc6_datagram_receive_generic(fd, buffer, sizeof(buffer), &address, &port, 0)) == sizeof(MSG));
        EXPECT(strcmp(buffer, MSG) == 0);
        ADVISE(diminuto_ipc6_compare(&address, &server) == 0);
        EXPECT(port == rendezvous);

        EXPECT(diminuto_ipc_get_datagram_timestamp(fd, &timestamp) >= 0);
        EXPECT(timestamp >= 0);
        CHECKPOINT("timestamp=%lld\n", (diminuto_lld_t)timestamp);
        EXPECT(diminuto_time_zulu(timestamp, &year, &month, &day, &hour, &minute, &second, &ticks) == 0);
        CHECKPOINT("timestamp=%04d-%02d-%02dT%02d:%02d:%02d.%09lld\n", year, month, day, hour, minute, second, (diminuto_lld_t)diminuto_frequency_ticks2fractionalseconds(ticks, 1000000000LL));

        EXPECT(diminuto_ipc6_close(fd) >= 0);

        STATUS();
    }

    {
        int fd;

        TEST();

#if !0
        EXPECT((fd = diminuto_ipc6_stream_consumer(diminuto_ipc6_address("amazon.com"), diminuto_ipc6_port("https", NULL))) >= 0);
        EXPECT(diminuto_ipc_type(fd) == AF_INET6);
        EXPECT(diminuto_ipc6_close(fd) >= 0);
#endif

        STATUS();
}

{
        int fd;

        TEST();

        if (Host != (const char *)0) {
            NOTIFY("If the connection request times out, try it again.\n");
            EXPECT((fd = diminuto_ipc6_stream_consumer(diminuto_ipc6_address(Host), diminuto_ipc6_port("https", NULL))) >= 0);
            EXPECT(diminuto_ipc_type(fd) == AF_INET6);
            EXPECT(diminuto_ipc6_close(fd) >= 0);
        }

        STATUS();
    }

    {
        int fd;
        char buffer[1];
        diminuto_ipv6_t address = TEST_INIT;
        diminuto_port_t port = TEST_PORT;

        TEST();

        EXPECT((fd = diminuto_ipc6_datagram_peer(0)) >= 0);
        EXPECT(diminuto_ipc_set_nonblocking(fd, !0) >= 0);
        EXPECT((diminuto_ipc6_datagram_receive_generic(fd, buffer, sizeof(buffer), &address, &port, 0)) < 0);
        EXPECT(errno == EAGAIN);
        EXPECT(diminuto_ipc6_compare(&address, &TEST6) == 0);
        EXPECT(port == TEST_PORT);
        EXPECT(diminuto_ipc6_close(fd) >= 0);

        STATUS();
    }

    {
        int fd;
        char buffer[1];
        diminuto_ipv6_t address = TEST_INIT;
        diminuto_port_t port = TEST_PORT;

        TEST();

        EXPECT((fd = diminuto_ipc6_datagram_peer(0)) >= 0);
        EXPECT((diminuto_ipc6_datagram_receive_generic(fd, buffer, sizeof(buffer), &address, &port, MSG_DONTWAIT)) < 0);
        EXPECT(errno == EAGAIN);
        EXPECT(diminuto_ipc6_compare(&address, &TEST6) == 0);
        EXPECT(port == TEST_PORT);
        EXPECT(diminuto_ipc6_close(fd) >= 0);

        STATUS();
    }

    {
        int fd;
        char buffer[1];
        diminuto_ipv6_t address = TEST_INIT;
        diminuto_port_t port = TEST_PORT;
        diminuto_sticks_t before;
        diminuto_sticks_t after;

        TEST();

        EXPECT((fd = diminuto_ipc6_datagram_peer(0)) >= 0);
        EXPECT(diminuto_alarm_install(0) >= 0);
        EXPECT(diminuto_timer_oneshot(2000000ULL) == 0);
        EXPECT((before = diminuto_time_elapsed()) >= 0);
        EXPECT((diminuto_ipc6_datagram_receive_generic(fd, buffer, sizeof(buffer), &address, &port, 0)) < 0);
        EXPECT((after = diminuto_time_elapsed()) >= 0);
        EXPECT(diminuto_timer_oneshot(0ULL) >= 0);
        CHECKPOINT("elapsed %lld - %lld = %lld\n", (long long int)after, (long long int)before, (long long int)(after - before));
        EXPECT(diminuto_alarm_check());
        EXPECT((after - before) >= 2000000LL);
        EXPECT(errno == EINTR);
        EXPECT(diminuto_ipc6_compare(&address, &TEST6) == 0);
        EXPECT(port == TEST_PORT);
        EXPECT(diminuto_ipc6_close(fd) >= 0);

        STATUS();
    }

    {
        diminuto_ipv6_t address;
        diminuto_port_t port;
        diminuto_port_t rendezvous = TEST_PORT;
        int service;
        pid_t pid;

        TEST();

        ASSERT((service = diminuto_ipc6_stream_provider(0)) >= 0);
        EXPECT(diminuto_ipc6_nearend(service, (diminuto_ipv6_t *)0, &rendezvous) >= 0);
        EXPECT(rendezvous != TEST_PORT);

        EXPECT((pid = fork()) >= 0);

        if (pid != 0) {

            int producer;
            int status;

            memcpy(&address, &TEST6, sizeof(address));
            port = TEST_PORT;
            EXPECT((producer = diminuto_ipc6_stream_accept_generic(service, &address, &port)) >= 0);
            EXPECT(diminuto_ipc6_compare(&address, &TEST6) != 0);
            EXPECT(port != TEST_PORT);
            EXPECT(port != rendezvous);

            memcpy(&address, &TEST6, sizeof(address));
            port = TEST_PORT;
            EXPECT(diminuto_ipc6_nearend(producer, &address, &port) >= 0);
            EXPECT(diminuto_ipc6_compare(&address, &TEST6) != 0);
            EXPECT(port != TEST_PORT);
            EXPECT(port == rendezvous);

            memcpy(&address, &TEST6, sizeof(address));
            port = TEST_PORT;
            EXPECT(diminuto_ipc6_farend(producer, &address, &port) >= 0);
            EXPECT(diminuto_ipc6_compare(&address, &TEST6) != 0);
            EXPECT(port != TEST_PORT);
            EXPECT(port != rendezvous);

            diminuto_delay(hertz / 1000, !0);

            EXPECT(diminuto_ipc6_close(producer) >= 0);
            EXPECT(diminuto_ipc6_close(service) >= 0);

            /*
             * If you don't wait for the child to exit, it may not yet have
             * closed its end of the socket thereby releasing the bound IP
             * address by the time the next unit test begins.
             */

            EXPECT(waitpid(pid, &status, 0) == pid);
            CHECKPOINT("pid=%d status=%d\n", pid, status);
            EXPECT(WIFEXITED(status));
            EXPECT(WEXITSTATUS(status) == 0);

        } else {

            int consumer;

            EXPECT(diminuto_ipc6_close(service) >= 0);
            diminuto_delay(hertz / 1000, !0);
            ASSERT((consumer = diminuto_ipc6_stream_consumer(diminuto_ipc6_address("::1"), rendezvous)) >= 0);
            diminuto_delay(hertz / 1000, !0);
            EXPECT(diminuto_ipc6_close(consumer) >= 0);

            EXIT();

        }

        STATUS();
    }

    {
        diminuto_ipv6_t address;
        diminuto_port_t port;
        diminuto_port_t rendezvous = TEST_PORT;
        int service;
        pid_t pid;

        TEST();

        ASSERT((service = diminuto_ipc6_stream_provider_base(DIMINUTO_IPC6_UNSPECIFIED, 0, (const char *)0, -1, ipv6only, (void *)0)) >= 0);
        EXPECT(diminuto_ipc6_nearend(service, (diminuto_ipv6_t *)0, &rendezvous) >= 0);
        EXPECT(rendezvous != TEST_PORT);

        EXPECT((pid = fork()) >= 0);

        if (pid != 0) {

            int producer;
            int status;

            memcpy(&address, &TEST6, sizeof(address));
            port = TEST_PORT;
            EXPECT((producer = diminuto_ipc6_stream_accept_generic(service, &address, &port)) >= 0);
            EXPECT(diminuto_ipc6_compare(&address, &TEST6) != 0);
            EXPECT(port != TEST_PORT);
            EXPECT(port != rendezvous);

            memcpy(&address, &TEST6, sizeof(address));
            port = TEST_PORT;
            EXPECT(diminuto_ipc6_nearend(producer, &address, &port) >= 0);
            EXPECT(diminuto_ipc6_compare(&address, &TEST6) != 0);
            EXPECT(port != TEST_PORT);
            EXPECT(port == rendezvous);

            memcpy(&address, &TEST6, sizeof(address));
            port = TEST_PORT;
            EXPECT(diminuto_ipc6_farend(producer, &address, &port) >= 0);
            EXPECT(diminuto_ipc6_compare(&address, &TEST6) != 0);
            EXPECT(port != TEST_PORT);
            EXPECT(port != rendezvous);

            diminuto_delay(hertz / 1000, !0);

            EXPECT(diminuto_ipc6_close(producer) >= 0);
            EXPECT(diminuto_ipc6_close(service) >= 0);

            /*
             * If you don't wait for the child to exit, it may not yet have
             * closed its end of the socket thereby releasing the bound IP
             * address by the time the next unit test begins.
             */

            EXPECT(waitpid(pid, &status, 0) == pid);
            CHECKPOINT("pid=%d status=%d\n", pid, status);
            EXPECT(WIFEXITED(status));
            EXPECT(WEXITSTATUS(status) == 0);

        } else {

            int consumer;

            EXPECT(diminuto_ipc6_close(service) >= 0);
            diminuto_delay(hertz / 1000, !0);
            ASSERT((consumer = diminuto_ipc6_stream_consumer(diminuto_ipc6_address("::1"), rendezvous)) >= 0);
            diminuto_delay(hertz / 1000, !0);
            EXPECT(diminuto_ipc6_close(consumer) >= 0);

            EXIT();

        }

        STATUS();
    }

    {
        diminuto_ipv6_t address;
        diminuto_port_t port;
        diminuto_port_t rendezvous = TEST_PORT;
        int service;
        pid_t pid;

        TEST();

        ASSERT((service = diminuto_ipc6_stream_provider_base(DIMINUTO_IPC6_UNSPECIFIED, 0, (const char *)0, -1, ipv6only, (void *)!0)) >= 0);
        EXPECT(diminuto_ipc6_nearend(service, (diminuto_ipv6_t *)0, &rendezvous) >= 0);
        EXPECT(rendezvous != TEST_PORT);

        EXPECT((pid = fork()) >= 0);

        if (pid != 0) {

            int producer;
            int status;

            memcpy(&address, &TEST6, sizeof(address));
            port = TEST_PORT;
            EXPECT((producer = diminuto_ipc6_stream_accept_generic(service, &address, &port)) >= 0);
            EXPECT(diminuto_ipc6_compare(&address, &TEST6) != 0);
            EXPECT(port != TEST_PORT);
            EXPECT(port != rendezvous);

            memcpy(&address, &TEST6, sizeof(address));
            port = TEST_PORT;
            EXPECT(diminuto_ipc6_nearend(producer, &address, &port) >= 0);
            EXPECT(diminuto_ipc6_compare(&address, &TEST6) != 0);
            EXPECT(port != TEST_PORT);
            EXPECT(port == rendezvous);

            memcpy(&address, &TEST6, sizeof(address));
            port = TEST_PORT;
            EXPECT(diminuto_ipc6_farend(producer, &address, &port) >= 0);
            EXPECT(diminuto_ipc6_compare(&address, &TEST6) != 0);
            EXPECT(port != TEST_PORT);
            EXPECT(port != rendezvous);

            diminuto_delay(hertz / 1000, !0);

            EXPECT(diminuto_ipc6_close(producer) >= 0);
            EXPECT(diminuto_ipc6_close(service) >= 0);

            /*
             * If you don't wait for the child to exit, it may not yet have
             * closed its end of the socket thereby releasing the bound IP
             * address by the time the next unit test begins.
             */

            EXPECT(waitpid(pid, &status, 0) == pid);
            CHECKPOINT("pid=%d status=%d\n", pid, status);
            EXPECT(WIFEXITED(status));
            EXPECT(WEXITSTATUS(status) == 0);

        } else {

            int consumer;

            EXPECT(diminuto_ipc6_close(service) >= 0);
            diminuto_delay(hertz / 1000, !0);
            ASSERT((consumer = diminuto_ipc6_stream_consumer(diminuto_ipc6_address("::1"), rendezvous)) >= 0);
            diminuto_delay(hertz / 1000, !0);
            EXPECT(diminuto_ipc6_close(consumer) >= 0);

            EXIT();

        }

        STATUS();
    }

    {
        diminuto_ipv6_t address;
        diminuto_port_t port;
        diminuto_port_t rendezvous = TEST_PORT;
        int service;
        pid_t pid;

        TEST();

        ASSERT((service = diminuto_ipc6_stream_provider_base(DIMINUTO_IPC6_UNSPECIFIED, 0, (const char *)0, -1, ipv6only, (void *)0)) >= 0);
        EXPECT(diminuto_ipc6_nearend(service, (diminuto_ipv6_t *)0, &rendezvous) >= 0);
        EXPECT(rendezvous != TEST_PORT);

        EXPECT((pid = fork()) >= 0);

        if (pid != 0) {

            int producer;
            int status;

            memcpy(&address, &TEST6, sizeof(address));
            port = TEST_PORT;
            EXPECT((producer = diminuto_ipc6_stream_accept_generic(service, &address, &port)) >= 0);
            EXPECT(diminuto_ipc6_compare(&address, &TEST6) != 0);
            EXPECT(port != TEST_PORT);
            EXPECT(port != rendezvous);

            memcpy(&address, &TEST6, sizeof(address));
            port = TEST_PORT;
            EXPECT(diminuto_ipc6_nearend(producer, &address, &port) >= 0);
            EXPECT(diminuto_ipc6_compare(&address, &TEST6) != 0);
            EXPECT(port != TEST_PORT);
            EXPECT(port == rendezvous);

            memcpy(&address, &TEST6, sizeof(address));
            port = TEST_PORT;
            EXPECT(diminuto_ipc6_farend(producer, &address, &port) >= 0);
            EXPECT(diminuto_ipc6_compare(&address, &TEST6) != 0);
            EXPECT(port != TEST_PORT);
            EXPECT(port != rendezvous);

            diminuto_delay(hertz / 1000, !0);

            EXPECT(diminuto_ipc6_close(producer) >= 0);
            EXPECT(diminuto_ipc6_close(service) >= 0);

            /*
             * If you don't wait for the child to exit, it may not yet have
             * closed its end of the socket thereby releasing the bound IP
             * address by the time the next unit test begins.
             */

            EXPECT(waitpid(pid, &status, 0) == pid);
            CHECKPOINT("pid=%d status=%d\n", pid, status);
            EXPECT(WIFEXITED(status));
            EXPECT(WEXITSTATUS(status) == 0);

        } else {

            int consumer;

            EXPECT(diminuto_ipc6_close(service) >= 0);
            diminuto_delay(hertz / 1000, !0);
            ASSERT((consumer = diminuto_ipc6_stream_consumer(diminuto_ipc6_address("::ffff:127.0.0.1"), rendezvous)) >= 0);
            diminuto_delay(hertz / 1000, !0);
            EXPECT(diminuto_ipc6_close(consumer) >= 0);

            EXIT();

        }

        STATUS();
    }

    {
        diminuto_port_t rendezvous = TEST_PORT;
        int service;
        pid_t pid;

        TEST();

        ASSERT((service = diminuto_ipc6_stream_provider_base(DIMINUTO_IPC6_UNSPECIFIED, 0, (const char *)0, -1, ipv6only, (void *)!0)) >= 0);
        EXPECT(diminuto_ipc6_nearend(service, (diminuto_ipv6_t *)0, &rendezvous) >= 0);
        EXPECT(rendezvous != TEST_PORT);

        EXPECT((pid = fork()) >= 0);

        if (pid != 0) {

            int status;

            diminuto_delay(hertz / 1000, !0);

            EXPECT(diminuto_ipc6_close(service) >= 0);

            /*
             * If you don't wait for the child to exit, it may not yet have
             * closed its end of the socket thereby releasing the bound IP
             * address by the time the next unit test begins.
             */

            EXPECT(waitpid(pid, &status, 0) == pid);
            CHECKPOINT("pid=%d status=%d\n", pid, status);
            EXPECT(WIFEXITED(status));
            EXPECT(WEXITSTATUS(status) == 0);

        } else {

            int consumer;

            EXPECT(diminuto_ipc6_close(service) >= 0);
            diminuto_delay(hertz / 1000, !0);
            EXPECT((consumer = diminuto_ipc6_stream_consumer(diminuto_ipc6_address("::ffff:127.0.0.1"), rendezvous)) < 0);
            diminuto_delay(hertz / 1000, !0);

            EXIT();

        }

        STATUS();
    }

    /*
     * There's a subtle design flaw in the next unit test which I first ran
     * about fifteen years ago when doing socket programming under SunOS.
     * It is possible for the two processes below to deadlock, one waiting
     * for space to become available on the output side, one waiting for data
     * to become available on the input side. I'm not concerned about this
     * unit test, but it shows how difficult socket programming in the real
     * world can be. Real world applications should use select(2) or its
     * equivalent to determine if output buffer space is available in the
     * kernel. See diminuto_mux.c and its unit test unittest-mux.c which
     * uses select(2) to multiplex the diminuto_ipc6 feature in just this way.
     */

    {
        diminuto_ipv6_t provider;
        diminuto_port_t rendezvous;
        diminuto_ipv6_t source;
        diminuto_ipv6_t address;
        diminuto_port_t port;
        int service;
        pid_t pid;

        TEST();

        provider = diminuto_ipc6_address("::1");
        ASSERT((service = diminuto_ipc6_stream_provider_generic(provider, 0, (const char *)0, -1)) >= 0);
        EXPECT(diminuto_ipc6_nearend(service, &source, &rendezvous) >= 0);
        EXPECT(diminuto_ipc6_compare(&source, &provider) == 0);
        EXPECT(rendezvous != 0);

        ASSERT((pid = fork()) >= 0);

        if (pid) {

            int producer;
            uint8_t output[256];
            uint8_t input[256];
            uint8_t datum;
            ssize_t sent;
            ssize_t received;
            ssize_t used;
            ssize_t available;
            ssize_t inputqueued;
            ssize_t outputqueued;
            uint8_t * here;
            uint8_t * there;
            uint8_t * current;
            size_t totalsent;
            size_t totalreceived;
            int status;

            ASSERT((producer = diminuto_ipc6_stream_accept_generic(service, &address, &port)) >= 0);

            here = output;
            used = sizeof(output);
            sent = 0;

            there = input;
            available = sizeof(input);
            received = 0;

            totalsent = 0;
            totalreceived = 0;

            datum = 0;
            do {
                output[datum] = datum;
            } while ((++datum) > 0);

            memset(input, 0, sizeof(input));

            do {

                if (totalsent < TOTAL) {

                    if (used > (TOTAL - totalsent)) {
                        used = TOTAL - totalsent;
                    }

                    ASSERT((sent = diminuto_ipc6_stream_write_generic(producer, here, 1, used)) > 0);
                    ASSERT(sent <= used);

                    EXPECT((outputqueued = diminuto_ipc_stream_get_pending(producer)) >= 0);

                    totalsent += sent;
                    COMMENT("producer sent %zd %zd %zu\n", sent, outputqueued, totalsent);

                    here += sent;
                    used -= sent;

                    if (used == 0) {
                        here = output;
                        used = sizeof(output);
                    }

                }

                current = there;

                if (available > (TOTAL - totalreceived)) {
                    available = TOTAL - totalreceived;
                }

                EXPECT((inputqueued = diminuto_ipc_stream_get_available(producer)) >= 0);

                ASSERT((received = diminuto_ipc6_stream_read(producer, there, available)) > 0);
                ASSERT(received <= available);

                totalreceived += received;
                COMMENT("producer received %zd %zd %zu\n", received, inputqueued, totalreceived);

                there += received;
                available -= received;

                while (current < there) {
                    ASSERT(*(current++) == (datum++));
                }

                if (available == 0) {
                    there = input;
                    available = sizeof(input);
                    memset(input, 0, sizeof(input));
                }

            } while (totalreceived < TOTAL);

            EXPECT(diminuto_ipc6_shutdown(producer) >= 0);
            EXPECT(diminuto_ipc6_shutdown(service) >= 0);

            ASSERT(diminuto_ipc6_close(producer) >= 0);
            ASSERT(diminuto_ipc6_close(service) >= 0);

            EXPECT(waitpid(pid, &status, 0) == pid);
            CHECKPOINT("pid=%d status=%d\n", pid, status);
            EXPECT(WIFEXITED(status));
            EXPECT(WEXITSTATUS(status) == 0);

        } else {

            int consumer;
            uint8_t buffer[64];
            ssize_t sent;
            ssize_t received;
            size_t totalsent;
            size_t totalreceived;
            ssize_t inputqueued;
            ssize_t outputqueued;

            ASSERT(diminuto_ipc6_close(service) >= 0);

            diminuto_delay(hertz / 1000, !0);

            ASSERT((consumer = diminuto_ipc6_stream_consumer(provider, rendezvous)) >= 0);

            totalreceived = 0;
            totalsent = 0;

            while (!0) {

                EXPECT((inputqueued = diminuto_ipc_stream_get_available(consumer)) >= 0);

                ASSERT((received = diminuto_ipc6_stream_read(consumer, buffer, sizeof(buffer))) >= 0);
                ASSERT(received <= sizeof(buffer));

                totalreceived += received;
                COMMENT("consumer received %zd %zd %zu\n", received, inputqueued, totalreceived);

                if (received == 0) {
                    break;
                }

                sent = 0;
                while (sent < received) {
                    ASSERT((sent = diminuto_ipc6_stream_write_generic(consumer,  buffer + sent, 1, received - sent)) > 0);
                    ASSERT(sent <= received);

                    EXPECT((outputqueued = diminuto_ipc_stream_get_pending(consumer)) >= 0);

                    totalsent += sent;
                    COMMENT("consumer sent %zd %zd %zu\n", sent, outputqueued, totalsent);

                    received -= sent;
                }
            }

            EXPECT(inputqueued == 0);

            EXPECT(diminuto_ipc6_shutdown(consumer) >= 0);

            ASSERT(diminuto_ipc6_close(consumer) >= 0);

            EXIT();
        }

        STATUS();
    }

    EXIT();
}
