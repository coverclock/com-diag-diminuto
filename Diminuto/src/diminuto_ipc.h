/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_IPC_PRIVATE_
#define _H_COM_DIAG_DIMINUTO_IPC_PRIVATE_

/**
 * @file
 * @copyright Copyright 2017 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This defines the IPC private API.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * This defines the IPC private API.
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
