/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_UNINTERRUPTIBLESECTION_
#define _H_COM_DIAG_DIMINUTO_UNINTERRUPTIBLESECTION_

/**
 * @file
 *
 * Copyright 2013-2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
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
		sigset_t _diminuto_uninterruptible_section_now_; \
		sigset_t _diminuto_uninterruptible_section_was_; \
		sigemptyset(&_diminuto_uninterruptible_section_now_); \
		int _diminuto_uninterruptible_section_signals_[] = { __VA_ARGS__ }; \
		int _diminuto_uninterruptible_section_ndx_; \
		for (_diminuto_uninterruptible_section_ndx_ = 0; _diminuto_uninterruptible_section_ndx_ < diminuto_countof(_diminuto_uninterruptible_section_signals_); ++_diminuto_uninterruptible_section_ndx_) { \
			sigaddset(&_diminuto_uninterruptible_section_now_, _diminuto_uninterruptible_section_signals_[_diminuto_uninterruptible_section_ndx_]); \
		} \
		pthread_sigmask(SIG_BLOCK, &_diminuto_uninterruptible_section_now_, &_diminuto_uninterruptible_section_was_); \
		do { \
			(void)0

/**
 * @def DIMINUTO_UNINTERRUPTIBLE_SECTION_END
 * End a code block that was uninterruptible by the signals specified at the
 * beginning of the block by returning the mask of blocked signals back to its
 * original state at the beginning of the block.
 */
#define DIMINUTO_UNINTERRUPTIBLE_SECTION_END \
		} while (0); \
		pthread_sigmask(SIG_SETMASK, &_diminuto_uninterruptible_section_was_, (sigset_t *)0); \
	} while (0)

#endif
