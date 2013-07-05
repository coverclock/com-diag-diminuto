/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_TESTIFY_
#define _H_COM_DIAG_DIMINUTO_TESTIFY_

/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

/**
 * Interpret a string as indicating true (closed, enabled, high, true, yes,
 * on, 1, 01, 0x1) or false (open, disabled, low, false, no, off, 0, 0x0).
 * @param testimony points to a C string.
 * @param otherwise is returned if the testimony is invalid, ambiguous, NULL.
 * @return !0 for true or 0 for false.
 */
int diminuto_testify(const char * testimony, int otherwise);

#endif
