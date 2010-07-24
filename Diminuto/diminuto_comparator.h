/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_COMPARATOR_
#define _H_COM_DIAG_DIMINUTO_COMPARATOR_

/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

typedef int (diminuto_comparator)(const void *, const void *);

extern int diminuto_compare_strings(const void *, const void *);

extern int diminuto_compare_pointers(const void *, const void *);

#endif
