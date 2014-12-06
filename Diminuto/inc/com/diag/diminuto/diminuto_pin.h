/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_PIN_
#define _H_COM_DIAG_DIMINUTO_PIN_

/**
 * @file
 *
 * Copyright 2013-2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * If you are reading a GPIO pin that changes in real-time, you will probably
 * need a debouncing algorithm and/or an edge detector. The Telegraph library,
 * which is not Linux-specific, has both. It can be found at
 * http://www.diag.com/navigation/downloads/Telegraph.html .
 */

#include <stdio.h>

/**
 * Place the feature in debug mode in which the functions write to files
 * in the specified directory (for example, /tmp) instead of in /sys. This
 * allows programs using this feature to be tested without actually manipulating
 * GPIO pins (which may or may not even exist on the system on which the code
 * is being unit tested). Note that the feature stores the actual pointer passed
 * as a parameter, so its data must persist for the duration of its use. If the
 * parameter is NULL, the default /sys directory is used by default.
 * @param tmp points to the new root directory to use instead of /sys, or NULL.
 * @return the prior root directory.
 */
extern const char * diminuto_pin_debug(const char * tmp);

/*******************************************************************************
 * LOW LEVEL API
 ******************************************************************************/

/**
 * Ask that the specified GPIO pin be exported to the /sys/class/gpio file
 * system.
 * @param pin identifies the pin by number from the data sheet.
 * @return >=0 for success, <0 for error.
 */
extern int diminuto_pin_export(int pin);

/**
 * Ask that the specified GPIO pin be unexported from the /sys/class/gpio file
 * system.
 * @param pin identifies the pin by number from the data sheet.
 * @return >=0 for success, <0 for error.
 */
extern int diminuto_pin_unexport(int pin);

/**
 * Ask that the specified GPIO pin be configured to be active low or high.
 * @param pin identifies the pin by number from the data sheet. This is only
 * useful for pins wired to produce an interrupt.
 * @param high if !0 configures the pin for active high, else active low.
 * @return >=0 for success, <0 for error.
 */
extern int diminuto_pin_active(int pin, int high);

typedef enum DiminutoPinEdge {
	DIMINUTO_PIN_EDGE_NONE = 0,
	DIMINUTO_PIN_EDGE_RISING = 1,
	DIMINUTO_PIN_EDGE_FALLING = 2,
	DIMINUTO_PIN_EDGE_BOTH = 3,
} diminuto_pin_edge_t;

/**
 * Ask that the specified GPIO pin be configured to for no edge (0), rising
 * edge (1), falling edge (2), or both edges (3). This is only useful for pins
 * wired to produce an interrupt.
 * @param pin identifies the pin by number from the data sheet.
 * @param edge is 0 for none, 1 for rising, 2 for falling, or 3 for both.
 * @return >=0 for success, <0 for error.
 */
extern int diminuto_pin_edge(int pin, diminuto_pin_edge_t edge);

/**
 * Ask that the specified GPIO pin be configured as an input or an output pin.
 * @param pin identifies the pin by number from the data sheet.
 * @param output if !0 configures the pin for output, else input.
 * @return >=0 for success, <0 for error.
 */
extern int diminuto_pin_direction(int pin, int output);

/**
 * Ask that the specified GPIO pin be configured as an output pin and
 * simultaneously its output state be set.
 * @param pin identifies the pin by number from the data sheet.
 * @param high if !0 sets the pin high, else low.
 * @return >=0 for success, <0 for error.
 */
extern int diminuto_pin_initialize(int pin, int high);

/**
 * Return a FILE pointer for the specified GPIO pin.
 * @param pin identifies the pin by number from the data sheet.
 * @return a FILE pointer or NULL if error.
 */
extern FILE * diminuto_pin_open(int pin);

/**
 * Return the value of a GPIO pin, true (high) or false (low). The application
 * is responsible for inversion (if, for example, the pin is active low).
 * @param fp points to an input GPIO FILE pointer.
 * @return >0 for high, 0 for low, <0 for error.
 */
extern int diminuto_pin_get(FILE * fp);

/**
 * Set a GPIO pin to low (false) or high (true). The application is responsible
 * for inversion (if, for example, the pin is active low).
 * @param fp points to an output GPIO FILE pointer.
 * @param assert is !0 for true, 0 for false.
 * @return >=0 for success, <0 for error.
 */
extern int diminuto_pin_put(FILE * fp, int assert);

/**
 * Close an opened FILE pointer for a GPIO pin.
 * @param fp points to a standard I/O FILE object.
 * @return NULL if successful or fp if error.
 */
extern FILE * diminuto_pin_close(FILE * fp);

/*******************************************************************************
 * HIGH LEVEL API
 ******************************************************************************/

/**
 * Return a FILE pointer for the specified GPIO pin configured as an input.
 * This function combines the open, export, and direction functions.
 * @param pin identifies the pin by number from the data sheet.
 * @return a FILE pointer or NULL if error.
 */
extern FILE * diminuto_pin_input(int pin);

/**
 * Return a FILE pointer for the specified GPIO pin configured as an output.
 * This function combines the open, export, and direction functions.
 * @param pin identifies the pin by number from the data sheet.
 * @return a FILE pointer or NULL if error.
 */
extern FILE * diminuto_pin_output(int pin);

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
 * Close a FILE pointer to a GPIO pin, and deconfigure the pin. It is not an
 * error to call this with a NULL FILE pointer and/or an unconfigured pin.
 * This functions combines the close and unexport functions.
 * @param fp points to a GPIO FILE pointer.
 * @param pin identifies the pin by number from the data sheet.
 * @return NULL for success, fp otherwise.
 */
extern FILE * diminuto_pin_unused(FILE * fp, int pin);

#endif
