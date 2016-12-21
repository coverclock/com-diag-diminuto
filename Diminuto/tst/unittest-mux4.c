/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013-2016 Digital Aggregates Corporation, Colorado, USA<BR>
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
#include "com/diag/diminuto/diminuto_ipc4.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_fd.h"
#include "com/diag/diminuto/diminuto_delay.h"
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

static const int MIN_UNINIT = ~(((int)1)<<((sizeof(int)*8)-1)); /* Most positive integer. */
static const int MAX_UNINIT = (((int)1)<<((sizeof(int)*8)-1)); /* Most negative integer. */
static const int NXT_UNINIT = -1;

#if defined(__arm__)
static const size_t TOTAL = 1024 * 1024 * 100;
#else
static const size_t TOTAL = 1024 * 1024 * 1024;
#endif

static void diminuto_mux_test(diminuto_sticks_t timeout)
{
    diminuto_mux_t mux;
    int pipefd[16][2]; /* 16^2==256 */
    int ss;
    int ii;
    int rc;
    int fd;
    int cc; /* consumers */
    int pp; /* producers */
    uint8_t input[countof(pipefd)];
    uint8_t output[countof(pipefd)];
    uint8_t buffer;
    int reads[1 << (sizeof(uint8_t) * 8)]; /* 1<<(1*8)==256 */
    int writes[1 << (sizeof(uint8_t) * 8)]; /* 1<<(1*8)==256 */
    diminuto_fd_map_t * mapp;
    int * datap;

    ss = diminuto_fd_count();
    mapp = diminuto_fd_map_alloc(ss);
    ASSERT(mapp != (diminuto_fd_map_t *)0);
    datap = malloc(sizeof(*datap) * ss);
    ASSERT(datap != (int *)0);
    ASSERT(diminuto_fd_map_ref(mapp, -1) == (void **)0);
    ASSERT(diminuto_fd_map_ref(mapp, ss) == (void **)0);
    for (ii = 0; ii < ss; ++ii) {
        datap[ii] = -1;
        ASSERT(diminuto_fd_map_ref(mapp, ii) != (void **)0);
        ASSERT(*diminuto_fd_map_ref(mapp, ii) == (void *)0);
        *diminuto_fd_map_ref(mapp, ii) = &datap[ii];
        ASSERT(*diminuto_fd_map_ref(mapp, ii) == &datap[ii]);
    }

    diminuto_mux_init(&mux);

    for (ii = 0; ii < countof(reads); ++ii) {
        reads[ii] = 0;
    }

    for (ii = 0; ii < countof(writes); ++ii) {
        writes[ii] = 0;
    }

    for (ii = 0; ii < countof(pipefd); ++ii) {
        input[ii] = ii * countof(input);
        output[ii] = ii * countof(output);
        ASSERT(pipe(pipefd[ii]) == 0);
        ASSERT(pipefd[ii][0] < ss);
        ASSERT(pipefd[ii][0] >= 0);
        ASSERT(pipefd[ii][1] < ss);
        ASSERT(pipefd[ii][0] != pipefd[ii][1]);
        ASSERT(diminuto_mux_register_read(&mux, pipefd[ii][0]) == 0);
        ASSERT(diminuto_mux_register_write(&mux, pipefd[ii][1]) == 0);
        *(int *)*diminuto_fd_map_ref(mapp, pipefd[ii][0]) = ii;
        *(int *)*diminuto_fd_map_ref(mapp, pipefd[ii][1]) = ii;
    }

    cc = countof(pipefd);
    pp = countof(pipefd);

    while ((cc > 0) || (pp > 0)) {
        rc = diminuto_mux_wait(&mux, timeout);
        if (rc == 0) {
            diminuto_yield();
            continue;
        }
        ASSERT(rc > 0);
        while ((fd = diminuto_mux_ready_read(&mux)) >= 0) {
            ASSERT(fd < ss);
            ii = *(int *)*diminuto_fd_map_ref(mapp, fd);
            ASSERT(ii >= 0);
            ASSERT(ii < countof(pipefd));
            rc = read(fd, &buffer, sizeof(buffer));
            if (rc == 0) {
                DIMINUTO_LOG_DEBUG("cc=%d pp=%d fd=%d ii=%d read(%d)=%d close\n", cc, pp, fd, ii, fd, rc);
                ASSERT(diminuto_mux_close(&mux, fd) == 0);
                --cc;
                continue;
            }
            DIMINUTO_LOG_DEBUG("cc=%d pp=%d fd=%d ii=%d read(%d)=%d %u\n", cc, pp, fd, ii, fd, rc, buffer);
            ASSERT(rc == sizeof(buffer));
            ASSERT(buffer == input[ii]);
            ++reads[buffer];
            ++input[ii];
        }
        while ((fd = diminuto_mux_ready_write(&mux)) >= 0) {
            ASSERT(fd < ss);
            ii = *(int *)*diminuto_fd_map_ref(mapp, fd);
            ASSERT(ii >= 0);
            ASSERT(ii < countof(pipefd));
            buffer = output[ii]++;
            rc = write(fd, &buffer, sizeof(buffer));
            DIMINUTO_LOG_DEBUG("cc=%d pp=%d fd=%d ii=%d write(%d)=%d %u\n", cc, pp, fd, ii, fd, rc, buffer);
            ASSERT(rc == sizeof(buffer));
            ++writes[buffer];
            if (buffer == (((ii + 1) * countof(output)) - 1)) {
                DIMINUTO_LOG_DEBUG("cc=%d pp=%d fd=%d ii=%d write(%d)=%d close\n", cc, pp, fd, ii, fd, rc);
                ASSERT(diminuto_mux_close(&mux, fd) == 0);
                --pp;
                continue;
            }
        }
    }

    ASSERT(cc == 0);
    ASSERT(pp == 0);

    for (ii = 0; ii < countof(reads); ++ii) {
        ASSERT(reads[ii] == 1);
    }

    for (ii = 0; ii < countof(writes); ++ii) {
        ASSERT(writes[ii] == 1);
    }

    free(datap);
    free(mapp);
}

int main(int argc, char ** argv)
{
    extern int diminuto_alarm_debug;

    SETLOGMASK();
    diminuto_alarm_debug = !0;

    {
        diminuto_mux_t mux;

        TEST();

        diminuto_mux_init(&mux);

        diminuto_mux_dump(&mux);

        ASSERT(mux.read.min == MIN_UNINIT);
        ASSERT(mux.read.max == MAX_UNINIT);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == MIN_UNINIT);
        ASSERT(mux.write.max == MAX_UNINIT);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == MIN_UNINIT);
        ASSERT(mux.accept.max == MAX_UNINIT);
        ASSERT(mux.accept.next == NXT_UNINIT);

        STATUS();
    }

    {
        diminuto_mux_t mux;
        int socket;

        TEST();

        socket = STDERR_FILENO + 1; /* Nominally 3. */

        diminuto_mux_init(&mux);
        ASSERT(mux.read.min == MIN_UNINIT);
        ASSERT(mux.read.max == MAX_UNINIT);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == MIN_UNINIT);
        ASSERT(mux.write.max == MAX_UNINIT);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == MIN_UNINIT);
        ASSERT(mux.accept.max == MAX_UNINIT);
        ASSERT(mux.accept.next == NXT_UNINIT);

        ASSERT(diminuto_mux_unregister_read(&mux, STDIN_FILENO) < 0);
        ASSERT(mux.read.min == MIN_UNINIT);
        ASSERT(mux.read.max == MAX_UNINIT);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == MIN_UNINIT);
        ASSERT(mux.write.max == MAX_UNINIT);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == MIN_UNINIT);
        ASSERT(mux.accept.max == MAX_UNINIT);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_unregister_write(&mux, STDOUT_FILENO) < 0);
        ASSERT(mux.read.min == MIN_UNINIT);
        ASSERT(mux.read.max == MAX_UNINIT);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == MIN_UNINIT);
        ASSERT(mux.write.max == MAX_UNINIT);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == MIN_UNINIT);
        ASSERT(mux.accept.max == MAX_UNINIT);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_unregister_write(&mux, STDERR_FILENO) < 0);
        ASSERT(mux.read.min == MIN_UNINIT);
        ASSERT(mux.read.max == MAX_UNINIT);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == MIN_UNINIT);
        ASSERT(mux.write.max == MAX_UNINIT);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == MIN_UNINIT);
        ASSERT(mux.accept.max == MAX_UNINIT);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_unregister_accept(&mux, socket) < 0);
        ASSERT(mux.read.min == MIN_UNINIT);
        ASSERT(mux.read.max == MAX_UNINIT);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == MIN_UNINIT);
        ASSERT(mux.write.max == MAX_UNINIT);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == MIN_UNINIT);
        ASSERT(mux.accept.max == MAX_UNINIT);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_unregister_signal(&mux, SIGHUP) < 0);
        ASSERT(mux.read.min == MIN_UNINIT);
        ASSERT(mux.read.max == MAX_UNINIT);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == MIN_UNINIT);
        ASSERT(mux.write.max == MAX_UNINIT);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == MIN_UNINIT);
        ASSERT(mux.accept.max == MAX_UNINIT);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_unregister_signal(&mux, SIGINT) < 0);
        ASSERT(mux.read.min == MIN_UNINIT);
        ASSERT(mux.read.max == MAX_UNINIT);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == MIN_UNINIT);
        ASSERT(mux.write.max == MAX_UNINIT);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == MIN_UNINIT);
        ASSERT(mux.accept.max == MAX_UNINIT);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_ready_read(&mux) < 0);
        ASSERT(mux.read.min == MIN_UNINIT);
        ASSERT(mux.read.max == MAX_UNINIT);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == MIN_UNINIT);
        ASSERT(mux.write.max == MAX_UNINIT);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == MIN_UNINIT);
        ASSERT(mux.accept.max == MAX_UNINIT);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_ready_write(&mux) < 0);
        ASSERT(mux.read.min == MIN_UNINIT);
        ASSERT(mux.read.max == MAX_UNINIT);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == MIN_UNINIT);
        ASSERT(mux.write.max == MAX_UNINIT);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == MIN_UNINIT);
        ASSERT(mux.accept.max == MAX_UNINIT);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_ready_accept(&mux) < 0);
        ASSERT(mux.read.min == MIN_UNINIT);
        ASSERT(mux.read.max == MAX_UNINIT);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == MIN_UNINIT);
        ASSERT(mux.write.max == MAX_UNINIT);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == MIN_UNINIT);
        ASSERT(mux.accept.max == MAX_UNINIT);
        ASSERT(mux.accept.next == NXT_UNINIT);

        ASSERT(diminuto_mux_register_read(&mux, STDIN_FILENO) == 0);
        ASSERT(mux.read.min == STDIN_FILENO);
        ASSERT(mux.read.max == STDIN_FILENO);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == MIN_UNINIT);
        ASSERT(mux.write.max == MAX_UNINIT);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == MIN_UNINIT);
        ASSERT(mux.accept.max == MAX_UNINIT);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_register_write(&mux, STDOUT_FILENO) == 0);
        ASSERT(mux.read.min == STDIN_FILENO);
        ASSERT(mux.read.max == STDIN_FILENO);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == STDOUT_FILENO);
        ASSERT(mux.write.max == STDOUT_FILENO);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == MIN_UNINIT);
        ASSERT(mux.accept.max == MAX_UNINIT);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_register_write(&mux, STDERR_FILENO) == 0);
        ASSERT(mux.read.min == STDIN_FILENO);
        ASSERT(mux.read.max == STDIN_FILENO);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == STDOUT_FILENO);
        ASSERT(mux.write.max == STDERR_FILENO);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == MIN_UNINIT);
        ASSERT(mux.accept.max == MAX_UNINIT);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_register_accept(&mux, socket) == 0);
        ASSERT(mux.read.min == STDIN_FILENO);
        ASSERT(mux.read.max == STDIN_FILENO);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == STDOUT_FILENO);
        ASSERT(mux.write.max == STDERR_FILENO);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == socket);
        ASSERT(mux.accept.max == socket);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_register_signal(&mux, SIGHUP) == 0);
        ASSERT(mux.read.min == STDIN_FILENO);
        ASSERT(mux.read.max == STDIN_FILENO);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == STDOUT_FILENO);
        ASSERT(mux.write.max == STDERR_FILENO);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == socket);
        ASSERT(mux.accept.max == socket);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_register_signal(&mux, SIGINT) == 0);
        ASSERT(mux.read.min == STDIN_FILENO);
        ASSERT(mux.read.max == STDIN_FILENO);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == STDOUT_FILENO);
        ASSERT(mux.write.max == STDERR_FILENO);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == socket);
        ASSERT(mux.accept.max == socket);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_ready_read(&mux) < 0);
        ASSERT(mux.read.min == STDIN_FILENO);
        ASSERT(mux.read.max == STDIN_FILENO);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == STDOUT_FILENO);
        ASSERT(mux.write.max == STDERR_FILENO);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == socket);
        ASSERT(mux.accept.max == socket);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_ready_write(&mux) < 0);
        ASSERT(mux.read.min == STDIN_FILENO);
        ASSERT(mux.read.max == STDIN_FILENO);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == STDOUT_FILENO);
        ASSERT(mux.write.max == STDERR_FILENO);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == socket);
        ASSERT(mux.accept.max == socket);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_ready_accept(&mux) < 0);
        ASSERT(mux.read.min == STDIN_FILENO);
        ASSERT(mux.read.max == STDIN_FILENO);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == STDOUT_FILENO);
        ASSERT(mux.write.max == STDERR_FILENO);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == socket);
        ASSERT(mux.accept.max == socket);
        ASSERT(mux.accept.next == NXT_UNINIT);

        diminuto_mux_dump(&mux);

        ASSERT(diminuto_mux_register_read(&mux, STDIN_FILENO) < 0);
        ASSERT(mux.read.min == STDIN_FILENO);
        ASSERT(mux.read.max == STDIN_FILENO);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == STDOUT_FILENO);
        ASSERT(mux.write.max == STDERR_FILENO);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == socket);
        ASSERT(mux.accept.max == socket);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_register_write(&mux, STDOUT_FILENO) < 0);
        ASSERT(mux.read.min == STDIN_FILENO);
        ASSERT(mux.read.max == STDIN_FILENO);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == STDOUT_FILENO);
        ASSERT(mux.write.max == STDERR_FILENO);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == socket);
        ASSERT(mux.accept.max == socket);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_register_write(&mux, STDERR_FILENO) < 0);
        ASSERT(mux.read.min == STDIN_FILENO);
        ASSERT(mux.read.max == STDIN_FILENO);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == STDOUT_FILENO);
        ASSERT(mux.write.max == STDERR_FILENO);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == socket);
        ASSERT(mux.accept.max == socket);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_register_accept(&mux, socket) < 0);
        ASSERT(mux.read.min == STDIN_FILENO);
        ASSERT(mux.read.max == STDIN_FILENO);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == STDOUT_FILENO);
        ASSERT(mux.write.max == STDERR_FILENO);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == socket);
        ASSERT(mux.accept.max == socket);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_register_signal(&mux, SIGHUP) < 0);
        ASSERT(mux.read.min == STDIN_FILENO);
        ASSERT(mux.read.max == STDIN_FILENO);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == STDOUT_FILENO);
        ASSERT(mux.write.max == STDERR_FILENO);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == socket);
        ASSERT(mux.accept.max == socket);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_register_signal(&mux, SIGINT) < 0);
        ASSERT(mux.read.min == STDIN_FILENO);
        ASSERT(mux.read.max == STDIN_FILENO);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == STDOUT_FILENO);
        ASSERT(mux.write.max == STDERR_FILENO);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == socket);
        ASSERT(mux.accept.max == socket);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_ready_read(&mux) < 0);
        ASSERT(mux.read.min == STDIN_FILENO);
        ASSERT(mux.read.max == STDIN_FILENO);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == STDOUT_FILENO);
        ASSERT(mux.write.max == STDERR_FILENO);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == socket);
        ASSERT(mux.accept.max == socket);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_ready_write(&mux) < 0);
        ASSERT(mux.read.min == STDIN_FILENO);
        ASSERT(mux.read.max == STDIN_FILENO);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == STDOUT_FILENO);
        ASSERT(mux.write.max == STDERR_FILENO);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == socket);
        ASSERT(mux.accept.max == socket);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_ready_accept(&mux) < 0);
        ASSERT(mux.read.min == STDIN_FILENO);
        ASSERT(mux.read.max == STDIN_FILENO);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == STDOUT_FILENO);
        ASSERT(mux.write.max == STDERR_FILENO);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == socket);
        ASSERT(mux.accept.max == socket);
        ASSERT(mux.accept.next == NXT_UNINIT);

        ASSERT(diminuto_mux_unregister_read(&mux, STDIN_FILENO) == 0);
        ASSERT(mux.read.min == MIN_UNINIT);
        ASSERT(mux.read.max == MAX_UNINIT);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == STDOUT_FILENO);
        ASSERT(mux.write.max == STDERR_FILENO);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == socket);
        ASSERT(mux.accept.max == socket);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_unregister_write(&mux, STDOUT_FILENO) == 0);
        ASSERT(mux.read.min == MIN_UNINIT);
        ASSERT(mux.read.max == MAX_UNINIT);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == STDERR_FILENO);
        ASSERT(mux.write.max == STDERR_FILENO);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == socket);
        ASSERT(mux.accept.max == socket);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_unregister_write(&mux, STDERR_FILENO) == 0);
        ASSERT(mux.read.min == MIN_UNINIT);
        ASSERT(mux.read.max == MAX_UNINIT);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == MIN_UNINIT);
        ASSERT(mux.write.max == MAX_UNINIT);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == socket);
        ASSERT(mux.accept.max == socket);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_unregister_accept(&mux, socket) == 0);
        ASSERT(mux.read.min == MIN_UNINIT);
        ASSERT(mux.read.max == MAX_UNINIT);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == MIN_UNINIT);
        ASSERT(mux.write.max == MAX_UNINIT);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == MIN_UNINIT);
        ASSERT(mux.accept.max == MAX_UNINIT);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_unregister_signal(&mux, SIGHUP) == 0);
        ASSERT(mux.read.min == MIN_UNINIT);
        ASSERT(mux.read.max == MAX_UNINIT);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == MIN_UNINIT);
        ASSERT(mux.write.max == MAX_UNINIT);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == MIN_UNINIT);
        ASSERT(mux.accept.max == MAX_UNINIT);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_unregister_signal(&mux, SIGINT) == 0);
        ASSERT(mux.read.min == MIN_UNINIT);
        ASSERT(mux.read.max == MAX_UNINIT);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == MIN_UNINIT);
        ASSERT(mux.write.max == MAX_UNINIT);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == MIN_UNINIT);
        ASSERT(mux.accept.max == MAX_UNINIT);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_ready_read(&mux) < 0);
        ASSERT(mux.read.min == MIN_UNINIT);
        ASSERT(mux.read.max == MAX_UNINIT);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == MIN_UNINIT);
        ASSERT(mux.write.max == MAX_UNINIT);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == MIN_UNINIT);
        ASSERT(mux.accept.max == MAX_UNINIT);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_ready_write(&mux) < 0);
        ASSERT(mux.read.min == MIN_UNINIT);
        ASSERT(mux.read.max == MAX_UNINIT);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == MIN_UNINIT);
        ASSERT(mux.write.max == MAX_UNINIT);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == MIN_UNINIT);
        ASSERT(mux.accept.max == MAX_UNINIT);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_ready_accept(&mux) < 0);
        ASSERT(mux.read.min == MIN_UNINIT);
        ASSERT(mux.read.max == MAX_UNINIT);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == MIN_UNINIT);
        ASSERT(mux.write.max == MAX_UNINIT);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == MIN_UNINIT);
        ASSERT(mux.accept.max == MAX_UNINIT);
        ASSERT(mux.accept.next == NXT_UNINIT);

        ASSERT(diminuto_mux_unregister_read(&mux, STDIN_FILENO) < 0);
        ASSERT(mux.read.min == MIN_UNINIT);
        ASSERT(mux.read.max == MAX_UNINIT);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == MIN_UNINIT);
        ASSERT(mux.write.max == MAX_UNINIT);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == MIN_UNINIT);
        ASSERT(mux.accept.max == MAX_UNINIT);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_unregister_write(&mux, STDOUT_FILENO) < 0);
        ASSERT(mux.read.min == MIN_UNINIT);
        ASSERT(mux.read.max == MAX_UNINIT);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == MIN_UNINIT);
        ASSERT(mux.write.max == MAX_UNINIT);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == MIN_UNINIT);
        ASSERT(mux.accept.max == MAX_UNINIT);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_unregister_write(&mux, STDERR_FILENO) < 0);
        ASSERT(mux.read.min == MIN_UNINIT);
        ASSERT(mux.read.max == MAX_UNINIT);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == MIN_UNINIT);
        ASSERT(mux.write.max == MAX_UNINIT);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == MIN_UNINIT);
        ASSERT(mux.accept.max == MAX_UNINIT);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_unregister_write(&mux, socket) < 0);
        ASSERT(mux.read.min == MIN_UNINIT);
        ASSERT(mux.read.max == MAX_UNINIT);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == MIN_UNINIT);
        ASSERT(mux.write.max == MAX_UNINIT);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == MIN_UNINIT);
        ASSERT(mux.accept.max == MAX_UNINIT);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_unregister_signal(&mux, SIGHUP) < 0);
        ASSERT(mux.read.min == MIN_UNINIT);
        ASSERT(mux.read.max == MAX_UNINIT);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == MIN_UNINIT);
        ASSERT(mux.write.max == MAX_UNINIT);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == MIN_UNINIT);
        ASSERT(mux.accept.max == MAX_UNINIT);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_unregister_signal(&mux, SIGINT) < 0);
        ASSERT(mux.read.min == MIN_UNINIT);
        ASSERT(mux.read.max == MAX_UNINIT);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == MIN_UNINIT);
        ASSERT(mux.write.max == MAX_UNINIT);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == MIN_UNINIT);
        ASSERT(mux.accept.max == MAX_UNINIT);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_ready_read(&mux) < 0);
        ASSERT(mux.read.min == MIN_UNINIT);
        ASSERT(mux.read.max == MAX_UNINIT);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == MIN_UNINIT);
        ASSERT(mux.write.max == MAX_UNINIT);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == MIN_UNINIT);
        ASSERT(mux.accept.max == MAX_UNINIT);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_ready_write(&mux) < 0);
        ASSERT(mux.read.min == MIN_UNINIT);
        ASSERT(mux.read.max == MAX_UNINIT);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == MIN_UNINIT);
        ASSERT(mux.write.max == MAX_UNINIT);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == MIN_UNINIT);
        ASSERT(mux.accept.max == MAX_UNINIT);
        ASSERT(mux.accept.next == NXT_UNINIT);
        ASSERT(diminuto_mux_ready_accept(&mux) < 0);
        ASSERT(mux.read.min == MIN_UNINIT);
        ASSERT(mux.read.max == MAX_UNINIT);
        ASSERT(mux.read.next == NXT_UNINIT);
        ASSERT(mux.write.min == MIN_UNINIT);
        ASSERT(mux.write.max == MAX_UNINIT);
        ASSERT(mux.write.next == NXT_UNINIT);
        ASSERT(mux.accept.min == MIN_UNINIT);
        ASSERT(mux.accept.max == MAX_UNINIT);
        ASSERT(mux.accept.next == NXT_UNINIT);

        STATUS();
    }

    {
        diminuto_mux_t mux;
        int socket;

        TEST();

        ASSERT((socket = diminuto_ipc4_stream_provider(0)) >= 0);

        diminuto_mux_init(&mux);

        ASSERT(diminuto_mux_register_read(&mux, STDIN_FILENO) == 0);
        ASSERT(diminuto_mux_register_write(&mux, STDOUT_FILENO) == 0);
        ASSERT(diminuto_mux_register_write(&mux, STDERR_FILENO) == 0);
        ASSERT(diminuto_mux_register_accept(&mux, socket) == 0);

        CHECKPOINT();
        ASSERT(diminuto_mux_wait(&mux, -1) == 2);
        CHECKPOINT();

        ASSERT(diminuto_mux_ready_read(&mux) < 0);

        ASSERT(diminuto_mux_ready_write(&mux) == STDOUT_FILENO);
        ASSERT(diminuto_mux_ready_write(&mux) == STDERR_FILENO);
        ASSERT(diminuto_mux_ready_write(&mux) < 0);

        CHECKPOINT();
        ASSERT(diminuto_mux_wait(&mux, -1) == 2);
        CHECKPOINT();

        ASSERT(diminuto_mux_ready_write(&mux) == STDOUT_FILENO);

        CHECKPOINT();
        ASSERT(diminuto_mux_wait(&mux, -1) == 2);
        CHECKPOINT();

        ASSERT(diminuto_mux_ready_write(&mux) == STDERR_FILENO);
        ASSERT(diminuto_mux_ready_write(&mux) == STDOUT_FILENO);
        ASSERT(diminuto_mux_ready_write(&mux) < 0);

        ASSERT(diminuto_mux_unregister_read(&mux, STDIN_FILENO) == 0);
        ASSERT(diminuto_mux_unregister_write(&mux, STDERR_FILENO) == 0);

        CHECKPOINT();
        ASSERT(diminuto_mux_wait(&mux, -1) == 1);
        CHECKPOINT();

        ASSERT(diminuto_mux_ready_write(&mux) == STDOUT_FILENO);
        ASSERT(diminuto_mux_ready_write(&mux) < 0);

        ASSERT(diminuto_mux_unregister_write(&mux, STDOUT_FILENO) == 0);
        ASSERT(diminuto_mux_close(&mux, socket) == 0);

        diminuto_mux_dump(&mux);

        CHECKPOINT();
        ASSERT(diminuto_mux_wait(&mux, -1) == 0);
        CHECKPOINT();

        ASSERT(diminuto_mux_ready_write(&mux) < 0);

        STATUS();
    }

    {
        diminuto_mux_t mux;

        TEST();

        diminuto_mux_init(&mux);

        ASSERT(diminuto_mux_register_read(&mux, STDIN_FILENO) == 0);
        ASSERT(diminuto_mux_register_write(&mux, STDOUT_FILENO) == 0);

        ASSERT(diminuto_mux_wait(&mux, -1) == 1);

        ASSERT(diminuto_mux_ready_read(&mux) < 0);

        ASSERT(diminuto_mux_ready_write(&mux) == STDOUT_FILENO);
        ASSERT(diminuto_mux_ready_write(&mux) < 0);

        ASSERT(diminuto_mux_unregister_write(&mux, STDOUT_FILENO) == 0);
        ASSERT(diminuto_mux_unregister_read(&mux, STDIN_FILENO) == 0);

        STATUS();
    }

    {
        TEST();

        diminuto_mux_test(-1);
        diminuto_mux_test(diminuto_frequency());
        diminuto_mux_test(0);

        STATUS();
    }

    {
        int listener;
        pid_t pid;
        diminuto_port_t rendezvous = 0;
        static const uint8_t ACK = '\006';

        TEST();

        ASSERT((listener = diminuto_ipc4_stream_provider(0)) >= 0);
        ASSERT(diminuto_ipc4_nearend(listener, (diminuto_ipv4_t *)0, &rendezvous) == 0);
        ASSERT(rendezvous > 0);

        ASSERT((pid = fork()) >= 0);

        if (pid) {

            /* PRODUCER */

            diminuto_ipv4_t address;
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
            uint16_t output_16;
            uint8_t input_a;
            uint8_t input_b;
            uint16_t input_16;

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
                    DIMINUTO_LOG_DEBUG("listener timed out\n");
                    ++timeouts;
                } else if (errno != EINTR) {
                    FATAL("diminuto_mux_wait: error");
                } else if (diminuto_alarm_check()) {
                    DIMINUTO_LOG_DEBUG("listener alarmed\n");
                    ++alarms;
                } else {
                    FATAL("diminuto_mux_wait: interrupted");
                }
            }

            ASSERT((fd = diminuto_mux_ready_accept(&mux)) >= 0);
            ASSERT(fd == listener);

            ASSERT((producer = diminuto_ipc4_stream_accept(fd, &address, &port)) >= 0);

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

            ASSERT((sent = diminuto_ipc4_datagram_send_flags(producer, &ACK, sizeof(ACK), address, 0, MSG_OOB)) == sizeof(ACK));
            DIMINUTO_LOG_DEBUG("producer ACKing   %10s %10d %10u %7.3lf%%\n", "", sent, 0, 0.0);

            do {

                while (!0) {
                    if ((ready = diminuto_mux_wait(&mux, diminuto_frequency() / 10)) > 0) {
                        break;
                    } else if (ready == 0) {
                        DIMINUTO_LOG_DEBUG("producer timed out\n");
                        ++timeouts;
                    } else if (errno != EINTR) {
                        FATAL("diminuto_mux_wait: error");
                    } else if (diminuto_alarm_check()) {
                        DIMINUTO_LOG_DEBUG("producer alarmed\n");
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

                        ASSERT((sent = diminuto_ipc4_stream_write(fd, here, 1, used)) > 0);
                        ASSERT(sent <= used);
                        output_16 = diminuto_fletcher_16(here, sent, &output_a, &output_b);

                        totalsent += sent;
                        percentage = totalsent;
                        percentage *= 100;
                        percentage /= TOTAL;
                        DIMINUTO_LOG_DEBUG("producer sent     %10s %10d %10u %7.3lf%%\n", "", sent, totalsent, percentage);

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
                    ASSERT((received = diminuto_ipc4_stream_read(fd, there, 1, available)) > 0);
                    ASSERT(received <= available);
                    input_16 = diminuto_fletcher_16(there, received, &input_a, &input_b);

                    totalreceived += received;
                    DIMINUTO_LOG_DEBUG("producer received %10d %10d %10u\n", readable, received, totalreceived);

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

            ADVISE(timeouts > 0);
            ADVISE(alarms > 0);

            EXPECT(waitpid(pid, &status, 0) == pid);
            EXPECT(WIFEXITED(status));
            EXPECT(WEXITSTATUS(status) == 0);

            STATUS();

        } else {

            /* CONSUMER */

            diminuto_ipv4_t address;
            diminuto_port_t port;
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
            int proceed;
            ssize_t readable;
            double percentage;

            ASSERT(diminuto_ipc4_close(listener) >= 0);

            diminuto_mux_init(&mux);

            ASSERT((consumer = diminuto_ipc4_stream_consumer(diminuto_ipc4_address("localhost"), rendezvous)) >= 0);
            ASSERT(diminuto_mux_register_read(&mux, consumer) == 0);
            ASSERT(diminuto_mux_register_urgent(&mux, consumer) == 0);

            totalreceived = 0;
            totalsent = 0;
            done = 0;
            proceed = 0;

            do {

                while (!0) {
                    if ((ready = diminuto_mux_wait(&mux, -1)) > 0) {
                        break;
                    } else if (ready == 0) {
                        diminuto_yield();
                    } else if (errno == EINTR) {
                        DIMINUTO_LOG_DEBUG("consumer interrupted\n");
                    } else {
                        FATAL("diminuto_mux_wait");
                    }
                }

                if (!proceed) {
                    while ((fd = diminuto_mux_ready_urgent(&mux)) >= 0) {
                        ASSERT(fd == consumer);
                        buffer[0] = '\0';
                        ASSERT((received = diminuto_ipc4_datagram_receive_flags(fd, buffer, 1, (diminuto_ipv4_t *)0, (diminuto_port_t *)0, MSG_OOB | MSG_DONTWAIT)) == 1);
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

                while ((fd = diminuto_mux_ready_read(&mux)) >= 0) {

                    ASSERT(fd == consumer);

                    ASSERT((readable = diminuto_fd_readable(fd)) >= 0);
                    ASSERT((received = diminuto_ipc4_stream_read(fd, buffer, 1, sizeof(buffer))) >= 0);
                    ASSERT(received <= sizeof(buffer));

                    totalreceived += received;
                    percentage = totalreceived;
                    percentage *= 100;
                    percentage /= TOTAL;
                    DIMINUTO_LOG_DEBUG("consumer received %10d %10d %10u %7.3lf%%\n", readable, received, totalreceived, percentage);

                    if (received == 0) {
                        done = !0;
                        break;
                    }

                    sent = 0;
                    while (sent < received) {
                        ASSERT((sent = diminuto_ipc4_stream_write(fd,  buffer + sent, 1, received - sent)) > 0);
                        ASSERT(sent <= received);

                        totalsent += sent;
                        DIMINUTO_LOG_DEBUG("consumer sent     %10s %10d %10u\n", "", sent, totalsent);

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
