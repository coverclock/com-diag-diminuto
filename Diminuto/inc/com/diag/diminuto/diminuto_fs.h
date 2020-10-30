/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_FS_
#define _H_COM_DIAG_DIMINUTO_FS_

/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Provides some simple file system tools.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The FS feature defines some tools to deal with a file system from the
 * application point of view. The most useful function is the recursive
 * file system walker, which can apply a callback function to each file
 * starting at any root in a file system. It also has a remarkably useful
 * function to programmatically duplicate "mkdir -p".
 */

#include "com/diag/diminuto/diminuto_types.h"
#include <sys/types.h>
#include <sys/stat.h>

/**
 * Enable or disable debugging output.
 * @param now is !0 to enable, 0 to disable.
 * @return the prior value of the debug flag.
 */
extern int diminuto_fs_debug(int now);

/**
 * This type defines an enumeration that is an abstraction of some bits in the
 * st_mode field returned in the stat structure by the stat(2) and related
 * system calls. Most of them duplicate what the ls(1) command returns as
 * the first character in the mode field. A hex digit indicates that it
 * wasn't a known type, but some of these are for practical purposes
 * impossible because the implied bit pattern matches a known type.
 */
typedef enum DiminutoFsType {
    DIMINUTO_FS_TYPE_NONE           = '\0',
    DIMINUTO_FS_TYPE_UNKNOWN        = '?',  /* Should be impossible. */
    DIMINUTO_FS_TYPE_TTY            = 't',  /* Special case of cdev. */
    DIMINUTO_FS_TYPE_SOCKET         = 's',
    DIMINUTO_FS_TYPE_SYMLINK        = 'l',
    DIMINUTO_FS_TYPE_FILE           = '-',
    DIMINUTO_FS_TYPE_BLOCKDEV       = 'b',
    DIMINUTO_FS_TYPE_DIRECTORY      = 'd',
    DIMINUTO_FS_TYPE_CHARACTERDEV   = 'c',
    DIMINUTO_FS_TYPE_FIFO           = 'p',
    DIMINUTO_FS_TYPE_0              = '0',
    DIMINUTO_FS_TYPE_1              = '1', /* Should be DIMINUTO_FS_TYPE_FIFO. */
    DIMINUTO_FS_TYPE_2              = '2', /* Should be DIMINTUO_FS_TYPE_CHARACTERDEV. */
    DIMINUTO_FS_TYPE_3              = '3',
    DIMINUTO_FS_TYPE_4              = '4', /* Should be DIMINUTO_FS_TYPE_DIRECTORY. */
    DIMINUTO_FS_TYPE_5              = '5',
    DIMINUTO_FS_TYPE_6              = '6', /* Should be DIMINUTO_FS_TYPE_BLOCKDEV. */
    DIMINUTO_FS_TYPE_7              = '7',
    DIMINUTO_FS_TYPE_8              = '8', /* Should be DIMINUTO_FS_TYPE_FILE. */
    DIMINUTO_FS_TYPE_9              = '0',
    DIMINUTO_FS_TYPE_A              = 'A', /* Should be DIMINUTO_FS_TYPE_SYMLINK. */
    DIMINUTO_FS_TYPE_B              = 'B',
    DIMINUTO_FS_TYPE_C              = 'C', /* Should be DIMINUTO_FS_TYPE_SOCKET. */
    DIMINUTO_FS_TYPE_D              = 'D',
    DIMINUTO_FS_TYPE_E              = 'E',
    DIMINUTO_FS_TYPE_F              = 'F',
} diminuto_fs_type_t;

/**
 * Determine an enumeration that indiciates the type of object the
 * specified mode returned by stat(2) or related system call represents.
 * @param mode is the mode value returned by stat(2) or related.
 * @return the type of object the mode represents.
 */
extern diminuto_fs_type_t diminuto_fs_type(mode_t mode);

/**
 * This type defines the prototype of a callback function invoked
 * by the walker framework for every object in the file system for
 * which the caller has permission to see.
 * @param statep points to a caller-defined state variable.
 * @param name is the latest path component name.
 * @param path is the complete path name.
 * @param depth is the current directory depth.
 * @param statp points to the stat(2) structure for the file.
 * @return 0 to continue, <0 to stop with an error, >0 to stop.
 */
typedef int (diminuto_fs_walker_t)(void * statep, const char * name, const char * path, size_t depth, const struct stat * statp);

/**
 * Recursively walk the file system tree, invoking the caller-defined callback
 * function for each object the caller has permission to see.
 * @param name is the latest path component name.
 * @param path is the complete path name.
 * @param total is the total path length starting at zero.
 * @param depth is the current directory depth starting at zero.
 * @param walkerp points to the callback function.
 * @param statep points to a caller-defined state variable.
 * @return <0 for an error, 0 if the walk was completed, or the callback value.
 */
extern int diminuto_fs_walker(const char * name, char * path, size_t total, size_t depth, diminuto_fs_walker_t * walkerp, void * statep);

/**
 * This is a convenience function for walking the file system tree beginning
 * at the specified root, calling a caller-defined callback function with a
 * caller-defined state variable at each object in the file system the caller
 * has permission to see. The function realpath(3) is applied to the root
 * to resolve softlinks and components like "." and ".." before calling
 * the recursive walker function.
 * @param root is the root at which to start.
 * @param walkerp points to the callback function or NULL.
 * @param statep points to a caller-defined state variable or NULL.
 * @return <0 for an error, 0 if the walk was completed, or the callback value.
 */
extern int diminuto_fs_walk(const char * root, diminuto_fs_walker_t * walkerp, void * statep);

/**
 * Make a directory path, in the manner of "mkdir(1) -p", possibly with many
 * individual nested directories, ignoring existing components. If an error
 * occurs, a partial directory path may be created.
 * @param path is the directory path.
 * @param mode is the mode of each new component.
 * @param all if the last component is also a directory (versus a file).
 * @return 0 for success, <0 with errno set if an error occurred.
 */
extern int diminuto_fs_mkdir_p(const char * path, mode_t mode, int all);

#endif
