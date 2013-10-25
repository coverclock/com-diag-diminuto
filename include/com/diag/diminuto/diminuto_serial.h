/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_SERIAL_
#define _H_COM_DIAG_DIMINUTO_SERIAL_

/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include <stdio.h>

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

#endif
