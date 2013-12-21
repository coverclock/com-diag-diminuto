/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_PLATFORM_
#define _H_COM_DIAG_DIMINUTO_PLATFORM_

/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include <sys/types.h>
#if defined(__UCLIBC_MAJOR__) && defined(__UCLIBC_MINOR__) && defined(__UCLIBC_SUBLEVEL__)
#	define COM_DIAG_DIMINUTO_PLATFORM_UCLIBC 1
#else
#	undef  COM_DIAG_DIMINUTO_PLATFORM_UCLIBC
#endif

#endif
