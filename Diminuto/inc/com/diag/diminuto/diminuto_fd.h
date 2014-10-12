/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_FD_
#define _H_COM_DIAG_DIMINUTO_FD_

/**
 * @file
 *
 * Copyright 2010-2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_types.h"

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

/**
 * Read bytes from a file descriptor into a buffer until at least a minimum
 * number of bytes are read and no more than a maximum number of bytes are
 * read. Less than the minimum can still be read if an error or end of file
 * occurs.
 * @param fd is an open file descriptor.
 * @param buffer points to the buffer into which data is read.
 * @param min is the minimum number of bytes to be read.
 * @param max is the maximum number of bytes to be read.
 * @return the number of bytes received, 0 if end of file, or <0 if an error occurred (errno will be EINTR for a timer expiray, EAGAIN for non-blocking).
 */
extern ssize_t diminuto_fd_read(int fd, void * buffer, size_t min, size_t max);

/**
 * Write bytes to a file descriptor from a buffer until at least a minimum
 * number of bytes are written and no more than a maximum number of bytes are
 * written. Less than the minimum can still be written if an error or end of
 * file occurs.
 * @param fd is an open file descriptor.
 * @param buffer points to the buffer from which data is written.
 * @param min is the minimum number of bytes to be written.
 * @param max is the maximum number of bytes to be written.
 * @return the number of bytes received, 0 if end of file, or <0 if an error occurred (errno will be EINTR for a timer expiray, EAGAIN for non-blocking).
 */
extern ssize_t diminuto_fd_write(int fd, const void * buffer, size_t min, size_t max);

/**
 * Return the number of bytes buffered for a file descriptor for device drivers
 * which do such buffering. This typically includes serial devices and sockets.
 * @param fd is an open file descriptor.
 * @return the number of bytes buffered or <0 if an error occurred.
 */
extern ssize_t diminuto_fd_readable(int fd);

/**
 * Return the maximum possible number of unique open file descriptor values. The
 * value of any file descriptor will range from zero to one less than this
 * number.
 * @return the maximum possible number of open file descriptors.
 */
extern size_t diminuto_fd_count(void);

/**
 * This is a table that can be used to map file descriptors to a void pointer
 * by using the fd as an index.
 */
typedef struct DiminutoFdMap {
	size_t count;
	void * data[1];
} diminuto_fd_map_t;

/**
 * Allocate and initialize a table that maps file descriptors to a void pointer
 * (as, to a structure or a function). Although any number of file descriptors
 * can be supported, applications will typically pass the maximum possible
 * number of unique open file descriptor values. This table is dynamically
 * allocated and must be freed by the application. The void pointers in the
 * table are initialized to null.
 * @param count is the number of entries in the table.
 * @return a pointer to a dynamically allocated file descriptor mapping table.
 */
extern diminuto_fd_map_t * diminuto_fd_map_alloc(size_t count);

/**
 * Map a file descriptor to position in the mapping table. Note that this does
 * not return the void pointer in the mapping table but a pointer to the void
 * pointer in the mapping table, or null if the file descriptor is invalid.
 * @param mapp points to the mapping table.
 * @param fd is the file descriptor.
 * @return a pointer to a position in the mapping table or null.
 */
extern void ** diminuto_fd_map_ref(diminuto_fd_map_t * mapp, int fd);

/**
 * Allocate a buffer with suitable alignment to be used for direct I/O that
 * bypasses the block cache. The buffer must be deallocated using free(3).
 * @param size is the size in bytes of the buffer to be allocated.
 * @return a pointer to the buffer or null.
 */
extern void * diminuto_fd_direct_alloc(size_t size);

/**
 * Open the named device with the specified flags and mode if the provided
 * file descriptor is not already open. Return either the same open file
 * descriptor or a new file descriptor. The device will be opened for DIRECT
 * I/O (bypassing the buffer cache) and for SYNCed writes (the caller will
 * block until the physical I/O to the underlying media is complete). Buffers
 * written to the file descriptor must be correctly aligned. See
 * diminuto_fd_direct_alloc().
 * @param fd is an open file descriptor or <0 if not already open.
 * @param device points to the name of the device.
 * @param flags is the flags controlling the type of open.
 * @param modes specifies the permissions of a created file.
 * @return an open file descriptor or <0 if an error occurred.
 */
extern int diminuto_fd_direct_acquire(int fd, const char * device, int flags, mode_t mode);

#endif
