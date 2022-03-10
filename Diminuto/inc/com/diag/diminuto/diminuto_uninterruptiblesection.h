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

#include "com/diag/diminuto/diminuto_countof.h"
#include <signal.h>
#include <pthread.h>

/**
 * Populate the provided signal set with the currently blocked signals and
 * then additionally block the signals in the provided array.
 * @param signals is an array of signal numbers.
 * @param count is the number of signal numbers in the array.
 * @param sp points to the signal set of previously block signals.
 * @return 0 for success, or <0 with errno set if an error occurred.
 */
extern int diminuto_uninterruptiblesection_block(const int signals[], size_t count, sigset_t * sp);

/**
 * Restore the blocked signals to those signals in the provided signal set.
 * @param vp points to the set of prior blocked signals.
 */
extern void diminuto_uninterruptiblesection_cleanup(void * vp);

/**
 * @def DIMINUTO_UNINTERRUPTIBLE_SECTION_BEGIN
 * Begin a code block that is uninterruptible by a comma separated list of
 * signals by adding those signals to the mask of signals that are already
 * blocked.
 */
#define DIMINUTO_UNINTERRUPTIBLE_SECTION_BEGIN(...) \
    do { \
        const int diminuto_uninterruptiblesection_signals[] = { __VA_ARGS__ }; \
        sigset_t diminuto_uninterruptiblesection_set; \
        if (diminuto_uninterruptiblesection_block(diminuto_uninterruptiblesection_signals, diminuto_countof(diminuto_uninterruptiblesection_signals), &diminuto_uninterruptiblesection_set) == 0) { \
            pthread_cleanup_push(diminuto_uninterruptiblesection_cleanup, &diminuto_uninterruptiblesection_set); \
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
            pthread_cleanup_pop(!0); \
        } \
    } while (0)

#endif
