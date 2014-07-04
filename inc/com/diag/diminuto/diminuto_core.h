/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_CORE_
#define _H_COM_DIAG_DIMINUTO_CORE_

/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

/**
 * Set the core dump resource limit to its maximum.
 * @return zero if successful, non-zero otherwise.
 */
extern int diminuto_core_enable(void);

#endif
