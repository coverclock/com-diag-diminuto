/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013-2015 Digital Aggregates Corporation, Colorado, USA<BR>
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
#include "com/diag/diminuto/diminuto_ipc.h"
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


static const int MIN_UNINIT = ~(((int)1)<<((sizeof(int)*8)-1)); /* Most positive integer. */
static const int MAX_UNINIT = (((int)1)<<((sizeof(int)*8)-1)); /* Most negative integer. */
static const int NXT_UNINIT = -1;

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

static void diminuto_mux_test(diminuto_ticks_t timeout)
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

        ASSERT((socket = diminuto_ipc_stream_provider(0)) >= 0);

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

        ASSERT(diminuto_mux_close(&mux, STDIN_FILENO) == 0);
        ASSERT(diminuto_mux_close(&mux, STDOUT_FILENO) == 0);

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
        pid_t pid;
        diminuto_port_t rendezvous = 0;

        TEST();

        ASSERT((pid = fork()) >= 0);

        if (pid) {

            /* PRODUCER */

            int listener;
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

            diminuto_mux_init(&mux);

            sigemptyset(&mask);
            sigaddset(&mask, SIGALRM);
            sigprocmask(SIG_BLOCK, &mask, (sigset_t *)0);

            ASSERT((listener = diminuto_ipc_stream_provider(0)) >= 0);
            ASSERT(diminuto_ipc_nearend(listener, (diminuto_ipv4_t *)0, &rendezvous) == 0);
            ASSERT(rendezvous > 0);
            ASSERT(diminuto_mux_register_accept(&mux, listener) == 0);

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
            ASSERT((producer = diminuto_ipc_stream_accept(fd, &address, &port)) >= 0);

            ASSERT(diminuto_mux_register_read(&mux, producer) == 0);
            ASSERT(diminuto_mux_register_write(&mux, producer) == 0);
            ASSERT(diminuto_mux_unregister_signal(&mux, SIGALRM) == 0);

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

                    if (totalsent < TOTAL) {

                        if (used > (TOTAL - totalsent)) {
                            used = TOTAL - totalsent;
                        }

                        ASSERT((sent = diminuto_ipc_stream_write(fd, here, 1, used)) > 0);
                        ASSERT(sent <= used);

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

                    ASSERT((readable = diminuto_fd_readable(fd)) > 0);
                    ASSERT((received = diminuto_ipc_stream_read(fd, there, 1, available)) > 0);
                    ASSERT(received <= available);

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
            ASSERT(diminuto_ipc_close(listener) >= 0);

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

            diminuto_mux_init(&mux);

            diminuto_delay(diminuto_delay_frequency(), !0);

            ASSERT((consumer = diminuto_ipc_stream_consumer(diminuto_ipc_address("localhost"), rendezvous)) >= 0);
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

                    ASSERT((readable = diminuto_fd_readable(fd)) >= 0);
                    ASSERT((received = diminuto_ipc_stream_read(fd, buffer, 1, sizeof(buffer))) >= 0);
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
                        ASSERT((sent = diminuto_ipc_stream_write(fd,  buffer + sent, 1, received - sent)) > 0);
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
