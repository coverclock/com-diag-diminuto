/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_EMIT_
#define _H_COM_DIAG_DIMINUTO_EMIT_

/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * This header file can legitimately be included more than once in a
 * single translation unit.
 */

#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_types.h"

typedef uint64_t diminuto_log_mask_t;

extern diminuto_log_mask_t diminuto_log_mask;

/*
 * These layers are completely arbitary and just serve as examples of how
 * logging might be broken up into functional areas. Define your own
 * eight functional areas as you see fit.
 */
#define DIMINUTO_LOG_PHYSICAL         0
#define DIMINUTO_LOG_DATALINK         1
#define DIMINUTO_LOG_NETWORK          2
#define DIMINUTO_LOG_TRANSPORT        3
#define DIMINUTO_LOG_SESSION          4
#define DIMINUTO_LOG_PRESENTATION     5
#define DIMINUTO_LOG_APPLICATION      6
#define DIMINUTO_LOG_OTHER            7

/**
 * @def DIMINUTO_LOG_MASK
 * Generate a sixty-four bit debug mask using a @a _LAYER_ value from zero to
 * seven and a @a _PRIORITY_ value from zero to seven.
 */
#define DIMINUTO_LOG_MASK(_LAYER_, _PRIORITY_) (1ULL<<(((_LAYER_)<<3)|(_PRIORITY_))

/**
 * @def DIMINUTO_LOG_ENABLE
 * Enable logging for @a _MASK_.
 */
#define DIMINUTO_LOG_ENABLE(_MASK_) (diminuto_log_mask |= (_MASK_))

/**
 * @def DIMINUTO_LOG_DISABLE
 * Disable logging for @a _MASK_.
 */
#define DIMINUTO_LOG_DISABLE(_MASK_) (diminuto_log_mask &= ~(_MASK_))

#define DIMINUTO_LOG_EMIT(_MASK_, _PRIORITY_, _FORMAT_, ...) \
    ((diminuto_log_mask & (_MASK_)) ? \
        DIMINUTO_LOG(_PRIORITY_, _FORMAT_, ## __VA_ARGS__) : (void)0)
 
#endif

/*
 * Define your own value for this preprocessor symbol before including this
 * header file and all messages in the including translation unit will be
 * logged to this layer.
 */
#if !defined(DIMINUTO_LOG_LAYER)
#	define DIMINUTO_LOG_LAYER DIMINUTO_LAYER_OTHER
#endif

/*
 * These macros provides a logging capability that can be conditionally
 * compiled in or out of a translation unit.
 */

#if !defined(DIMINUTO_LOG_ERROR_ENABLE)
#	define DIMINUTO_LOG_ERROR_ENABLE (1)
#endif

#if defined(DIMINUTO_LOG_ERROR)
#	undef DIMINUTO_LOG_ERROR
#endif

#if DIMINUTO_LOG_ERROR_ENABLE
#	define DIMINUTO_LOG_ERROR(_FORMAT_, ...) \
		DIMINUTO_LOG_EMIT(\
				DIMINUTO_LOG_MASK(DIMINUTO_LOG_LAYER, DIMINUTO_PRIORITY_ERROR), \
				LOG_ERR, _FORMAT_, __VA_ARGS__)
#else
#	define DIMINUTO_LOG_ERROR(_FORMAT_, ...) \
		((void)0)
#endif
