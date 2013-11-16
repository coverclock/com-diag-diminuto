/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_PIN_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_PIN_PRIVATE_

/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include <stdio.h>

/**
 * Ask that the specified GPIO pin be exported to the /sys/class/gpio file
 * system.
 * @param pin identifies the pin by number from the data sheet.
 * @return >=0 for success, <0 for error.
 */
extern int diminuto_pin_export(int pin);

/**
 * Ask that the specified GPIO pin be configured as an input or an output pin.
 * @param pin identifies the pin by number from the data sheet.
 * @param output if true configures the pin for output, else input.
 * @return >=0 for success, <0 for error.
 */
extern int diminuto_pin_direction(int pin, int output);

/**
 * Return a FILE pointer for the specified GPIO pin.
 * @param pin identifies the pin by number from the data sheet.
 * @return a FILE pointer or NULL if error.
 */
extern FILE * diminuto_pin_open(int pin);

#endif
