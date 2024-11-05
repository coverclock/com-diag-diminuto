/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_SERIALIZEDSECTION_
#define _H_COM_DIAG_DIMINUTO_SERIALIZEDSECTION_

/**
 * @file
 * @copyright Copyright 2015-2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements macros to to bracket sections serialized with spin locks.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * The Serialized Section feature uses the GNU test-and-set built-in with
 * bracketing macros to implement a code section serialized between multiple
 * processors.
 */

#include "com/diag/diminuto/diminuto_barrier.h" /* For diminuto_spinlock_t. */

#if !0
#   if defined(__GNUC__) && defined(__GNUC_MINOR__)
#      if ((((__GNUC__)*1000)+(__GNUC_MINOR__))>=4001)

            /**
             * @def DIMINUTO_SERIALIZED_SECTION_BEGIN
             * Start a code block that is unconditionally serialized using a
             * spin lock and busy waiting by blocking on an integer lock
             * variable specified by the caller as a pointer to a volatile
             * variable of type int but be provided as the argument @a _INTP_.
             * Note that the while loop iterates as long as the builtin
             * returns !0 (true, or already locked by someone else), and exits
             * once the builtin returns 0 (false, or unlocked), indicating that
             * the lock has transitioned to 1 (true, or locked on the caller's
             * behalf).
             */
#           define DIMINUTO_SERIALIZED_SECTION_BEGIN(_INTP_) \
                do { \
                    volatile diminuto_spinlock_t * _diminuto_serializedsection_sp; \
                    _diminuto_serializedsection_sp = (_INTP_); \
                    while (__sync_lock_test_and_set(_diminuto_serializedsection_sp, 1)); \
                    if (!0) { \
                        do { \
                            ((void)0)

            /**
             * @def DIMINUTO_SERIALIZED_SECTION_TRY
             * Start a code block that is conditionally serialized using a spin
             * lock and testing on an integer lock variable specified by the
             * caller as a pointer to a volatile variable of type int but be
             * provided as the argument @a _INTP_. Note that there is no busy
             * waiting; the conditional section is entered and executed iff
             * the the builtin returns 0 (false, or unlocked). An application
             * may change the state of its own variable inside the serialized
             * section to determine if the spin lock succeeded.
             */
#           define DIMINUTO_SERIALIZED_SECTION_TRY(_INTP_) \
                do { \
                    volatile diminuto_spinlock_t * _diminuto_serializedsection_sp; \
                    _diminuto_serializedsection_sp = (_INTP_); \
                    if (!__sync_lock_test_and_set(_diminuto_serializedsection_sp, 1)) { \
                        do { \
                            ((void)0)

            /**
             * @def DIMINUTO_SERIALIZED_SECTION_END
             * End a code block that was serialized using a spin lock and busy
             * waiting by releasing the integer lock variable specified at the
             * beginning of the block.
             */
#           define DIMINUTO_SERIALIZED_SECTION_END \
                        } while (0); \
                        __sync_lock_release(_diminuto_serializedsection_sp); \
                    } \
                } while (0)

#       endif
#   endif
#endif

#if defined(DIMINUTO_SERIALIZED_SECTION_BEGIN) && defined(DIMINUTO_SERIALIZED_SECTION_END) && defined(DIMINUTO_SERIALIZED_SECTION_TRY)
#   define COM_DIAG_DIMINUTO_SERIALIZED_SECTION (!0)
#else
#   undef COM_DIAG_DIMINUTO_SERIALIZED_SECTION
#   warning DIMINUTO_SERIALIZED_SECTION_BEGIN, DIMINUTO_SERIALIZED_SECTION_TRY, and DIMINUTO_SERIALIZED_SECTION_END not available!
#   define DIMINUTO_SERIALIZED_SECTION_BEGIN(_INTP_) \
        do { \
            if (0) { \
                do { \
                    ((void)0)
#   define DIMINUTO_SERIALIZED_SECTION_TRY(_INTP_) \
        do { \
            if (0) { \
                do { \
                    ((void)0)
#   define DIMINUTO_SERIALIZED_SECTION_END \
                } while (0); \
            } \
        } while (0)
#endif

#endif
