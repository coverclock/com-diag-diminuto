/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_FD_
#define _H_COM_DIAG_DIMINUTO_FD_

/**
 * @file
 *
 * Copyright 2010-2017 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
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
 * @return the number of bytes read, 0 if end of file, or <0 if an error occurred (errno will be EINTR for a timer expiray, EAGAIN for non-blocking).
 */
extern ssize_t diminuto_fd_read_generic(int fd, void * buffer, size_t min, size_t max);

/**
 * Read bytes from a file descriptor into a buffer.
 * @param fd is an open file descriptor.
 * @param buffer points to the buffer into which data is read.
 * @param size is the maximum number of bytes to be read.
 * @return the number of bytes read, 0 if end of file, or <0 if an error occurred (errno will be EINTR for a timer expiray, EAGAIN for non-blocking).
 */
static inline ssize_t diminuto_fd_read(int fd, void * buffer, size_t size)
{
    return diminuto_fd_read_generic(fd, buffer, 1, size);
}

/**
 * Write bytes to a file descriptor from a buffer until at least a minimum
 * number of bytes are written and no more than a maximum number of bytes are
 * written. Less than the minimum can still be written if an error or end of
 * file occurs.
 * @param fd is an open file descriptor.
 * @param buffer points to the buffer from which data is written.
 * @param min is the minimum number of bytes to be written.
 * @param max is the maximum number of bytes to be written.
 * @return the number of bytes written, 0 if end of file, or <0 if an error occurred (errno will be EINTR for a timer expiray, EAGAIN for non-blocking).
 */
extern ssize_t diminuto_fd_write_generic(int fd, const void * buffer, size_t min, size_t max);

/**
 * Write bytes to a file descriptor from a buffer.
 * @param fd is an open file descriptor.
 * @param buffer points to the buffer from which data is written.
 * @param size is the maximum number of bytes to be written.
 * @return the number of bytes written, 0 if end of file, or <0 if an error occurred (errno will be EINTR for a timer expiray, EAGAIN for non-blocking).
 */
static inline ssize_t diminuto_fd_write(int fd, const void * buffer, size_t size)
{
    return diminuto_fd_write_generic(fd, buffer, size, size);
}

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
 * number. In this implementation, this is the number of file descriptors
 * supported by the select(2) system call; the actual number of open files
 * returned by sysconf(3) for _SC_OPEN_MAX or getrlimit(2) for RLIMIT_NOFILE
 *  may be significantly larger.
 * @return the maximum possible number of open file descriptors or <0 for error.
 */
extern ssize_t diminuto_fd_count(void);

/**
 * Return the maximum possible number of unique open file descriptor values. The
 * value of any file descriptor will range from zero to one less than this
 * number. In this implementation, this is the number of file descriptors
 * returned by the getrlimit(2) system call for RLIMIT_NOFILE.
 * @return the maximum possible number of open file descriptors or <0 for error.
 */
extern ssize_t diminuto_fd_limit(void);

/**
 * Return the maximum possible number of unique open file descriptor values. The
 * value of any file descriptor will range from zero to one less than this
 * number. In this implementation, this is the number of file descriptors
 * returned by the sysconf(3) function for _SC_OPEN_MAX.
 * @return the maximum possible number of open file descriptors or <0 for error.
 */
extern ssize_t diminuto_fd_maximum(void);

/**
 * This is a table that can be used to map file descriptors to a void pointer
 * by using the fd as an index.
 */
typedef struct DiminutoFdMap {
    size_t count;
    void * data[0];
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
 * This type defines an enumeration that is an abstraction of some bits in the
 * st_mode field returned in the stat structure by the stat(2) and related
 * system calls. Most of them duplicate what the ls(1) command returns as
 * the first character in the mode field. A hex digit indicates that it
 * wasn't a known type, but some of these are for practical purposes
 * impossible because the implied bit pattern matches a known type.
 */
typedef enum DiminutoFdType {
    DIMINUTO_FD_TYPE_NONE           = '\0',
    DIMINUTO_FD_TYPE_UNKNOWN        = '?',  /* Should be impossible. */
    DIMINUTO_FD_TYPE_TTY            = 't',  /* Special case of cdev. */
    DIMINUTO_FD_TYPE_SOCKET         = 's',
    DIMINUTO_FD_TYPE_SYMLINK        = 'l',
    DIMINUTO_FD_TYPE_FILE           = '-',
    DIMINUTO_FD_TYPE_BLOCKDEV       = 'b',
    DIMINUTO_FD_TYPE_DIRECTORY      = 'd',
    DIMINUTO_FD_TYPE_CHARACTERDEV   = 'c',
    DIMINUTO_FD_TYPE_FIFO           = 'p',
    DIMINUTO_FD_TYPE_0              = '0',
    DIMINUTO_FD_TYPE_1              = '1', /* Should be DIMINUTO_FD_TYPE_FIFO. */
    DIMINUTO_FD_TYPE_2              = '2', /* Should be DIMINTUO_FD_TYPE_CHARACTERDEV. */
    DIMINUTO_FD_TYPE_3              = '3',
    DIMINUTO_FD_TYPE_4              = '4', /* Should be DIMINUTO_FD_TYPE_DIRECTORY. */
    DIMINUTO_FD_TYPE_5              = '5',
    DIMINUTO_FD_TYPE_6              = '6', /* Should be DIMINUTO_FD_TYPE_BLOCKDEV. */
    DIMINUTO_FD_TYPE_7              = '7',
    DIMINUTO_FD_TYPE_8              = '8', /* Should be DIMINUTO_FD_TYPE_FILE. */
    DIMINUTO_FD_TYPE_9              = '0',
    DIMINUTO_FD_TYPE_A              = 'A', /* Should be DIMINUTO_FD_TYPE_SYMLINK. */
    DIMINUTO_FD_TYPE_B              = 'B',
    DIMINUTO_FD_TYPE_C              = 'C', /* Should be DIMINUTO_FD_TYPE_SOCKET. */
    DIMINUTO_FD_TYPE_D              = 'D',
    DIMINUTO_FD_TYPE_E              = 'E',
    DIMINUTO_FD_TYPE_F              = 'F',
} diminuto_fd_type_t;

/**
 * Determine an enumeration that indiciates the type of object the
 * specified mode returned by stat(2) or related system call represents.
 * @param mode is the mode value returned by stat(2) or related.
 * @return the type of object the mode represents.
 */
extern diminuto_fd_type_t diminuto_fd_mode2type(mode_t mode);

/**
 * Determine an enumeration that indicates the type of object the specified
 * file descriptor represents. Note that the fstat(2) call under Linux will
 * never return the type of symbolic link; it will always return the type of
 * object to which the symbolic link points.
 * @param fd is an open file descriptor.
 * @return the type of object the file descriptor represents.
 */
extern diminuto_fd_type_t diminuto_fd_type(int fd);

#endif
