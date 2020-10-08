/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_PIN_
#define _H_COM_DIAG_DIMINUTO_PIN_

/**
 * @file
 *
 * Copyright 2013-2016 Digital Aggregates Corporation, Colorado, USA.
 * Licensed under the terms in LICENSE.txt.
 *
 * The Pin feature implements an interface to interrogate and manipulate
 * General Purpose I/O pins. On most platforms, this will require running
 * with elevated privileges.
 *
 * If you are reading a GPIO pin that changes in real-time, you will probably
 * need a debouncing algorithm and/or an edge detector. See the cue feature
 * for such capabilities implemented in software. These capabilities are not
 * part of the pin feature since they can be used completely independently.
 *
 * The fact that non-digital devices - like mechanical switches - tend to
 * bounce - sometimes for many milliseconds - makes interrupt-driven edge
 * detection problematic unless the debouncing is implemented in hardware.
 *
 * Some domains that I've worked in, like aircraft avionics, don't have digital
 * logic high/ground signaling but rather ground/open signaling, where ground
 * typically means asserted and open means unasserted. To make things even more
 * complicated, this interpretation is sometimes reversed. In either case, the
 * intent is to make the action stimulated by the discrete benign to aircraft
 * safety in the event that a wire is severed. Such ground/open signals are
 * referred to as "discretes", and their conversion to and from conventional
 * digital logic is typically done in hardware. Companies such as Holt
 * Integrated Circuits Inc. make semiconductor chips for the aerospace industry
 * that handle this.
 *
 * You can use the diminuto_pin feature with the diminuto_mux or diminuto_poll
 * features to block until a GPIO pin changes. Use the fileno(3) function from
 * stdio to extract the file descriptor from the object to which the FILE
 * pointer points, and register the descriptor for urgent events (exceptions).
 *
 * I tested the edge interrupt feature with the pintool utility using a
 * Raspberry Pi 3B. Alas, there is a known bug in the GPIO implementation
 * (probably in the device driver) that treats a "rising" edge configuration
 * as "both" edge, so you get select(2) or poll(2) firings on both the rising
 * and the falling edges. You can read the pin to tell which is which, but
 * it was unexpected. Some of the discussion on the interwebs lead you to
 * believe that this is a bug in the Pi's Python library, but that's just
 * because that's what most people are using. This C library uses the GPIO
 * device driver that is exposed via /sys/class/gpio and it sees the same
 * misbehavior.
 */

#include "com/diag/diminuto/diminuto_types.h"
#include <stdio.h>

/*******************************************************************************
 * TYPES
 ******************************************************************************/

/**
 * This enumeration describes the possible edge detection values.
 */
typedef enum DiminutoPinEdge {
    DIMINUTO_PIN_EDGE_NONE = 0,
    DIMINUTO_PIN_EDGE_RISING = 1,
    DIMINUTO_PIN_EDGE_FALLING = 2,
    DIMINUTO_PIN_EDGE_BOTH = 3,
} diminuto_pin_edge_t;

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

/**
 * THis is a printf format for the user-space GPIO API in the /sys file system.
 */
extern const char DIMINUTO_PIN_ROOT_CLASS_GPIO_EXPORT[];

/**
 * THis is a printf format for the user-space GPIO API in the /sys file system.
 */
extern const char DIMINUTO_PIN_ROOT_CLASS_GPIO_PIN_DIRECTION[];

/**
 * THis is a printf format for the user-space GPIO API in the /sys file system.
 */
extern const char DIMINUTO_PIN_ROOT_CLASS_GPIO_PIN_ACTIVELOW[];

/**
 * THis is a printf format for the user-space GPIO API in the /sys file system.
 */
extern const char DIMINUTO_PIN_ROOT_CLASS_GPIO_PIN_EDGE[];

/**
 * THis is a printf format for the user-space GPIO API in the /sys file system.
 */
extern const char DIMINUTO_PIN_ROOT_CLASS_GPIO_PIN_VALUE[];

/**
 * THis is a printf format for the user-space GPIO API in the /sys file system.
 */
extern const char DIMINUTO_PIN_ROOT_CLASS_GPIO_UNEXPORT[];

/*******************************************************************************
 * GLOBALS
 ******************************************************************************/

/**
 * The pin configuration functions use this count to determine how long to
 * delay after a failure following an export.
 */
extern diminuto_ticks_t diminuto_pin_delay;

/**
 * The pin configuration functions use this count to determine how many times
 * to retry after a failure following an export.
 */
extern int diminuto_pin_tries;

/*******************************************************************************
 * DEBUG
 ******************************************************************************/

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
 * CORE API
 ******************************************************************************/

/**
 * This is a general mechanism to configure GPIO pins using the /sys API.
 * It allows EINVAL on fputs(3) and fflush(3) to be optionally ignored.
 * @param format points to a printf format string.
 * @param pin is the GPIO pin number.
 * @param string points to the name of the specific feature in the /sys API.
 * @param ignore if true causes certain errors to be ignored.
 * @param tries is the total number of times the operation can be tried.
 * @param delay is the number of ticks to delay before retrying.
 * @return >=0 for success, <0 for error with errno set.
 */
extern int diminuto_pin_configure_generic(const char * format, int pin, const char * string, int ignore, int tries, diminuto_ticks_t delay);

/**
 * This is a general mechanism to export or unexport GPIO pins using the /sys API.
 * It allows EINVAL on fputs(3) and fflush(3) to be optionally ignored.
 * N.B. See also the notes on diminuto_pin_export().
 * @param format points to a printf format string.
 * @param pin is the GPIO pin number.
 * @param ignore if true causes certain errors to be ignored.
 * @return >=0 for success, <0 for error with errno set.
 */
extern int diminuto_pin_port_generic(const char * format, int pin, int ignore);

/*******************************************************************************
 * HELPERS
 ******************************************************************************/

/**
 * This is a general mechanism to configure GPIO pins using the /sys API.
 * @param format points to a printf format string.
 * @param pin is the GPIO pin number.
 * @param string points to the name of the specific feature in the /sys API.
 * @return >=0 for success, <0 for error with errno set.
 */
static inline int diminuto_pin_configure(const char * format, int pin, const char * string)
{
    return diminuto_pin_configure_generic(format, pin, string, 0, diminuto_pin_tries, diminuto_pin_delay);
}

/**
 * This is a general mechanism to export or unexport GPIO pins using the /sys API.
 * N.B. See also the notes on diminuto_pin_export().
 * @param format points to a printf format string.
 * @param pin is the GPIO pin number.
 * @return >=0 for success, <0 for error with errno set.
 */
static inline int diminuto_pin_port(const char * format, int pin)
{
    return diminuto_pin_port_generic(format, pin, 0);
}

/*******************************************************************************
 * LOW LEVEL API
 ******************************************************************************/

/**
 * Ask that the specified GPIO pin be exported to the /sys/class/gpio file
 * system. N.B. Newly exporting a GPIO pin engages the Linux udev feature.
 * This is an asynchronous feature that creates, in this case, new files
 * in the /sys file system. The time it takes for this action to complete
 * is non-deterministic. Hence, a race condition can result in which
 * successive functions in this API can be called before the export is
 * complete, resulting in errors such as EPERM, EACCESS, ENOENT, or others.
 * @param pin identifies the pin by number from the data sheet.
 * @return >=0 for success, <0 for error with errno set.
 */
static inline int diminuto_pin_export(int pin)
{
    return diminuto_pin_port(DIMINUTO_PIN_ROOT_CLASS_GPIO_EXPORT, pin);
}

/**
 * Ask that the specified GPIO pin be unexported from the /sys/class/gpio file
 * system.
 * @param pin identifies the pin by number from the data sheet.
 * @return >=0 for success, <0 for error with errno set.
 */
static inline int diminuto_pin_unexport(int pin)
{
    return diminuto_pin_port_generic(DIMINUTO_PIN_ROOT_CLASS_GPIO_UNEXPORT, pin, 0);
}

/**
 * Ask that the specified GPIO pin be unexported from the /sys/class/gpio file
 * system. It is not an error to unexport a pin that has not been exported;
 * the error message is suppressed from a common application initialization
 * or error recovery code path.
 * @param pin identifies the pin by number from the data sheet.
 * @return >=0 for success, <0 for error with errno set.
 */
static inline int diminuto_pin_unexport_ignore(int pin)
{
    return diminuto_pin_port_generic(DIMINUTO_PIN_ROOT_CLASS_GPIO_UNEXPORT, pin, !0);
}

/**
 * Ask that the specified GPIO pin be configured to be active low or high.
 * This is only useful if the pin is wired to produce an interrupt.
 * @param pin identifies the pin by number from the data sheet.
 * @param high if !0 configures the pin for active high, else active low.
 * @return >=0 for success, <0 for error with errno set.
 */
static inline int diminuto_pin_active(int pin, int high)
{
    return diminuto_pin_configure(DIMINUTO_PIN_ROOT_CLASS_GPIO_PIN_ACTIVELOW, pin, high ? "0\n" : "1\n");
}

/**
 * Ask that the specified GPIO pin be configured to for no edge (0), rising
 * edge (1), falling edge (2), or both edges (3). This is only useful for GPIO
 * hardware and device drivers that support the select(2) system call, so that
 * a user-space application can block until the pin state changes. It is only
 * practical for for GPIO pins with exceptionally clean transitions (e.g.
 * a digital logic output) or for those with some kind of debouncing circuit
 * (e.g. an FPGA).
 * @param pin identifies the pin by number from the data sheet.
 * @param edge is 0 for none, 1 for rising, 2 for falling, or 3 for both.
 * @return >=0 for success, <0 for error with errno set.
 */
extern int diminuto_pin_edge(int pin, diminuto_pin_edge_t edge);

/**
 * Ask that the specified GPIO pin be configured as an input or an output pin.
 * @param pin identifies the pin by number from the data sheet.
 * @param output if !0 configures the pin for output, else input.
 * @return >=0 for success, <0 for error with errno set.
 */
static inline int diminuto_pin_direction(int pin, int output)
{
    return diminuto_pin_configure(DIMINUTO_PIN_ROOT_CLASS_GPIO_PIN_DIRECTION, pin, output ? "out\n" : "in\n");
}

/**
 * Ask that the specified GPIO pin be configured as an output pin and
 * simultaneously its output state be set.
 * @param pin identifies the pin by number from the data sheet.
 * @param high if !0 sets the pin high, else low.
 * @return >=0 for success, <0 for error with errno set.
 */
static inline int diminuto_pin_initialize(int pin, int high)
{
    return diminuto_pin_configure(DIMINUTO_PIN_ROOT_CLASS_GPIO_PIN_DIRECTION, pin, high ? "high\n" : "low\n");
}

/**
 * Return a FILE pointer for the specified GPIO pin.
 * @param pin identifies the pin by number from the data sheet.
 * @param output if !0 opens the pin for write, else read.
 * @return a FILE pointer or NULL if error.
 */
extern FILE * diminuto_pin_open(int pin, int output);

/**
 * Return the value of a GPIO pin, true (high) or false (low). The application
 * is responsible for inversion (if, for example, the pin is active low).
 * @param fp points to an input GPIO FILE pointer.
 * @return >0 for high, 0 for low, <0 for error with errno set.
 */
extern int diminuto_pin_get(FILE * fp);

/**
 * Set a GPIO pin to low (false) or high (true). The application is responsible
 * for inversion (if, for example, the pin is active low).
 * @param fp points to an output GPIO FILE pointer.
 * @param assert is !0 for true, 0 for false.
 * @return >=0 for success, <0 for error with errno set.
 */
extern int diminuto_pin_put(FILE * fp, int assert);

/**
 * Close an opened FILE pointer for a GPIO pin.
 * @param fp points to a standard I/O FILE object.
 * @return NULL if successful or fp if error.
 */
extern FILE * diminuto_pin_close(FILE * fp);

/**
 * Return a FILE pointer for the specified GPIO pin configured as an input or
 * an output. This function combines the export, direction, and open functions.
 * It tries to handle the case where the export takes a while to complete.
 * @param pin identifies the pin by number from the data sheet.
 * @param output if !0 configures the pin for output, else input.
 * @return a FILE pointer or NULL if error.
 */
extern FILE * diminuto_pin_setup(int pin, int output);

/*******************************************************************************
 * HIGH LEVEL API
 ******************************************************************************/

/**
 * Return a FILE pointer for the specified GPIO pin configured as an input.
 * This function combines the export, direction, and open functions. It tries
 * to handle the case where the export takes a while to complete.
 * @param pin identifies the pin by number from the data sheet.
 * @return a FILE pointer or NULL if error.
 */
static inline FILE * diminuto_pin_input(int pin)
{
    return diminuto_pin_setup(pin, 0);
}

/**
 * Return a FILE pointer for the specified GPIO pin configured as an output.
 * This function combines the export, direction, and open functions. It tries
 * to handle the case where the export takes a while to complete.
 * @param pin identifies the pin by number from the data sheet.
 * @return a FILE pointer or NULL if error.
 */
static inline FILE * diminuto_pin_output(int pin)
{
    return diminuto_pin_setup(pin, !0);
}

/**
 * Set a GPIO pin to high (true). The application is responsible
 * for inversion (if, for example, the pin is active low).
 * @param fp points to an output GPIO FILE pointer.
 * @return >=0 for success, <0 for error with errno set.
 */
static inline int diminuto_pin_set(FILE * fp)
{
    return diminuto_pin_put(fp, !0);
}

/**
 * Set a GPIO pin to low (false). The application is responsible
 * for inversion (if, for example, the pin is active low).
 * @param fp points to an output GPIO FILE pointer.
 * @return >=0 for success, <0 for error with errno set.
 */
static inline int diminuto_pin_clear(FILE * fp)
{
    return diminuto_pin_put(fp, 0);
}

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
