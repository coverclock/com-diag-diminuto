/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_mux.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <signal.h>
#include <unistd.h>
#include <errno.h>

static void diminuto_mux_set_init(diminuto_mux_t * that, diminuto_mux_set_t * set)
{
	set->next = -1;
	FD_ZERO(&set->active);
	FD_ZERO(&set->ready);
}

void diminuto_mux_init(diminuto_mux_t * that)
{
	that->nfds = -1;
	diminuto_mux_set_init(that, &that->read);
	diminuto_mux_set_init(that, &that->write);
	sigprocmask(SIG_BLOCK, (sigset_t *)0, &that->save);
	sigemptyset(&that->mask);
}

static int diminuto_mux_set_register(diminuto_mux_t * that, diminuto_mux_set_t * set, int fd)
{
	int rc;

	rc = !FD_ISSET(fd, &set->active);
	if (rc) {
		FD_SET(fd, &set->active);
		FD_CLR(fd, &set->ready);
		if (fd > that->nfds) {
			that->nfds = fd;
		}
	}

	return rc ? 0 : -1;
}

int diminuto_mux_register_read(diminuto_mux_t * that, int fd)
{
	return diminuto_mux_set_register(that, &that->read, fd);
}

int diminuto_mux_register_write(diminuto_mux_t * that, int fd)
{
	return diminuto_mux_set_register(that, &that->write, fd);
}

static int diminuto_mux_set_unregister(diminuto_mux_t * that, diminuto_mux_set_t * set, int fd)
{
	int rc;
	int nfds;
	int reads;
	int writes;

	rc = FD_ISSET(fd, &set->active);
	if (rc) {
		FD_CLR(fd, &set->active);
		FD_CLR(fd, &set->ready);
		if (fd == that->nfds) {
			nfds = that->nfds;
			that->nfds = -1;
			reads = 0;
			writes = 0;
			for (fd = 0; fd < nfds; ++fd) {
				if (FD_ISSET(fd, &that->read.active)) {
					that->nfds = fd;
					++reads;
				}
				if (FD_ISSET(fd, &that->write.active)) {
					that->nfds = fd;
					++writes;
				}
			}
			if (reads == 0) {
				that->read.next = -1;
			} else if (that->read.next > that->nfds) {
				that->read.next = 0;
			} else {
				/* Do nothing. */
			}
			if (writes == 0) {
				that->write.next = -1;
			} else if (that->write.next > that->nfds) {
				that->write.next = 0;
			} else {
				/* Do nothing. */
			}
		}
	}

	return rc ? 0 : -1;
}

int diminuto_mux_unregister_read(diminuto_mux_t * that, int fd)
{
	return diminuto_mux_set_unregister(that, &that->read, fd);
}

int diminuto_mux_unregister_write(diminuto_mux_t * that, int fd)
{
	return diminuto_mux_set_unregister(that, &that->write, fd);
}

int diminuto_mux_register_signal(diminuto_mux_t * that, int signum)
{
	int rc = -1;

	if (sigismember(&that->mask, signum) != 0) {
		/* Do nothing. */
	} else if (sigaddset(&that->mask, signum) < 0) {
		/* Do nothing. */
	} else {
		rc = 0;
	}

    return rc;
}

int diminuto_mux_unregister_signal(diminuto_mux_t * that, int signum)
{
	int rc = -1;

	if (sigismember(&that->mask, signum) <= 0) {
		/* Do nothing. */
	} else if (sigdelset(&that->mask, signum) < 0) {
		/* Do nothing. */
	} else {
		rc = 0;
	}

    return rc;
}

int diminuto_mux_block_signals(diminuto_mux_t * that)
{
	return sigprocmask(SIG_BLOCK, &that->mask, &that->save);
}

int diminuto_mux_unblock_signals(diminuto_mux_t * that)
{
	return sigprocmask(SIG_SETMASK, &that->save, (sigset_t *)0);
}

int diminuto_mux_wait(diminuto_mux_t * that, diminuto_ticks_t ticks)
{
	int rc = 0;
	struct timespec * top = (struct timespec *)0;
	struct timespec timeout;
	sigset_t * smp = (sigset_t *)0;

	if (that->nfds >= 0) {

		that->read.ready = that->read.active;
		that->write.ready = that->write.active;

		if (ticks >= 0) {
			timeout.tv_sec = diminuto_frequency_ticks2wholeseconds(ticks);
			timeout.tv_nsec = diminuto_frequency_ticks2fractionalseconds(ticks, diminuto_mux_frequency());
			top = &timeout;
		}

		if (!sigisemptyset(&that->mask)) {
			smp = &that->mask;
		}

		rc = pselect(that->nfds + 1, &that->read.ready, &that->write.ready, (fd_set *)0, top, smp);

		if (rc > 0) {
			if (that->read.next < 0) {
				that->read.next = 0;
			}
			if (that->write.next < 0) {
				that->write.next = 0;
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

static int diminuto_mux_ready(diminuto_mux_t * that, diminuto_mux_set_t * set)
{
	int fd = -1;
	int limit;
	int modulo;

	if (set->next >= 0) {

		limit = set->next;
		modulo = that->nfds + 1;

		do {

			if (FD_ISSET(set->next, &set->ready)) {
				fd = set->next;
				FD_CLR(fd, &set->ready);
			}

			set->next = (set->next + 1) % modulo;

		} while ((fd < 0) && (set->next != limit));

	}

	return fd;
}

int diminuto_mux_ready_read(diminuto_mux_t * that)
{
	return diminuto_mux_ready(that, &that->read);
}

int diminuto_mux_ready_write(diminuto_mux_t * that)
{
	return diminuto_mux_ready(that, &that->write);
}

int diminuto_mux_close(diminuto_mux_t * that, int fd)
{
	int rc = -2;

	if ((diminuto_mux_unregister_read(that, fd) < 0) && (diminuto_mux_unregister_write(that, fd) < 0)) {
		/* Do nothing. */
	} else if ((rc = close(fd)) == 0) {
		/* Do nothing. */
	} else {
		diminuto_perror("diminuto_mux_close");
	}

	return rc;
}
