/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_ipc.h"
#include "diminuto_time.h"
#include "diminuto_timer.h"
#include "diminuto_delay.h"
#include "diminuto_unittest.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>

static const diminuto_ipv4_t LOCALHOST = 0x7f000001UL;
static const diminuto_port_t PORT = 0xfff0;
static const diminuto_port_t PORT1 = 65535;
static const diminuto_port_t PORT2 = 65534;

int main(void)
{
    {
        diminuto_ipv4_t address;
        char buffer[sizeof("NNN.NNN.NNN.NNN")] = { 0 };

        address = diminuto_ipc_address("127.0.0.1");
        EXPECT(address == LOCALHOST);

        EXPECT(diminuto_ipc_dotnotation(address, buffer, sizeof(buffer)) == buffer);
        EXPECT(strcmp(buffer, "127.0.0.1") == 0);
    }

    {
        diminuto_ipv4_t address;

        address = diminuto_ipc_address("localhost");
        EXPECT(address == LOCALHOST);

        address = diminuto_ipc_address("www.diag.com");
        EXPECT(address != 0UL);

        address = diminuto_ipc_address("invalid.domain");
#if 0
        /*
         * Damned internet service providers map invalid domains to a "help"
         * page. "invalid.domain" becomes 0xd0448f32 a.k.a. 208.68.143.50
         * a.k.a. "search5.comcast.com". That's not helpful!
         */
        EXPECT(address == 0UL);
#endif
    }

    {
        diminuto_port_t port;

        port = diminuto_ipc_port("80", NULL);
        EXPECT(port == 80);

        port = diminuto_ipc_port("80", "tcp");
        EXPECT(port == 80);

        port = diminuto_ipc_port("80", "udp");
        EXPECT(port == 80);

        port = diminuto_ipc_port("http", NULL);
        EXPECT(port == 80);    

        port = diminuto_ipc_port("http", "tcp");
        EXPECT(port == 80);

        port = diminuto_ipc_port("tftp", "udp");
        EXPECT(port == 69);

        port = diminuto_ipc_port("login", "tcp");
        EXPECT(port == 513);

        port = diminuto_ipc_port("login", "tcp");
        EXPECT(port == 513);

        port = diminuto_ipc_port("login", "udp");
        EXPECT(port == 0);

        port = diminuto_ipc_port("who", NULL);
        EXPECT(port == 513);

        port = diminuto_ipc_port("who", "tcp");
        EXPECT(port == 0);

        port = diminuto_ipc_port("who", "udp");
        EXPECT(port == 513);

        port = diminuto_ipc_port("unknown", NULL);
        EXPECT(port == 0);

        port = diminuto_ipc_port("unknown", "tcp");
        EXPECT(port == 0);

        port = diminuto_ipc_port("unknown", "udp");
        EXPECT(port == 0);

    }

    {
        int fd;

        EXPECT((fd = diminuto_ipc_datagram_peer(PORT1)) >= 0);
        EXPECT(diminuto_ipc_close(fd) >= 0);
    }

    {
        int fd;

        EXPECT((fd = diminuto_ipc_stream_consumer(diminuto_ipc_address("www.diag.com"), diminuto_ipc_port("http", NULL))) >= 0);
        EXPECT(diminuto_ipc_close(fd) >= 0);

        EXPECT((fd = diminuto_ipc_stream_consumer(diminuto_ipc_address("www.amazon.com"), diminuto_ipc_port("http", NULL))) >= 0);
        EXPECT(diminuto_ipc_close(fd) >= 0);
    }

    {
        int fd;
    
        EXPECT((fd = diminuto_ipc_stream_provider(PORT)) >= 0);
        EXPECT(diminuto_ipc_close(fd) >= 0);
    }

    {
        int fd;

        EXPECT((fd = diminuto_ipc_stream_provider(PORT)) >= 0);

        EXPECT(diminuto_ipc_set_nonblocking(fd, !0) >= 0);
        EXPECT(diminuto_ipc_set_nonblocking(fd, 0) >= 0);

        EXPECT(diminuto_ipc_set_reuseaddress(fd, !0) >= 0);
        EXPECT(diminuto_ipc_set_reuseaddress(fd, 0) >= 0);

        EXPECT(diminuto_ipc_set_keepalive(fd, !0) >= 0);
        EXPECT(diminuto_ipc_set_keepalive(fd, 0) >= 0);

        if (geteuid() == 0) {
            EXPECT(diminuto_ipc_set_debug(fd, !0) >= 0);
            EXPECT(diminuto_ipc_set_debug(fd, 0) >= 0);
        }

        EXPECT(diminuto_ipc_set_linger(fd, 1000000UL) >= 0);
        EXPECT(diminuto_ipc_set_linger(fd, 0) >= 0);

        EXPECT(diminuto_ipc_close(fd) >= 0);
    }

    {
        int fd1;
        int fd2;
        const char MSG1[] = "Chip Overclock";
        const char MSG2[] = "Digital Aggregates Corporation";
        char buffer[64];
        diminuto_ipv4_t address = 0;
        diminuto_port_t port = 0;

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
    }

    {
        int fd;
        char buffer[1];
        diminuto_ipv4_t address = 0x12345678;
        diminuto_port_t port = 0x9abc;

        EXPECT((fd = diminuto_ipc_datagram_peer(PORT1)) >= 0);
        EXPECT(diminuto_ipc_set_nonblocking(fd, !0) >= 0);
        EXPECT((diminuto_ipc_datagram_receive(fd, buffer, sizeof(buffer), &address, &port)) < 0);
        EXPECT(errno == EAGAIN);
        EXPECT(address == 0x12345678);
        EXPECT(port == 0x9abc);
        EXPECT(diminuto_ipc_close(fd) >= 0);
    }

    {
        int fd;
        char buffer[1];
        diminuto_ipv4_t address = 0x12345678;
        diminuto_port_t port = 0x9abc;

        EXPECT((fd = diminuto_ipc_datagram_peer(PORT1)) >= 0);
        EXPECT((diminuto_ipc_datagram_receive_flags(fd, buffer, sizeof(buffer), &address, &port, MSG_DONTWAIT)) < 0);
        EXPECT(errno == EAGAIN);
        EXPECT(address == 0x12345678);
        EXPECT(port == 0x9abc);
        EXPECT(diminuto_ipc_close(fd) >= 0);
    }

    {
        int fd;
        char buffer[1];
        diminuto_ipv4_t address = 0x12345678;
        diminuto_port_t port = 0x9abc;
        diminuto_usec_t before;
        diminuto_usec_t after;

        EXPECT((fd = diminuto_ipc_datagram_peer(PORT1)) >= 0);
        EXPECT(diminuto_alarm_install(0) >= 0);
        diminuto_timer_oneshot(2000000ULL);
        before = diminuto_time();
#if 1
        EXPECT((diminuto_ipc_datagram_receive(fd, buffer, sizeof(buffer), &address, &port)) < 0);
#else
        diminuto_delay(5000000ULL, !0);
#endif
        after = diminuto_time();
        EXPECT(diminuto_alarm_check());
        EXPECT((after - before) >= 2000000ULL);
        EXPECT(errno == EINTR);
        EXPECT(address == 0x12345678);
        EXPECT(port == 0x9abc);
        EXPECT(diminuto_ipc_close(fd) >= 0);
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

            address = 0;
            port = -1;
            EXPECT((producer = diminuto_ipc_stream_accept(rendezvous, &address, &port)) >= 0);
            EXPECT(address == LOCALHOST);
            EXPECT(port != -1);
            EXPECT(port != PORT);

            address = 0;
            port = -1;
            EXPECT(diminuto_ipc_nearend(producer, &address, &port) >= 0);
            EXPECT(address == LOCALHOST);
            EXPECT(port == PORT);

            address = 0;
            port = -1;
            EXPECT(diminuto_ipc_farend(producer, &address, &port) >= 0);
            EXPECT(address == LOCALHOST);
            EXPECT(port != -1);
            EXPECT(port != PORT);

            diminuto_delay(1000, !0);

    		EXPECT(diminuto_ipc_close(producer) >= 0);
    		EXPECT(diminuto_ipc_close(rendezvous) >= 0);

        } else {

            int consumer;

    		EXPECT(diminuto_ipc_close(rendezvous) >= 0);

            diminuto_delay(1000, !0);

            EXPECT((consumer = diminuto_ipc_stream_consumer(diminuto_ipc_address("localhost"), PORT)) >= 0);

            diminuto_delay(10000, !0);

    		EXPECT(diminuto_ipc_close(consumer) >= 0);

            exit(0);

        }
    }

    {
        diminuto_ipv4_t address;
        diminuto_port_t port;
        int rendezvous;
        pid_t pid;

        EXPECT((rendezvous = diminuto_ipc_stream_provider(PORT)) >= 0);

        EXPECT((pid = fork()) >= 0);

        if (pid == 0) {

            int producer;

            EXPECT((producer = diminuto_ipc_stream_accept(rendezvous, &address, &port)) >= 0);

            diminuto_delay(1000, !0);

    		EXPECT(diminuto_ipc_close(producer) >= 0);
    		EXPECT(diminuto_ipc_close(rendezvous) >= 0);

            exit(0);

        } else {

            int consumer;

            diminuto_delay(1000, !0);

            EXPECT((consumer = diminuto_ipc_stream_consumer(diminuto_ipc_address("localhost"), PORT)) >= 0);

            address = 0;
            port = -1;
            EXPECT(diminuto_ipc_farend(consumer, &address, &port) >= 0);
            EXPECT(address == LOCALHOST);
            EXPECT(port == PORT);

            address = 0;
            port = -1;
            EXPECT(diminuto_ipc_nearend(consumer, &address, &port) >= 0);
            EXPECT(address == LOCALHOST);
            EXPECT(port != -1);
            EXPECT(port != PORT);
        }
    }

#if 0
    {
        diminuto_ipv4_t address;
        diminuto_port_t port;
        int rendezvous;

        EXPECT((rendezvous = diminuto_ipc_stream_provider(PORT)) >= 0);

        EXPECT((pid = fork()) >= 0);

        if (pid) {

            int producer;
            char buffer[256];
            ssize_t bytes;

            address = 0;
            port = -1;
            EXPECT((producer = diminuto_ipc_stream_accept(rendezvous, &address, &port)) >= 0);
            EXPECT(address == LOCALHOST);
            EXPECT(port != -1);
            EXPECT(port != PORT);

            address = 0;
            port = -1;
            EXPECT(diminuto_ipc_nearend(producer, address, port) >= 0);
            EXPECT(address == LOCALHOST);
            EXPECT(port == PORT);

            address = 0;
            port = -1;
            EXPECT(diminuto_ipc_farend(producer, address, port) >= 0);
            EXPECT(address == LOCALHOST);
            EXPECT(port != -1);
            EXPECT(port != PORT);

        } else {

            int consumer;
            char buffer[256];
            ssize_t bytes;

            EXPECT((consumer = diminuto_ipc_stream_consumer(diminuto_ipc_address("localhost"), PORT)) >= 0);

            address = 0;
            port = -1;
            EXPECT(diminuto_ipc_farend(consumer, address, port) >= 0);
            EXPECT(address == LOCALHOST);
            EXPECT(port == PORT);

            address = 0;
            port = -1;
            EXPECT(diminuto_ipc_nearend(consumer, address, port) >= 0);
            EXPECT(address == LOCALHOST);
            EXPECT(port != -1);
            EXPEXT(port != PORT);
        }


            while (!0) {

				EXPECT((bytes = diminuto_ipc_stream_read(producer, &received, 1, sizeof(buffer))) >= 0);

				if (rc != 1) {
					errorf("%s[%d]: (%u!=%u) (%d)!\n",
						__FILE__, __LINE__, rc, 1, errno);
					++errors;
				}
				if (received != sent) {
					errorf("%s[%d]: (0x%x!=0x%x)!\n",
						__FILE__, __LINE__, received, sent);
					++errors;
				}

				rc = diminuto_ipc_stream_write(consumer,  &sent, 1, 1);
				if (rc != 1) {
					errorf("%s[%d]: (%u!=%u) (%d)!\n",
						__FILE__, __LINE__, rc, 1, errno);
					++errors;
				}
			}


        }

        address = 0;
        port = -1;
        EXPECT((producer = diminuto_ipc_stream_accept(rendezvous, address, port)) >= 0);
        EXPECT(address == LOCALHOST);
        EXPECT(port == PORT);
        EXPECT(diminuto_ipc_dotnotation(address, dotnotation, sizeof(dotnotation)) == dotnotation);
        EXPECT(strcmp(dotnotation, "127.0.0.1") == 0);

        /* This only works because the kernel buffers socket data. */

        for (int index = 0; index < 256; ++index) {

            char sent = index;
            char received = ~sent;

			EXPECT(diminuto_ipc_stream_write(producer, &sent, 1, 1) == 1);

			EXPECT(diminuto_ipc_stream_read(consumer, &received, 1, 1) == 1);
			if (rc != 1) {
				errorf("%s[%d]: (%u!=%u) (%d)!\n",
					__FILE__, __LINE__, rc, 1, errno);
				++errors;
			}
			if (received != sent) {
				errorf("%s[%d]: (0x%x!=0x%x)!\n",
					__FILE__, __LINE__, received, sent);
				++errors;
			}

			sent = index;
			received = ~sent;

			rc = diminuto_ipc_stream_write(consumer,  &sent, 1, 1);
			if (rc != 1) {
				errorf("%s[%d]: (%u!=%u) (%d)!\n",
					__FILE__, __LINE__, rc, 1, errno);
				++errors;
			}

			rc = diminuto_ipc_stream_read(producer, &received, 1, 1);
			if (rc != 1) {
				errorf("%s[%d]: (%u!=%u) (%d)!\n",
					__FILE__, __LINE__, rc, 1, errno);
				++errors;
			}
			if (received != sent) {
				errorf("%s[%d]: (0x%x!=0x%x)!\n",
					__FILE__, __LINE__, received, sent);
				++errors;
			}
		}

		EXPECT(diminuto_ipc_close(producer) >= 0);
		EXPECT(diminuto_ipc_close(consumer) >= 0);

		EXPECT(diminuto_ipc_close(rendezvous) >= 0);
    }
#endif

    EXIT();
}
