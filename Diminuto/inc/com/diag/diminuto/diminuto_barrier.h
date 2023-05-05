/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_BARRIER_
#define _H_COM_DIAG_DIMINUTO_BARRIER_

/**
 * @file
 * @copyright Copyright 2008-2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Provides memory barriers with acquire and release semantics.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * Provides memory barriers with acquire and release semantics. EXPERIMENTAL
 *
 * The read (acquire) and write (release) barriers are achieved as side effects
 * of using gcc built-in atomic primitives that in this case otherwise do
 * nothing.
 *
 * If you want bracketing macros for this, see the Coherent Section feature.
 *
 * If you want actual spinlocks, see the Serialized Section feature.
 *
 * REFERENCES
 *
 * <https://gcc.gnu.org/onlinedocs/gcc-4.1.0/gcc/Atomic-Builtins.html>
 */

typedef int diminuto_spinlock_t;

#if !0
#   if defined(__GNUC__)&&defined(__GNUC_MINOR__)
#       if ((((__GNUC__)*1000)+(__GNUC_MINOR__))>=4001)

            /**
             * @def diminuto_barrier
             * Use a gcc built-in function (if it exists) to create a full
             * memory barrier.
             */
#           define diminuto_barrier() __sync_synchronize()

            /**
             * @def diminuto_barrier_acquire
             * Use a gcc built-in function (if it exists) to create an acquire
             * (read) memory barrier. (The barrier is a side-effect.)
             */
#           define diminuto_barrier_acquire() do { volatile diminuto_spinlock_t _diminuto_barrier_acquire_lock_ = 0; __sync_lock_test_and_set(&_diminuto_barrier_acquire_lock_, 1); } while (0)

            /**
             * @def diminuto_barrier_release
             * Use a gcc built-in function (if it exists) to create a release
             * (write) memory barrier. (The barrier is a side-effect.)
             */
#           define diminuto_barrier_release() do { volatile diminuto_spinlock_t _diminuto_barrier_release_lock_ = 1; __sync_lock_release(&_diminuto_barrier_release_lock_); } while (0)

#       endif
#   endif
#endif

#if defined(diminuto_barrier)
#   define COM_DIAG_DIMINUTO_BARRIER (!0)
#else
#   undef COM_DIAG_DIMINUTO_BARRIER
#   define diminuto_barrier() ((void)0)
#   warning diminuto_barrier() is a no-op!
#endif

#if defined(diminuto_barrier_acquire)
#   define COM_DIAG_DIMINUTO_BARRIER_ACQUIRE (!0)
#else
#   undef COM_DIAG_DIMINUTO_BARRIER_ACQUIRE
#   define diminuto_barrier_acquire() diminuto_barrier()
#   warning diminuto_barrier_acquire() is a synonym for diminuto_barrier()!
#endif

#if defined(diminuto_barrier_release)
#   define COM_DIAG_DIMINUTO_BARRIER_RELEASE (!0)
#else
#   undef COM_DIAG_DIMINUTO_BARRIER_RELEASE
#   define diminuto_barrier_release() diminuto_barrier()
#   warning diminuto_barrier_release() is a synonym for diminuto_barrier()!
#endif

static inline void diminuto_barrier_f(void) {
    diminuto_barrier();
}

static inline void diminuto_barrier_acquire_f(void) {
    diminuto_barrier_acquire();
}

static inline void diminuto_barrier_release_f(void) {
    diminuto_barrier_release();
}

#endif
