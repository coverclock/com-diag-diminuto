/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_MUX_
#define _H_COM_DIAG_DIMINUTO_MUX_

/**
 * @file
 * @copyright Copyright 2013-2025 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Provides a model for doing multiplexing using pselect(2).
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Mux feature implements an easily used file descriptor and socket
 * multiplexer that uses the pselect(2) system call. It was inspired by
 * similar code the author wrote eons ago for SunOS. It provides a registry
 * of sockets used for read(2) and/or write(2) (for data sockets), or accept(2)
 * (for listening sockets). The application calls the provided wait function,
 * then interrogates the registry for those sockets that are "ready" (that is,
 * on which read(2), write(2), or accept(2) may be performed without blocking).
 * The algorithm round-robins on the ready sockets in each category to prevent
 * starvation.
 *
 * NOTE: Because the diminuto_mux feature uses the pselect(2) system call, it
 * is limited to using the first 1024 file descriptors enumerated 0 to 1023.
 * This is a limit imposed by the size of the fd_set bit mask. The ppoll(2)
 * system call circumvents this limit, but alas even though the diminuto_poll
 * feature uses this system call instead, it still uses the fd_set bit mask
 * to manage file descriptors, hence it has the same limitation.
 *
 * REFERENCES
 *
 * Aivars Kalvans, "Using select(2)", 2017-04-06,
 * http://aivarsk.github.io/2017/04/06/select/
 *
 * Dan Kegel, "The C10K Problem", 2014-02-05,
 * http://www.kegel.com/c10k.html
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
static inline diminuto_sticks_t diminuto_mux_frequency(void) {
    return COM_DIAG_DIMINUTO_MUX_FREQUENCY;
}

/**
 * This is the multiplexer set state.
 */
typedef struct DiminutoMuxSet {
    fd_set          active; /**< FDs to multiplex. */
    fd_set          ready;  /**< FDs that are ready. */
    diminuto_fd_t   min;    /**< Minimum FD in active set. */
    diminuto_fd_t   max;    /**< Maximum FD in active set. */
    diminuto_fd_t   next;   /**< Next FD in try in active set. */
} diminuto_mux_set_t;

/**
 * This is the multiplexer state.
 */
typedef struct DiminutoMux {
    fd_set              read_or_accept;         /**< FDs to read/accept. */
    fd_set              urgent_or_interrupt;    /**< FDs to urgent/interrupt. */
    diminuto_mux_set_t  read;                   /**< FDs for reading. */
    diminuto_mux_set_t  write;                  /**< FDs for writing. */
    diminuto_mux_set_t  accept;                 /**< FDs for accepting. */
    diminuto_mux_set_t  urgent;                 /**< FDs for urgent. */
    diminuto_mux_set_t  interrupt;              /**< FDs for interrupt. */
    sigset_t            mask;                   /**< Signals being managed. */
} diminuto_mux_t;

/**
 * @def DIMINUTO_MUX_INIT
 * Static initializer for Diminuto mux object. Does NOT duplicate
 * what diminuto_mux_init() does. Instead, initializes all fields in the
 * mux object in known values (zeros).
 */
#define DIMINUTO_MUX_INIT { { { 0, } } }

/**
 * Initialize a multiplexer. Any prior state is lost, although this does not
 * effect any file descriptors. The multiplexer inherits the current signal
 * mask of the caller such that signals that are blocked when the multiplexer
 * is initialized will continue to be blocked when the multiplexer is waiting
 * (in other words, blocked signals are automatically registered).
 * Signals can be removed from this cached mask by unregistering them.
 * @param muxp points to a multiplexer structure.
 * @return a pointer ot the object or NULL if an error occurred.
 */
extern diminuto_mux_t * diminuto_mux_init(diminuto_mux_t * muxp);

/**
 * Handle any necessary deinitialization on a multiplexer.
 * param muxp points to the multiplexer structure.
 * @return NULL or a pointer ot the object if an error occurred.
 */
static inline diminuto_mux_t * diminuto_mux_fini(diminuto_mux_t * muxp)
{
    return (diminuto_mux_t *)0;
}

/**
 * Register a file descriptor with the multiplexer for reading.
 * @param muxp points to an initialized multiplexer structure.
 * @param fd is an unregistered file descriptor.
 * @return >=0 for success, <0 for error.
 */
extern int diminuto_mux_register_read(diminuto_mux_t * muxp, int fd);

/**
 * Register a file descriptor with the multiplexer for writing.
 * @param muxp points to an initialized multiplexer structure.
 * @param fd is an unregistered file descriptor.
 * @return >=0 for success, <0 for error.
 */
extern int diminuto_mux_register_write(diminuto_mux_t * muxp, int fd);

/**
 * Register a listening file descriptor with the multiplexer for accepting
 * connections.
 * It is an error to register a registered file descriptor.
 * @param muxp points to an initialized multiplexer structure.
 * @param fd is an unregistered file descriptor.
 * @return >=0 for success, <0 for error.
 */
extern int diminuto_mux_register_accept(diminuto_mux_t * muxp, int fd);

/**
 * Register a file descriptor with the multiplexer for urgent exceptions,
 * which typically means for urgent out-of-band one byte receives on a
 * stream socket.
 * @param muxp points to an initialized multiplexer structure.
 * @param fd is an unregistered file descriptor.
 * @return >=0 for success, <0 for error.
 */
extern int diminuto_mux_register_urgent(diminuto_mux_t * muxp, int fd);

/**
 * Register a file descriptor with the multiplexer for interrupt exceptions,
 * which typically means for state changes on a general purpose input/output
 * (GPIO) pin configured to interrupt on edge or level transitions.
 * @param muxp points to an initialized multiplexer structure.
 * @param fd is an unregistered file descriptor.
 * @return >=0 for success, <0 for error.
 */
extern int diminuto_mux_register_interrupt(diminuto_mux_t * muxp, int fd);

/**
 * Add a signal to the mask of those to be atomically blocked while the
 * multiplexer is waiting.
 * @param muxp points to an initialized multiplexer structure.
 * @param signum is an unregistered signal to be blocked.
 * @return >=0 for success, <0 for error.
 */
extern int diminuto_mux_register_signal(diminuto_mux_t * muxp, int signum);

/**
 * Unregister a file descriptor from the multiplexer for reading.
 * @param muxp points to an initialized multiplexer structure.
 * @param fd is an registered file descriptor.
 * @return 0 for success, <0 for error.
 */
extern int diminuto_mux_unregister_read(diminuto_mux_t * muxp, int fd);

/**
 * Unregister a file descriptor from the multiplexer for writing.
 * @param muxp points to an initialized multiplexer structure.
 * @param fd is a registered file descriptor.
 * @return 0 for success, <0 for error.
 */
extern int diminuto_mux_unregister_write(diminuto_mux_t * muxp, int fd);

/**
 * Unregister a listening file descriptor from the multiplexer for accepting.
 * @param muxp points to an initialized multiplexer structure.
 * @param fd is a registered file descriptor.
 * @return 0 for success, <0 for error.
 */
extern int diminuto_mux_unregister_accept(diminuto_mux_t * muxp, int fd);

/**
 * Unregister a file descriptor from the multiplexer for urgent exceptions.
 * @param muxp points to an initialized multiplexer structure.
 * @param fd is a registered file descriptor.
 * @return 0 for success, <0 for error.
 */
extern int diminuto_mux_unregister_urgent(diminuto_mux_t * muxp, int fd);

/**
 * Unregister a file descriptor with the multiplexer for interrupt exceptions.
 * @param muxp points to an initialized multiplexer structure.
 * @param fd is an unregistered file descriptor.
 * @return 0 for success, <0 for error.
 */
extern int diminuto_mux_unregister_interrupt(diminuto_mux_t * muxp, int fd);

/**
 * Remove a signal from the mask of those to be atomically blocked while the
 * multiplexer is waiting.
 * @param muxp points to an initialized multiplexer structure.
 * @param signum is a registered signal to be unblocked.
 * @return 0 for success, <0 for error.
 */
extern int diminuto_mux_unregister_signal(diminuto_mux_t * muxp, int signum);

/**
 * Wait until one or more registered file descriptors are ready for reading,
 * writing, or accepting, a timeout occurs, or a signal interrupt occurs. A
 * timeout of zero returns immediately, which is useful for polling. A timeout
 * that is negative causes the multiplexer to block indefinitely until either
 * a file descriptor is ready or a signal is caught. This is an alternative
 * API call that allows the application to provide its own signal mask (which
 * it may have for other reasons).
 * @param muxp points to an initialized multiplexer structure.
 * @param timeout is a timeout period in ticks, 0 for polling, <0 for blocking.
 * @param maskp points to the signal mask or NULL if none.
 * @return the number of ready file descriptors, 0 for a timeout, <0 for error.
 */
extern int diminuto_mux_wait_generic(diminuto_mux_t * muxp, diminuto_sticks_t timeout, const sigset_t * maskp);

/**
 * Wait until one or more registered file descriptors are ready for reading,
 * writing, or accepting, a timeout occurs, or a signal interrupt occurs. A
 * timeout of zero returns immediately, which is useful for polling. A timeout
 * that is negative causes the multiplexer to block indefinitely until either
 * a file descriptor is ready or one of the registered signals is caught. This
 * API call uses the signal mask in the mux structure that contains registered
 * signals.
 * @param muxp points to an initialized multiplexer structure.
 * @param timeout is a timeout period in ticks, 0 for polling, <0 for blocking.
 * @return the number of ready file descriptors, 0 for a timeout, <0 for error.
 */
static inline int diminuto_mux_wait(diminuto_mux_t * muxp, diminuto_sticks_t timeout)
{
    return diminuto_mux_wait_generic(muxp, timeout, &(muxp->mask));
}

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
 * Return the next registered file descriptor that is ready for exceptions.
 * Typically this means there is urgent data available to be received
 * out-of-band on the descriptor.
 * @param muxp points to an initialized multiplexer structure.
 * @return a file descriptor ready for accepting, or <0 if none.
 */
extern int diminuto_mux_ready_urgent(diminuto_mux_t * muxp);

/**
 * Return the next registered file descriptor that is ready for interrupting.
 * @param muxp points to an initialized multiplexer structure. This is the
 * same from a system point of view as an urgent descriptor, but the
 * nomenclature differs for certaion I/O operations.
 * @return a file descriptor ready for accepting, or <0 if none.
 */
extern int diminuto_mux_ready_interrupt(diminuto_mux_t * muxp);

/**
 * Unregister a registered file descriptor and close it. The file descriptor
 * is not closed if it is not registered.
 * @param muxp points to an initialized multiplexer structure.
 * @param fd is a registered file descriptor.
 * @return 0 for success, <0 for error.
 */
extern int diminuto_mux_close(diminuto_mux_t * muxp, int fd);

/**
 * Dump a multiplexer structure.
 * @param muxp points to the multiplexer structure.
 */
extern void diminuto_mux_dump(const diminuto_mux_t * muxp);

#endif
