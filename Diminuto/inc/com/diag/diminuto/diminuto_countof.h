/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_COUNTOF_
#define _H_COM_DIAG_DIMINUTO_COUNTOF_

/**
 * @file
 *
 * Copyright 2010, 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#define diminuto_countof(_ARRAY_) (sizeof(_ARRAY_) / sizeof(_ARRAY_[0]))

#ifndef countof
#   define countof(_ARRAY_) diminuto_countof(_ARRAY_)
#endif

#endif
