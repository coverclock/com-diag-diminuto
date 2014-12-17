/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013-2014 Digital Aggregates Corporation, Colorado, USA<BR>
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

static void diminuto_mux_set_init(diminuto_mux_t * muxp, diminuto_mux_set_t * setp)
{
	setp->next = -1;
	FD_ZERO(&setp->active);
	FD_ZERO(&setp->ready);
}

void diminuto_mux_init(diminuto_mux_t * muxp)
{
	muxp->maxfd = -1;
	diminuto_mux_set_init(muxp, &muxp->read);
	diminuto_mux_set_init(muxp, &muxp->write);
	sigprocmask(SIG_BLOCK, (sigset_t *)0, &muxp->mask);
}

static int diminuto_mux_set_register(diminuto_mux_t * muxp, diminuto_mux_set_t * setp, int fd)
{
	int rc;

	rc = !FD_ISSET(fd, &setp->active);
	if (rc) {
		FD_SET(fd, &setp->active);
		FD_CLR(fd, &setp->ready);
		if (fd > muxp->maxfd) {
			muxp->maxfd = fd;
		}
	}

	return rc ? 0 : -1;
}

int diminuto_mux_register_read(diminuto_mux_t * muxp, int fd)
{
	return diminuto_mux_set_register(muxp, &muxp->read, fd);
}

int diminuto_mux_register_write(diminuto_mux_t * muxp, int fd)
{
	return diminuto_mux_set_register(muxp, &muxp->write, fd);
}

static int diminuto_mux_set_unregister(diminuto_mux_t * muxp, diminuto_mux_set_t * setp, int fd)
{
	int rc;
	int maxfd;
	int reads;
	int writes;

	/*
	 * This code exposed an interesting bug in Cygwin: FD_ISSET(3) is supposed
	 * to return an int but in Cygwin it returns a long. This works on platforms
	 * on which int and long are the same. But in Cygwin for x86_64, int is four
	 * bytes and long is eight bytes. And on Cygwin, FD_ISSET(3) returns not a
	 * boolean (0 or !0) but a shifted bit and'ed out from the long file
	 * descriptor mask. Demoting the long returned by FD_ISSET(3) to an int
	 * turns returns for file descriptor values from 32 to 63 (or any multiple
	 * of those) into a zero. I fixed this by the "!!" to reduce the long to a
	 * boolean before it is assigned to the int. The register function already
	 * effectively had this fix in it as a side effect of its "!", which was
	 * more or less an accident of my coding style that it was there at all.
	 */

	rc = !!FD_ISSET(fd, &setp->active);
	if (rc) {
		FD_CLR(fd, &setp->active);
		FD_CLR(fd, &setp->ready);
		if (fd == muxp->maxfd) {
			maxfd = muxp->maxfd;
			muxp->maxfd = -1;
			reads = 0;
			writes = 0;
			for (fd = 0; fd < maxfd; ++fd) {
				if (FD_ISSET(fd, &muxp->read.active)) {
					muxp->maxfd = fd;
					++reads;
				}
				if (FD_ISSET(fd, &muxp->write.active)) {
					muxp->maxfd = fd;
					++writes;
				}
			}
			if (reads == 0) {
				muxp->read.next = -1;
			} else if (muxp->read.next > muxp->maxfd) {
				muxp->read.next = 0;
			} else {
				/* Do nothing. */
			}
			if (writes == 0) {
				muxp->write.next = -1;
			} else if (muxp->write.next > muxp->maxfd) {
				muxp->write.next = 0;
			} else {
				/* Do nothing. */
			}
		}
	}

	return rc ? 0 : -1;
}

int diminuto_mux_unregister_read(diminuto_mux_t * muxp, int fd)
{
	return diminuto_mux_set_unregister(muxp, &muxp->read, fd);
}

int diminuto_mux_unregister_write(diminuto_mux_t * muxp, int fd)
{
	return diminuto_mux_set_unregister(muxp, &muxp->write, fd);
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

int diminuto_mux_wait(diminuto_mux_t * muxp, diminuto_ticks_t ticks)
{
	int rc = 0;
	struct timespec * top = (struct timespec *)0;
	struct timespec timeout;
	int nfds;

	nfds = (muxp->maxfd >= 0) ? (muxp->maxfd + 1) : 0;

	/*
	 * It's perfectly legal to call this function with no registered file
	 * descriptors. In fact, that's a common idiom in POSIX systems, using
	 * the select(2) system call (or here, pselect(2)) to wait. But if you call
	 * this function with no registered file descriptors _and_ an infinite wait
	 * time, that's probably not correct. So we check for that.
	 */

	if ((nfds > 0) || (ticks >= 0)) {

		muxp->read.ready = muxp->read.active;
		muxp->write.ready = muxp->write.active;

		if (ticks >= 0) {
			timeout.tv_sec = diminuto_frequency_ticks2wholeseconds(ticks);
			timeout.tv_nsec = diminuto_frequency_ticks2fractionalseconds(ticks, diminuto_mux_frequency());
			top = &timeout;
		}

		rc = pselect(nfds, &muxp->read.ready, &muxp->write.ready, (fd_set *)0, top, &muxp->mask);

		if (rc > 0) {
			if (muxp->read.next < 0) {
				muxp->read.next = 0;
			}
			if (muxp->write.next < 0) {
				muxp->write.next = 0;
			}
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

static int diminuto_mux_ready(diminuto_mux_t * muxp, diminuto_mux_set_t * setp)
{
	int fd = -1;
	int limit;
	int modulo;

	if (setp->next >= 0) {

		limit = setp->next;
		modulo = muxp->maxfd + 1;

		do {

			if (FD_ISSET(setp->next, &setp->ready)) {
				fd = setp->next;
				FD_CLR(fd, &setp->ready);
			}

			setp->next = (setp->next + 1) % modulo;

		} while ((fd < 0) && (setp->next != limit));

	}

	return fd;
}

int diminuto_mux_ready_read(diminuto_mux_t * muxp)
{
	return diminuto_mux_ready(muxp, &muxp->read);
}

int diminuto_mux_ready_write(diminuto_mux_t * muxp)
{
	return diminuto_mux_ready(muxp, &muxp->write);
}

int diminuto_mux_close(diminuto_mux_t * muxp, int fd)
{
	int rc = -2;

	if ((diminuto_mux_unregister_read(muxp, fd) < 0) && (diminuto_mux_unregister_write(muxp, fd) < 0)) {
		/* Do nothing. */
	} else if ((rc = close(fd)) == 0) {
		/* Do nothing. */
	} else {
		diminuto_perror("diminuto_mux_close");
	}

	return rc;
}
