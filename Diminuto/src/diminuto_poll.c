/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2015-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Poll feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Poll feature.
 */

#include "com/diag/diminuto/diminuto_poll.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_heap.h"
#include <signal.h>
#include <errno.h>
#include <string.h>
#include "../src/diminuto_mux.h"

#if !defined(__USE_GNU)
#   warning ppoll(2) not available on this platform!
static inline int ppoll(struct pollfd *__fds, nfds_t __nfds, const struct timespec *__timeout, const __sigset_t *__ss)
{
    errno = ENOSYS;
    return -1;
}
#endif

diminuto_poll_t * diminuto_poll_init(diminuto_poll_t * pollp)
{
    diminuto_mux_init(&pollp->mux);
    pollp->pollfd = (struct pollfd *)0;
    pollp->nfds = 0;
    pollp->min = DIMINUTO_MUX_MOSTPOSITIVE;
    pollp->max = DIMINUTO_MUX_MOSTNEGATIVE;
    pollp->refresh = 0;
    return pollp;
}

diminuto_poll_t * diminuto_poll_fini(diminuto_poll_t * pollp)
{
    free(pollp->pollfd);
    pollp->pollfd = (struct pollfd *)0;
    pollp->nfds = 0;
    pollp->min = DIMINUTO_MUX_MOSTPOSITIVE;
    pollp->max = DIMINUTO_MUX_MOSTNEGATIVE;
    pollp->refresh = !0;
    return (diminuto_poll_t *)0;
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
    } else if (!(pollp->refresh)) {
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

    diminuto_log_emit("poll@%p: nfds=%lu\n", pollp, (long unsigned int)pollp->nfds);
    diminuto_log_emit("poll@%p: min=%d\n", pollp, pollp->min);
    diminuto_log_emit("poll@%p: max=%d\n", pollp, pollp->max);
    diminuto_log_emit("poll@%p: refresh=%d\n", pollp, pollp->refresh);
    diminuto_log_emit("poll@%p: pollfd=%p\n", pollp, pollp->pollfd);
    for (ii = 0; ii < pollp->nfds; ++ii) {
        pollfdp = &pollp->pollfd[ii];
        if (pollfdp->fd >= 0) {
            diminuto_log_emit("poll@%p: pollfd[%lu].fd=%d\n", pollp, (long unsigned int)ii, pollfdp->fd);
            diminuto_log_emit("poll@%p: pollfd[%lu].events=0x%x\n", pollp, (long unsigned int)ii, pollfdp->events);
            diminuto_log_emit("poll@%p: pollfd[%lu].revents=0x%x\n", pollp, (long unsigned int)ii, pollfdp->revents);
        }
    }
    diminuto_mux_dump(&pollp->mux);
}
