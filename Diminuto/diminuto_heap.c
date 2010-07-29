/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_heap.h"

diminuto_heap_alloc_func * diminuto_heap_alloc_funcp = 0;
diminuto_heap_free_func * diminuto_heap_free_funcp = 0;
