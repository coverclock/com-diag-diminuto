/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_BARRIER_
#define _H_COM_DIAG_DIMINUTO_BARRIER_

/**
 * @file
 *
 * Copyright 2008-2019 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 * This is largely experimental.
 */

typedef int diminuto_spinlock_t;

#if defined(__GNUC__)
#   if defined(__GNUC_MINOR__)
#       if ((((__GNUC__)*1000)+(__GNUC_MINOR__))>=4001)
            /**
             * @def diminuto_barrier
             * Use a gcc built-in function (if it exists) to create a full
             * memory barrier.
             */
#           define diminuto_barrier() __sync_synchronize()
#       endif
#   endif
#endif

#if !defined(diminuto_barrier)
#   define diminuto_barrier() ((void)0)
#   warning diminuto_barrier() is a no-op!
#endif

#if defined(__GNUC__)
#   if defined(__GNUC_MINOR__)
#       if ((((__GNUC__)*1000)+(__GNUC_MINOR__))>=4001)
            /**
             * @def diminuto_acquire
             * Use a gcc built-in function (if it exists) to create an acquire
             * (read) memory barrier. (The barrier is a side-effect.)
             */
#           define diminuto_acquire() do { volatile diminuto_spinlock_t _diminuto_acquire_lock_ = 0; __sync_lock_test_and_set(&_diminuto_acquire_lock_, 1); } while (0)
#       endif
#   endif
#endif

#if !defined(diminuto_acquire)
#   define diminuto_acquire() diminuto_barrier()
#   warning diminuto_acquire() is a synonym for diminuto_barrier()!
#endif

#if defined(__GNUC__)
#   if defined(__GNUC_MINOR__)
#       if ((((__GNUC__)*1000)+(__GNUC_MINOR__))>=4001)
            /**
             * @def diminuto_release
             * Use a gcc built-in function (if it exists) to create a release
             * (write) memory barrier. (The barrier is a side-effect.)
             */
#           define diminuto_release() do { volatile diminuto_spinlock_t _diminuto_release_lock_ = 1; __sync_lock_release(&_diminuto_release_lock_); } while (0)
#       endif
#   endif
#endif

#if !defined(diminuto_release)
#   define diminuto_release() diminuto_barrier()
#   warning diminuto_release() is a synonym for diminuto_barrier()!
#endif

#endif
