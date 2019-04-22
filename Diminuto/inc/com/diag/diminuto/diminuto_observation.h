/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_OBSERVATION_
#define _H_COM_DIAG_DIMINUTO_OBSERVATION_

/**
 * @file
 *
 * Copyright 2019 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/**
 * Given a path name and a file mode, create a temporary file with a unique
 * name and with that mode in the same directory, and return the dynamically
 * allocated temporary file name and and an open file descriptor.
 * @param path is the path name that the file will have once committed.
 * @param tempp points to where a pointer to the temporary file name is stored.
 * @return an open file descriptor if successful, <0 if an error occurred.
 */
extern int diminuto_observation_create(const char * path, char ** tempp);

/**
 * Commit the data in the temporary file by closing the open file descriptor,
 * renaming the temporary file to the original path name, and deallocating the
 * temporary file name.
 * @param fd is the open file descriptor.
 * @param temp is the temporary file name returned by the create.
 * @return <0 if successful, the original fd if an error occurred.
 */
extern int diminuto_observation_commit(int fd, char * temp);

/**
 * Discard the data in the temporary file by closing the open file descriptor,
 * deleting the temporary file, and deallocating the temporary file name.
 * @param fd is the open file descriptor.
 * @param temp is the temporary file name returned by the create.
 * @return <0 if successful, the original fd if an error occurred.
 */
extern int diminuto_observation_discard(int fd, char * temp);

#endif
