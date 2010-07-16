/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_FD_
#define _H_COM_DIAG_DIMINUTO_FD_

/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include <sys/types.h>

/**
 * Open the named device with the specified flags and mode if the provided
 * file descriptor is not already open. Return either the same open file
 * descriptor or a new file descriptor.
 * @param fd is an open file descriptor or <0 if not already open.
 * @param device points to the name of the device.
 * @param flags is the flags controlling the type of open.
 * @param modes specifies the permissions of a created file.
 * @return an open file descriptor or <0 if an error occurred.
 */
extern int diminuto_fd_acquire(int fd, const char * device, int flags, mode_t modes);

/**
 * Close a file descriptor if it is open. Return -1 which can be assigned
 * to the file descriptor to indicate it is closed.
 * @param fd is an open file descriptor or <0 if already closed.
 * @param device points to the name of the device or NULL.
 * @return <0 or the open file descriptor if an error occurred.
 */
extern int diminuto_fd_relinquish(int fd, const char * device);

extern ssize_t diminuto_fd_read(int fd, void * buffer, size_t min, size_t max);

extern ssize_t diminuto_fd_write(int fd, const void * buffer, size_t min, size_t max);

#endif
