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
 */

#include <stddef.h>

/*******************************************************************************
 * <stdlib.h>-LIKE FUNCTIONS
 ******************************************************************************/

extern void * diminuto_buffer_malloc(size_t size);

extern void diminuto_buffer_free(void * ptr);

extern void * diminuto_buffer_realloc(void * ptr, size_t size);

extern void * diminuto_buffer_calloc(size_t nmemb, size_t size);

/*******************************************************************************
 * <string.h>-LIKE FUNCTIONS
 ******************************************************************************/

extern char * diminuto_buffer_strdup(const char *s);

extern char * diminuto_buffer_strndup(const char *s, size_t n);

/*******************************************************************************
 * ANCILLARY FUNCTIONS
 ******************************************************************************/

extern void diminuto_buffer_log(void);

extern void diminuto_buffer_fini(void);

extern unsigned int diminuto_buffer_hash(size_t requested, size_t * actualp);

extern size_t diminuto_buffer_effective(unsigned int index);

#endif
