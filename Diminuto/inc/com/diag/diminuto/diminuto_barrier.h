/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_BARRIER_
#define _H_COM_DIAG_DIMINUTO_BARRIER_

/**
 * @file
 * @copyright Copyright 2008-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Provides memory barriers with acquire and release semantics.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * 
 * Provides memory barriers with acquire and release semantics. EXPERIMENTAL
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
             * @def diminuto_acquire
             * Use a gcc built-in function (if it exists) to create an acquire
             * (read) memory barrier. (The barrier is a side-effect.)
             */
#           define diminuto_acquire() do { volatile diminuto_spinlock_t _diminuto_acquire_lock_ = 0; __sync_lock_test_and_set(&_diminuto_acquire_lock_, 1); } while (0)

            /**
             * @def diminuto_release
             * Use a gcc built-in function (if it exists) to create a release
             * (write) memory barrier. (The barrier is a side-effect.)
             */
#           define diminuto_release() do { volatile diminuto_spinlock_t _diminuto_release_lock_ = 1; __sync_lock_release(&_diminuto_release_lock_); } while (0)

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

#if defined(diminuto_acquire)
#   define COM_DIAG_DIMINUTO_ACQUIRE (!0)
#else
#   undef COM_DIAG_DIMINUTO_ACQUIRE
#   define diminuto_acquire() diminuto_barrier()
#   warning diminuto_acquire() is a synonym for diminuto_barrier()!
#endif

#if defined(diminuto_release)
#   define COM_DIAG_DIMINUTO_RELEASE (!0)
#else
#   undef COM_DIAG_DIMINUTO_RELEASE
#   define diminuto_release() diminuto_barrier()
#   warning diminuto_release() is a synonym for diminuto_barrier()!
#endif

static inline void diminuto_barrier_f(void) {
    diminuto_barrier();
}

static inline void diminuto_acquire_f(void) {
    diminuto_acquire();
}

static inline void diminuto_release_f(void) {
    diminuto_release();
}

#endif
