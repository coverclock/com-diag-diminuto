/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Mux feature for Local.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Mux feature for Local sockets.
 * This is more or less adapted from the mux6 unit test, except that
 * UNIX domain sockets do not support out-of-band (OOB) messaging.
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
#include "com/diag/diminuto/diminuto_ipcl.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_fd.h"
#include "com/diag/diminuto/diminuto_fletcher.h"
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

static const char ENDPOINT[] = "/tmp/com-diag-diminuto-unittest-muxl.sock";

/*
 */

int main(int argc, char ** argv)
{
    extern int diminuto_alarm_debug;

    SETLOGMASK();

    diminuto_alarm_debug = !0;

    {
        int listener;
        diminuto_local_t local;
        const char * canonical;
        pid_t pid;

        TEST();

        ASSERT((canonical = diminuto_ipcl_path(ENDPOINT, local, sizeof(local))) == local);

        ADVISE(diminuto_ipcl_remove(canonical) < 0);

        ASSERT((listener = diminuto_ipcl_stream_provider(canonical)) >= 0);

        ASSERT((pid = fork()) >= 0);

        if (pid) {

            /* PRODUCER */

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
            uint16_t output_16;
            uint8_t input_a;
            uint8_t input_b;
            uint16_t input_16;

            ASSERT(sigemptyset(&mask) == 0);
            ASSERT(sigaddset(&mask, SIGALRM) == 0);
            ASSERT(sigprocmask(SIG_BLOCK, &mask, (sigset_t *)0) == 0);

            ASSERT(diminuto_mux_init(&mux) == &mux);

            ASSERT(diminuto_mux_register_accept(&mux, listener) >= 0);
            ASSERT(diminuto_mux_unregister_signal(&mux, SIGALRM) >= 0);

            diminuto_mux_dump(&mux);

            timeouts = 0;
            alarms = 0;

            while (!0) {
                if ((ready = diminuto_mux_wait(&mux, diminuto_frequency())) > 0) {
                    break;
                } else if (ready == 0) {
                    CHECKPOINT("listener timed out\n");
                    ++timeouts;
                } else if (errno != EINTR) {
                    FATAL("diminuto_mux_wait: error");
                } else if (diminuto_alarm_check()) {
                    CHECKPOINT("listener alarmed\n");
                    ++alarms;
                } else {
                    FATAL("diminuto_mux_wait: interrupted");
                }
            }

            ASSERT((fd = diminuto_mux_ready_accept(&mux)) >= 0);
            ASSERT(fd == listener);

            ASSERT((producer = diminuto_ipcl_stream_accept(fd)) >= 0);

            ASSERT(diminuto_mux_register_read(&mux, producer) >= 0);
            ASSERT(diminuto_mux_register_write(&mux, producer) >= 0);

            diminuto_mux_dump(&mux);

            ASSERT(diminuto_alarm_install(0) == 0);
            ASSERT(diminuto_timer_periodic(diminuto_frequency()) == 0);

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

            input_a = input_b = output_a = output_b = 0;

            do {

                while (!0) {
                    if ((ready = diminuto_mux_wait(&mux, diminuto_frequency() / 10)) > 0) {
                        break;
                    } else if (ready == 0) {
                        CHECKPOINT("producer timed out\n");
                        ++timeouts;
                    } else if (errno != EINTR) {
                        FATAL("diminuto_mux_wait: error");
                    } else if (diminuto_alarm_check()) {
                        CHECKPOINT("producer alarmed\n");
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

                        ASSERT((sent = diminuto_ipcl_stream_write_generic(fd, here, 1, used)) > 0);
                        ASSERT(sent <= used);
                        output_16 = diminuto_fletcher_16(here, sent, &output_a, &output_b);

                        totalsent += sent;
                        percentage = totalsent;
                        percentage *= 100;
                        percentage /= TOTAL;
                        COMMENT("producer sent     %10s %10zd %10zu %7.3lf%%\n", "", sent, totalsent, percentage);

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
                    ASSERT((received = diminuto_ipcl_stream_read(fd, there, available)) > 0);
                    ASSERT(received <= available);
                    input_16 = diminuto_fletcher_16(there, received, &input_a, &input_b);

                    totalreceived += received;
                    COMMENT("producer received %10zd %10zd %10zu\n", readable, received, totalreceived);

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

            ASSERT(input_16 == output_16);

            NOTIFY("timeouts=%d\n", timeouts);
            ADVISE(timeouts > 0);
            NOTIFY("alarms=%d\n", alarms);
            ADVISE(alarms > 0);

            EXPECT(waitpid(pid, &status, 0) == pid);
            EXPECT(WIFEXITED(status));
            EXPECT(WEXITSTATUS(status) == 0);

            ASSERT(diminuto_mux_fini(&mux) == (diminuto_mux_t *)0);

            ASSERT(diminuto_ipcl_remove(canonical) >= 0);

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

            ASSERT(diminuto_ipcl_close(listener) >= 0);

            ASSERT(diminuto_mux_init(&mux) == &mux);

            ASSERT(diminuto_delay(diminuto_frequency() * 2, !0) >= 0);

            ASSERT((consumer = diminuto_ipcl_stream_consumer(diminuto_ipcl_path(ENDPOINT, local, sizeof(local)))) >= 0);
            ASSERT(diminuto_mux_register_read(&mux, consumer) >= 0);
            ASSERT(diminuto_mux_register_urgent(&mux, consumer) >= 0);

            totalreceived = 0;
            totalsent = 0;
            done = 0;

            do {

                while (!0) {
                    if ((ready = diminuto_mux_wait(&mux, -1)) > 0) {
                        break;
                    } else if (ready == 0) {
                        diminuto_yield();
                    } else if (errno == EINTR) {
                        CHECKPOINT("consumer interrupted\n");
                    } else {
                        FATAL("diminuto_mux_wait");
                    }
                }

                while ((fd = diminuto_mux_ready_read(&mux)) >= 0) {

                    ASSERT(fd == consumer);

                    ASSERT((readable = diminuto_fd_readable(fd)) >= 0);
                    ASSERT((received = diminuto_ipcl_stream_read(fd, buffer, sizeof(buffer))) >= 0);
                    ASSERT(received <= sizeof(buffer));

                    totalreceived += received;
                    percentage = totalreceived;
                    percentage *= 100;
                    percentage /= TOTAL;
                    COMMENT("consumer received %10zd %10zd %10zu %7.3lf%%\n", readable, received, totalreceived, percentage);

                    if (received == 0) {
                        done = !0;
                        break;
                    }

                    sent = 0;
                    while (sent < received) {
                        ASSERT((sent = diminuto_ipcl_stream_write_generic(fd,  buffer + sent, 1, received - sent)) > 0);
                        ASSERT(sent <= received);

                        totalsent += sent;
                        COMMENT("consumer sent     %10s %10zd %10zu\n", "", sent, totalsent);

                        received -= sent;
                    }

                }

            } while (!done);

            ASSERT(diminuto_mux_close(&mux, consumer) == 0);

            ASSERT(diminuto_mux_fini(&mux) == (diminuto_mux_t *)0);

            EXIT();
        }

        STATUS();

    }

    EXIT();
}
