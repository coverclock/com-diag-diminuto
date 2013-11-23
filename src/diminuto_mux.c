/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * WORK IN PROGRESS
 */

#include "com/diag/diminuto/diminuto_mux.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include <signal.h>
#include <string.h>

static void diminuto_mux_set_init(diminuto_mux_t * that, diminuto_mux_set_t * set)
{
	set->minimum = ~(1 << ((sizeof(set->minimum) * 8) - 1));
	set->maximum = -1;
	set->next = -1;
	FD_ZERO(&set->active);
	FD_ZERO(&set->ready);
}

void diminuto_mux_init(diminuto_mux_t * that)
{
	that->count = 0;
	that->nfds = -1;
	diminuto_mux_set_init(that, &that->read);
	diminuto_mux_set_init(that, &that->write);
	sigemptyset(&that->mask);

	return 0;
}

static int diminuto_mux_register(diminuto_mux_t * that, diminuto_mux_set_t * set, int fd)
{
	int rc;

	rc = FD_ISSET(fd, &set->active);
	if (!rc) {

		++that->count;

		if (fd > that->nfds) {
			that->nfds = fd;
		}

		if (fd < set->minimum) {
			set->minimum = fd;
		}

		if (fd > set->maximum) {
			set->maximum = fd;
		}

		FD_SET(fd, &set->active);
		FD_CLR(fd, &set->ready);

	}

	return fd;
}

int diminuto_mux_register_read(diminuto_mux_t * that, int fd)
{
	return diminuto_mux_register(that, &that->read, fd);
}

int diminuto_mux_register_write(diminuto_mux_t * that, int fd)
{
	return diminuto_mux_register(that, &that->write, fd);
}

static int diminuto_mux_unregister(diminuto_mux_t * that, diminuto_mux_set_t * set, int fd)
{
	int rc;

	rc = FD_ISSET(fd, &that->read.active);
	if (!rc) {

		/* Do nothing. */

	} else if (--that->count == 0) {

		diminuto_mux_init(that);

	} else {

		if (fd == that->nfds) {
			--that->nfds;
		}

		if (fd == set->maximum) {
			--set->maximum;
		}

		if (fd == set->minimum) {
			++set->maximum;
		}

		if (set->maximum < set->minimum) {
			set->minimum = ~(1 << ((sizeof(set->minimum) * 8) - 1));
			set->maximum = -1;
			FD_ZERO(&set->active);
			FD_ZERO(&set->ready);
		} else {
			FD_CLR(fd, &set->active);
			FD_CLR(fd, &set->ready);
		}

	}

	return rc;
}

int diminuto_mux_unregister_read(diminuto_mux_t * that, int fd)
{
	return diminuto_mux_unregister(that, &that->read, fd);
}

int diminuto_mux_unregister_write(diminuto_mux_t * that, int fd)
{
	return diminuto_mux_unregister(that, &that->write, fd);
}

int diminuto_mux_register_signal(diminuto_mux_t * that, int signum)
{
    return sigaddset(&that->mask, signum);
}

int diminuto_mux_unregister_signal(diminuto_mux_t * that, int signum)
{
	return sigdelset(&that->mask, signum);
}

int diminuto_mux_wait(diminuto_mux_t * that, diminuto_ticks_t ticks)
{
	int rc = 0;
	struct timespec * top = (struct timespec *)0;
	struct timespec timeout;

	if (that->count > 0) {
		that->read.ready = that->read.active;
		that->write.ready = that->write.active;
		if (ticks >= 0) {
			top = &timeout;
			top->tv_sec = diminuto_frequency_ticks2wholeseconds(ticks);
			top->tv_nsec = diminuto_frequency_ticks2fractionalseconds(ticks, diminuto_mux_frequency());
		}
		rc = pselect(that->nfds, &that->read.ready, &that->write.ready, (fd_set *)0, top, &that->mask);
		that->read.next = that->read.minimum;
		that->write.next = that->write.minimum;
	}

	return rc;
}

static int diminuto_mux_ready(diminuto_mux_t * that, diminuto_mux_set_t * set)
{
	int fd = -1;

	while (set->next <= set->maximum) {
		if (FD_ISSET(set->next, &set->ready)) {
			fd = set->next;
			break;
		}
		++set->next;
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
	return (diminuto_mux_unregister_read(that, fd) || diminuto_mux_unregister_write(that, fd)) ? close(fd) : -2;
}

static int diminuto_mux_set_fini(diminuto_mux_t * that, diminuto_mux_set_t * set)
{
	int rc = 0;
	int fd;

	for (fd = set->minimum; fd <= set->maximum; ++fd) {
		if (diminuto_mux_unregister(that, set, fd)) {
			if (close(fd) < 0) {
				rc = -1;
			}
		}
	}

	return rc;
}

int diminuto_mux_fini(diminuto_mux_t * that)
{
	int rc = 0;

	if (diminuto_mux_set_fini(that, &that->read) < 0) {
		rc = -1;
	}

	if (diminuto_mux_set_fini(that, &that->write) < 0) {
		rc = -1;
	}

	diminuto_mux_init(that);

	return rc;
}
