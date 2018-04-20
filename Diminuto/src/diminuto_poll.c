/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2015-2016 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "diminuto_mux.h"
#include "com/diag/diminuto/diminuto_poll.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_heap.h"
#include <signal.h>
#include <errno.h>
#include <string.h>

void diminuto_poll_init(diminuto_poll_t * pollp)
{
    diminuto_mux_init(&pollp->mux);
    pollp->pollfd = (struct pollfd *)0;
    pollp->nfds = 0;
    pollp->min = DIMINUTO_MUX_MOSTPOSITIVE;
    pollp->max = DIMINUTO_MUX_MOSTNEGATIVE;
    pollp->refresh = 0;
}

void diminuto_poll_fini(diminuto_poll_t * pollp)
{
    free(pollp->pollfd);
    pollp->pollfd = (struct pollfd *)0;
    pollp->nfds = 0;
    pollp->min = DIMINUTO_MUX_MOSTPOSITIVE;
    pollp->max = DIMINUTO_MUX_MOSTNEGATIVE;
    pollp->refresh = !0;
}

static inline void poll_bounds(diminuto_poll_t * pollp, diminuto_mux_set_t * setp)
{
    if (setp->min < pollp->min) { pollp->min = setp->min; }
    if (setp->max > pollp->max) { pollp->max = setp->max; }
}

static void poll_before(diminuto_poll_t * pollp, diminuto_mux_set_t * setp, short mask)
{
    if (setp->max >= 0) {
        int fd;
        for (fd = setp->min; fd <= setp->max; ++fd) {
            if (FD_ISSET(fd, &setp->active)) {
                struct pollfd * pollfdp;
                pollfdp = &(pollp->pollfd[fd - pollp->min]);
                pollfdp->fd = fd;
                pollfdp->events |= mask;
            }
        }
    }
}

static void poll_after(diminuto_poll_t * pollp, diminuto_mux_set_t * setp, short mask)
{
    if (setp->max >= 0) {
        int fd;
        for (fd = setp->min; fd <= setp->max; ++fd) {
            struct pollfd * pollfdp;
            pollfdp = &(pollp->pollfd[fd - pollp->min]);
            if (pollfdp->fd < 0) {
                /* Do nothing. */
            } else if ((pollfdp->revents & mask) == 0) {
                FD_CLR(fd, &setp->ready);
            } else {
                FD_SET(fd, &setp->ready);
            }
        }
    }
}

int diminuto_poll_wait_generic(diminuto_poll_t * pollp, diminuto_sticks_t ticks, const sigset_t * maskp)
{
    int rc = 0;
    struct timespec * top = (struct timespec *)0;
    struct timespec timeout;
    nfds_t nfds = 0;
    int fd;
    int min;
    int max;
    struct pollfd * pollfdp;

    pollp->min = DIMINUTO_MUX_MOSTPOSITIVE;
    pollp->max = DIMINUTO_MUX_MOSTNEGATIVE;

    poll_bounds(pollp, &pollp->mux.read);
    poll_bounds(pollp, &pollp->mux.write);
    poll_bounds(pollp, &pollp->mux.accept);
    poll_bounds(pollp, &pollp->mux.urgent);
    poll_bounds(pollp, &pollp->mux.interrupt);

    if (pollp->min <= pollp->max) { nfds = pollp->max - pollp->min + 1; }

    if (nfds > pollp->nfds) {
        pollp->pollfd = diminuto_heap_realloc(pollp->pollfd, nfds * sizeof(struct pollfd));
        pollp->nfds = nfds;
    }

    if (nfds <= 0) {
        /* Do nothing. */
    } else if (!pollp->refresh) {
        /* Do nothing. */
    } else {
        for (fd = 0; fd < pollp->nfds; ++fd) {
            pollfdp = &(pollp->pollfd[fd]);
            pollfdp->fd = -1;
            pollfdp->events = 0;
            pollfdp->revents = 0;
        }
        poll_before(pollp, &pollp->mux.read, POLLIN);
        poll_before(pollp, &pollp->mux.write, POLLOUT);
        poll_before(pollp, &pollp->mux.accept, POLLIN);
        poll_before(pollp, &pollp->mux.urgent, POLLPRI);
        poll_before(pollp, &pollp->mux.interrupt, POLLPRI);
        pollp->refresh = 0;
    }

    if (ticks >= 0) {
        timeout.tv_sec = diminuto_frequency_ticks2wholeseconds(ticks);
        timeout.tv_nsec = diminuto_frequency_ticks2fractionalseconds(ticks, diminuto_mux_frequency());
        top = &timeout;
    }

    if ((nfds > 0) || (ticks >= 0)) {

        rc = ppoll(pollp->pollfd, nfds, top, maskp);

        if (rc > 0) {
            poll_after(pollp, &pollp->mux.read, POLLIN);
            poll_after(pollp, &pollp->mux.write, POLLOUT);
            poll_after(pollp, &pollp->mux.accept, POLLIN);
            poll_after(pollp, &pollp->mux.urgent, POLLPRI);
            poll_after(pollp, &pollp->mux.interrupt, POLLPRI);
            diminuto_mux_set_reset(&pollp->mux.read);
            diminuto_mux_set_reset(&pollp->mux.write);
            diminuto_mux_set_reset(&pollp->mux.accept);
            diminuto_mux_set_reset(&pollp->mux.urgent);
            diminuto_mux_set_reset(&pollp->mux.interrupt);
        } else if (rc == 0) {
            /* Do nothing. */
        } else if (errno == EINTR) {
            /* Do nothing. */
        } else {
            diminuto_perror("diminuto_poll_wait");
        }

    }

    return rc;
}

void diminuto_poll_dump(diminuto_poll_t * pollp)
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
