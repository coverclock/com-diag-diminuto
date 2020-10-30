/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_BUFFER_
#define _H_COM_DIAG_DIMINUTO_BUFFER_

/**
 * @file
 * @copyright Copyright 2015-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Provides an alternative to dynamically allocating memory.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Buffer feature is a prototype-compatible replacement for several
 * functions defined in <stdlib.h> and <string.h> that dynamically allocate
 * and free memory from the heap. Using these functions instead of the standard
 * ones allows memory to be allocated from and freed back to linked lists of
 * fixed sized buffers. This is typically faster[1] than the standard functions,
 * does not fragment the heap[2], and allows buffers to be preallocated to the
 * applications which use this feature. These functions can be used as they are,
 * or they can be proxied to the Diminuto heap and string features so that an
 * application can be converted to using these functions instead of, for
 * example, malloc(3) and strdup(3), at run time. Because the standard functions
 * don't have a mechanism to pass state (like an extra pointer parameter) and
 * because the Diminuto proxying feature doesn't allow the use of, say,
 * preprocessor macros that could be used to hide an extra parameter while
 * exposing the standard argument lists, the buffer pool provided by this
 * feature is a static resource (which in effect is what the heap is too). This
 * means that all applications using this feature share the same buffer pool
 * (just as they share the heap).
 *
 * This feature was inspired by the similar buffer feature in the NCAR libtools
 * package, although it is a completely different, and a much improved,
 * implementation.
 *
 * This feature has its own suite of unit tests. But for an example of using it
 * with the proxy feature, see the unit tests for Assay, a separate library
 * built on top of Diminuto.
 *
 * [1] Benchmarking suggests that glibc heap management is much faster than
 * what existed in the SunOS days in 1989, not at all to my surprise.
 *
 * [2] My experience working with large C++ code bases that routinely used
 * new/delete to dynamically allocate memory in real-time in telecom protocol
 * stacks running on PowerPC and ARM processors using Linux 2.6 and later
 * suggests that heap fragmentation isn't really an issue.
 *
 * Your mileage may vary.
 *
 * REFERENCES
 *
 * J. L. Sloan, "Parcels with TAGS", NCAR TECHNICAL NOTE, NCAR/TN-377+IA,
 * National Center for Atmospheric Research, 1992-10, section 4, "Storage
 * Management", p. 10, http://www.diag.com/ftp/NCAR_Sloan_Parcels.pdf
 *
 * Assay, Digital Aggregate Corp.,
 * http://www.diag.com/navigation/downloads/Assay.html
 */

#include <stddef.h>
#include "com/diag/diminuto/diminuto_buffer_pool.h"

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
 * @param n is the maximum number of characters to copy from the string.
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
 * Preallocate the specified number of buffers of the specified size from the
 * heap and place them in the buffer pool. If the requested size is larger than
 * the pool can accomodate, no buffers are allocated and zero is returned.
 * @param nmemb is the number of buffers to preallocate from the heap.
 * @param size is the size of each buffer in bytes.
 * @return the total number of bytes allocated, including overhead.
 */
extern size_t diminuto_buffer_prealloc(size_t nmemb, size_t size);

/**
 * Call diminuto_buffer_prealloc. Note that nothing is returned, so there
 * is no indication taht the preallocation failed. This API is provided
 * just to conform to the init/fini pattern.
 * @param nmemb is the number of buffers to preallocate from the heap.
 * @param size is the size of each buffer in bytes.
 */
static inline void diminuto_buffer_init(size_t nmemb, size_t size)
{
    (void)diminuto_buffer_prealloc(nmemb, size);
}

/**
 * Enable or disable debug mode.
 * @param debug if true enables debugging otherwise disables debugging.
 * @return the prior debug state.
 */
extern int diminuto_buffer_debug(int debug);

/**
 * Enable or disable the forced failure of allocation of new buffers from the
 * heap using malloc(3). Once this is enabled, no new buffers can be created.
 * Allocations for which no buffers of the suitable size are available will
 * return null. This is used for unit testing, but can also be used to block the
 * dynamic allocation of additional memory after buffers have been preallocated.
 * @param nomalloc is true causes subsequent use of malloc(3) to instead fail.
 * @return the prior fail state.
 */
extern int diminuto_buffer_nomalloc(int nomalloc);

/**
 * Write internal state information about this feature to the debug log.
 * @return the total number of bytes in the pool of unused buffers.
 */
extern size_t diminuto_buffer_log(void);

/*******************************************************************************
 * DEPENDENCY INJECTION
 ******************************************************************************/

/**
 * Establish an external custom buffer pool (if the parameter is non-null),
 * or return the feature to using its own internal buffer pool (if the parameter
 * is null), for all subsequent requests. To avoid exposing the internal buffer
 * header format, the array of linked list heads is an array of void pointers
 * (void *) which the caller must have initialized (typically to null). A
 * shallow copy is made of the contents of the structure. When freed, buffers
 * are always returned to the currently active pool, which is not necessarily
 * the one from which they were allocated; hence wackiness may ensue if the two
 * pools are not the same.
 * @param poolp points to the new pool structure, or null.
 * @return !0 if a custom buffer pool was established, 0 if the internal pool.
 */
extern int diminuto_buffer_set(diminuto_buffer_pool_t * poolp);

#endif
