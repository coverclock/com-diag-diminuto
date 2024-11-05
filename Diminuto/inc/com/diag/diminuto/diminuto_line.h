/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_LINE_
#define _H_COM_DIAG_DIMINUTO_LINE_

/**
 * @file
 * @copyright Copyright 2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Provides an interface to handle GPIO pins via the ioctl ABI.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This feature controls General Purpose I/O (GPIO) lines. GPIO is a term for
 * reading and manipulating digital signal lines, typically on a System On a
 * Chip (SOC). I use this feature to, for example, read the One Pulse Per Second
 * (1PPS) signal from a GNSS receiver, to receive an interupt from a sensor, or
 * to control status LEDs (the brightness of the LED can even be controlled
 * using Pulse Width Modulation (PWM), for example using the Modulator feature.
 * This implementation is based on the Linux kernel's GPIO character device
 * drivers and their ioctl-based ABI. The Line feature replaces the Pin feature
 * (which still exists) which is based on the deprecated sysfs-based ABI.
 *
 * REFERENCES
 *
 * <https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/driver-api/gpio>
 *
 * <https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/tools/gpio>
 */

#include <linux/gpio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*******************************************************************************
 * CODE GENERATORS
 ******************************************************************************/

/**
 * @def COM_DIAG_DIMINUTO_LINE_ROOT
 * Defines the root path for line devices.
 */
#define COM_DIAG_DIMINUTO_LINE_ROOT "/dev"

/**
 * @def COM_DIAG_DIMINUTO_LINE_PREFIX
 * Defines the prefix for line devices.
 */
#define COM_DIAG_DIMINUTO_LINE_PREFIX "gpiochip"

/*******************************************************************************
 * TYPES
 ******************************************************************************/

/**
 * These flags may ba or'ed together to describe a line when it is opened.
 * What configurations are possible depend on the underlying GPIO controller,
 * the target's hardware design, and the developer's intent. On some systems,
 * a single GPIO line may be both an input and an output.
 */
typedef enum DiminutoLineFlag {
	DIMINUTO_LINE_FLAG_ACTIVE_LOW =         GPIO_V2_LINE_FLAG_ACTIVE_LOW,       /**< Line is active low. */
	DIMINUTO_LINE_FLAG_INPUT =              GPIO_V2_LINE_FLAG_INPUT,            /**< Configure line for input. */
	DIMINUTO_LINE_FLAG_OUTPUT =             GPIO_V2_LINE_FLAG_OUTPUT,           /**< Configure line for output. */
	DIMINUTO_LINE_FLAG_EDGE_RISING =        GPIO_V2_LINE_FLAG_EDGE_RISING,      /**< Line indicates on the rising edge. */
	DIMINUTO_LINE_FLAG_EDGE_FALLING =       GPIO_V2_LINE_FLAG_EDGE_FALLING,     /**< Line indicates on the falling edge. */
	DIMINUTO_LINE_FLAG_EDGE_BOTH =          GPIO_V2_LINE_FLAG_EDGE_RISING | GPIO_V2_LINE_FLAG_EDGE_FALLING,
	DIMINUTO_LINE_FLAG_READ =               GPIO_V2_LINE_FLAG_INPUT | GPIO_V2_LINE_FLAG_EDGE_RISING | GPIO_V2_LINE_FLAG_EDGE_FALLING,
	DIMINUTO_LINE_FLAG_OPEN_DRAIN =         GPIO_V2_LINE_FLAG_OPEN_DRAIN,       /**< Configure line for open drain. */
	DIMINUTO_LINE_FLAG_OPEN_SOURCE =        GPIO_V2_LINE_FLAG_OPEN_SOURCE,      /**< Configure line for open source. */
	DIMINUTO_LINE_FLAG_BIAS_PULL_UP =       GPIO_V2_LINE_FLAG_BIAS_PULL_UP,     /**< Configure line with a pull up resistor. */
	DIMINUTO_LINE_FLAG_BIAS_PULL_DOWN =     GPIO_V2_LINE_FLAG_BIAS_PULL_DOWN,   /**< Configure line with a pull down resister. */
	DIMINUTO_LINE_FLAG_BIAS_DISABLED =      GPIO_V2_LINE_FLAG_BIAS_DISABLED,    /**< Configure line with a bias disabled. */
} diminuto_line_flag_t;

/**
 * THis is the data type of an offset that identifies a line within a
 * GPIO controller a.k.a. a gpiochip. Line offset values can be in range
 * of zero to sixty-three.
 */
typedef unsigned int diminuto_line_offset_t;

/**
 * This is the data type of a bit mask, every bit of which references a
 * specific line within a single GPIO controller a.k.a. gpiochip. There
 * can be sixty-four lines within a single gpiochip, each referenced by the
 * the value (1ULL << line).
 */
typedef uint64_t diminuto_line_bits_t;

/**
 * If a debouncer is available in the GPIO controller or device driver,
 * it's debounce window is specified in microseconds in this data type.
 */
typedef uint32_t diminuto_line_duration_t;

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

/**
 * This is the default root of the device directory (typically a special file
 * system instantiated at boot time).
 */
static const char DIMINUTO_LINE_ROOT[] = COM_DIAG_DIMINUTO_LINE_ROOT;

/**
 * Each GPIO controller instantiates a device whose name begins with this
 * prefix in the device directory.
 */
static const char DIMINUTO_LINE_PREFIX[] = COM_DIAG_DIMINUTO_LINE_PREFIX;

/*******************************************************************************
 * DEBUGGING
 ******************************************************************************/

/**
 * The GPIO ABI allows applications to label GPIO lines with a consumer name,
 * for example for purposes of debugging. This function changes the default
 * consumer name applied when new lines are opened.
 * @param next is the new consumer name to be applied upon open.
 * @return the prior consumer name.
 */
extern const char * diminuto_line_consumer(const char * next);

/*******************************************************************************
 * FINDING
 ******************************************************************************/

/**
 * Find a particular line across all GPIO controllers based on its symbolic
 * name configured in the controller. Return the device name of the controller,
 * and the offset of the line within the controller. To do this, the function
 * walks the file system tree starting at the specified root, looks at every
 * character device whose name begins with the specified prefix, interrogates
 * every device for its lines, and looks for a line with the specified name.
 * @param name is the name for which to search.
 * @param root is the location in the file system to begin walking.
 * @param prefix is the beginning of the character device for which to look.
 * @param buffer is where the device name will be stored.
 * @param size is the size of the buffer in bytes.
 * @param linep points to the variable into which the offset will be stored.
 * @return a pointer ot the buffer with the device name or NULL if not found.
 */
extern const char * diminuto_line_find_generic(const char * name, const char * root, const char * prefix, char * buffer, size_t size, diminuto_line_offset_t * linep);

/**
 * Find a particular line across all GPIO controllers based on its symbolic
 * name configured in the controller. Return the device name of the controller,
 * and the offset of the line within the controller. To do this, the function
 * walks the file system tree starting at the specified root, looks at every
 * character device whose name begins with the specified prefix, interrogates
 * every device for its lines, and looks for a line with the specified name.
 * The default root and prefix are used.
 * @param name is the name for which to search.
 * @param buffer is where the device name will be stored.
 * @param length is the size of the buffer in bytes.
 * @param linep points to the variable into which the offset will be stored.
 * @return a pointer ot the buffer with the device name or NULL if not found.
 */
static inline const char * diminuto_line_find(const char * name, char * buffer, size_t length, diminuto_line_offset_t * linep) {
    return diminuto_line_find_generic(name, DIMINUTO_LINE_ROOT, DIMINUTO_LINE_PREFIX, buffer, length, linep);
}

/*******************************************************************************
 * PARSING
 ******************************************************************************/

/*
 * e.g. "/dev/gpiochip4:-15" == /dev/gpiochip4 line 15 inverted.
 */

/**
 * Parse a character string parameter (e.g. from the command line) to try
 * to determine the character device name and line offset. The parameter
 * can be a device name and an offset, e.g. "/dev/gpiochip4:-15", where
 * the optional minus sign indicates that the line is logically inverted,
 * or it can be a symbolic name known to the device driver, e.g. "-GPIO18",
 * where the minus sign similarly indicates that the line is inverted. Note
 * that an offset of "0", "-0", and even "+0", are valid, since 0 is valid
 * offset.
 * @param parameter is the input parameter string.
 * @param buffer is where the device name will be stored.
 * @param size is the size of the buffer in bytes.
 * @param linep points to the variable into which the offset will be stored.
 * @param invertedp points to a variable set to true if inversion is specified.
 * @return a pointer ot the buffer with the device name or NULL if not found.
 */
extern const char * diminuto_line_parse(const char * parameter, char * buffer, size_t size, diminuto_line_offset_t * linep, int * invertedp);

/*******************************************************************************
 * GENERICS
 ******************************************************************************/

/**
 * Open a GPIO device and line offsets and return a file descriptor that
 * can be used with ioctl(2), read(2), select(2), close(2), etc. If more
 * than one line offset is spefified, all lines are treated identically,
 * for all operations.
 * @param path is the path to the character device.
 * @param line is an array of line offsets.
 * @param lines is the number of offsets in the array.
 * @param flags is a bit map of flags used to configure the lines.
 * @param useconds is duration of the debounce window in useconds or 0 for none.
 * @return an open file descriptor or <0 for error.
 */
extern int diminuto_line_open_generic(const char * path, const diminuto_line_offset_t line[], size_t lines, diminuto_line_bits_t flags, diminuto_line_duration_t useconds);

/**
 * Gets the values of one or more lines that have been opened For input.
 * @param fd is the open file descriptor.
 * @param mask is a bit mask identifying which lines to read.
 * @param bitsp points to a variable into which the result bits are read.
 * return >=0 for success, <0 for error.
 */
extern int diminuto_line_get_generic(int fd, diminuto_line_bits_t mask, diminuto_line_bits_t * bitsp);

/**
 * Puts the values to one or more lines that have been opened for output.
 * @param fd is the open file descriptor.
 * @param mask is a bit mask identifying which lines to write.
 * @param bits a variable from which the value bits are written.
 * return >=0 for success, <0 for error.
 */
extern int diminuto_line_put_generic(int fd, diminuto_line_bits_t mask, diminuto_line_bits_t bits);

/**
 * Close a file descriptor.
 * @param fd is the open file destriptor.
 */
extern int diminuto_line_close(int fd);

/*******************************************************************************
 * MUTIPLEXING
 ******************************************************************************/

/**
 * Open a line for input and return a file descriptor which will wait on a
 * rising or falling edge as specified in the flags parameter, and return
 * a 1 for a rising edge, or 0 for a falling edge. If the controller or device
 * driver provides debouncing, a debounce wind can be specified in microseconds,
 * or zero for none.
 * @param path is the path to the character device.
 * @param line is the offset of the single line.
 * @param flags is a bit map of flags used to configure the line; if 0, the flags for INPUT, RISING, and FALLING are used.
 * @param useconds is duration of the debounce window in useconds or 0 for none.
 * @return an open file descriptor or <0 for error.
 */
static inline int diminuto_line_open_read(const char * path, diminuto_line_offset_t line, diminuto_line_bits_t flags, diminuto_line_duration_t useconds) {
    return diminuto_line_open_generic(path, &line, 1, (flags == 0) ? DIMINUTO_LINE_FLAG_READ : flags, useconds);
}

/**
 * Wait for an event on the line opened for reading, and return 0 for a falling
 * edge or 1 for a risng edge.
 * @param fd is the open file descriptor.
 * @return 0 for a falling edge, 1 for a rising edge.
 */
extern int diminuto_line_read(int fd);

/*******************************************************************************
 * POLLING
 ******************************************************************************/

/**
 * Open a single line given the name of a characer device and a line offset.
 * @param path is the path to the character device.
 * @param line is the offset of the single line.
 * @param flags is a bit map of flags used to configure the line.
 * @return an open file descriptor or <0 for error.
 */
static inline int diminuto_line_open(const char * path, diminuto_line_offset_t line, diminuto_line_bits_t flags) {
    return diminuto_line_open_generic(path, &line, 1, flags, 0);
}

/**
 * Get the current state of a single line.
 * @param fd is an open file descriptor.
 * @return the state of the line, 0 for false, 1 for true.
 */
extern int diminuto_line_get(int fd);

/**
 * Put the new state onto a sigle line.
 * @param fd is an open file descriptor.
 * @param state is 0 for false, 1 for true.
 * @return the new state of the line, 0 for false, 1 for true.
 */
extern int diminuto_line_put(int fd, int state);

/*******************************************************************************
 * HELPERS
 ******************************************************************************/
 
/**
 * Set the new state of a sigle line to true.
 * @param fd is an open file descriptor.
 * @return the new state of the line, 0 for false, 1 for true.
 */
static inline int diminuto_line_set(int fd)
{
    return diminuto_line_put(fd, !0);
}

/**
 * Clear the new state of a sigle line to false.
 * @param fd is an open file descriptor.
 * @return the new state of the line, 0 for false, 1 for true.
 */
static inline int diminuto_line_clear(int fd)
{
    return diminuto_line_put(fd, 0);
}

/*******************************************************************************
 * EXAMPLES
 ******************************************************************************/

/**
 * Open a single line for input given the name of a characer device and a
 * line offset.
 * @param path is the path to the character device.
 * @param line is the offset of the single line.
 * @return an open file descriptor or <0 for error.
 */
static inline int diminuto_line_open_input(const char * path, diminuto_line_offset_t line) {
    return diminuto_line_open(path, line, DIMINUTO_LINE_FLAG_INPUT);
}

/**
 * Open a single line for input and active low given the name of a characer
 * device and a line offset.
 * @param path is the path to the character device.
 * @param line is the offset of the single line.
 * @return an open file descriptor or <0 for error.
 */
static inline int diminuto_line_open_input_inverted(const char * path, diminuto_line_offset_t line) {
    return diminuto_line_open(path, line, DIMINUTO_LINE_FLAG_INPUT | DIMINUTO_LINE_FLAG_ACTIVE_LOW);
}

/**
 * Open a single line for output given the name of a characer device and a
 * line offset.
 * @param path is the path to the character device.
 * @param line is the offset of the single line.
 * @return an open file descriptor or <0 for error.
 */
static inline int diminuto_line_open_output(const char * path, diminuto_line_offset_t line) {
    return diminuto_line_open(path, line, DIMINUTO_LINE_FLAG_OUTPUT);
}

/**
 * Open a single line for output and active low given the name of a characer
 * device and a line offset.
 * @param path is the path to the character device.
 * @param line is the offset of the single line.
 * @return an open file descriptor or <0 for error.
 */
static inline int diminuto_line_open_output_inverted(const char * path, diminuto_line_offset_t line) {
    return diminuto_line_open(path, line, DIMINUTO_LINE_FLAG_OUTPUT | DIMINUTO_LINE_FLAG_ACTIVE_LOW);
}

#endif
