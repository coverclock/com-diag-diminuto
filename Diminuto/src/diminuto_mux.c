/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013-2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_mux.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <signal.h>
#include <unistd.h>
#include <errno.h>

static const int MIN_UNINIT = ~(((int)1)<<((sizeof(int)*8)-1)); /* Most positive integer. */
static const int MAX_UNINIT = (((int)1)<<((sizeof(int)*8)-1)); /* Most negative integer. */
static const int NXT_UNINIT = -1;

static inline void mux_set_init(diminuto_mux_set_t * setp)
{
    setp->next = -1;
    setp->min = MIN_UNINIT;
    setp->max = MAX_UNINIT;
    FD_ZERO(&setp->active);
    FD_ZERO(&setp->ready);
}

void diminuto_mux_init(diminuto_mux_t * muxp)
{
    FD_ZERO(&muxp->effective);
    mux_set_init(&muxp->read);
    mux_set_init(&muxp->write);
    mux_set_init(&muxp->accept);
    sigprocmask(SIG_BLOCK, (sigset_t *)0, &muxp->mask);
}

static int mux_register(diminuto_mux_t * muxp, diminuto_mux_set_t * setp, int fd, int effective)
{
    int rc = -1;

    if (!((0 <= fd) && (fd < FD_SETSIZE))) {
        /* Do nothing. */
    } else if (FD_ISSET(fd, &setp->active)) {
        /* Do nothing. */
    } else {
        FD_SET(fd, &setp->active);
        FD_CLR(fd, &setp->ready);
        if (effective) { FD_SET(fd, &muxp->effective); }
        if (fd < setp->min) { setp->min = fd; }
        if (fd > setp->max) { setp->max = fd; }
        rc = 0;
    }

    return rc;
}

int diminuto_mux_register_read(diminuto_mux_t * muxp, int fd)
{
    return mux_register(muxp, &muxp->read, fd, !0);
}

int diminuto_mux_register_write(diminuto_mux_t * muxp, int fd)
{
    return mux_register(muxp, &muxp->write, fd, 0);
}

int diminuto_mux_register_accept(diminuto_mux_t * muxp, int fd)
{
    return mux_register(muxp, &muxp->accept, fd, !0);
}

static int mux_unregister(diminuto_mux_t * muxp, diminuto_mux_set_t * setp, int fd, int effective)
{
    int rc = -1;
    int min;
    int max;

    /*
     * This code exposed an interesting bug in Cygwin: FD_ISSET(3) is supposed
     * to return an int but in Cygwin it returns a long. This works on platforms
     * on which int and long are the same. But in Cygwin for x86_64, int is four
     * bytes and long is eight bytes. And on Cygwin, FD_ISSET(3) returns not a
     * boolean (0 or !0) but a shifted bit and'ed out from the long file
     * descriptor mask. Demoting the long returned by FD_ISSET(3) to an int
     * turns a return for file descriptor values from 32 to 63 (or any multiple
     * of those) into a zero. Using idioms like "!" and "!!" to reduce the long
     * to whatever a boolean might be (int) eliminates this issue.
     */

    if (!((0 <= fd) && (fd < FD_SETSIZE))) {
        /* Do nothing. */
    } else if (!FD_ISSET(fd, &setp->active)) {
        /* Do nothing. */
    } else {
        FD_CLR(fd, &setp->active);
        FD_CLR(fd, &setp->ready);
        if (effective) { FD_CLR(fd, &muxp->effective); }
        min = MIN_UNINIT;
        max = MAX_UNINIT;
        for (fd = setp->min; fd <= setp->max; ++fd) {
            if (FD_ISSET(fd, &setp->active)) {
                if (fd < min) { min = fd; }
                if (fd > max) { max = fd; }
            }
        }
        if (setp->next < 0) {
            /* Do nothing. */
        } else if (max < 0) {
            setp->next = NXT_UNINIT; /* None. */
        } else if (setp->next < min) {
            setp->next = min; /* Skip. */
        } else if (setp->next > max) {
            setp->next = min; /* Wrap. */
        } else {
            /* Do nothing. */
        }
        setp->min = min;
        setp->max = max;
        rc = 0;
    }

    return rc;
}

int diminuto_mux_unregister_read(diminuto_mux_t * muxp, int fd)
{
    return mux_unregister(muxp, &muxp->read, fd, !0);
}

int diminuto_mux_unregister_write(diminuto_mux_t * muxp, int fd)
{
    return mux_unregister(muxp, &muxp->write, fd, 0);
}

int diminuto_mux_unregister_accept(diminuto_mux_t * muxp, int fd)
{
    return mux_unregister(muxp, &muxp->accept, fd, !0);
}

int diminuto_mux_register_signal(diminuto_mux_t * muxp, int signum)
{
    int rc = -1;

    if (sigismember(&muxp->mask, signum) != 0) {
        /* Do nothing. */
    } else if (sigaddset(&muxp->mask, signum) < 0) {
        /* Do nothing. */
    } else {
        rc = 0;
    }

    return rc;
}

int diminuto_mux_unregister_signal(diminuto_mux_t * muxp, int signum)
{
    int rc = -1;

    if (sigismember(&muxp->mask, signum) <= 0) {
        /* Do nothing. */
    } else if (sigdelset(&muxp->mask, signum) < 0) {
        /* Do nothing. */
    } else {
        rc = 0;
    }

    return rc;
}

static inline void mux_set_census(diminuto_mux_set_t * setp, int * nfdsp)
{
    if (*nfdsp <= setp->max) { *nfdsp = setp->max + 1; }
}

static inline void mux_set_reset(diminuto_mux_set_t * setp)
{
    if (setp->next >= 0) {
        /* Do nothing. */
    } else if (setp->max < 0) {
       /* Do nothing. */
    } else {
        setp->next = setp->min;
    }
}

int diminuto_mux_wait(diminuto_mux_t * muxp, diminuto_ticks_t ticks)
{
    int rc = 0;
    struct timespec * top = (struct timespec *)0;
    struct timespec timeout;
    int nfds = 0;
    fd_set effective;
    int fd;

    mux_set_census(&muxp->read, &nfds);
    mux_set_census(&muxp->write, &nfds);
    mux_set_census(&muxp->accept, &nfds);

    /*
     * It's perfectly legal to call this function with no registered file
     * descriptors. In fact, that's a common idiom in POSIX systems, using
     * the select(2) system call (or here, pselect(2)) to wait. But if you call
     * this function with no registered file descriptors _and_ an infinite wait
     * time, that's probably not correct. So we check for that. We should
     * probably return with an error, but instead we just indicate that no file
     * descriptors are ready.
     */

    if ((nfds > 0) || (ticks >= 0)) {

        /*
         * Handling listen(2)-ing file descriptors that will want to do an
         * accept(2) is complicated by the fact that they are handled by
         * pselect(2) as if they are reading. So the effective reading set
         * is the union of the read and accept sets. Then after the pselect(2)
         * returns, we have to figure out which of the ready read descriptors
         * are actually registered for read versus registered for accept.
         * I wish there was a portable way to OR and AND fd_sets together, or
         * to see if a set were completely empty.
         */

        effective = muxp->effective;
        muxp->write.ready = muxp->write.active;

        if (ticks >= 0) {
            timeout.tv_sec = diminuto_frequency_ticks2wholeseconds(ticks);
            timeout.tv_nsec = diminuto_frequency_ticks2fractionalseconds(ticks, diminuto_mux_frequency());
            top = &timeout;
        }

        rc = pselect(nfds, &effective, &muxp->write.ready, (fd_set *)0, top, &muxp->mask);

        /*
         * Can a file descriptor be both read(2)-ing and accept(2)-ing? In
         * decades of writing code based on the BSD socket API that's never been
         * my experience. IMNSHO that's probably a bug on the part of the
         * application. We don't check for that, but it's not supported either.
         * (Similarly for write(2)-ing and accept(2)-ing.)
         */

        if (rc > 0) {
            muxp->read.ready = effective;
            muxp->accept.ready = effective;
            mux_set_reset(&muxp->read);
            mux_set_reset(&muxp->write);
            mux_set_reset(&muxp->accept);
        } else if (rc == 0) {
            /* Do nothing. */
        } else if (errno == EINTR) {
            /* Do nothing. */
        } else {
            diminuto_perror("diminuto_mux_wait");
        }

    }

    return rc;
}

/*
 * Thanks to the fact that we have to have the pselect(2) readfds mask do double
 * duty for both read(2) and accept(2), we check for both the active and ready
 * bits below. They will never be different for the write(2) bits, but can be
 * for the read(2) or accept(2) bits.
 */
static int mux_set_ready(diminuto_mux_set_t * setp)
{
    int fd = -1;
    int wrapped;

    if ((wrapped = setp->next) >= 0) {
        do {
            if (FD_ISSET(setp->next, &setp->ready)) {
                if (FD_ISSET(setp->next, &setp->active)) {
                    fd = setp->next;
                }
                FD_CLR(fd, &setp->ready);
            }
            setp->next = (setp->next < setp->max) ? (setp->next + 1) : setp->min;
        } while ((fd < 0) && (setp->next != wrapped));
        if (fd < 0) {
            setp->next = NXT_UNINIT;
        }
    }

    return fd;
}

int diminuto_mux_ready_read(diminuto_mux_t * muxp)
{
    return mux_set_ready(&muxp->read);
}

int diminuto_mux_ready_write(diminuto_mux_t * muxp)
{
    return mux_set_ready(&muxp->write);
}

int diminuto_mux_ready_accept(diminuto_mux_t * muxp)
{
    return mux_set_ready(&muxp->accept);
}

int diminuto_mux_close(diminuto_mux_t * muxp, int fd)
{
    int rc = -2;

    if ((diminuto_mux_unregister_read(muxp, fd) < 0) && (diminuto_mux_unregister_write(muxp, fd) < 0) && (diminuto_mux_unregister_accept(muxp, fd) < 0)) {
        /* Do nothing. */
    } else if ((rc = close(fd)) == 0) {
        /* Do nothing. */
    } else {
        diminuto_perror("diminuto_mux_close");
    }

    return rc;
}
