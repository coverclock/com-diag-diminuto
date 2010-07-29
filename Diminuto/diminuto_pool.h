/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_POOL_
#define _H_COM_DIAG_DIMINUTO_POOL_

/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_types.h"
#include "diminuto_list.h"

typedef diminuto_list diminuto_pool;

extern diminuto_pool * diminuto_pool_init(diminuto_pool * poolp, size_t size);

extern diminuto_pool * diminuto_pool_fini(diminuto_pool * poolp);

extern void * diminuto_pool_alloc(diminuto_pool * poolp);

extern void diminuto_pool_free(void * pointer);

#endif
