/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_BUFFER_
#define _H_COM_DIAG_DIMINUTO_BUFFER_

/**
 * @file
 *
 * Copyright 2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * These functions are designed to be proxied to the Diminuto heap and string
 * features.
 */

#include <stddef.h>

/*******************************************************************************
 * <stdlib.h>-LIKE FUNCTIONS
 ******************************************************************************/

/**
 * Allocate a buffer of at least the specified size.
 * @param size is the required minimum size in bytes.
 * @return a pointer to the buffer or null with errno if error.
 */
extern void * diminuto_buffer_malloc(size_t size);

/**
 * Free a buffer.
 * @param ptr points to the buffer or may be null.
 */
extern void diminuto_buffer_free(void * ptr);

/**
 * Allocate a buffer of at least the specified size and copy as much of the old
 * buffer to the new buffer as can fit. The new size can be larger, smaller, or
 * even the same size as the old buffer. If the pointer to the old buffer is
 * null, this is equivalent to calling diminuto_buffer_malloc(). If the new size
 * is zero, this is equivalent to calling diminuto_buffer_free().
 * @param ptr points to the old buffer.
 * @param size is the required minimum size in bytes.
 * @return a pointer to the new buffer or null if freeing or with errno if error.
 */
extern void * diminuto_buffer_realloc(void * ptr, size_t size);

/**
 * Allocate a buffer at least as big enough to contain the specified number of
 * members each of the specified size. The buffer returned is zeroed out.
 * @param nmemb is the number of members.
 * @param size is the size of each member in bytes.
 * @return a pointer to the zeroed buffer or null with errno if error.
 */
extern void * diminuto_buffer_calloc(size_t nmemb, size_t size);

/*******************************************************************************
 * <string.h>-LIKE FUNCTIONS
 ******************************************************************************/

/**
 * Allocate a buffer at least large enough to contain the specified string
 * including its terminating nul. Copy the string to the buffer including the
 * terminating nul.
 * @param s points to the string.
 * @return a buffer containing a copy of the string.
 */
extern char * diminuto_buffer_strdup(const char * s);

/**
 * Allocate a buffer at least large enough to contain the specified number of
 * characters from the specified string plus a terminating nul. Copy the
 * specified number of characters from the string to the buffer and terminate
 * the new buffer with a nul.
 * @param s points to the string.
 * @return a buffer containing a copy of the string.
 */
extern char * diminuto_buffer_strndup(const char * s, size_t n);

/*******************************************************************************
 * ANCILLARY FUNCTIONS
 ******************************************************************************/

/**
 * Free any dynamically acquired and currently unused buffers. This does not
 * affect any buffers currently in use by the application. It is useful mostly
 * during unit testing with valgrind(1) to insure that dynamically acquired
 * memory has been released.
 */
extern void diminuto_buffer_fini(void);

/**
 * Enable or disable debug mode.
 * @param debug if true enables debugging otherwise disables debugging.
 * @return the prior debug state.
 */
extern int diminuto_buffer_debug(int debug);

/**
 * Write internal state information about this feature to the debug log.
 */
extern void diminuto_buffer_log(void);

#endif
