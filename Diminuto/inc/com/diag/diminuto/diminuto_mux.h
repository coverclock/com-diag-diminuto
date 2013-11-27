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

/**
 * This is the multiplexer read or a write set state.
 */
typedef struct DiminutoMuxSet {
	int next;
	fd_set active;
	fd_set ready;
} diminuto_mux_set_t;

/**
 * This is the multiplexer state.
 */
typedef struct DiminutoMux {
	int nfds;
	diminuto_mux_set_t read;
	diminuto_mux_set_t write;
	sigset_t mask;
} diminuto_mux_t;

/**
 * Return the resolution of the Diminuto multiplexer timeout units in ticks per
 * second (Hertz). Timeout durations smaller than the equivalent period in ticks
 * may not yield the expected results. In fact, there is no guarantee that the
 * underlying software platform or hardware target can support timeout durations
 * with even this resolution.
 * @return the resolution in ticks per second.
 */
static inline diminuto_ticks_t diminuto_mux_frequency(void) {
	return 1000000000LL;
}

/**
 * Initialize a multiplexer. Any prior state is lost, although this does not
 * effect any file descriptors.
 * @param that points to a multiplexer structure.
 */
extern void diminuto_mux_init(diminuto_mux_t * that);

/**
 * Register a file descriptor with the multiplexer for reading.
 * @param that points to an initialized multiplexer structure.
 * @param fd is an unregistered file descriptor.
 * @return 0 for success, <0 for error.
 */
extern int diminuto_mux_register_read(diminuto_mux_t * that, int fd);

/**
 * Unregister a file descriptor from the multiplexer for reading.
 * @param that points to an initialized multiplexer structure.
 * @param fd is an registered file descriptor.
 * @return 0 for success, <0 for error.
 */
extern int diminuto_mux_unregister_read(diminuto_mux_t * that, int fd);

/**
 * Register a file descriptor with the multiplexer for writing.
 * @param that points to an initialized multiplexer structure.
 * @param fd is an unregistered file descriptor.
 * @return 0 for success, <0 for error.
 */
extern int diminuto_mux_register_write(diminuto_mux_t * that, int fd);

/**
 * Unregister a file descriptor from the multiplexer for writing.
 * @param that points to an initialized multiplexer structure.
 * @param fd is a registered file descriptor.
 * @return 0 for success, <0 for error.
 */
extern int diminuto_mux_unregister_write(diminuto_mux_t * that, int fd);

/**
 * Register a blocked signal with the multiplexer for interrupting while
 * waiting.
 * @param that points to an initialized multiplexer structure.
 * @param signum is an unregistered blocked signal.
 * @return 0 for success, <0 for error.
 */
extern int diminuto_mux_register_signal(diminuto_mux_t * that, int signum);

/**
 * Unregister a signal from the multiplexer for interrupting while waiting.
 * @param that points to an initialized multiplexer structure.
 * @param signum is a registered blocked signal.
 * @return 0 for success, <0 for error.
 */
extern int diminuto_mux_unregister_signal(diminuto_mux_t * that, int signum);

/**
 * Wait until one or more registered file descriptors are ready for either
 * reading or writing, a timeout occurs, or a signal interrupt occurs. A
 * timeout of zero returns immediately, which is useful for polling. A timeout
 * that is negative causes the multiplexer to block indefinitely until either
 * a file descriptor is ready or one of the registered signals is caught.
 * @param that points to an initialized multiplexer structure.
 * @param timeout is a timeout period in ticks, 0 for polling, <0 for none.
 * @return the number of ready file descriptors, 0 for a timeout, <0 for error.
 */
extern int diminuto_mux_wait(diminuto_mux_t * that, diminuto_ticks_t timeout);

/**
 * Return the next registered file descriptor that is ready for reading.
 * @param that points to an initialized multiplexer structure.
 * @return 0 for success, <0 for error.
 */
extern int diminuto_mux_ready_read(diminuto_mux_t * that);

/**
 * Return the next registered file descriptor that is ready for writing.
 * @param that points to an initialized multiplexer structure.
 * @return 0 for success, <0 for error.
 */
extern int diminuto_mux_ready_write(diminuto_mux_t * that);

/**
 * Unregister a registered file descriptor and close it. The file descriptor
 * is not closed if it is not registered.
 * @param that points to an initialized multiplexer structure.
 * @param fd is a registered file descriptor.
 * @return 0 for success, <0 for error.
 */
extern int diminuto_mux_close(diminuto_mux_t * that, int fd);

#endif
