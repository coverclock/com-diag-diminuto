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

extern int diminuto_serial_set(int fd, int bitspersecond, int databits, int paritybits, int stopbits, int modemcontrol);

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
 * If the specified file stream identifies a device that is a terminal
 * (whatever the operating system thinks that means), eliminates any
 * buffering of output to the port such that output characters are
 * emitted immediately.
 * @param fp points to the file stream.
 * @return 0 for success or <0 if an error occurred.
 */
extern int diminuto_serial_unbuffered(FILE * fp);

#endif
