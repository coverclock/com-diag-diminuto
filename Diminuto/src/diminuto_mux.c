/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013-2016 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_mux.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <unistd.h>
#include <errno.h>

void diminuto_mux_init(diminuto_mux_t * muxp)
{
    FD_ZERO(&muxp->read_or_accept);
    FD_ZERO(&muxp->urgent_or_interrupt);
    diminuto_mux_set_init(&muxp->read);
    diminuto_mux_set_init(&muxp->write);
    diminuto_mux_set_init(&muxp->accept);
    diminuto_mux_set_init(&muxp->urgent);
    sigprocmask(SIG_BLOCK, (sigset_t *)0, &muxp->mask);
}

int diminuto_mux_register(diminuto_mux_t * muxp, diminuto_mux_set_t * setp, int fd)
{
    int rc = -1;

    if (!((0 <= fd) && (fd < FD_SETSIZE))) {
        /* Do nothing. */
    } else if (FD_ISSET(fd, &setp->active)) {
        /* Do nothing. */
    } else {
        FD_SET(fd, &setp->active);
        FD_CLR(fd, &setp->ready);
        if (setp == &muxp->read) { FD_SET(fd, &muxp->read_or_accept); }
        if (setp == &muxp->accept) { FD_SET(fd, &muxp->read_or_accept); }
        if (setp == &muxp->urgent) { FD_SET(fd, &muxp->urgent_or_interrupt); }
        if (setp == &muxp->interrupt) { FD_SET(fd, &muxp->urgent_or_interrupt); }
        if (fd < setp->min) { setp->min = fd; }
        if (fd > setp->max) { setp->max = fd; }
        rc = 0;
    }

    return rc;
}

int diminuto_mux_register_read(diminuto_mux_t * muxp, int fd)
{
    return diminuto_mux_register(muxp, &muxp->read, fd);
}

int diminuto_mux_register_write(diminuto_mux_t * muxp, int fd)
{
    return diminuto_mux_register(muxp, &muxp->write, fd);
}

int diminuto_mux_register_accept(diminuto_mux_t * muxp, int fd)
{
    return diminuto_mux_register(muxp, &muxp->accept, fd);
}

int diminuto_mux_register_urgent(diminuto_mux_t * muxp, int fd)
{
    return diminuto_mux_register(muxp, &muxp->urgent, fd);
}

int diminuto_mux_register_interrupt(diminuto_mux_t * muxp, int fd)
{
    return diminuto_mux_register(muxp, &muxp->interrupt, fd);
}

void diminuto_mux_set_bound(diminuto_mux_set_t * setp)
{
    int min;
    int max;
    int fd;

    min = DIMINUTO_MUX_MOSTPOSITIVE;
    max = DIMINUTO_MUX_MOSTNEGATIVE;
    for (fd = setp->min; fd <= setp->max; ++fd) {
        if (FD_ISSET(fd, &setp->active)) {
            if (fd < min) { min = fd; }
            if (fd > max) { max = fd; }
        }
    }
    setp->min = min;
    setp->max = max;
}

void diminuto_mux_set_normalize(diminuto_mux_set_t * setp)
{
    if (setp->next < 0) {
        /* Do nothing. */
    } else if (setp->max < 0) {
        setp->next = -1; /* None. */
    } else if (setp->next < setp->min) {
        setp->next = setp->min; /* Skip. */
    } else if (setp->next > setp->max) {
        setp->next = setp->min; /* Wrap. */
    } else {
        /* Do nothing. */
    }
}

int diminuto_mux_unregister(diminuto_mux_t * muxp, diminuto_mux_set_t * setp, int fd)
{
    int rc = -1;

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
        if (setp == &muxp->read) { FD_CLR(fd, &muxp->read_or_accept); }
        if (setp == &muxp->accept) { FD_CLR(fd, &muxp->read_or_accept); }
        if (setp == &muxp->urgent) { FD_CLR(fd, &muxp->urgent_or_interrupt); }
        if (setp == &muxp->interrupt) { FD_CLR(fd, &muxp->urgent_or_interrupt); }
        diminuto_mux_set_bound(setp);
        diminuto_mux_set_normalize(setp);
        rc = 0;
    }

    return rc;
}

int diminuto_mux_unregister_read(diminuto_mux_t * muxp, int fd)
{
    return diminuto_mux_unregister(muxp, &muxp->read, fd);
}

int diminuto_mux_unregister_write(diminuto_mux_t * muxp, int fd)
{
    return diminuto_mux_unregister(muxp, &muxp->write, fd);
}

int diminuto_mux_unregister_accept(diminuto_mux_t * muxp, int fd)
{
    return diminuto_mux_unregister(muxp, &muxp->accept, fd);
}

int diminuto_mux_unregister_urgent(diminuto_mux_t * muxp, int fd)
{
    return diminuto_mux_unregister(muxp, &muxp->urgent, fd);
}

int diminuto_mux_unregister_interrupt(diminuto_mux_t * muxp, int fd)
{
    return diminuto_mux_unregister(muxp, &muxp->interrupt, fd);
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

int diminuto_mux_wait(diminuto_mux_t * muxp, diminuto_sticks_t ticks)
{
    int rc = 0;
    struct timespec * top = (struct timespec *)0;
    struct timespec timeout;
    int nfds = 0;
    fd_set read_or_accept;
    fd_set urgent_or_interrupt;
    int fd;

    diminuto_mux_set_census(&muxp->read, &nfds);
    diminuto_mux_set_census(&muxp->write, &nfds);
    diminuto_mux_set_census(&muxp->accept, &nfds);
    diminuto_mux_set_census(&muxp->urgent, &nfds);

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
         * pselect(2) as if they are reading. So the read_or_accept reading set
         * is the union of the read and accept sets. Then after the pselect(2)
         * returns, we have to figure out which of the ready read descriptors
         * are actually registered for read versus registered for accept.
         * I wish there was a portable way to OR and AND fd_sets together, or
         * to see if a set were completely empty.
         */

        read_or_accept = muxp->read_or_accept;
        muxp->write.ready = muxp->write.active;
        urgent_or_interrupt = muxp->urgent_or_interrupt;

        if (ticks >= 0) {
            timeout.tv_sec = diminuto_frequency_ticks2wholeseconds(ticks);
            timeout.tv_nsec = diminuto_frequency_ticks2fractionalseconds(ticks, diminuto_mux_frequency());
            top = &timeout;
        }

        rc = pselect(nfds, &read_or_accept, &muxp->write.ready, &urgent_or_interrupt, top, &muxp->mask);

        /*
         * Can a file descriptor be both read(2)-ing and accept(2)-ing? In
         * decades of writing code based on the BSD socket API that's never been
         * my experience. IMNSHO that's probably a bug on the part of the
         * application. We don't check for that, but it's not supported either.
         * (Similarly for write(2)-ing and accept(2)-ing.)
         */

        if (rc > 0) {
            muxp->read.ready = read_or_accept;
            muxp->accept.ready = read_or_accept;
            muxp->urgent.ready = urgent_or_interrupt;
            muxp->interrupt.ready = urgent_or_interrupt;
            diminuto_mux_set_reset(&muxp->read);
            diminuto_mux_set_reset(&muxp->write);
            diminuto_mux_set_reset(&muxp->accept);
            diminuto_mux_set_reset(&muxp->urgent);
            diminuto_mux_set_reset(&muxp->interrupt);
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

int diminuto_mux_set_ready(diminuto_mux_set_t * setp)
{
    int fd = -1;
    int wrapped;

    /*
     * Thanks to the fact that we have to have the pselect(2) readfds mask do
     * double duty for both read(2) and accept(2), we check for both the active
     * and ready bits below. They will never be different for the write(2) bits,
     * but can be for the read(2) or accept(2) bits. (Ditto for urgent and
     * interrupt.)
     */

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
            setp->next = -1;
        }
    }

    return fd;
}

int diminuto_mux_ready_read(diminuto_mux_t * muxp)
{
    return diminuto_mux_set_ready(&muxp->read);
}

int diminuto_mux_ready_write(diminuto_mux_t * muxp)
{
    return diminuto_mux_set_ready(&muxp->write);
}

int diminuto_mux_ready_accept(diminuto_mux_t * muxp)
{
    return diminuto_mux_set_ready(&muxp->accept);
}

int diminuto_mux_ready_urgent(diminuto_mux_t * muxp)
{
    return diminuto_mux_set_ready(&muxp->urgent);
}

int diminuto_mux_ready_interrupt(diminuto_mux_t * muxp)
{
    return diminuto_mux_set_ready(&muxp->interrupt);
}

int diminuto_mux_close(diminuto_mux_t * muxp, int fd)
{
    int rc;
    int accepting;
    int reading;
    int writing;
    int urgenting;
    int interrupting;

    /*
     * There was a remarkably subtle bug here that bit me because I momentarily
     * forgot that boolean logic in C doesn't _quite_ follow De Morgan's Laws:
     * OR expressions are required to short circuit and not evaluate successive
     * terms when a prior term fails. This removes the side effects (if any)
     * of any later terms.
     */

    accepting = diminuto_mux_unregister_accept(muxp, fd);
    reading = diminuto_mux_unregister_read(muxp, fd);
    writing = diminuto_mux_unregister_write(muxp, fd);
    urgenting = diminuto_mux_unregister_urgent(muxp, fd);
    interrupting = diminuto_mux_unregister_urgent(muxp, fd);

    if ((accepting < 0) && (reading < 0) && (writing < 0) && (urgenting < 0) && (interrupting < 0)) {
        rc = -2;
        errno = EINVAL;
        diminuto_perror("diminuto_mux_close: unregister");
    } else if ((rc = close(fd)) == 0) {
        /* Do nothing. */
    } else {
        diminuto_perror("diminuto_mux_close: close");
    }

    return rc;
}

const char * diminuto_mux_set_name(diminuto_mux_t * muxp, diminuto_mux_set_t * setp)
{
    return (setp == &muxp->read) ? "read" : (setp == &muxp->write) ? "write" : (setp == &muxp->accept) ? "accept" : (setp == &muxp->urgent) ? "urgent" : (setp == &muxp->interrupt) ? "interrupt" : "other";
}

void diminuto_mux_sigs_dump(sigset_t * sigs)
{
    int signum;

    for (signum = 1; signum < NSIG; ++signum) {
        if (sigismember(sigs, signum)) {
            DIMINUTO_LOG_DEBUG(" %d", signum);
        }
    }
}

void diminuto_mux_fds_dump(fd_set * fds)
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

void diminuto_mux_set_dump(diminuto_mux_t * muxp, diminuto_mux_set_t * setp)
{
    const char * name;

    name = diminuto_mux_set_name(muxp, setp);
    DIMINUTO_LOG_DEBUG("mux@%p: %s.next=%d", muxp, name, setp->next);
    DIMINUTO_LOG_DEBUG("mux@%p: %s.min=%d", muxp, name, setp->min);
    DIMINUTO_LOG_DEBUG("mux@%p: %s.max=%d", muxp, name, setp->max);
    DIMINUTO_LOG_DEBUG("mux@%p: %s.active=<", muxp, name); diminuto_mux_fds_dump(&setp->active); DIMINUTO_LOG_DEBUG(">");
    DIMINUTO_LOG_DEBUG("mux@%p: %s.ready=<", muxp, name); diminuto_mux_fds_dump(&setp->ready); DIMINUTO_LOG_DEBUG(">");
}

void diminuto_mux_dump(diminuto_mux_t * muxp)
{
    int signum;

    DIMINUTO_LOG_DEBUG("mux@%p: read_or_accept=<", muxp); diminuto_mux_fds_dump(&muxp->read_or_accept); DIMINUTO_LOG_DEBUG(">");
    DIMINUTO_LOG_DEBUG("mux@%p: urgent_or_interrupt=<", muxp); diminuto_mux_fds_dump(&muxp->urgent_or_interrupt); DIMINUTO_LOG_DEBUG(">");
    diminuto_mux_set_dump(muxp, &muxp->read);
    diminuto_mux_set_dump(muxp, &muxp->write);
    diminuto_mux_set_dump(muxp, &muxp->accept);
    diminuto_mux_set_dump(muxp, &muxp->urgent);
    diminuto_mux_set_dump(muxp, &muxp->interrupt);
    DIMINUTO_LOG_DEBUG("mux@%p: mask=<", muxp); diminuto_mux_sigs_dump(&muxp->mask); DIMINUTO_LOG_DEBUG(">");
}
