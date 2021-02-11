/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_OBSERVATION_
#define _H_COM_DIAG_DIMINUTO_OBSERVATION_

/**
 * @file
 * @copyright Copyright 2019-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements an atomic observation mechanism.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Observation feature provides a mechanism for an application to
 * create and use an output file, but not have that output file be
 * visible to other processes or threads until it is completely
 * written out and closed.
 *
 * Unlike most Diminuto features, Observations uses dynamically
 * allocated memory in which to format and store the temporary
 * and checkpoint file names. The feature takes care of automatically
 * freeing the memory when it is no longer needed.
 */

#include <stdio.h>
#include <sys/stat.h>

/**
 * @def COM_DIAG_DIMINUTO_OBSERVATION_MODE
 * This is the default file mode value.
 */
#define COM_DIAG_DIMINUTO_OBSERVATION_MODE (0600)

/**
 * This is the default file mode constant.
 */
static const mode_t DIMINUTO_OBSERVATION_MODE = COM_DIAG_DIMINUTO_OBSERVATION_MODE;

/**
 * Given a path name and a file mode, create a temporary file with a unique
 * name and with that mode in the same directory, and return the dynamically
 * allocated temporary file name and and a FILE pointer open for read/write.
 * @param path is the path name that the file will have once committed.
 * @param tempp points to where a pointer to the temporary file name is stored.
 * @param mode is the file mode to be given to the temporary file.
 * @return an open FILE pointer or NULL if an error occurred.
 */
extern FILE * diminuto_observation_create_generic(const char * path, char ** tempp, mode_t mode);

/**
 * Given a path name, create a temporary file with a unique name and with that
 * mode in the same directory, and return the dynamically allocated temporary
 * file name and and a FILE pointer open for read/write. The file will have
 * the mode 600 (the POSIX standard for mkstemp(3)).
 * @param path is the path name that the file will have once committed.
 * @param tempp points to where a pointer to the temporary file name is stored.
 * @return an open FILE pointer or NULL if an error occurred.
 */
static inline FILE * diminuto_observation_create(const char * path, char ** tempp) {
    return diminuto_observation_create_generic(path, tempp, DIMINUTO_OBSERVATION_MODE);
}

/**
 * Commit the data in the temporary file by closing the open FILE pointer,
 * renaming the temporary file to the original path name, and freeing the
 * memory dynamically allocated for the temporary file name.
 * @param fp is the open FILE pointer.
 * @param tempp points to where a pointer to the temporary file name is stored.
 * @return NULL if successful, the original FILE pointer if an error occurred.
 */
extern FILE * diminuto_observation_commit(FILE * fp, char ** tempp);

/**
 * Checkpoint the data in the temporary file by linking the file to a
 * permanent file in the same directory whose name is a date and time
 * stamp with microsecond resolution. This function uses memory
 * dynamically allocated and freed in which to create the permanent
 * file name.
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
