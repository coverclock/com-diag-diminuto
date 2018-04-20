/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_COHERENTSECTION_
#define _H_COM_DIAG_DIMINUTO_COHERENTSECTION_

/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_barrier.h"

/**
 * @def DIMINUTO_COHERENT_SECTION_BEGIN
 * Begin a code section that has a read (acquire) memory barrier at the
 * beginning and a write (release) memory barrier at the end.
 */
#define DIMINUTO_COHERENT_SECTION_BEGIN \
	do { \
		diminuto_acquire(); \
		do { \
			(void)0

/**
 * @def DIMINUTO_COHERENT_SECTION_END
 * End a code section that has a read (acquire) memory barrier at the
 * beginning and a write (release) memory barrier at the end.
 */
#define DIMINUTO_COHERENT_SECTION_END \
		} while (0); \
		diminuto_release(); \
	} while (0)

#endif
