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
#include "com/diag/diminuto/diminuto_poll.h"
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
#include <sys/socket.h>

#if defined(__arm__)
static const size_t TOTAL = 1024 * 1024 * 100;
#else
static const size_t TOTAL = 1024 * 1024 * 1024;
#endif

static const char * diminuto_mux_set_name(diminuto_mux_t * muxp, diminuto_mux_set_t * setp)
{
    return (setp == &muxp->read) ? "read" : (setp == &muxp->write) ? "write" : (setp == &muxp->accept) ? "accept" : (setp == &muxp->urgent) ? "urgent" : "other";
}

static void diminuto_mux_sigs_dump(sigset_t * sigs)
{
    int signum;

    for (signum = 1; signum < NSIG; ++signum) {
        if (sigismember(sigs, signum)) {
            DIMINUTO_LOG_DEBUG(" %d\n", signum);
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
            DIMINUTO_LOG_DEBUG(" %d\n", fd);
        }
    }

}

static void diminuto_mux_set_dump(diminuto_mux_t * muxp, diminuto_mux_set_t * setp)
{
    const char * name;

    name = diminuto_mux_set_name(muxp, setp);
    DIMINUTO_LOG_DEBUG("mux@%p: %s.next=%d\n", muxp, name, setp->next);
    DIMINUTO_LOG_DEBUG("mux@%p: %s.min=%d\n", muxp, name, setp->min);
    DIMINUTO_LOG_DEBUG("mux@%p: %s.max=%d\n", muxp, name, setp->max);
    DIMINUTO_LOG_DEBUG("mux@%p: %s.active=<\n", muxp, name); diminuto_mux_fds_dump(&setp->active); DIMINUTO_LOG_DEBUG(">\n");
    DIMINUTO_LOG_DEBUG("mux@%p: %s.ready=<\n", muxp, name); diminuto_mux_fds_dump(&setp->ready); DIMINUTO_LOG_DEBUG(">\n");
}

static void diminuto_mux_dump(diminuto_mux_t * muxp)
{
    int signum;

    /* read_or_accept isn't used by the poller. */
    diminuto_mux_set_dump(muxp, &muxp->read);
    diminuto_mux_set_dump(muxp, &muxp->write);
    diminuto_mux_set_dump(muxp, &muxp->accept);
    diminuto_mux_set_dump(muxp, &muxp->urgent);
    DIMINUTO_LOG_DEBUG("mux@%p: mask=<\n", muxp); diminuto_mux_sigs_dump(&muxp->mask); DIMINUTO_LOG_DEBUG(">\n");
}

static void diminuto_poll_dump(diminuto_poll_t * pollp)
{
    nfds_t ii;
    struct pollfd * pollfdp;

    DIMINUTO_LOG_DEBUG("poll@%p: nfds=%u\n", pollp, pollp->nfds);
    DIMINUTO_LOG_DEBUG("poll@%p: min=%d\n", pollp, pollp->min);
    DIMINUTO_LOG_DEBUG("poll@%p: max=%d\n", pollp, pollp->max);
    DIMINUTO_LOG_DEBUG("poll@%p: refresh=%d\n", pollp, pollp->refresh);
    DIMINUTO_LOG_DEBUG("poll@%p: pollfd=%p\n", pollp, pollp->pollfd);
    for (ii = 0; ii < pollp->nfds; ++ii) {
        pollfdp = &pollp->pollfd[ii];
        if (pollfdp->fd >= 0) {
            DIMINUTO_LOG_DEBUG("poll@%p: pollfd[%u].fd=%d\n", pollp, ii, pollfdp->fd);
            DIMINUTO_LOG_DEBUG("poll@%p: pollfd[%u].events=0x%x\n", pollp, ii, pollfdp->events);
            DIMINUTO_LOG_DEBUG("poll@%p: pollfd[%u].revents=0x%x\n", pollp, ii, pollfdp->revents);
        }
    }
    diminuto_mux_dump(&pollp->mux);
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
        static const uint8_t ACK = '\006';

        TEST();

        ASSERT((listener = diminuto_ipc6_stream_provider(0)) >= 0);
        ASSERT(diminuto_ipc6_nearend(listener, (diminuto_ipv6_t *)0, &rendezvous) == 0);
        ASSERT(rendezvous > 0);

        ASSERT((pid = fork()) >= 0);

        if (pid) {

            /* PRODUCER */

            diminuto_ipv6_t address;
            diminuto_port_t port;
            diminuto_poll_t poll;
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

            diminuto_poll_init(&poll);

            ASSERT(diminuto_poll_register_accept(&poll, listener) == 0);
            ASSERT(diminuto_poll_unregister_signal(&poll, SIGALRM) == 0);

            diminuto_poll_dump(&poll);

            while (!0) {
                if ((ready = diminuto_poll_wait(&poll, diminuto_frequency())) > 0) {
                    break;
                } else if (ready == 0) {
                    DIMINUTO_LOG_INFORMATION("listener timed out\n");
                    ++timeouts;
                } else if (errno != EINTR) {
                    FATAL("diminuto_poll_wait: error");
                } else if (diminuto_alarm_check()) {
                    DIMINUTO_LOG_INFORMATION("listener alarmed\n");
                    ++alarms;
                } else {
                    FATAL("diminuto_poll_wait: interrupted");
                }
            }

            ASSERT((fd = diminuto_poll_ready_accept(&poll)) >= 0);
            ASSERT(fd == listener);

            ASSERT((producer = diminuto_ipc6_stream_accept(fd, &address, &port)) >= 0);

            ASSERT(diminuto_poll_register_read(&poll, producer) == 0);
            ASSERT(diminuto_poll_register_write(&poll, producer) == 0);

            diminuto_poll_dump(&poll);

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

            ASSERT((sent = diminuto_ipc6_datagram_send_flags(producer, &ACK, sizeof(ACK), address, 0, MSG_OOB)) == sizeof(ACK));
            DIMINUTO_LOG_DEBUG("producer ACKing   %10s %10d %10u %7.3lf%%\n", "", sent, 0, 0.0);

            do {

                while (!0) {
                    if ((ready = diminuto_poll_wait(&poll, diminuto_frequency() / 10)) > 0) {
                        break;
                    } else if (ready == 0) {
                        DIMINUTO_LOG_INFORMATION("producer timed out\n");
                        ++timeouts;
                    } else if (errno != EINTR) {
                        FATAL("diminuto_poll_wait: error");
                    } else if (diminuto_alarm_check()) {
                        DIMINUTO_LOG_INFORMATION("producer alarmed\n");
                        ++alarms;
                    } else {
                        FATAL("diminuto_poll_wait: interrupted");
                    }
                }

                while ((fd = diminuto_poll_ready_write(&poll)) >= 0) {

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

                while ((fd = diminuto_poll_ready_read(&poll)) >= 0) {

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

            ASSERT(diminuto_poll_close(&poll, producer) == 0);
            ASSERT(diminuto_poll_close(&poll, listener) == 0);
            diminuto_poll_fini(&poll);

            ASSERT(input_8 == output_8);

            ADVISE(timeouts > 0);
            ADVISE(alarms > 0);

            EXPECT(waitpid(pid, &status, 0) == pid);
            EXPECT(WIFEXITED(status));
            EXPECT(WEXITSTATUS(status) == 0);

            STATUS();

        } else {

            /* CONSUMER */

            diminuto_poll_t poll;
            int consumer;
            uint8_t buffer[64];
            ssize_t sent;
            ssize_t received;
            size_t totalsent;
            size_t totalreceived;
            int ready;
            int fd;
            int done;
            int proceed;
            ssize_t readable;
            double percentage;

            ASSERT(diminuto_ipc6_close(listener) >= 0);

            diminuto_poll_init(&poll);

            ASSERT((consumer = diminuto_ipc6_stream_consumer(diminuto_ipc6_address("localhost"), rendezvous)) >= 0);
            ASSERT(diminuto_poll_register_read(&poll, consumer) == 0);
            ASSERT(diminuto_poll_register_urgent(&poll, consumer) == 0);

            totalreceived = 0;
            totalsent = 0;
            done = 0;

            do {

                while (!0) {
                    if ((ready = diminuto_poll_wait(&poll, -1)) > 0) {
                        break;
                    } else if (ready == 0) {
                        diminuto_yield();
                    } else if (errno == EINTR) {
                        DIMINUTO_LOG_DEBUG("consumer interrupted\n");
                    } else {
                        FATAL("diminuto_poll_wait");
                    }
                }

                if (!proceed) {
                    while ((fd = diminuto_poll_ready_urgent(&poll)) >= 0) {
                        ASSERT(fd == consumer);
                        buffer[0] = '\0';
                        ASSERT((received = diminuto_ipc6_datagram_receive_flags(fd, buffer, 1, (diminuto_ipv6_t *)0, (diminuto_port_t *)0, MSG_OOB | MSG_DONTWAIT)) == 1);
                        DIMINUTO_LOG_DEBUG("consumer ACKed    %10d %10d %10u %7.3lf%%\n", 0, received, 0, 0.0);
                        ASSERT(buffer[0] == ACK);
                        if (buffer[0] == ACK) {
                            proceed = !0;
                            break;
                        }
                    }
                    if (!proceed) {
                        continue;
                    }
                }

                while ((fd = diminuto_poll_ready_read(&poll)) >= 0) {

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

            ASSERT(diminuto_poll_close(&poll, consumer) == 0);
            diminuto_poll_fini(&poll);

            exit(0);
        }


    }

    EXIT();
}
