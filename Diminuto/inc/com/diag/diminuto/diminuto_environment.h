/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_ENVIRONMENT_
#define _H_COM_DIAG_DIMINUTO_ENVIRONMENT_

/**
 * @file
 * @copyright Copyright 2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Defines the Environment API.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * Provides a reader-writer lock and an API to serialize environment
 * operations on a first-come first-served basis. This defines the public
 * API.
 */

#include "com/diag/diminuto/diminuto_readerwriter.h"

/**
 * This is the reader/writer lock used to control access to the process
 * environment on a first-come first-served basis.
 */
extern diminuto_readerwriter_t diminuto_environment_readerwriter;

/**
 * @def DIMINUTO_ENVIRONMENT_READER_BEGIN
 * Begin a block of code that reads the environment.
 */
#define DIMINUTO_ENVIRONMENT_READER_BEGIN \
    DIMINUTO_READER_BEGIN(&diminuto_environment_readerwriter)

/**
 * @def DIMINUTO_ENVIRONMENT_READER_END
 * End a block of code that reads the environment.
 */
#define DIMINUTO_ENVIRONMENT_READER_END \
    DIMINUTO_READER_END

/**
 * @def DIMINUTO_ENVIRONMENT_WRITER_BEGIN
 * Start a block of code that changes the environment.
 */
#define DIMINUTO_ENVIRONMENT_WRITER_BEGIN \
    DIMINUTO_WRITER_BEGIN(&diminuto_environment_readerwriter)

/**
 * @def DIMINUTO_ENVIRONMENT_WRITER_END
 * End a block of code that changes the environment.
 */
#define DIMINUTO_ENVIRONMENT_WRITER_END \
    DIMINUTO_WRITER_END

/**
 * Get the value of an environmental variable.
 * @param nae points to the name of the environmental variahle.
 * @return the value of the environmental variable or NULL if none.
 */
extern char * diminuto_environment_get(const char * name);

/**
 * Add or change the value of an environmental varable using a string of
 * the form name=value. The string pointed to by string becomes part of
 * the environment, and changing the strring changes the environment
 * without any access controls.
 * @param string points to the name=value string.
 * @return 0 for success or !0 with errno set if an error occurred.
 */
extern int diminuto_environment_put(char * string);

/**
 * Add or change the vlaue of an environmental variable.
 * @param name points to the name of the environmental variable.
 * @param value points to the new value.
 * @param overwrite if true allows the variable to be changed.
 * @return 0 for success or -1 with errno set if an error occurred.
 */
extern int diminuto_environment_set(const char * name, const char * value, int overwrite);

/**
 * Deletes a variable from the environment.`it is not an error for the variable
 * to not exist in the environment.
 * @param name points to the name of the variable.
 * @return 0 for success or -1 with errno set if an error occurred.
 */
extern int diminuto_environment_unset(const char * name);

/**
 * Get the value of an environmental variable.
 * @param nae points to the name of the environmental variahle.
 * @return the value of the environmental variable or NULL if none.
 */
static inline char * diminuto_getenv(const char * name) {
    return diminuto_environment_get(name);
}

/**
 * Add or change the value of an environmental varable using a string of
 * the form name=value. The string pointed to by string becomes part of
 * the environment, and changing the strring changes the environment
 * without any access controls.
 * @param string points to the name=value string.
 * @return 0 for success or !0 with errno set if an error occurred.
 */
static inline int diminuto_putenv(char * string) {
    return diminuto_environment_put(string);
}

/**
 * Add or change the value of an environmental variable.
 * @param name points to the name of the environmental variable.
 * @param value points to the new value.
 * @param overwrite if true allows the variable to be changed.
 * @return 0 for success or -1 with errno set if an error occurred.
 */
static inline int diminuto_setenv(const char * name, const char * value, int overwrite) {
    return diminuto_environment_set(name, value, overwrite);
}

/**
 * Deletes a variable from the environment.`it is not an error for the variable
 * to not exist in the environment.
 * @param name points to the name of the variable.
 * @return 0 for success or -1 with errno set if an error occurred.
 */
static inline int diminuto_unsetenv(const char * name) {
    return diminuto_environment_unset(name);
}

#if 0
#define getenv   diminuto_getenv
#define putenv   diminuto_putenv
#define setenv   diminuto_setenv
#define unsetenv diminuto_unsetenv
#endif

#endif
