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

#include <stdio.h>

/**
 * Given a path name and a file mode, create a temporary file with a unique
 * name and with that mode in the same directory, and return the dynamically
 * allocated temporary file name and and a FILE pointer open for read/write.
 * @param path is the path name that the file will have once committed.
 * @param tempp points to where a pointer to the temporary file name is returned.
 * @return an open FILE pointer or NULL if an error occurred.
 */
extern FILE * diminuto_observation_create(const char * path, char ** tempp);

/**
 * Commit the data in the temporary file by closing the open FILE pointer,
 * renaming the temporary file to the original path name, and deallocating the
 * temporary file name.
 * @param fp is the open FILE pointer.
 * @param tempp points to where a pointer to the temporary file name is stored.
 * @return NULL if successful, the original FILE pointer if an error occurred.
 */
extern FILE * diminuto_observation_commit(FILE * fp, char ** tempp);

/**
 * Checkpoint the data in the temporary file by linking the file to a
 * permanent file in the same directory whose name is a date and time
 * stamp with microsecond resolution.
 * @param fp is the open FILE pointer.
 * @param tempp points to where a pointer to the temporary file name is stored.
 * @return the open FILE pointer or NULL if an error occurred.
 */
extern FILE * diminuto_observation_checkpoint(FILE * fp, char ** tempp);

/**
 * Discard the data in the temporary file by closing the open FILE pointer,
 * deleting the temporary file, and deallocating the temporary file name.
 * @param fp is the open FILE pointer.
 * @param tempp points to where a pointer to the temporary file name is stored.
 * @return NULL if successful, the original FILE pointer if an error occurred.
 */
extern FILE * diminuto_observation_discard(FILE * fp, char ** tempp);

#endif
