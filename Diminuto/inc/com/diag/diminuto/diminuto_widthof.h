/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_WIDTHOF_
#define _H_COM_DIAG_DIMINUTO_WIDTHOF_

/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

/**
 * @def diminuto_widthof
 * Given a variable or type named @a _TYPE_ compute the number of bits in it.
 */
#define diminuto_widthof(_TYPE_) (sizeof(_TYPE_) * 8)

#ifndef widthof
#   define widthof(_TYPE_) diminuto_widthof(_TYPE_)
#endif

#endif
