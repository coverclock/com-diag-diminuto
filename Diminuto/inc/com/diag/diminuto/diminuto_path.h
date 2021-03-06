/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_PATH_
#define _H_COM_DIAG_DIMINUTO_PATH_

/**
 * @file
 * @copyright Copyright 2013-2014 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Provides some tools for walking the file system.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Path Feature provides some tools to handle finding a file in the
 * file system given a PATH-style string of colon-separated directories.
 */

/**
 * Find a file in colon-separated list of directories in C string.
 * This is what the "which" bash command does with it searches the
 * environmental variable PATH for an executable file. For example, given a
 * keyword of "PATH" and a file name of "ls", this function should return
 * "/bin/ls". The path name that it returns is dynamically allocated and must
 * be freed by the application.
 * @param string is the C string containing the colon-separated list..
 * @param file is the name of the file.
 * @return the dynamically allocated path name of the file, or NULL if not found.
 */
extern char * diminuto_path_scan(const char * string, const char * file);

/**
 * Find a file in colon-separated list of directories in an environmental
 * variable. This is what the "which" bash command does with it searches the
 * environmental variable PATH for an executable file. For example, given a
 * keyword of "PATH" and a file name of "ls", this function should return
 * "/bin/ls". The path name that it returns is dynamically allocated and must
 * be freed by the application.
 * @param keyword is the name of the environmental variable.
 * @param file is the name of the file.
 * @return the dynamically allocated path name of the file, or NULL if not found.
 */
extern char * diminuto_path_find(const char * keyword, const char * file);

#endif
