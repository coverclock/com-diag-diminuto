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
 *
 * There is a lower level interface upon which these functions are implemented.
 * It is deliberately not exposed as part of the public interface, but it is
 * never the less safe to use if you choose to do so. See the implementation
 * of this feature for the necessary function prototypes.
 *
 * If you are reading a GPIO pin that changes in real-time, you will probably
 * need a debouncing algorithm and/or an edge detector. The Telegraph library,
 * which is not Linux-specific, has both. It can be found at
 * http://www.diag.com/navigation/downloads/Telegraph.html .
 */

#include <stdio.h>

/**
 * Place the feature in debug mode in which the functions write to files
 * in the /tmp directory instead of in /sys. This allows programs using this
 * feature to be tested without actually manipulating GPIO pins (which may
 * or may not even exist).
 * @param enable if true places feature in debug mode, otherwise in run mode.
 * @return the prior mode.
 */
extern int diminuto_pin_debug(int enable);

/**
 * Return a FILE pointer for the specified GPIO pin configured for as an input.
 * @param pin identifies the pin by number from the data sheet.
 * @return a FILE pointer or NULL if error.
 */
extern FILE * diminuto_pin_input(int pin);

/**
 * Return a FILE pointer for the specified GPIO pin configured for as an output.
 * @param pin identifies the pin by number from the data sheet.
 * @return a FILE pointer or NULL if error.
 */
extern FILE * diminuto_pin_output(int pin);

/**
 * Return the value of a GPIO pin, true (high) or false (low). The application
 * is responsible for inversion (if, for example, the pin is active low).
 * @param fp points to an input GPIO FILE pointer.
 * @return >0 for high, 0 for low, <0 for error.
 */
extern int diminuto_pin_get(FILE * fp);

/**
 * Set a GPIO pin to high (true). The application is responsible
 * for inversion (if, for example, the pin is active low).
 * @param fp points to an output GPIO FILE pointer.
 * @return >=0 for success, <0 for error.
 */
extern int diminuto_pin_set(FILE * fp);

/**
 * Set a GPIO pin to low (false). The application is responsible
 * for inversion (if, for example, the pin is active low).
 * @param fp points to an output GPIO FILE pointer.
 * @return >=0 for success, <0 for error.
 */
extern int diminuto_pin_clear(FILE * fp);

/**
 * Set a GPIO pin to low (false) or high (true). The application is responsible
 * for inversion (if, for example, the pin is active low).
 * @param fp points to an output GPIO FILE pointer.
 * @param assert is !0 for true, 0 for false.
 * @return >=0 for success, <0 for error.
 */
extern int diminuto_pin_put(FILE * fp, int assert);

#endif
