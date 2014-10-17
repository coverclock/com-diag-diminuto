/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_PLATFORM_
#define _H_COM_DIAG_DIMINUTO_PLATFORM_

/**
 * @file
 *
 * Copyright 2013-2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#undef COM_DIAG_DIMINUTO_PLATFORM_GLIBC
#undef COM_DIAG_DIMINUTO_PLATFORM_UCLIBC
#undef COM_DIAG_DIMINUTO_PLATFORM_BIONIC

#include <features.h>
#include <sys/cdefs.h>

#if defined(__GLIBC__) && defined(__GLIBC_MINOR__)
#	define COM_DIAG_DIMINUTO_PLATFORM_GLIBC (((__GLIBC__) << 16) + __GLIBC_MINOR__)
#elif defined(__UCLIBC_MAJOR__) && defined(__UCLIBC_MINOR__) && defined(__UCLIBC_SUBLEVEL__)
#	define COM_DIAG_DIMINUTO_PLATFORM_UCLIBC (1000000 + ((__UCLIBC_MAJOR__) * 10000) + ((__UCLIBC_MINOR__) * 100) + (__UCLIBC_SUBLEVEL__))
#elif defined(__BIONIC__)
#	define COM_DIAG_DIMINUTO_PLATFORM_BIONIC (!0)
#else
#	warning Cannot implicitly determine platform!
#endif

#endif
