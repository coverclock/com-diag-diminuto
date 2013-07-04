/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_VERSION_
#define _H_COM_DIAG_DIMINUTO_VERSION_

/**
 * @file
 *
 * Copyright 2010-2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * Inserts a build stamp into the translation unit into which it is included
 * that can be extracted from the resulting object code using command line
 * tools like "string". Allows the date and time stamp provided by the C
 * compiler to be overridden from the command line to accommodate folks who
 * get nervous when two successive builds do not produce the identically same
 * binary. Remarkably, such paranoid folks includes certification agencies like
 * the FAA. Oh how many times have I wished that __DATE__ was in ISO8601 format
 * and __TIME__ was in 24-hour UTC.
 */

#include "com/diag/diminuto/diminuto_token.h"

#if !defined(DIMINUTO_VERSION_VINTAGE)
#	define DIMINUTO_VERSION_VINTAGE 0.0.0.0
#endif

#if !defined(DIMINUTO_VERSION_DATE)
#	define DIMINUTO_VERSION_DATE __DATE__
#endif

#if !defined(DIMINUTO_VERSION_TIME)
#	define DIMINUTO_VERSION_TIME __TIME__
#endif

static const char DIMINUTO_VERSION[] __attribute__((__used__)) = "DIMINUTO_VERSION=" DIMINUTO_TOKEN_TOKEN(DIMINUTO_VERSION_VINTAGE) "|" DIMINUTO_VERSION_DATE "|" DIMINUTO_VERSION_TIME;

#endif
