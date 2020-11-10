/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the IPC feature for Local sockets.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the IPC feature for Local (UNIX Domain) sockets.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_ipcl.h"
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

static const char LOCAL1[] = "/tmp/one.sock";
static const char LOCAL2[] = "/tmp/two.sock";
static const char UNNAMED[] = "";
static const size_t LIMIT = 256;
static const size_t TOTAL = 1024 * 1024 * 100;

int main(int argc, char * argv[])
{
    diminuto_ticks_t hertz;
    const char * Path = 0;
    extern char * optarg;
    extern int optind;
    extern int opterr;
    extern int optopt;
    int opt;

    SETLOGMASK();

    while ((opt = getopt(argc, argv, "p:")) >= 0) {
        switch (opt) {
        case 'p':
            /* e.g. "/var/run/unix.sock" */
            Path = optarg;
            break;
        default:
            break;
        }
    }

    hertz = diminuto_frequency();

#define CANONICALIZE(_PATH_, _FILE_, _RESULT_) \
    do { \
        static const char file[] = _FILE_; \
        const char * relative = (_PATH_); \
        diminuto_local_buffer_t absolute = { '\0', }; \
        char * result; \
        const char * printable; \
        size_t minimum = 0; \
        size_t actual = 0; \
        minimum = strlen(file); \
        result = diminuto_ipcl_canonicalize(relative, absolute, sizeof(absolute)); \
        actual = strlen(absolute); \
        printable = diminuto_ipcl_path2string(result); \
        EXPECT(printable != (const char *)0); \
        COMMENT("relative=\"%s\" absolute=\"%s\" %s", relative, printable, (result != (char *)0) ? "GOOD" : "BAD"); \
        EXPECT(((_RESULT_) && (result == absolute) && (absolute[0] == '/') && (strlen(absolute) >= minimum) && (strcmp(absolute + actual - minimum, file) == 0)) || ((!(_RESULT_)) && (result == (char *)0) && (absolute[0] == '\0'))); \
    } while (0)

    {
        /*
         * See also the unit tests for FS Canonicalize and IPC Endpoint
         * which test the same underlying realpath(3)-based capability.
         */
        TEST();
        CANONICALIZE("/", "", 0); 
        CANONICALIZE("/.", "", 0); 
        CANONICALIZE("/..", "", 0); 
        CANONICALIZE("/tmp/", "", 0); 
        CANONICALIZE("/var/tmp/", "", 0); 
        CANONICALIZE("/var/tmp/../run/", "", 0); 
        CANONICALIZE("/var/does-not-exist/unix.sock", "", 0); 
        CANONICALIZE("/unix.sock", "/unix.sock", !0); 
        CANONICALIZE("/.unix.sock", "/.unix.sock", !0); 
        CANONICALIZE("/../unix.sock", "/unix.sock", !0); 
        CANONICALIZE("/tmp/unix.sock", "/unix.sock", !0); 
        CANONICALIZE("/var/tmp/unix.sock", "/unix.sock", !0); 
        CANONICALIZE("/var/tmp/../../run/unix.sock", "/unix.sock", !0); 
        CANONICALIZE("/var/tmp/../../run/./unix.sock", "/unix.sock", !0); 
        CANONICALIZE("unix.sock", "/unix.sock", !0); 
        CANONICALIZE(".unix.sock", "/.unix.sock", !0); 
        CANONICALIZE("./unix.sock", "/unix.sock", !0); 
        CANONICALIZE("../unix.sock", "/unix.sock", !0); 
        STATUS();
    }

    {
        TEST();

        ADVISE(diminuto_ipcl_remove(LOCAL1) < 0);
        ADVISE(diminuto_ipcl_remove(LOCAL2) < 0);

        STATUS();
    }

    {
        int fd;
        diminuto_local_buffer_t local;

        TEST();

        COMMENT("endpoint=\"%s\"\n", LOCAL1);
        EXPECT((fd = diminuto_ipcl_datagram_peer(LOCAL1)) >= 0);
        EXPECT(diminuto_ipcl_nearend(fd, local, sizeof(local)) >= 0);
        COMMENT("datagram peer nearend=\"%s\"\n", local);
        EXPECT(diminuto_ipcl_farend(fd, local, sizeof(local)) < 0);
        EXPECT(diminuto_ipcl_close(fd) >= 0);
        EXPECT((fd = diminuto_ipcl_datagram_peer(LOCAL1)) < 0);
        EXPECT(diminuto_ipcl_remove(LOCAL1) >= 0);
        EXPECT((fd = diminuto_ipcl_datagram_peer(LOCAL1)) >= 0);
        EXPECT(diminuto_ipcl_close(fd) >= 0);
        EXPECT(diminuto_ipcl_remove(LOCAL1) >= 0);

        STATUS();
    }

    {
        int fd;
        diminuto_local_buffer_t local;

        TEST();

        COMMENT("endpoint=\"%s\"\n", UNNAMED);
        EXPECT((fd = diminuto_ipcl_datagram_peer(UNNAMED)) >= 0);
        EXPECT(diminuto_ipcl_nearend(fd, local, sizeof(local)) >= 0);
        COMMENT("datagram peer nearend=\"%s\"\n", local);
        EXPECT(diminuto_ipcl_farend(fd, local, sizeof(local)) < 0);
        EXPECT(diminuto_ipcl_close(fd) >= 0);
        EXPECT((fd = diminuto_ipcl_datagram_peer(UNNAMED)) >= 0);
        EXPECT(diminuto_ipcl_close(fd) >= 0);
        EXPECT((fd = diminuto_ipcl_datagram_peer(UNNAMED)) >= 0);
        EXPECT(diminuto_ipcl_close(fd) >= 0);

        STATUS();
    }

    {
        int fd;
        diminuto_local_buffer_t local;

        TEST();

        COMMENT("endpoint=\"%s\"\n", LOCAL1);
        EXPECT((fd = diminuto_ipcl_stream_provider(LOCAL1)) >= 0);
        EXPECT(diminuto_ipcl_nearend(fd, local, sizeof(local)) >= 0);
        COMMENT("stream provider nearend=\"%s\"\n", local);
        EXPECT(diminuto_ipcl_farend(fd, local, sizeof(local)) < 0);
        EXPECT(diminuto_ipcl_close(fd) >= 0);
        EXPECT((fd = diminuto_ipcl_stream_provider(LOCAL1)) < 0);
        EXPECT(diminuto_ipcl_remove(LOCAL1) >= 0);
        EXPECT((fd = diminuto_ipcl_stream_provider(LOCAL1)) >= 0);
        EXPECT(diminuto_ipcl_close(fd) >= 0);
        EXPECT(diminuto_ipcl_remove(LOCAL1) >= 0);

        STATUS();
    }

    {
        int fd;
        diminuto_local_buffer_t local;

        TEST();

        COMMENT("endpoint=\"%s\"\n", UNNAMED);
        EXPECT((fd = diminuto_ipcl_stream_provider(UNNAMED)) < 0);

        STATUS();
    }

    {
        int fd;

        TEST();

        EXPECT((fd = diminuto_ipcl_stream_provider(LOCAL1)) >= 0);
        EXPECT(diminuto_ipcl_remove(LOCAL1) >= 0);

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

        EXPECT(diminuto_ipcl_close(fd) >= 0);

        STATUS();
    }

    {
        int fd1;
        int fd2;
        const char MSG1[] = "Chip Overclock";
        const char MSG2[] = "Digital Aggregates Corporation";
        char buffer[64];
        diminuto_local_buffer_t local1;
        diminuto_local_buffer_t local2;
        diminuto_local_buffer_t local3;
        diminuto_local_buffer_t local4;

        TEST();

        EXPECT((fd1 = diminuto_ipcl_datagram_peer(UNNAMED)) >= 0);
        EXPECT(diminuto_ipcl_nearend(fd1, local1, sizeof(local1)) >= 0);
        COMMENT("local1=\"%s\"\n", local1);
        EXPECT(strcmp(local1, UNNAMED) == 0);

        EXPECT((fd2 = diminuto_ipcl_datagram_peer(LOCAL2)) >= 0);
        EXPECT(diminuto_ipcl_nearend(fd2, local2, sizeof(local2)) >= 0);
        COMMENT("local2=\"%s\"\n", local2);
        EXPECT(strcmp(local2, LOCAL2) == 0);

        /* This only works because the kernel buffers socket data. */

        EXPECT((diminuto_ipcl_datagram_send(fd1, MSG1, sizeof(MSG1), LOCAL2)) == sizeof(MSG1));
        EXPECT((diminuto_ipcl_datagram_receive_generic(fd2, buffer, sizeof(buffer), local3, sizeof(local3), 0)) == sizeof(MSG1));
        COMMENT("local3=\"%s\"\n", local3);
        EXPECT(strcmp(buffer, MSG1) == 0);

        EXPECT((diminuto_ipcl_datagram_send(fd1, MSG2, sizeof(MSG2), LOCAL2)) == sizeof(MSG2));
        EXPECT((diminuto_ipcl_datagram_receive_generic(fd2, buffer, sizeof(buffer), local4, sizeof(local4), 0)) == sizeof(MSG2));
        COMMENT("local4=\"%s\"\n", local4);
        EXPECT(strcmp(buffer, MSG2) == 0);

        EXPECT(diminuto_ipcl_close(fd1) >= 0);
        EXPECT(diminuto_ipcl_close(fd2) >= 0);
        EXPECT(diminuto_ipcl_remove(LOCAL2) >= 0);
    }

    {
        int fd1;
        int fd2;
        const char MSG1[] = "Chip Overclock";
        const char MSG2[] = "Digital Aggregates Corporation";
        char buffer[64];
        diminuto_local_buffer_t local1;
        diminuto_local_buffer_t local2;
        diminuto_local_buffer_t local3;
        diminuto_local_buffer_t local4;

        TEST();

        EXPECT((fd1 = diminuto_ipcl_datagram_peer(LOCAL1)) >= 0);
        EXPECT(diminuto_ipcl_nearend(fd1, local1, sizeof(local1)) >= 0);
        COMMENT("local1=\"%s\"\n", local1);
        EXPECT(strcmp(local1, LOCAL1) == 0);

        EXPECT((fd2 = diminuto_ipcl_datagram_peer(LOCAL2)) >= 0);
        EXPECT(diminuto_ipcl_nearend(fd2, local2, sizeof(local2)) >= 0);
        COMMENT("local2=\"%s\"\n", local2);
        EXPECT(strcmp(local2, LOCAL2) == 0);

        /* This only works because the kernel buffers socket data. */

        EXPECT((diminuto_ipcl_datagram_send(fd1, MSG1, sizeof(MSG1), LOCAL2)) == sizeof(MSG1));
        EXPECT((diminuto_ipcl_datagram_receive_generic(fd2, buffer, sizeof(buffer), local3, sizeof(local3), 0)) == sizeof(MSG1));
        COMMENT("local3=\"%s\"\n", local3);
        EXPECT(strcmp(local3, LOCAL1) == 0);
        EXPECT(strcmp(buffer, MSG1) == 0);

        EXPECT((diminuto_ipcl_datagram_send(fd2, MSG2, sizeof(MSG2), LOCAL1)) == sizeof(MSG2));
        EXPECT((diminuto_ipcl_datagram_receive_generic(fd1, buffer, sizeof(buffer), local4, sizeof(local4), 0)) == sizeof(MSG2));
        COMMENT("local4=\"%s\"\n", local4);
        EXPECT(strcmp(local4, LOCAL2) == 0);
        EXPECT(strcmp(buffer, MSG2) == 0);

        EXPECT(diminuto_ipcl_close(fd1) >= 0);
        EXPECT(diminuto_ipcl_close(fd2) >= 0);
        EXPECT(diminuto_ipcl_remove(LOCAL1) >= 0);
        EXPECT(diminuto_ipcl_remove(LOCAL2) >= 0);

        STATUS();
    }

    {
        int fd1;
        int fd2;
        const char MSG1[] = "Chip Overclock";
        const char MSG2[] = "Digital Aggregates Corporation";
        char buffer[64];
        diminuto_local_buffer_t local1;
        diminuto_local_buffer_t local2;
        diminuto_local_buffer_t local3;
        diminuto_local_buffer_t local4;

        TEST();

        EXPECT((fd1 = diminuto_ipcl_datagram_peer(LOCAL1)) >= 0);
        EXPECT(diminuto_ipcl_nearend(fd1, local1, sizeof(local1)) >= 0);
        COMMENT("local1=\"%s\"\n", local1);
        EXPECT(strcmp(local1, LOCAL1) == 0);
        EXPECT((fd2 = diminuto_ipcl_datagram_peer(LOCAL2)) >= 0);
        EXPECT(diminuto_ipcl_nearend(fd2, local2, sizeof(local2)) >= 0);
        COMMENT("local2=\"%s\"\n", local2);
        EXPECT(strcmp(local2, LOCAL2) == 0);

        /* This only works because the kernel buffers socket data. */

        EXPECT((diminuto_ipcl_datagram_send(fd1, MSG1, sizeof(MSG1), LOCAL2)) == sizeof(MSG1));
        EXPECT((diminuto_ipcl_datagram_receive_generic(fd2, buffer, sizeof(buffer), local3, sizeof(local3), 0)) == sizeof(MSG1));
        COMMENT("local3=\"%s\"\n", local3);
        EXPECT(strcmp(local3, LOCAL1) == 0);
        EXPECT(strcmp(buffer, MSG1) == 0);

        EXPECT((diminuto_ipcl_datagram_send(fd2, MSG2, sizeof(MSG2), LOCAL1)) == sizeof(MSG2));
        EXPECT((diminuto_ipcl_datagram_receive_generic(fd1, buffer, sizeof(buffer), local4, sizeof(local4), 0)) == sizeof(MSG2));
        COMMENT("local4=\"%s\"\n", local4);
        EXPECT(strcmp(local4, LOCAL2) == 0);
        EXPECT(strcmp(buffer, MSG2) == 0);

        EXPECT(diminuto_ipcl_close(fd1) >= 0);
        EXPECT(diminuto_ipcl_close(fd2) >= 0);
        EXPECT(diminuto_ipcl_remove(LOCAL2) >= 0);

        STATUS();
    }

#if 0
    {
        int fd1;
        int fd2;
        const char MSG1[] = "Chip Overclock";
        const char MSG2[] = "Digital Aggregates Corporation";
        char buffer[64];
        diminuto_local_buffer_t local1;
        diminuto_local_buffer_t local2;
        diminuto_local_buffer_t local3;
        diminuto_local_buffer_t local4;

        TEST();

        EXPECT((fd1 = diminuto_ipcl_datagram_peer(LOCAL1)) >= 0);
        EXPECT(diminuto_ipcl_nearend(fd1, local1, sizeof(local1)) >= 0);
        COMMENT("local1=\"%s\"\n", local1);
        EXPECT(strcmp(local1, LOCAL1) == 0);
        EXPECT((fd2 = diminuto_ipcl_datagram_peer(UNNAMED)) >= 0);
        EXPECT(diminuto_ipcl_nearend(fd2, local2, sizeof(local2)) >= 0);
        COMMENT("local2=\"%s\"\n", local2);
        EXPECT(strcmp(local2, UNNAMED) == 0);

        /* This only works because the kernel buffers socket data. */

        EXPECT((diminuto_ipcl_datagram_send(fd1, MSG1, sizeof(MSG1), LOCAL1)) == sizeof(MSG1));
        COMMENT("SENT\n");
        EXPECT((diminuto_ipcl_datagram_receive_generic(fd2, buffer, sizeof(buffer), local3, sizeof(local3), 0)) == sizeof(MSG1));
        COMMENT("RECEIVED\n");
        EXPECT(strcmp(local3, LOCAL1) == 0);
        EXPECT(strcmp(buffer, MSG1) == 0);

        EXPECT((diminuto_ipcl_datagram_send(fd2, MSG2, sizeof(MSG2), LOCAL1)) == sizeof(MSG2));
        EXPECT((diminuto_ipcl_datagram_receive_generic(fd1, buffer, sizeof(buffer), local4, sizeof(local4), 0)) == sizeof(MSG2));
        EXPECT(strcmp(local4, LOCAL1) == 0);
        EXPECT(strcmp(buffer, MSG2) == 0);

        EXPECT(diminuto_ipcl_close(fd1) >= 0);
        EXPECT(diminuto_ipcl_close(fd2) >= 0);
        EXPECT(diminuto_ipcl_remove(LOCAL1) >= 0);

        STATUS();
    }

    {
        int fd1;
        int fd2;
        const char MSG1[] = "Chip Overclock";
        const char MSG2[] = "Digital Aggregates Corporation";
        char buffer[64];
        diminuto_ipv4_t binding = 0;
        diminuto_ipv4_t address = 0;
        diminuto_ipv4_t address1 = 0;
        diminuto_ipv4_t address2 = 0;
        diminuto_port_t port = 0;
        diminuto_port_t port1 = 0;
        diminuto_port_t port2 = 0;

        TEST();

        binding = diminuto_ipcl_address("localhost");
        COMMENT("0x%8.8x %d\n", binding, 0);

        EXPECT((fd1 = diminuto_ipcl_datagram_peer(0)) >= 0);
        EXPECT(diminuto_ipcl_nearend(fd1, &address1, &port1) >= 0);
        /* fd1 not bound to specific address, and port is ephemeral. */
        EXPECT(port1 != 0);

        EXPECT(binding == 0x7f000001);
        EXPECT((fd2 = diminuto_ipcl_datagram_peer_generic(binding, 0, (const char *)0)) >= 0);
        EXPECT(diminuto_ipcl_nearend(fd2, &address2, &port2) >= 0);

        EXPECT(address2 == binding);

        /* This only works because the kernel buffers socket data. */
        EXPECT((diminuto_ipcl_datagram_send(fd1, MSG1, sizeof(MSG1), binding, port2)) == sizeof(MSG1));
        EXPECT((diminuto_ipcl_datagram_receive_generic(fd2, buffer, sizeof(buffer), &address, &port, 0)) == sizeof(MSG1));
        EXPECT(strcmp(buffer, MSG1) == 0);
        EXPECT(port == port1);

        EXPECT((diminuto_ipcl_datagram_send(fd2, MSG2, sizeof(MSG2), binding, port1)) == sizeof(MSG2));
        EXPECT((diminuto_ipcl_datagram_receive_generic(fd1, buffer, sizeof(buffer), &address, &port, 0)) == sizeof(MSG2));
        EXPECT(address == binding);
        EXPECT(port == port2);
        EXPECT(strcmp(buffer, MSG2) == 0);

        EXPECT(diminuto_ipcl_close(fd1) >= 0);
        EXPECT(diminuto_ipcl_close(fd2) >= 0);

        STATUS();
    }

    {
        int fd;
        char buffer[1];
        diminuto_ipv4_t address = 0x12345678;
        diminuto_port_t port = 0x9abc;
        diminuto_port_t rendezvous = 0xdef0;

        TEST();

        EXPECT((fd = diminuto_ipcl_datagram_peer(0)) >= 0);
        EXPECT(diminuto_ipcl_nearend(fd, (diminuto_ipv4_t *)0, &rendezvous) >= 0);
        EXPECT(diminuto_ipc_set_nonblocking(fd, !0) >= 0);
        EXPECT((diminuto_ipcl_datagram_receive_generic(fd, buffer, sizeof(buffer), &address, &port, 0)) < 0);
        EXPECT(errno == EAGAIN);
        EXPECT(address == 0x12345678);
        EXPECT(port == 0x9abc);
        EXPECT(diminuto_ipcl_close(fd) >= 0);

        STATUS();
    }

    {
        int fd;
        char buffer[1];
        diminuto_ipv4_t address = 0x12345678;
        diminuto_port_t port = 0x9abc;
        diminuto_port_t rendezvous = 0xdef0;

        TEST();

        EXPECT((fd = diminuto_ipcl_datagram_peer(0)) >= 0);
        EXPECT(diminuto_ipcl_nearend(fd, (diminuto_ipv4_t *)0, &rendezvous) >= 0);
        EXPECT((diminuto_ipcl_datagram_receive_generic(fd, buffer, sizeof(buffer), &address, &port, MSG_DONTWAIT)) < 0);
        EXPECT(errno == EAGAIN);
        EXPECT(address == 0x12345678);
        EXPECT(port == 0x9abc);
        EXPECT(diminuto_ipcl_close(fd) >= 0);

        STATUS();
    }

    {
        int fd;
        char buffer[1];
        diminuto_ipv4_t address = 0x12345678;
        diminuto_port_t port = 0x9abc;
        diminuto_port_t rendezvous = 0xdef0;
        diminuto_sticks_t before;
        diminuto_sticks_t after;

        TEST();

        EXPECT((fd = diminuto_ipcl_datagram_peer(0)) >= 0);
        EXPECT(diminuto_ipcl_nearend(fd, (diminuto_ipv4_t *)0, &rendezvous) >= 0);
        EXPECT(diminuto_alarm_install(0) >= 0);
        EXPECT(diminuto_timer_oneshot(2000000ULL) == 0);
        EXPECT((before = diminuto_time_elapsed()) >= 0);
        EXPECT((diminuto_ipcl_datagram_receive_generic(fd, buffer, sizeof(buffer), &address, &port, 0)) < 0);
        EXPECT((after = diminuto_time_elapsed()) >= 0);
        EXPECT(diminuto_timer_oneshot(0ULL) >= 0);
        CHECKPOINT("elapsed %lld - %lld = %lld\n", after, before, after - before);
        EXPECT(diminuto_alarm_check());
        EXPECT((after - before) >= 2000000LL);
        EXPECT(errno == EINTR);
        EXPECT(address == 0x12345678);
        EXPECT(port == 0x9abc);
        EXPECT(diminuto_ipcl_close(fd) >= 0);

        STATUS();
    }

    {
        diminuto_ipv4_t address;
        diminuto_port_t port;
        diminuto_port_t rendezvous;
        int service;
        pid_t pid;

        EXPECT((service = diminuto_ipcl_stream_provider(0)) >= 0);
        EXPECT(diminuto_ipcl_nearend(service, (diminuto_ipv4_t *)0, &rendezvous) >= 0);

        EXPECT((pid = fork()) >= 0);

        if (pid != 0) {

            int producer;
            int status;

            address = 0;
            port = (diminuto_port_t)-1;
            EXPECT((producer = diminuto_ipcl_stream_accept_generic(service, &address, &port)) >= 0);
            EXPECT(address == DIMINUTO_IPC4_LOOPBACK);
            EXPECT(port != (diminuto_port_t)-1);
            EXPECT(port != rendezvous);

            address = 0;
            port = -(diminuto_port_t)1;
            EXPECT(diminuto_ipcl_nearend(producer, &address, &port) >= 0);
            EXPECT(address == DIMINUTO_IPC4_LOOPBACK);
            EXPECT(port == rendezvous);

            address = 0;
            port = (diminuto_port_t)-1;
            EXPECT(diminuto_ipcl_farend(producer, &address, &port) >= 0);
            EXPECT(address == DIMINUTO_IPC4_LOOPBACK);
            EXPECT(port != (diminuto_port_t)-1);
            EXPECT(port != rendezvous);

            diminuto_delay(hertz / 1000, !0);

            EXPECT(diminuto_ipcl_close(producer) >= 0);
            EXPECT(diminuto_ipcl_close(service) >= 0);

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

            EXPECT(diminuto_ipcl_close(service) >= 0);

            diminuto_delay(hertz / 1000, !0);

            EXPECT((consumer = diminuto_ipcl_stream_consumer(diminuto_ipcl_address("localhost"), rendezvous)) >= 0);

            diminuto_delay(hertz / 1000, !0);

            EXPECT(diminuto_ipcl_close(consumer) >= 0);

            EXIT();

        }

        STATUS();
    }

    {
        diminuto_ipv4_t address;
        diminuto_port_t port;
        diminuto_port_t rendezvous;
        int service;
        pid_t pid;

        TEST();

        EXPECT((service = diminuto_ipcl_stream_provider(0)) >= 0);
        EXPECT(diminuto_ipcl_nearend(service, (diminuto_ipv4_t *)0, &rendezvous) >= 0);

        EXPECT((pid = fork()) >= 0);

        if (pid == 0) {

            int producer;

            EXPECT((producer = diminuto_ipcl_stream_accept_generic(service, &address, &port)) >= 0);

            diminuto_delay(hertz / 1000, !0);

            EXPECT(diminuto_ipcl_close(producer) >= 0);
            EXPECT(diminuto_ipcl_close(service) >= 0);

            EXIT();

        } else {

            int consumer;
            int status;

            EXPECT(diminuto_ipcl_close(service) >= 0);

            diminuto_delay(hertz / 1000, !0);

            EXPECT((consumer = diminuto_ipcl_stream_consumer(diminuto_ipcl_address("localhost"), rendezvous)) >= 0);

            address = 0;
            port = (diminuto_port_t)-1;
            EXPECT(diminuto_ipcl_farend(consumer, &address, &port) >= 0);
            EXPECT(address == DIMINUTO_IPC4_LOOPBACK);
            EXPECT(port == rendezvous);

            address = 0;
            port = (diminuto_port_t)-1;
            EXPECT(diminuto_ipcl_nearend(consumer, &address, &port) >= 0);
            EXPECT(address == DIMINUTO_IPC4_LOOPBACK);
            EXPECT(port != (diminuto_port_t)-1);
            EXPECT(port != rendezvous);

            EXPECT(diminuto_ipcl_close(consumer) >= 0);

            /*
             * If you don't wait for the child to exit, it may not yet have
             * closed its end of the socket thereby releasing the bound IP
             * address by the time the next unit test begins.
             */

            EXPECT(waitpid(pid, &status, 0) == pid);
            CHECKPOINT("pid=%d status=%d\n", pid, status);
            EXPECT(WIFEXITED(status));
            EXPECT(WEXITSTATUS(status) == 0);
        }

        STATUS();
    }
#endif

#if 0
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
        diminuto_ipv4_t provider;
        diminuto_ipv4_t source;
        diminuto_port_t rendezvous;
        diminuto_ipv4_t address;
        diminuto_port_t port;
        int service;
        pid_t pid;

        TEST();

        ASSERT((provider = diminuto_ipcl_address("localhost")) == 0x7f000001);
        ASSERT((service = diminuto_ipcl_stream_provider_generic(provider, 0, (const char *)0, 16)) >= 0);
        EXPECT(diminuto_ipcl_nearend(service, &source, &rendezvous) >= 0);
        EXPECT(source == provider);
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

            ASSERT((producer = diminuto_ipcl_stream_accept_generic(service, &address, &port)) >= 0);

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

                    ASSERT((sent = diminuto_ipcl_stream_write_generic(producer, here, 1, used)) > 0);
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

                ASSERT((received = diminuto_ipcl_stream_read(producer, there, available)) > 0);
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

            EXPECT(diminuto_ipcl_shutdown(producer) >= 0);
            EXPECT(diminuto_ipcl_shutdown(service) >= 0);

            ASSERT(diminuto_ipcl_close(producer) >= 0);
            ASSERT(diminuto_ipcl_close(service) >= 0);

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

            ASSERT(diminuto_ipcl_close(service) >= 0);

            diminuto_delay(hertz / 1000, !0);

            ASSERT((consumer = diminuto_ipcl_stream_consumer(provider, rendezvous)) >= 0);

            totalreceived = 0;
            totalsent = 0;

            while (!0) {

                EXPECT((inputqueued = diminuto_ipc_stream_get_available(consumer)) >= 0);

                ASSERT((received = diminuto_ipcl_stream_read(consumer, buffer, sizeof(buffer))) >= 0);
                ASSERT(received <= sizeof(buffer));

                totalreceived += received;
                COMMENT("consumer received %zd %zd %zu\n", received, inputqueued, totalreceived);

                if (received == 0) {
                    break;
                }

                sent = 0;
                while (sent < received) {
                    ASSERT((sent = diminuto_ipcl_stream_write_generic(consumer,  buffer + sent, 1, received - sent)) > 0);
                    ASSERT(sent <= received);

                    EXPECT((outputqueued = diminuto_ipc_stream_get_pending(consumer)) >= 0);

                    totalsent += sent;
                    COMMENT("consumer sent %zd %zd %zu\n", sent, outputqueued, totalsent);

                    received -= sent;
                }
            }

            EXPECT(inputqueued == 0);

            EXPECT(diminuto_ipcl_shutdown(consumer) >= 0);

            ASSERT(diminuto_ipcl_close(consumer) >= 0);

            EXIT();
        }

        STATUS();
    }
#endif

    EXIT();
}
