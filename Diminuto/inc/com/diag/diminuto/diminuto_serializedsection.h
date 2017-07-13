/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_SERIALIZEDSECTION_
#define _H_COM_DIAG_DIMINUTO_SERIALIZEDSECTION_

/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 * This is largely experimental.
 */

typedef volatile int diminuto_serialized_section_spinlock_t;

#if defined(__GNUC__)
#   if defined(__GNUC_MINOR__)
#       if ((((__GNUC__)*1000)+(__GNUC_MINOR__))>=4001)

/**
 * @def DIMINUTO_SERIALIZED_SECTION_BEGIN
 * Start a code block that is serialized using a spin lock and busy waiting by
 * blocking on an integer lock variable specified by the caller as a pointer to
 * a volatile variable of type int but be provided as the argument @a _INTP_.
 */
#           define DIMINUTO_SERIALIZED_SECTION_BEGIN(_INTP_) \
                do { \
                    volatile int * _diminuto_serialized_section_spinlock_p_ = (_INTP_); \
                    while (__sync_lock_test_and_set(_diminuto_serialized_section_spinlock_p_, 1)); \
                    do { \
                        (void)0

/**
 * @def DIMINUTO_SERIALIZED_SECTION_END
 * End a code block that was serialized using a spin lock and busy waiting by
 * releasing the integer lock variable specified at the beginning of the block.
 */
#           define DIMINUTO_SERIALIZED_SECTION_END \
                } while (0); \
                __sync_lock_release(_diminuto_serialized_section_spinlock_p_); \
            } while (0)

#       endif
#   endif
#endif

#if !defined(DIMINUTO_SERIALIZED_SECTION_BEGIN) || !defined(DIMINUTO_SERIALIZED_SECTION_END)
#   warning DIMINUTO_SERIALISED_SECTION_BEGIN and DIMINUTO_SERIALIZED_SECTION_END are no-ops!
#   undef DIMINUTO_SERIALIZED_SECTION_BEGIN
#   defined DIMINUTO_SERIALIZED_SECTION_BEGIN(_INTP_) do {
#   undef DIMINUTO_SERIALIZED_SECTION_END
#   defined DIMINUTO_SERIALIZED_SECTION_END } while (0)
#endif

#endif
