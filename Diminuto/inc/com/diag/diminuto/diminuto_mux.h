/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_MUX_
#define _H_COM_DIAG_DIMINUTO_MUX_

/**
 * @file
 *
 * Copyright 2013-2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * This is a socket multiplexer that uses the pselect(2) system call. It is
 * inspired by similar code I wrote eons ago for SunOS. It provides a registry
 * of sockets used for read(2) and/or write(2) (for data sockets), or accept(2)
 * (for listening sockets). The application calls the provided wait function,
 * then interrogates the registry for those sockets that are "ready" (that is,
 * on which read(2), write(2), or accept(2) may be performed without blocking).
 * The algorithm round-robins on the ready sockets in each category to prevent
 * starvation.
 */

#include "com/diag/diminuto/diminuto_types.h"
#include <signal.h>
#include <sys/select.h>

/**
 * @def COM_DIAG_DIMINUTO_MUX_FREQUENCY
 * This manifest constant is the frequency in Hertz at which this feature
 * operates. The inverse of this value is the smallest unit of time in fractions
 * of a second that this feature can express or use. This constant is provided
 * for use in those cases where it is useful to have the value at compile time.
 * However, you should always prefer to use the inline function when possible.
 */
#define COM_DIAG_DIMINUTO_MUX_FREQUENCY (1000000000LL)

/**
 * Return the resolution of the Diminuto multiplexer timeout units in ticks per
 * second (Hertz). Timeout durations smaller than the equivalent period in ticks
 * may not yield the expected results. In fact, there is no guarantee that the
 * underlying software platform or hardware target can support timeout durations
 * with even this resolution.
 * @return the resolution in ticks per second.
 */
static inline diminuto_ticks_t diminuto_mux_frequency(void) {
    return COM_DIAG_DIMINUTO_MUX_FREQUENCY;
}

/**
 * This is the multiplexer set state.
 */
typedef struct DiminutoMuxSet {
    int min;
    int max;
    int next;
    fd_set active;
    fd_set ready;
} diminuto_mux_set_t;

/**
 * This is the multiplexer state.
 */
typedef struct DiminutoMux {
    fd_set effective;
    diminuto_mux_set_t read;
    diminuto_mux_set_t write;
    diminuto_mux_set_t accept;
    sigset_t mask;
} diminuto_mux_t;

/**
 * Initialize a multiplexer. Any prior state is lost, although this does not
 * effect any file descriptors. The multiplexer inherits the current signal
 * mask of the caller such that signals that are blocked when the multiplexer
 * is initialized will continue to be blocked when the multiplexer is waiting.
 * Signals can be removed from this cached mask using the unblock multiplexer
 * function.
 * @param muxp points to a multiplexer structure.
 */
extern void diminuto_mux_init(diminuto_mux_t * muxp);

/**
 * Register a file descriptor with the multiplexer for reading. It is an error
 * to register a registered file descriptor. Note that although it is common
 * for a socket to be registered for both read and write, registering a socket
 * for both read or write and accept is not supported.
 * @param muxp points to an initialized multiplexer structure.
 * @param fd is an unregistered file descriptor.
 * @return 0 for success, <0 for error.
 */
extern int diminuto_mux_register_read(diminuto_mux_t * muxp, int fd);

/**
 * Register a file descriptor with the multiplexer for writing. It is an error
 * to register a registered file descriptor. Note that although it is common
 * for a socket to be registered for both read and write, registering a socket
 * for both read or write and accept is not supported.
 * @param muxp points to an initialized multiplexer structure.
 * @param fd is an unregistered file descriptor.
 * @return 0 for success, <0 for error.
 */
extern int diminuto_mux_register_write(diminuto_mux_t * muxp, int fd);

/**
 * Register a listening file descriptor with the multiplexer for accepting.
 * It is an error to register a registered file descriptor.
 * @param muxp points to an initialized multiplexer structure.
 * @param fd is an unregistered file descriptor.
 * @return 0 for success, <0 for error.
 */
extern int diminuto_mux_register_accept(diminuto_mux_t * muxp, int fd);

/**
 * Unregister a file descriptor from the multiplexer for reading. It is an error
 * to unregister an unregistered file descriptor. Note that although it is
 * common for a socket to be registered for both read and write, registering a
 * socket for both read or write and accept is not supported.
 * @param muxp points to an initialized multiplexer structure.
 * @param fd is an registered file descriptor.
 * @return 0 for success, <0 for error.
 */
extern int diminuto_mux_unregister_read(diminuto_mux_t * muxp, int fd);

/**
 * Unregister a file descriptor from the multiplexer for writing. It is an error
 * to unregister an unregistered file descriptor.
 * @param muxp points to an initialized multiplexer structure.
 * @param fd is a registered file descriptor.
 * @return 0 for success, <0 for error.
 */
extern int diminuto_mux_unregister_write(diminuto_mux_t * muxp, int fd);

/**
 * Unregister a listening file descriptor from the multiplexer for accepting.
 * It is an error to unregister an unregistered file descriptor.
 * @param muxp points to an initialized multiplexer structure.
 * @param fd is a registered file descriptor.
 * @return 0 for success, <0 for error.
 */
extern int diminuto_mux_unregister_accept(diminuto_mux_t * muxp, int fd);

/**
 * Add a signal to the mask of those to be atomically unblocked while the
 * multiplexer is waiting.
 * @param muxp points to an initialized multiplexer structure.
 * @param signum is an unregistered blocked signal.
 * @return 0 for success, <0 for error.
 */
extern int diminuto_mux_register_signal(diminuto_mux_t * muxp, int signum);

/**
 * Remove a signal from the mask of those to be atomically unblocked while the
 * multiplexer is waiting.
 * @param muxp points to an initialized multiplexer structure.
 * @param signum is a registered blocked signal.
 * @return 0 for success, <0 for error.
 */
extern int diminuto_mux_unregister_signal(diminuto_mux_t * muxp, int signum);

/**
 * Wait until one or more registered file descriptors are ready for reading,
 * writing, or accepting, a timeout occurs, or a signal interrupt occurs. A
 * timeout of zero returns immediately, which is useful for polling. A timeout
 * that is negative causes the multiplexer to block indefinitely until either
 * a file descriptor is ready or one of the registered signals is caught.
 * @param muxp points to an initialized multiplexer structure.
 * @param timeout is a timeout period in ticks, 0 for polling, <0 for blocking.
 * @return the number of ready file descriptors, 0 for a timeout, <0 for error.
 */
extern int diminuto_mux_wait(diminuto_mux_t * muxp, diminuto_ticks_t timeout);

/**
 * Return the next registered file descriptor that is ready for reading.
 * @param muxp points to an initialized multiplexer structure.
 * @return a file descriptor ready for reading, or <0 if none.
 */
extern int diminuto_mux_ready_read(diminuto_mux_t * muxp);

/**
 * Return the next registered file descriptor that is ready for writing.
 * @param muxp points to an initialized multiplexer structure.
 * @return a file descriptor ready for writing, or <0 if none.
 */
extern int diminuto_mux_ready_write(diminuto_mux_t * muxp);

/**
 * Return the next registered file descriptor that is ready for accepting.
 * @param muxp points to an initialized multiplexer structure.
 * @return a file descriptor ready for accepting, or <0 if none.
 */
extern int diminuto_mux_ready_accept(diminuto_mux_t * muxp);

/**
 * Unregister a registered file descriptor and close it. The file descriptor
 * is not closed if it is not registered.
 * @param muxp points to an initialized multiplexer structure.
 * @param fd is a registered file descriptor.
 * @return 0 for success, <0 for error.
 */
extern int diminuto_mux_close(diminuto_mux_t * muxp, int fd);

#endif
