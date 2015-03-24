/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010-2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_ipc.h"
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

static const diminuto_ipv4_t LOCALHOST = 0x7f000001UL;
static const size_t LIMIT = 256;
static const diminuto_port_t PORT = 0xfff0;
static const diminuto_port_t PORT1 = 65535;
static const diminuto_port_t PORT2 = 65534;
static const size_t TOTAL = 1024 * 1024 * 100;

int main(void)
{
    diminuto_ticks_t hertz;

    SETLOGMASK();

	hertz = diminuto_frequency();

	{
        diminuto_ipv4_t address;
        char buffer[sizeof("NNN.NNN.NNN.NNN")] = { 0 };

        TEST();

        address = diminuto_ipc_address("127.0.0.1");
        DIMINUTO_LOG_DEBUG("%s \"%s\" 0x%8.8x 0x%8.8x\n", DIMINUTO_LOG_HERE, "127.0.0.1", address, LOCALHOST);
        EXPECT(address == LOCALHOST);

        EXPECT(diminuto_ipc_dotnotation(address, buffer, sizeof(buffer)) == buffer);
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\"\n", DIMINUTO_LOG_HERE, buffer, "127.0.0.1");
        EXPECT(strcmp(buffer, "127.0.0.1") == 0);

        STATUS();
	}

    {
        diminuto_ipv4_t address;

        TEST();

        address = diminuto_ipc_address("localhost");
        DIMINUTO_LOG_DEBUG("%s \"%s\" 0x%8.8x 0x%8.8x\n", DIMINUTO_LOG_HERE, "localhost", address, LOCALHOST);
        EXPECT(address == LOCALHOST);

        address = diminuto_ipc_address("www.diag.com");
        DIMINUTO_LOG_DEBUG("%s \"%s\" 0x%8.8x 0x%8.8x\n", DIMINUTO_LOG_HERE, "www.diag.com", address, 0UL);
        EXPECT(address != 0UL);

        address = diminuto_ipc_address("invalid.domain");
        DIMINUTO_LOG_DEBUG("%s \"%s\" 0x%8.8x 0x%8.8x\n", DIMINUTO_LOG_HERE, "invalid.domain", address, 0UL);

        /*
         * Damned internet service providers map invalid domains to a "help"
         * page. "invalid.domain" becomes 0xd0448f32 a.k.a. 208.68.143.50
         * a.k.a. "search5.comcast.com". That's not helpful!
         */
        ADVISE(address == 0UL);

        STATUS();
    }

    {
        diminuto_ipv4_t * addresses;
        size_t ii;

        TEST();

        addresses = diminuto_ipc_addresses("google.com");
        ASSERT(addresses != (diminuto_ipv4_t *)0);

        for (ii = 0; ii < LIMIT; ++ii) {
            DIMINUTO_LOG_DEBUG("%s \"%s\" 0x%8.8x 0x%8.8x\n", DIMINUTO_LOG_HERE, "google.com", addresses[ii], 0UL);
            if (addresses[ii] == 0UL) {
                break;
            }
        }
        /*
         * "nslookup google.com" used to resolve as multiple addresses. But
         * today it doesn't. So I added the "amazon.com" test below. So far
         * Amazon.com still resolves as multiple addresses.
         */
        EXPECT(ii > 0);
        EXPECT(ii < LIMIT);

    	free(addresses);

        STATUS();
    }

    {
        diminuto_ipv4_t * addresses;
        size_t ii;

        TEST();

        addresses = diminuto_ipc_addresses("amazon.com");
        ASSERT(addresses != (diminuto_ipv4_t *)0);

        for (ii = 0; ii < LIMIT; ++ii) {
            DIMINUTO_LOG_DEBUG("%s \"%s\" 0x%8.8x 0x%8.8x\n", DIMINUTO_LOG_HERE, "amazon.com", addresses[ii], 0UL);
            if (addresses[ii] == 0UL) {
                break;
            }
        }
        EXPECT(ii > 1);
        EXPECT(ii < LIMIT);

    	free(addresses);

        STATUS();
    }

    {
        diminuto_ipv4_t * addresses;
        size_t ii;

        TEST();

        addresses = diminuto_ipc_addresses("www.diag.com");
        ASSERT(addresses != (diminuto_ipv4_t *)0);

        for (ii = 0; ii < LIMIT; ++ii) {
            DIMINUTO_LOG_DEBUG("%s \"%s\" 0x%8.8x 0x%8.8x\n", DIMINUTO_LOG_HERE, "www.diag.com", addresses[ii], 0UL);
            if (addresses[ii] == 0UL) {
                break;
            }
        }
        EXPECT(ii == 1);

    	free(addresses);

        STATUS();
    }

    {
        diminuto_port_t port;

        TEST();

        port = diminuto_ipc_port("80", NULL);
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "80", "(null)", port, 80);
        EXPECT(port == 80);

        port = diminuto_ipc_port("80", "tcp");
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "80", "tcp", port, 80);
        EXPECT(port == 80);

        port = diminuto_ipc_port("80", "udp");
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "80", "udp", port, 80);
        EXPECT(port == 80);

        port = diminuto_ipc_port("http", NULL);
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "http", "(null)", port, 80);
        EXPECT(port == 80);    

        port = diminuto_ipc_port("http", "tcp");
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "http", "tcp", port, 80);
        EXPECT(port == 80);

        port = diminuto_ipc_port("tftp", "udp");
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "tftp", "udp", port, 69);
        EXPECT(port == 69);

        port = diminuto_ipc_port("login", NULL);
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "login", "(null)", port, 513);
        EXPECT(port == 513);

        port = diminuto_ipc_port("login", "tcp");
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "login", "tcp", port, 513);
        EXPECT(port == 513);

        port = diminuto_ipc_port("login", "udp");
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "login", "udp", port, 0);
        EXPECT(port == 0);

        port = diminuto_ipc_port("who", NULL);
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "who", "(null)", port, 513);
        EXPECT(port == 513);

        port = diminuto_ipc_port("who", "tcp");
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "who", "tcp", port, 0);
        EXPECT(port == 0);

        port = diminuto_ipc_port("who", "udp");
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "who", "udp", port, 513);
        EXPECT(port == 513);

        port = diminuto_ipc_port("unknown", NULL);
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "unknown", "(null)", port, 0);
        EXPECT(port == 0);

        port = diminuto_ipc_port("unknown", "tcp");
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "unknown", "tcp", port, 0);
        EXPECT(port == 0);

        port = diminuto_ipc_port("unknown", "udp");
        DIMINUTO_LOG_DEBUG("%s \"%s\" \"%s\" %d %d\n", DIMINUTO_LOG_HERE, "unknown", "udp", port, 0);
        EXPECT(port == 0);

        STATUS();
    }

    {
        int fd;

        TEST();

        EXPECT((fd = diminuto_ipc_datagram_peer(PORT1)) >= 0);
        EXPECT(diminuto_ipc_close(fd) >= 0);

        STATUS();
    }

    {
        int fd;

        TEST();

        EXPECT((fd = diminuto_ipc_stream_consumer(diminuto_ipc_address("www.diag.com"), diminuto_ipc_port("http", NULL))) >= 0);
        EXPECT(diminuto_ipc_close(fd) >= 0);

        EXPECT((fd = diminuto_ipc_stream_consumer(diminuto_ipc_address("www.amazon.com"), diminuto_ipc_port("http", NULL))) >= 0);
        EXPECT(diminuto_ipc_close(fd) >= 0);

        STATUS();
    }

    {
        int fd;

        TEST();

        EXPECT((fd = diminuto_ipc_stream_provider(PORT)) >= 0);
        EXPECT(diminuto_ipc_close(fd) >= 0);

        STATUS();
    }

    {
        int fd;

        TEST();

        EXPECT((fd = diminuto_ipc_stream_provider(PORT)) >= 0);

        EXPECT(diminuto_ipc_set_nonblocking(fd, !0) >= 0);
        EXPECT(diminuto_ipc_set_nonblocking(fd, 0) >= 0);

        EXPECT(diminuto_ipc_set_reuseaddress(fd, 0) >= 0);
        EXPECT(diminuto_ipc_set_reuseaddress(fd, !0) >= 0);

        EXPECT(diminuto_ipc_set_keepalive(fd, !0) >= 0);
        EXPECT(diminuto_ipc_set_keepalive(fd, 0) >= 0);

        if (geteuid() == 0) {
            EXPECT(diminuto_ipc_set_debug(fd, !0) >= 0);
            EXPECT(diminuto_ipc_set_debug(fd, 0) >= 0);
        }

        EXPECT(diminuto_ipc_set_linger(fd, hertz) >= 0);
        EXPECT(diminuto_ipc_set_linger(fd, 0) >= 0);

        EXPECT(diminuto_ipc_close(fd) >= 0);

        STATUS();
    }

    {
        int fd1;
        int fd2;
        const char MSG1[] = "Chip Overclock";
        const char MSG2[] = "Digital Aggregates Corporation";
        char buffer[64];
        diminuto_ipv4_t address = 0;
        diminuto_port_t port = 0;

        TEST();

        EXPECT((fd1 = diminuto_ipc_datagram_peer(PORT1)) >= 0);
        EXPECT((fd2 = diminuto_ipc_datagram_peer(PORT2)) >= 0);

        /* This only works because the kernel buffers socket data. */

        EXPECT((diminuto_ipc_datagram_send(fd1, MSG1, sizeof(MSG1), diminuto_ipc_address("localhost"), PORT2)) == sizeof(MSG1));
        EXPECT((diminuto_ipc_datagram_receive(fd2, buffer, sizeof(buffer), &address, &port)) == sizeof(MSG1));
        EXPECT(address == diminuto_ipc_address("localhost"));
        EXPECT(port == PORT1);
        EXPECT(strcmp(buffer, MSG1) == 0);

        EXPECT((diminuto_ipc_datagram_send(fd2, MSG2, sizeof(MSG2), diminuto_ipc_address("localhost"), PORT1)) == sizeof(MSG2));
        EXPECT((diminuto_ipc_datagram_receive(fd1, buffer, sizeof(buffer), &address, &port)) == sizeof(MSG2));
        EXPECT(address == diminuto_ipc_address("localhost"));
        EXPECT(port == PORT2);
        EXPECT(strcmp(buffer, MSG2) == 0);

        EXPECT(diminuto_ipc_close(fd1) >= 0);
        EXPECT(diminuto_ipc_close(fd2) >= 0);

        STATUS();
    }

    {
        int fd;
        char buffer[1];
        diminuto_ipv4_t address = 0x12345678;
        diminuto_port_t port = 0x9abc;

        TEST();

        EXPECT((fd = diminuto_ipc_datagram_peer(PORT1)) >= 0);
        EXPECT(diminuto_ipc_set_nonblocking(fd, !0) >= 0);
        EXPECT((diminuto_ipc_datagram_receive(fd, buffer, sizeof(buffer), &address, &port)) < 0);
        EXPECT(errno == EAGAIN);
        EXPECT(address == 0x12345678);
        EXPECT(port == 0x9abc);
        EXPECT(diminuto_ipc_close(fd) >= 0);

        STATUS();
    }

    {
        int fd;
        char buffer[1];
        diminuto_ipv4_t address = 0x12345678;
        diminuto_port_t port = 0x9abc;

        TEST();

        EXPECT((fd = diminuto_ipc_datagram_peer(PORT1)) >= 0);
        EXPECT((diminuto_ipc_datagram_receive_flags(fd, buffer, sizeof(buffer), &address, &port, MSG_DONTWAIT)) < 0);
        EXPECT(errno == EAGAIN);
        EXPECT(address == 0x12345678);
        EXPECT(port == 0x9abc);
        EXPECT(diminuto_ipc_close(fd) >= 0);

        STATUS();
    }

    {
        int fd;
        char buffer[1];
        diminuto_ipv4_t address = 0x12345678;
        diminuto_port_t port = 0x9abc;
        diminuto_ticks_t before;
        diminuto_ticks_t after;

        TEST();

        EXPECT((fd = diminuto_ipc_datagram_peer(PORT1)) >= 0);
        EXPECT(diminuto_alarm_install(0) >= 0);
        diminuto_timer_oneshot(2000000ULL);
        before = diminuto_time_elapsed();
        EXPECT((diminuto_ipc_datagram_receive(fd, buffer, sizeof(buffer), &address, &port)) < 0);
        after = diminuto_time_elapsed();
        EXPECT(diminuto_alarm_check());
        EXPECT((after - before) >= 2000000LL);
        EXPECT(errno == EINTR);
        EXPECT(address == 0x12345678);
        EXPECT(port == 0x9abc);
        EXPECT(diminuto_ipc_close(fd) >= 0);

        STATUS();
    }

    {
        diminuto_ipv4_t address;
        diminuto_port_t port;
        int rendezvous;
        pid_t pid;

        EXPECT((rendezvous = diminuto_ipc_stream_provider(PORT)) >= 0);

        EXPECT((pid = fork()) >= 0);

        if (pid != 0) {

            int producer;
            int status;

            address = 0;
            port = (diminuto_port_t)-1;
            EXPECT((producer = diminuto_ipc_stream_accept(rendezvous, &address, &port)) >= 0);
            EXPECT(address == LOCALHOST);
            EXPECT(port != (diminuto_port_t)-1);
            EXPECT(port != PORT);

            address = 0;
            port = -(diminuto_port_t)1;
            EXPECT(diminuto_ipc_nearend(producer, &address, &port) >= 0);
            EXPECT(address == LOCALHOST);
            EXPECT(port == PORT);

            address = 0;
            port = (diminuto_port_t)-1;
            EXPECT(diminuto_ipc_farend(producer, &address, &port) >= 0);
            EXPECT(address == LOCALHOST);
            EXPECT(port != (diminuto_port_t)-1);
            EXPECT(port != PORT);

            diminuto_delay(hertz / 1000, !0);

    		EXPECT(diminuto_ipc_close(producer) >= 0);
    		EXPECT(diminuto_ipc_close(rendezvous) >= 0);

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

    		EXPECT(diminuto_ipc_close(rendezvous) >= 0);

            diminuto_delay(hertz / 1000, !0);

            EXPECT((consumer = diminuto_ipc_stream_consumer(diminuto_ipc_address("localhost"), PORT)) >= 0);

            diminuto_delay(hertz / 1000, !0);

    		EXPECT(diminuto_ipc_close(consumer) >= 0);

            exit(0);

        }

        STATUS();
    }

    {
        diminuto_ipv4_t address;
        diminuto_port_t port;
        int rendezvous;
        pid_t pid;

        TEST();

        EXPECT((rendezvous = diminuto_ipc_stream_provider(PORT)) >= 0);

        EXPECT((pid = fork()) >= 0);

        if (pid == 0) {

            int producer;

            EXPECT((producer = diminuto_ipc_stream_accept(rendezvous, &address, &port)) >= 0);

            diminuto_delay(hertz / 1000, !0);

    		EXPECT(diminuto_ipc_close(producer) >= 0);
    		EXPECT(diminuto_ipc_close(rendezvous) >= 0);

            exit(0);

        } else {

            int consumer;
            int status;

    		EXPECT(diminuto_ipc_close(rendezvous) >= 0);

            diminuto_delay(hertz / 1000, !0);

            EXPECT((consumer = diminuto_ipc_stream_consumer(diminuto_ipc_address("localhost"), PORT)) >= 0);

            address = 0;
            port = (diminuto_port_t)-1;
            EXPECT(diminuto_ipc_farend(consumer, &address, &port) >= 0);
            EXPECT(address == LOCALHOST);
            EXPECT(port == PORT);

            address = 0;
            port = (diminuto_port_t)-1;
            EXPECT(diminuto_ipc_nearend(consumer, &address, &port) >= 0);
            EXPECT(address == LOCALHOST);
            EXPECT(port != (diminuto_port_t)-1);
            EXPECT(port != PORT);

    		EXPECT(diminuto_ipc_close(consumer) >= 0);

    		/*
    		 * If you don't wait for the child to exit, it may not yet have
    		 * closed its end of the socket thereby releasing the bound IP
    		 * address by the time the next unit test begins.
    		 */

            EXPECT(waitpid(pid, &status, 0) == pid);
            EXPECT(WIFEXITED(status));
            EXPECT(WEXITSTATUS(status) == 0);
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
     * uses select(2) to multiplex the diminuto_ipc feature in just this way.
     */

    {
        diminuto_ipv4_t address;
        diminuto_port_t port;
        int rendezvous;
        pid_t pid;

        TEST();

        ASSERT((rendezvous = diminuto_ipc_stream_provider(PORT)) >= 0);

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

            ASSERT((producer = diminuto_ipc_stream_accept(rendezvous, &address, &port)) >= 0);

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

					ASSERT((sent = diminuto_ipc_stream_write(producer, here, 1, used)) > 0);
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

				ASSERT((received = diminuto_ipc_stream_read(producer, there, 1, available)) > 0);
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

    		ASSERT(diminuto_ipc_close(producer) >= 0);
    		ASSERT(diminuto_ipc_close(rendezvous) >= 0);

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

    		ASSERT(diminuto_ipc_close(rendezvous) >= 0);

            diminuto_delay(hertz / 1000, !0);

            ASSERT((consumer = diminuto_ipc_stream_consumer(diminuto_ipc_address("localhost"), PORT)) >= 0);

            totalreceived = 0;
            totalsent = 0;

            while (!0) {

				ASSERT((received = diminuto_ipc_stream_read(consumer, buffer, 1, sizeof(buffer))) >= 0);
				ASSERT(received <= sizeof(buffer));

				totalreceived += received;
				DIMINUTO_LOG_DEBUG("consumer received %d %u\n", received, totalreceived);

				if (received == 0) {
					break;
				}

				sent = 0;
				while (sent < received) {
					ASSERT((sent = diminuto_ipc_stream_write(consumer,  buffer + sent, 1, received - sent)) > 0);
					ASSERT(sent <= received);

					totalsent += sent;
					DIMINUTO_LOG_DEBUG("consumer sent %d %u\n", sent, totalsent);

					received -= sent;
				}
			}

    		ASSERT(diminuto_ipc_close(consumer) >= 0);

    		exit(0);
        }

        STATUS();
    }

    EXIT();
}
