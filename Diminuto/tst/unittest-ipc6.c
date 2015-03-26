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
#include "com/diag/diminuto/diminuto_ipc6.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_alarm.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define TEST_PORT 0xDEAD
#define TEST_WORD 0xDEADC0DE
#define TEST_INIT { 0xDEAD, 0xDEAD, 0xDEAD, 0xDEAD, 0xDEAD, 0xDEAD, 0xDEAD, 0xDEAD }

static const diminuto_ipv6_t SERVER64 = { 0, 0, 0, 0, 0, 0xffff, ((192 << 8) + 168), ((1 << 8) + 222) };
static const diminuto_ipv4_t SERVER4 = ((((((192 << 8) + 168) << 8) + 1) << 8) + 222);
static const diminuto_ipv4_t LOCALHOST4 = ((((((127 << 8) + 0) << 8) + 0) << 8) + 1);
static const diminuto_ipv6_t TEST6 = TEST_INIT;

static const size_t LIMIT = 256;
static const size_t TOTAL = 1024 * 1024 * 100;

int main(void)
{
    diminuto_ticks_t hertz;

    SETLOGMASK();

    hertz = diminuto_frequency();

    {
        diminuto_ipc6_sockaddr_t sa;

        ASSERT(sizeof(sa) == 256);
        ASSERT(sizeof(sa.sa_family) == sizeof(unsigned short int));
        ASSERT(sizeof(sa.sa_data) == (sizeof(sa) - sizeof(unsigned short int)));
    }

    {
        diminuto_ipv4_t address4;

        TEST();

        address4 = TEST_WORD;
        ASSERT(!diminuto_ipc6_ipv62ipv4(DIMINUTO_IPC6_LOOPBACK, &address4));
        ASSERT(address4 == TEST_WORD);

        address4 = TEST_WORD;
        ASSERT(diminuto_ipc6_ipv62ipv4(DIMINUTO_IPC6_LOOPBACK4, &address4));
        ASSERT(address4 == LOCALHOST4);

        STATUS();
    }

    {
        diminuto_ipv6_t address6 = TEST_INIT;
        diminuto_ipv4_t address4 = TEST_WORD;
        char buffer[sizeof("XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX")];

        TEST();

        diminuto_ipc6_ipv42ipv6(SERVER4, &address6);
        ASSERT(memcmp(&address6, &SERVER64, sizeof(address6)) == 0);

        ASSERT(diminuto_ipc6_ipv62ipv4(address6, &address4));
        ASSERT(address4 == SERVER4);

        STATUS();
    }

    {
        diminuto_ipv4_t address4 = TEST_WORD;
        diminuto_ipv6_t address6 = TEST_INIT;

        TEST();

        ASSERT(diminuto_ipc6_ipv62ipv4(SERVER64, &address4));
        ASSERT(address4 == SERVER4);

        diminuto_ipc6_ipv42ipv6(address4, &address6);
        ASSERT(memcmp(&address6, &SERVER64, sizeof(address6)) == 0);

        STATUS();
    }

    {
        diminuto_ipv6_t address6;
        char buffer[sizeof("XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX")];

        TEST();

        address6 = diminuto_ipc6_address("::1");
        DIMINUTO_LOG_DEBUG("%s \"%s\" %4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x %4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x\n", DIMINUTO_LOG_HERE, "::1", DIMINUTO_IPC6_LOOPBACK.u16[0], DIMINUTO_IPC6_LOOPBACK.u16[1], DIMINUTO_IPC6_LOOPBACK.u16[2], DIMINUTO_IPC6_LOOPBACK.u16[3], DIMINUTO_IPC6_LOOPBACK.u16[4], DIMINUTO_IPC6_LOOPBACK.u16[5], DIMINUTO_IPC6_LOOPBACK.u16[6], DIMINUTO_IPC6_LOOPBACK.u16[7], address6.u16[0], address6.u16[1], address6.u16[2], address6.u16[3], address6.u16[4], address6.u16[5], address6.u16[6], address6.u16[7]);
        EXPECT(memcmp(&address6, &DIMINUTO_IPC6_LOOPBACK, sizeof(address6)) == 0);

        EXPECT(diminuto_ipc6_colonnotation(address6, buffer, sizeof(buffer)) == buffer);
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\"\n", DIMINUTO_LOG_HERE, buffer, "::1");
        EXPECT(strcmp(buffer, "::1") == 0);

        STATUS();
    }

    {
        diminuto_ipv6_t address6;
        char buffer[sizeof("XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX")];

        TEST();

        address6 = diminuto_ipc6_address("localhost");
        DIMINUTO_LOG_DEBUG("%s \"%s\" %4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x\n", DIMINUTO_LOG_HERE, "localhost", address6.u16[0], address6.u16[1], address6.u16[2], address6.u16[3], address6.u16[4], address6.u16[5], address6.u16[6], address6.u16[7]);
        /*
         * This next test depends on how the local host is administered with an
         * IPv6 local host address, or whether we get back a IPv4 local host
         * address encapsulated in an IPv6 address. Either is okay.
         */
        EXPECT((memcmp(&address6, &DIMINUTO_IPC6_LOOPBACK, sizeof(address6)) == 0) || (memcmp(&address6, &DIMINUTO_IPC6_LOOPBACK4, sizeof(address6)) == 0));
        EXPECT(diminuto_ipc6_colonnotation(address6, buffer, sizeof(buffer)) == buffer);
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\"\n", DIMINUTO_LOG_HERE, "localhost", buffer);

        address6 = diminuto_ipc6_address("www.diag.com");
        DIMINUTO_LOG_DEBUG("%s \"%s\" %4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x\n", DIMINUTO_LOG_HERE, "www.diag.com", address6.u16[0], address6.u16[1], address6.u16[2], address6.u16[3], address6.u16[4], address6.u16[5], address6.u16[6], address6.u16[7]);
        EXPECT(memcmp(&address6, &DIMINUTO_IPC6_UNSPECIFIED, sizeof(address6)) != 0);
        EXPECT(diminuto_ipc6_colonnotation(address6, buffer, sizeof(buffer)) == buffer);
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\"\n", DIMINUTO_LOG_HERE, "www.diag.com", buffer);

        address6 = diminuto_ipc6_address("invalid.domain");
        DIMINUTO_LOG_DEBUG("%s \"%s\" %4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x\n", DIMINUTO_LOG_HERE, "invalid.domain", address6.u16[0], address6.u16[1], address6.u16[2], address6.u16[3], address6.u16[4], address6.u16[5], address6.u16[6], address6.u16[7]);
        /*
         * Damned internet service providers map invalid domains to a "help"
         * page. "invalid.domain" becomes 0xd0448f32 a.k.a. 208.68.143.50
         * a.k.a. "search5.comcast.com". That's not helpful!
         */
        ADVISE(memcmp(&address6, &DIMINUTO_IPC6_UNSPECIFIED, sizeof(address6)) == 0);
        EXPECT(diminuto_ipc6_colonnotation(address6, buffer, sizeof(buffer)) == buffer);
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\"\n", DIMINUTO_LOG_HERE, "invalid.domain", buffer);

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
            DIMINUTO_LOG_DEBUG("%s \"%s\" %4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x\n", DIMINUTO_LOG_HERE, "google.com", addresses[ii].u16[0], addresses[ii].u16[1], addresses[ii].u16[2], addresses[ii].u16[3], addresses[ii].u16[4], addresses[ii].u16[5], addresses[ii].u16[6], addresses[ii].u16[7]);
            if (memcmp(&addresses[ii], &DIMINUTO_IPC6_UNSPECIFIED, sizeof(addresses[ii])) == 0) {
                break;
            }
            EXPECT(diminuto_ipc6_colonnotation(addresses[ii], buffer, sizeof(buffer)) == buffer);
            DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\"\n", DIMINUTO_LOG_HERE, "google.com", buffer);
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
            DIMINUTO_LOG_DEBUG("%s \"%s\" %4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x\n", DIMINUTO_LOG_HERE, "amazon.com", addresses[ii].u16[0], addresses[ii].u16[1], addresses[ii].u16[2], addresses[ii].u16[3], addresses[ii].u16[4], addresses[ii].u16[5], addresses[ii].u16[6], addresses[ii].u16[7]);
            if (memcmp(&addresses[ii], &DIMINUTO_IPC6_UNSPECIFIED, sizeof(addresses[ii])) == 0) {
                break;
            }
            EXPECT(diminuto_ipc6_colonnotation(addresses[ii], buffer, sizeof(buffer)) == buffer);
            DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\"\n", DIMINUTO_LOG_HERE, "amazon.com", buffer);
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

        addresses = diminuto_ipc6_addresses("diag.com");
        ASSERT(addresses != (diminuto_ipv6_t *)0);

        for (ii = 0; ii < LIMIT; ++ii) {
            DIMINUTO_LOG_DEBUG("%s \"%s\" %4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x:%4.4x\n", DIMINUTO_LOG_HERE, "diag.com", addresses[ii].u16[0], addresses[ii].u16[1], addresses[ii].u16[2], addresses[ii].u16[3], addresses[ii].u16[4], addresses[ii].u16[5], addresses[ii].u16[6], addresses[ii].u16[7]);
            if (memcmp(&addresses[ii], &DIMINUTO_IPC6_UNSPECIFIED, sizeof(addresses[ii])) == 0) {
                break;
            }
            EXPECT(diminuto_ipc6_colonnotation(addresses[ii], buffer, sizeof(buffer)) == buffer);
            DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\"\n", DIMINUTO_LOG_HERE, "diag.com", buffer);
        }
        EXPECT(ii > 0);
        EXPECT(ii < LIMIT);

        free(addresses);

        STATUS();
    }

    {
        diminuto_port_t port;

        TEST();

        port = diminuto_ipc6_port("80", NULL);
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "80", "(null)", port, 80);
        EXPECT(port == 80);

        port = diminuto_ipc6_port("80", "tcp");
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "80", "tcp", port, 80);
        EXPECT(port == 80);

        port = diminuto_ipc6_port("80", "udp");
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "80", "udp", port, 80);
        EXPECT(port == 80);

        port = diminuto_ipc6_port("http", NULL);
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "http", "(null)", port, 80);
        EXPECT(port == 80);    

        port = diminuto_ipc6_port("http", "tcp");
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "http", "tcp", port, 80);
        EXPECT(port == 80);

        port = diminuto_ipc6_port("tftp", "udp");
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "tftp", "udp", port, 69);
        EXPECT(port == 69);

        port = diminuto_ipc6_port("login", NULL);
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "login", "(null)", port, 513);
        EXPECT(port == 513);

        port = diminuto_ipc6_port("login", "tcp");
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "login", "tcp", port, 513);
        EXPECT(port == 513);

        port = diminuto_ipc6_port("login", "udp");
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "login", "udp", port, 0);
        EXPECT(port == 0);

        port = diminuto_ipc6_port("who", NULL);
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "who", "(null)", port, 513);
        EXPECT(port == 513);

        port = diminuto_ipc6_port("who", "tcp");
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "who", "tcp", port, 0);
        EXPECT(port == 0);

        port = diminuto_ipc6_port("who", "udp");
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "who", "udp", port, 513);
        EXPECT(port == 513);

        port = diminuto_ipc6_port("unknown", NULL);
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "unknown", "(null)", port, 0);
        EXPECT(port == 0);

        port = diminuto_ipc6_port("unknown", "tcp");
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "unknown", "tcp", port, 0);
        EXPECT(port == 0);

        port = diminuto_ipc6_port("unknown", "udp");
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "unknown", "udp", port, 0);
        EXPECT(port == 0);

        STATUS();
    }

    {
        int fd;
        diminuto_ipv6_t address6 = TEST_INIT;
        diminuto_port_t port = TEST_PORT;
        char buffer[sizeof("XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX")];

        TEST();

        EXPECT((fd = diminuto_ipc6_datagram_peer(0)) >= 0);
        EXPECT(diminuto_ipc6_nearend(fd, &address6, &port) == 0);
        EXPECT(memcmp(&address6, &TEST6, sizeof(address6)) != 0);
        EXPECT(port != TEST_PORT);
        EXPECT(diminuto_ipc6_close(fd) >= 0);
        EXPECT(diminuto_ipc6_colonnotation(address6, buffer, sizeof(buffer)) == buffer);
        DIMINUTO_LOG_DEBUG("%s \"%s\" %u\n", DIMINUTO_LOG_HERE, buffer, port);

        STATUS();
    }

    {
        int fd;
        diminuto_ipv6_t address6 = TEST_INIT;
        diminuto_port_t port = TEST_PORT;
        char buffer[sizeof("XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX:XXXX")];

        TEST();

        EXPECT((fd = diminuto_ipc6_stream_provider(0)) >= 0);
        EXPECT(diminuto_ipc6_nearend(fd, &address6, &port) == 0);
        EXPECT(memcmp(&address6, &TEST6, sizeof(address6)) != 0);
        EXPECT(port != TEST_PORT);
        EXPECT(port != 0);
        EXPECT(diminuto_ipc6_close(fd) >= 0);
        EXPECT(diminuto_ipc6_colonnotation(address6, buffer, sizeof(buffer)) == buffer);
        DIMINUTO_LOG_DEBUG("%s \"%s\" %u\n", DIMINUTO_LOG_HERE, buffer, port);

        STATUS();
    }

    {
        int fd;

        TEST();

        EXPECT((fd = diminuto_ipc6_stream_provider(0)) >= 0);

        EXPECT(diminuto_ipc6_set_nonblocking(fd, !0) >= 0);
        EXPECT(diminuto_ipc6_set_nonblocking(fd, 0) >= 0);

        EXPECT(diminuto_ipc6_set_reuseaddress(fd, 0) >= 0);
        EXPECT(diminuto_ipc6_set_reuseaddress(fd, !0) >= 0);

        EXPECT(diminuto_ipc6_set_keepalive(fd, !0) >= 0);
        EXPECT(diminuto_ipc6_set_keepalive(fd, 0) >= 0);

        if (geteuid() == 0) {
            EXPECT(diminuto_ipc6_set_debug(fd, !0) >= 0);
            EXPECT(diminuto_ipc6_set_debug(fd, 0) >= 0);
        }

        EXPECT(diminuto_ipc6_set_linger(fd, hertz) >= 0);
        EXPECT(diminuto_ipc6_set_linger(fd, 0) >= 0);

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

        server = diminuto_ipc6_address("localhost");
        EXPECT(memcmp(&server, &DIMINUTO_IPC6_UNSPECIFIED, sizeof(server)) != 0);
        EXPECT((fd = diminuto_ipc6_datagram_peer(0)) >= 0);
        EXPECT(diminuto_ipc6_nearend(fd, (diminuto_ipv6_t *)0, &rendezvous) == 0);
        EXPECT(rendezvous != TEST_PORT);

        /* This only works because the kernel buffers socket data. */

        EXPECT((diminuto_ipc6_datagram_send(fd, MSG, sizeof(MSG), server, rendezvous)) == sizeof(MSG));
        EXPECT((diminuto_ipc6_datagram_receive(fd, buffer, sizeof(buffer), &address, &port)) == sizeof(MSG));
        EXPECT(strcmp(buffer, MSG) == 0);
        ADVISE(memcmp(&address, &server, sizeof(address)) == 0);
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

        TEST();

        server = diminuto_ipc6_address("::1");
        EXPECT(memcmp(&server, &DIMINUTO_IPC6_UNSPECIFIED, sizeof(server)) != 0);
        EXPECT((fd = diminuto_ipc6_datagram_peer(0)) >= 0);
        EXPECT(diminuto_ipc6_nearend(fd, (diminuto_ipv6_t *)0, &rendezvous) == 0);
        EXPECT(rendezvous != 0);

        /* This only works because the kernel buffers socket data. */

        EXPECT((diminuto_ipc6_datagram_send(fd, MSG, sizeof(MSG), server, rendezvous)) == sizeof(MSG));
        EXPECT((diminuto_ipc6_datagram_receive(fd, buffer, sizeof(buffer), &address, &port)) == sizeof(MSG));
        EXPECT(strcmp(buffer, MSG) == 0);
        ADVISE(memcmp(&address, &server, sizeof(address)) == 0);
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

        TEST();

        server = diminuto_ipc6_address("::ffff:127.0.0.1");
        EXPECT(memcmp(&server, &DIMINUTO_IPC6_UNSPECIFIED, sizeof(server)) != 0);
        EXPECT((fd = diminuto_ipc6_datagram_peer(0)) >= 0);
        EXPECT(diminuto_ipc6_nearend(fd, (diminuto_ipv6_t *)0, &rendezvous) == 0);
        EXPECT(rendezvous != 0);

        /* This only works because the kernel buffers socket data. */

        EXPECT((diminuto_ipc6_datagram_send(fd, MSG, sizeof(MSG), server, rendezvous)) == sizeof(MSG));
        EXPECT((diminuto_ipc6_datagram_receive(fd, buffer, sizeof(buffer), &address, &port)) == sizeof(MSG));
        EXPECT(strcmp(buffer, MSG) == 0);
        ADVISE(memcmp(&address, &server, sizeof(address)) == 0);
        EXPECT(port == rendezvous);

        EXPECT(diminuto_ipc6_close(fd) >= 0);

        STATUS();
    }

    {
        int fd;

        TEST();

        EXPECT((fd = diminuto_ipc6_stream_consumer(diminuto_ipc6_address("www.diag.com"), diminuto_ipc6_port("http", NULL))) >= 0);
        EXPECT(diminuto_ipc6_close(fd) >= 0);

        EXPECT((fd = diminuto_ipc6_stream_consumer(diminuto_ipc6_address("www.amazon.com"), diminuto_ipc6_port("http", NULL))) >= 0);
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
        EXPECT(diminuto_ipc6_set_nonblocking(fd, !0) >= 0);
        EXPECT((diminuto_ipc6_datagram_receive(fd, buffer, sizeof(buffer), &address, &port)) < 0);
        EXPECT(errno == EAGAIN);
        EXPECT(memcmp(&address, &TEST6, sizeof(address)) == 0);
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
        EXPECT((diminuto_ipc6_datagram_receive_flags(fd, buffer, sizeof(buffer), &address, &port, MSG_DONTWAIT)) < 0);
        EXPECT(errno == EAGAIN);
        EXPECT(memcmp(&address, &TEST6, sizeof(address)) == 0);
        EXPECT(port == TEST_PORT);
        EXPECT(diminuto_ipc6_close(fd) >= 0);

        STATUS();
    }

    {
        int fd;
        char buffer[1];
        diminuto_ipv6_t address = TEST_INIT;
        diminuto_port_t port = TEST_PORT;
        diminuto_ticks_t before;
        diminuto_ticks_t after;

        TEST();

        EXPECT((fd = diminuto_ipc6_datagram_peer(0)) >= 0);
        EXPECT(diminuto_alarm_install(0) >= 0);
        diminuto_timer_oneshot(2000000ULL);
        before = diminuto_time_elapsed();
        EXPECT((diminuto_ipc6_datagram_receive(fd, buffer, sizeof(buffer), &address, &port)) < 0);
        after = diminuto_time_elapsed();
        EXPECT(diminuto_alarm_check());
        EXPECT((after - before) >= 2000000LL);
        EXPECT(errno == EINTR);
        EXPECT(memcmp(&address, &TEST6, sizeof(address)) == 0);
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

        EXPECT((service = diminuto_ipc6_stream_provider(0)) >= 0);
        EXPECT(diminuto_ipc6_nearend(service, (diminuto_ipv6_t *)0, &rendezvous) >= 0);
        EXPECT(rendezvous != TEST_PORT);

        EXPECT((pid = fork()) >= 0);

        if (pid != 0) {

            int producer;
            int status;

            memcpy(&address, &TEST6, sizeof(address));
            port = TEST_PORT;
            EXPECT((producer = diminuto_ipc6_stream_accept(service, &address, &port)) >= 0);
            EXPECT(memcmp(&address, &TEST6, sizeof(address)) != 0);
            EXPECT(port != TEST_PORT);
            EXPECT(port != rendezvous);

            memcpy(&address, &TEST6, sizeof(address));
            port = TEST_PORT;
            EXPECT(diminuto_ipc6_nearend(producer, &address, &port) >= 0);
            EXPECT(memcmp(&address, &TEST6, sizeof(address)) != 0);
            EXPECT(port != TEST_PORT);
            EXPECT(port == rendezvous);

            memcpy(&address, &TEST6, sizeof(address));
            port = TEST_PORT;
            EXPECT(diminuto_ipc6_farend(producer, &address, &port) >= 0);
            EXPECT(memcmp(&address, &TEST6, sizeof(address)) != 0);
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
            EXPECT(WIFEXITED(status));
            EXPECT(WEXITSTATUS(status) == 0);

        } else {

            int consumer;

            EXPECT(diminuto_ipc6_close(service) >= 0);
            diminuto_delay(hertz / 1000, !0);
            EXPECT((consumer = diminuto_ipc6_stream_consumer(diminuto_ipc6_address("localhost"), rendezvous)) >= 0);
            diminuto_delay(hertz / 1000, !0);
            EXPECT(diminuto_ipc6_close(consumer) >= 0);

            exit(0);

        }

        STATUS();
    }

    /*
     * There's a subtle design flaw in the next unit test which I first ran
     * first about fifteen years ago when doing socket programming under SunOS.
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
        diminuto_ipv6_t address;
        diminuto_port_t port;
        diminuto_port_t rendezvous;
        int service;
        pid_t pid;

        TEST();

        ASSERT((service = diminuto_ipc6_stream_provider(0)) >= 0);
        EXPECT(diminuto_ipc6_nearend(service, (diminuto_ipv6_t *)0, &rendezvous) >= 0);

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
            uint8_t * here;
            uint8_t * there;
            uint8_t * current;
            size_t totalsent;
            size_t totalreceived;
            int status;

            ASSERT((producer = diminuto_ipc6_stream_accept(service, &address, &port)) >= 0);

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

                    ASSERT((sent = diminuto_ipc6_stream_write(producer, here, 1, used)) > 0);
                    ASSERT(sent <= used);

                    totalsent += sent;
                    DIMINUTO_LOG_DEBUG("producer sent %d %u\n", sent, totalsent);

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

                ASSERT((received = diminuto_ipc6_stream_read(producer, there, 1, available)) > 0);
                ASSERT(received <= available);

                totalreceived += received;
                DIMINUTO_LOG_DEBUG("producer received %d %u\n", received, totalreceived);

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

            ASSERT(diminuto_ipc6_close(producer) >= 0);
            ASSERT(diminuto_ipc6_close(service) >= 0);

            EXPECT(waitpid(pid, &status, 0) == pid);
            EXPECT(WIFEXITED(status));
            EXPECT(WEXITSTATUS(status) == 0);

        } else {

            int consumer;
            uint8_t buffer[64];
            ssize_t sent;
            ssize_t received;
            size_t totalsent;
            size_t totalreceived;

            ASSERT(diminuto_ipc6_close(service) >= 0);

            diminuto_delay(hertz / 1000, !0);

            ASSERT((consumer = diminuto_ipc6_stream_consumer(diminuto_ipc6_address("localhost"), rendezvous)) >= 0);

            totalreceived = 0;
            totalsent = 0;

            while (!0) {

                ASSERT((received = diminuto_ipc6_stream_read(consumer, buffer, 1, sizeof(buffer))) >= 0);
                ASSERT(received <= sizeof(buffer));

                totalreceived += received;
                DIMINUTO_LOG_DEBUG("consumer received %d %u\n", received, totalreceived);

                if (received == 0) {
                    break;
                }

                sent = 0;
                while (sent < received) {
                    ASSERT((sent = diminuto_ipc6_stream_write(consumer,  buffer + sent, 1, received - sent)) > 0);
                    ASSERT(sent <= received);

                    totalsent += sent;
                    DIMINUTO_LOG_DEBUG("consumer sent %d %u\n", sent, totalsent);

                    received -= sent;
                }
            }

            ASSERT(diminuto_ipc6_close(consumer) >= 0);

            exit(0);
        }

        STATUS();
    }

    EXIT();
}
