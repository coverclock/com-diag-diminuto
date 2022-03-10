/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_UNINTERRUPTIBLESECTION_
#define _H_COM_DIAG_DIMINUTO_UNINTERRUPTIBLESECTION_

/**
 * @file
 * @copyright Copyright 2013-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements bracketing macros for code that cannot be interrupted.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Uninterruptible Section feature provides bracketing macros for
 * code sections that cannot be interrupted by specified set of signals.
 */

#include <signal.h>
#include <pthread.h>
#include "com/diag/diminuto/diminuto_countof.h"

/**
 * @def DIMINUTO_UNINTERRUPTIBLE_SECTION_BEGIN
 * Begin a code block that is uninterruptible by a comma separated list of
 * signals by adding those signals to the mask of signals that are already
 * blocked.
 */
#define DIMINUTO_UNINTERRUPTIBLE_SECTION_BEGIN(...) \
    do { \
        const int diminuto_uninterruptible_section_signals[] = { __VA_ARGS__ }; \
        int diminuto_uninterruptible_section_ndx = 0; \
        sigset_t diminuto_uninterruptible_section_now; \
        sigset_t diminuto_uninterruptible_section_was; \
        sigemptyset(&diminuto_uninterruptible_section_now); \
        for (diminuto_uninterruptible_section_ndx = 0; diminuto_uninterruptible_section_ndx < diminuto_countof(diminuto_uninterruptible_section_signals); ++diminuto_uninterruptible_section_ndx) { \
            sigaddset(&diminuto_uninterruptible_section_now, diminuto_uninterruptible_section_signals[diminuto_uninterruptible_section_ndx]); \
        } \
        pthread_sigmask(SIG_BLOCK, &diminuto_uninterruptible_section_now, &diminuto_uninterruptible_section_was); \
        do { \
            ((void)0)

/**
 * @def DIMINUTO_UNINTERRUPTIBLE_SECTION_END
 * End a code block that was uninterruptible by the signals specified at the
 * beginning of the block by returning the mask of blocked signals back to its
 * original state at the beginning of the block.
 */
#define DIMINUTO_UNINTERRUPTIBLE_SECTION_END \
        } while (0); \
        pthread_sigmask(SIG_SETMASK, &diminuto_uninterruptible_section_was, (sigset_t *)0); \
    } while (0)

#endif
