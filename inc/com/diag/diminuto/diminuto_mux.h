/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_MUX_
#define _H_COM_DIAG_DIMINUTO_MUX_

/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * WORK IN PROGRESS
 */

#include "com/diag/diminuto/diminuto_types.h"
#include <sys/select.h>

typedef struct DiminutoMuxSet {
	int minimum;
	int maximum;
	int next;
	fd_set active;
	fd_set ready;
} diminuto_mux_set_t;

typedef struct DiminutoMux {
	int count;
	int nfds;
	diminuto_mux_set_t read;
	diminuto_mux_set_t write;
	sigset_t mask;
} diminuto_mux_t;

/**
 * Return the resolution of the Diminuto mux units in ticks per second (Hertz).
 * Although the underlying platform may be able to return time with this
 * resolution, there is no guarantee that the underlying platform actually has
 * this degree of accuracy.
 * @return the resolution in ticks per second.
 */
static inline diminuto_ticks_t diminuto_mux_frequency(void) {
	return 1000000000LL;
}

extern int diminuto_mux_init(diminuto_mux_t * that);

extern int diminuto_mux_register_read(diminuto_mux_t * that, int fd);

extern int diminuto_mux_unregister_read(diminuto_mux_t * that, int fd);

extern int diminuto_mux_register_write(diminuto_mux_t * that, int fd);

extern int diminuto_mux_unregister_write(diminuto_mux_t * that, int fd);

extern int diminuto_mux_register_signal(diminuto_mux_t * that, int signum);

extern int diminuto_mux_unregister_signal(diminuto_mux_t * that, int signum);

extern int diminuto_mux_wait(diminuto_mux_t * that, diminuto_ticks_t timeout);

extern int diminuto_mux_ready_read(diminuto_mux_t * that);

extern int diminuto_mux_ready_write(diminuto_mux_t * that);

extern int diminuto_mux_close(diminuto_mux_t * that, int fd);

extern int diminuto_mux_fini(diminuto_mux_t * that);

#endif
