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

/**
 * If the specified file descriptor identifies a device that is a
 * serial port, places the port into "raw" mode as defined in termios(3).
 * @param fd is the file descriptor.
 * @return 0 for success or <0 if an error occurred.
 */
extern int diminuto_serial_raw(int fd);

#endif

