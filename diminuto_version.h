/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_VERSION_
#define _H_COM_DIAG_DIMINUTO_VERSION_

/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * Inserts a build stamp into the translation unit into which it is included
 * that can be extracted from the resulting object code using command line
 * tools like "string".
 */

#define DIMINUTO_VERSION_STRING(_ARG_) #_ARG_

#define DIMINUTO_VERSION_LINE(_ARG_) DIMINUTO_VERSION_STRING(_ARG_)

static const char DIMINUTO_VERSION[] __attribute__((__used__)) = "DIMINUTO_VERSION=" __FILE__ " " DIMINUTO_VERSION_LINE(__LINE__) " " __DATE__ " " __TIME__;

#endif
