/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_DUMP_
#define _H_COM_DIAG_DIMINUTO_DUMP_

/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>

void diminuto_dump_generic(FILE * fp, const void * data, size_t length, int upper, char dot, int virtualize, uintptr_t first, size_t indent);

#endif
