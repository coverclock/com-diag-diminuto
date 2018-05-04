/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_SERIAL_
#define _H_COM_DIAG_DIMINUTO_SERIAL_

/**
 * @file
 *
 * Copyright 2010-2017 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include <stdio.h>

/**
 * These are the data rates that Diminuto Serial supports. The enumeration
 * isn't used in the function call because the typical use case is that
 * the data rate is being set by an integer parameter from the command
 * line.
 */
typedef enum DiminutoSerialBitsPerSecond {
    DIMINUTO_SERIAL_BITSPERSECOND_NOCHANGE      = -1,
    DIMINUTO_SERIAL_BITSPERSECOND_DISCONNECT    = 0,
    DIMINUTO_SERIAL_BITSPERSECOND_50            = 50,
    DIMINUTO_SERIAL_BITSPERSECOND_75            = 75,
    DIMINUTO_SERIAL_BITSPERSECOND_110           = 110,
    DIMINUTO_SERIAL_BITSPERSECOND_134           = 134,
    DIMINUTO_SERIAL_BITSPERSECOND_150           = 150,
    DIMINUTO_SERIAL_BITSPERSECOND_200           = 200,
    DIMINUTO_SERIAL_BITSPERSECOND_300           = 300,
    DIMINUTO_SERIAL_BITSPERSECOND_600           = 600,
    DIMINUTO_SERIAL_BITSPERSECOND_1200          = 1200,
    DIMINUTO_SERIAL_BITSPERSECOND_1800          = 1800,
    DIMINUTO_SERIAL_BITSPERSECOND_2400          = 2400,
    DIMINUTO_SERIAL_BITSPERSECOND_4800          = 4800,
    DIMINUTO_SERIAL_BITSPERSECOND_9600          = 9600,
    DIMINUTO_SERIAL_BITSPERSECOND_19200         = 19200,
    DIMINUTO_SERIAL_BITSPERSECOND_38400         = 38400,
    DIMINUTO_SERIAL_BITSPERSECOND_57600         = 57600,
    DIMINUTO_SERIAL_BITSPERSECOND_115200        = 115200,
    DIMINUTO_SERIAL_BITSPERSECOND_230400        = 230400,
    DIMINUTO_SERIAL_BITSPERSECOND_NOMINAL       = 115200,
} diminuto_serial_bitspersecond_t;

/**
 * These are the data bits that Diminuto Serial supports. The enumeration
 * isn't used in the function call because the typical use case is that
 * the data bits are being set by an integer parameter from the command
 * line.
 */
typedef enum DiminutoSerialDataBits {
    DIMINUTO_SERIAL_DATABITS_5          = 5,
    DIMINUTO_SERIAL_DATABITS_6          = 6,
    DIMINUTO_SERIAL_DATABITS_7          = 7,
    DIMINUTO_SERIAL_DATABITS_8          = 8,
    DIMINUTO_SERIAL_DATABITS_NOMINAL    = 8,
} diminuto_serial_databits_t;

/**
 * These are the parity bits that Diminuto Serial supports. The enumeration
 * isn't used in the function call because the typical use case is that
 * the parity bits are being set by an integer parameter from the command
 * line.
 */
typedef enum DiminutoSerialParityBit {
    DIMINUTO_SERIAL_PARITYBIT_NONE      = 0,
    DIMINUTO_SERIAL_PARITYBIT_ODD       = 1,
    DIMINUTO_SERIAL_PARITYBIT_EVEN      = 2,
    DIMINUTO_SERIAL_PARITYBIT_NOMINAL   = 0,
} diminuto_serial_paritybit_t;

/**
 * These are the stop bits that Diminuto Serial supports. The enumeration
 * isn't used in the function call because the typical use case is that
 * the stop bits are being set by an integer parameter from the command
 * line.
 */
typedef enum DiminutoSerialStopBits {
    DIMINUTO_SERIAL_STOPBITS_1          = 1,
    DIMINUTO_SERIAL_STOPBITS_2          = 2,
    DIMINUTO_SERIAL_STOPBITS_NOMINAL    = 1,
} diminuto_serial_stopbits_t;

/**
 * If the specified file descriptor identifies a device that is a terminal
 * (whatever the operating system thinks that means), places, sets the port
 * according to the specified speed and data format and enables or disables
 * modem control. For example, "115200 8n1" would be (fd, 115200, 8, 0, 1, 0).
 * @param fd is the file descriptor.
 * @param bitspersecond is the desired speed (e.g. 9600, 115200) or -1 to not set speed.
 * @param databits is the number of data bits (e.g. 5, 6, 7, or 8).
 * @param paritybit is zero for no parity, even for even, odd for odd.
 * @param stopbits is the number of stop bits (e.g. 1 or 2).
 * @param modemcontrol if true enables modem control, otherwise disables it.
 * @param xonxoff if true enables software flow control, otherwise disables it.
 * @param rtscts if true enables hardware flow control, otherwise disables it.
 * @return 0 for success or <0 if an error occurred.
 */
extern int diminuto_serial_set(int fd, int bitspersecond, int databits, int paritybit, int stopbits, int modemcontrol, int xonxoff, int rtscts);

/**
 * If the specified file descriptor identifies a device that is a
 * terminal (whatever the operating system thinks that means), places the
 * port into "raw" mode as defined in termios(3) such that input characters
 * are not interpreted by the Linux terminal
 * protocol driver.
 * @param fd is the file descriptor.
 * @return 0 for success or <0 if an error occurred.
 */
extern int diminuto_serial_raw(int fd);

/**
 * If the specified FILE stream identifies a device that is a terminal
 * (whatever the operating system thinks that means), eliminates any
 * buffering of output to the port such that output characters are
 * emitted immediately.
 * @param fp points to the file stream.
 * @return 0 for success or <0 if an error occurred.
 */
extern int diminuto_serial_unbuffered(FILE * fp);

/**
 * If the specified file descriptor identifies a device that is a terminal
 * (whatever the operating system thinks that means), block until the Data
 * Carrier Detect (DCD or sometimes just CD) line is asserted.
 * @param fd is the file descriptor.
 * @return >=0 for DCD asserted, <0 if an error occurred.
 */
extern int diminuto_serial_wait(int fd);

/**
 * If the specified file descriptor identifies a device that is a terminal
 * (whatever the operating system thinks that means), return the number of
 * bytes available to be read.
 * @param fd is the file descriptor.
 * @return >=0 for bytes available to be read, <0 if an error occurred.
 */
extern int diminuto_serial_available(int fd);

#endif
