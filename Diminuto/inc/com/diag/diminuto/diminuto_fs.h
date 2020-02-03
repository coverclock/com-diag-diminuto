/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_FS_
#define _H_COM_DIAG_DIMINUTO_FS_

/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_types.h"
#include <sys/types.h>
#include <sys/stat.h>

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

typedef int (diminuto_fs_walker_t)(void * statep, const char * name, const char * path, size_t depth, const struct stat * statp);

extern int diminuto_fs_walker(const char * name, char * path, size_t total, size_t depth, diminuto_fs_walker_t * walkerp, void * statep);

extern int diminuto_fs_walk(const char * root, diminuto_fs_walker_t * walkerp, void * statep);

#endif
