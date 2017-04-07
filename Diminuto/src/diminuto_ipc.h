/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_IPC_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_IPC_PRIVATE_

/**
 * @file
 *
 * Copyright 2017 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * Defines the private internal API for the IPC module.
 */

/**
 * Enable or disable a file control option (fcntl(2)) on a socket.
 * @param fd is an open socket file descriptor.
 * @param enable is true to enable, false to disable.
 * @param mask is a bit mask indication the option to enable or disable.
 * @return >=0 for success, <0 for error.
 */
extern int diminuto_ipc_set_status(int fd, int enable, long mask);

/**
 * Set a socket option (setsockopt(2)) on a socket.
 * @param fd is an open socket file descriptor.
 * @param level is the level in the protocol stack at which to set.
 * @param option is the option to set.
 * @param value is the value (if any) to apply to the option.
 * @return >=0 for success, <0 for error.
 */
extern int diminuto_ipc_set_socket(int fd, int level, int option, int value);

/**
 * Get an I/O control option (ioctl(2)) on a socket.
 * @param fd is an open socket file descriptor.
 * @param option is the option to get.
 * @return the value of the option, or <0 for error.
 */
extern int diminuto_ipc_get_control(int fd, int option);

#endif
