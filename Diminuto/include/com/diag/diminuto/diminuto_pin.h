/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_PIN_
#define _H_COM_DIAG_DIMINUTO_PIN_

/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include <stdio.h>

extern int diminuto_pin_debug(int enable);

extern FILE * diminuto_pin_input(int pin);

extern FILE * diminuto_pin_output(int pin);

extern int diminuto_pin_set(FILE * fp, int assert);

extern int diminuto_pin_get(FILE * fp);

#endif
