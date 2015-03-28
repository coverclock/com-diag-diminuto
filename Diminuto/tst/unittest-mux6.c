/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_core.h"
#include "com/diag/diminuto/diminuto_mux.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_alarm.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_ipc6.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_fd.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#if defined(__arm__)
static const size_t TOTAL = 1024 * 1024 * 100;
#else
static const size_t TOTAL = 1024 * 1024 * 1024;
#endif

static const char * diminuto_mux_set_name(diminuto_mux_t * muxp, diminuto_mux_set_t * setp)
{
    return (setp == &muxp->read) ? "read" : (setp == &muxp->write) ? "write" : (setp == &muxp->accept) ? "accept" : "other";
}

static void diminuto_mux_sigs_dump(sigset_t * sigs)
{
    int signum;

    for (signum = 1; signum < NSIG; ++signum) {
        if (sigismember(sigs, signum)) {
            DIMINUTO_LOG_DEBUG(" %d", signum);
        }
    }
}

static void diminuto_mux_fds_dump(fd_set * fds)
{
    int fd;
    int nfds;

    nfds = diminuto_fd_count();
    for (fd = 0; fd < nfds; ++fd) {
        if (FD_ISSET(fd, fds)) {
            DIMINUTO_LOG_DEBUG(" %d", fd);
        }
    }

}

static void diminuto_mux_set_dump(diminuto_mux_t * muxp, diminuto_mux_set_t * setp)
{
    const char * name;

    name = diminuto_mux_set_name(muxp, setp);
    DIMINUTO_LOG_DEBUG("mux@%p: %s.next=%d", muxp, name, setp->next);
    DIMINUTO_LOG_DEBUG("mux@%p: %s.min=%d", muxp, name, setp->min);
    DIMINUTO_LOG_DEBUG("mux@%p: %s.max=%d", muxp, name, setp->max);
    DIMINUTO_LOG_DEBUG("mux@%p: %s.active=<", muxp, name); diminuto_mux_fds_dump(&setp->active); DIMINUTO_LOG_DEBUG(">");
    DIMINUTO_LOG_DEBUG("mux@%p: %s.ready=<", muxp, name); diminuto_mux_fds_dump(&setp->ready); DIMINUTO_LOG_DEBUG(">");
}

static void diminuto_mux_dump(diminuto_mux_t * muxp)
{
    int signum;

    DIMINUTO_LOG_DEBUG("mux@%p: effective=<", muxp); diminuto_mux_fds_dump(&muxp->effective); DIMINUTO_LOG_DEBUG(">");
    diminuto_mux_set_dump(muxp, &muxp->read);
    diminuto_mux_set_dump(muxp, &muxp->write);
    diminuto_mux_set_dump(muxp, &muxp->accept);
    DIMINUTO_LOG_DEBUG("mux@%p: mask=<", muxp); diminuto_mux_sigs_dump(&muxp->mask); DIMINUTO_LOG_DEBUG(">");
}

static uint16_t fletcher8(const void * buffer, size_t length, uint8_t * ap, uint8_t * bp)
{
    const uint8_t * pp;

    for (pp = (const uint8_t *)buffer; length > 0; --length) {
        *ap += *(pp++);
        *bp += *ap;
    }

    return (*bp << 8) | *ap;
}

int main(int argc, char ** argv)
{
    extern int diminuto_alarm_debug;

    SETLOGMASK();
    diminuto_alarm_debug = !0;

    {
        int listener;
        pid_t pid;
        diminuto_port_t rendezvous = 0;

        TEST();

        ASSERT((listener = diminuto_ipc6_stream_provider(0)) >= 0);
        ASSERT(diminuto_ipc6_nearend(listener, (diminuto_ipv6_t *)0, &rendezvous) == 0);
        ASSERT(rendezvous > 0);

        ASSERT((pid = fork()) >= 0);

        if (pid) {

            /* PRODUCER */

            diminuto_ipv6_t address;
            diminuto_port_t port;
            diminuto_mux_t mux;
            int producer;
            uint8_t output[1 << (sizeof(uint8_t) * 8)];
            uint8_t input[1 << (sizeof(uint8_t) * 8)];
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
            int ready;
            int fd;
            int timeouts;
            int alarms;
            sigset_t mask;
            ssize_t readable;
            double percentage;
            uint8_t output_a;
            uint8_t output_b;
            uint16_t output_8;
            uint8_t input_a;
            uint8_t input_b;
            uint16_t input_8;

            ASSERT(sigemptyset(&mask) == 0);
            ASSERT(sigaddset(&mask, SIGALRM) == 0);
            ASSERT(sigprocmask(SIG_BLOCK, &mask, (sigset_t *)0) == 0);

            diminuto_mux_init(&mux);

            ASSERT(diminuto_mux_register_accept(&mux, listener) == 0);
            ASSERT(diminuto_mux_unregister_signal(&mux, SIGALRM) == 0);

            diminuto_mux_dump(&mux);

            while (!0) {
                if ((ready = diminuto_mux_wait(&mux, diminuto_frequency())) > 0) {
                    break;
                } else if (ready == 0) {
                    DIMINUTO_LOG_INFORMATION("listener timed out\n");
                    ++timeouts;
                } else if (errno != EINTR) {
                    FATAL("diminuto_mux_wait: error");
                } else if (diminuto_alarm_check()) {
                    DIMINUTO_LOG_INFORMATION("listener alarmed\n");
                    ++alarms;
                } else {
                    FATAL("diminuto_mux_wait: interrupted");
                }
            }

            ASSERT((fd = diminuto_mux_ready_accept(&mux)) >= 0);
            ASSERT(fd == listener);

            ASSERT((producer = diminuto_ipc6_stream_accept(fd, &address, &port)) >= 0);

            ASSERT(diminuto_mux_register_read(&mux, producer) == 0);
            ASSERT(diminuto_mux_register_write(&mux, producer) == 0);

            diminuto_mux_dump(&mux);

            ASSERT(diminuto_alarm_install(0) == 0);
            ASSERT(diminuto_timer_oneshot(diminuto_frequency()) == 0);

            here = output;
            used = sizeof(output);
            sent = 0;

            there = input;
            available = sizeof(input);
            received = 0;

            timeouts = 0;
            alarms = 0;

            totalsent = 0;
            totalreceived = 0;

            datum = 0;
            do {
                output[datum] = datum;
            } while ((++datum) > 0);

            memset(input, 0, sizeof(input));

            input_a = input_b = output_a = output_b = 0;

            do {

                while (!0) {
                    if ((ready = diminuto_mux_wait(&mux, diminuto_frequency() / 10)) > 0) {
                        break;
                    } else if (ready == 0) {
                        DIMINUTO_LOG_INFORMATION("producer timed out\n");
                        ++timeouts;
                    } else if (errno != EINTR) {
                        FATAL("diminuto_mux_wait: error");
                    } else if (diminuto_alarm_check()) {
                        DIMINUTO_LOG_INFORMATION("producer alarmed\n");
                        ++alarms;
                    } else {
                        FATAL("diminuto_mux_wait: interrupted");
                    }
                }

                while ((fd = diminuto_mux_ready_write(&mux)) >= 0) {

                    ASSERT(fd == producer);

                    if (totalsent < TOTAL) {

                        if (used > (TOTAL - totalsent)) {
                            used = TOTAL - totalsent;
                        }

                        ASSERT((sent = diminuto_ipc6_stream_write(fd, here, 1, used)) > 0);
                        ASSERT(sent <= used);
                        output_8 = fletcher8(here, sent, &output_a, &output_b);

                        totalsent += sent;
                        percentage = totalsent;
                        percentage *= 100;
                        percentage /= TOTAL;
                        DIMINUTO_LOG_INFORMATION("producer sent     %10s %10d %10u %7.3lf%%\n", "", sent, totalsent, percentage);

                        here += sent;
                        used -= sent;

                        if (used == 0) {
                            here = output;
                            used = sizeof(output);
                        }

                    }

                }

                current = there;

                if (available > (TOTAL - totalreceived)) {
                    available = TOTAL - totalreceived;
                }

                while ((fd = diminuto_mux_ready_read(&mux)) >= 0) {

                    ASSERT(fd == producer);

                    ASSERT((readable = diminuto_fd_readable(fd)) > 0);
                    ASSERT((received = diminuto_ipc6_stream_read(fd, there, 1, available)) > 0);
                    ASSERT(received <= available);
                    input_8 = fletcher8(there, received, &input_a, &input_b);

                    totalreceived += received;
                    DIMINUTO_LOG_INFORMATION("producer received %10d %10d %10u\n", readable, received, totalreceived);

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

                }

            } while (totalreceived < TOTAL);

            ASSERT(diminuto_mux_close(&mux, producer) == 0);
            ASSERT(diminuto_mux_close(&mux, listener) == 0);

            ASSERT(input_8 == output_8);

            ADVISE(timeouts > 0);
            ADVISE(alarms > 0);

            EXPECT(waitpid(pid, &status, 0) == pid);
            EXPECT(WIFEXITED(status));
            EXPECT(WEXITSTATUS(status) == 0);

            STATUS();

        } else {

            /* CONSUMER */

            diminuto_mux_t mux;
            int consumer;
            uint8_t buffer[64];
            ssize_t sent;
            ssize_t received;
            size_t totalsent;
            size_t totalreceived;
            int ready;
            int fd;
            int done;
            ssize_t readable;
            double percentage;

            ASSERT(diminuto_ipc6_close(listener) >= 0);

            diminuto_mux_init(&mux);

            diminuto_delay(diminuto_delay_frequency(), !0);

            ASSERT((consumer = diminuto_ipc6_stream_consumer(diminuto_ipc6_address("localhost"), rendezvous)) >= 0);
            ASSERT(diminuto_mux_register_read(&mux, consumer) == 0);

            totalreceived = 0;
            totalsent = 0;
            done = 0;

            diminuto_delay(diminuto_delay_frequency(), !0);

            do {

                while (!0) {
                    if ((ready = diminuto_mux_wait(&mux, -1)) > 0) {
                        break;
                    } else if (ready == 0) {
                        diminuto_yield();
                    } else if (errno == EINTR) {
                        DIMINUTO_LOG_INFORMATION("consumer interrupted\n");
                    } else {
                        FATAL("diminuto_mux_wait");
                    }
                }

                while ((fd = diminuto_mux_ready_read(&mux)) >= 0) {

                    ASSERT(fd == consumer);

                    ASSERT((readable = diminuto_fd_readable(fd)) >= 0);
                    ASSERT((received = diminuto_ipc6_stream_read(fd, buffer, 1, sizeof(buffer))) >= 0);
                    ASSERT(received <= sizeof(buffer));

                    totalreceived += received;
                    percentage = totalreceived;
                    percentage *= 100;
                    percentage /= TOTAL;
                    DIMINUTO_LOG_INFORMATION("consumer received %10d %10d %10u %7.3lf%%\n", readable, received, totalreceived, percentage);

                    if (received == 0) {
                        done = !0;
                        break;
                    }

                    sent = 0;
                    while (sent < received) {
                        ASSERT((sent = diminuto_ipc6_stream_write(fd,  buffer + sent, 1, received - sent)) > 0);
                        ASSERT(sent <= received);

                        totalsent += sent;
                        DIMINUTO_LOG_INFORMATION("consumer sent     %10s %10d %10u\n", "", sent, totalsent);

                        received -= sent;
                    }

                }

            } while (!done);

            ASSERT(diminuto_mux_close(&mux, consumer) == 0);

            exit(0);
        }


    }

    EXIT();
}
