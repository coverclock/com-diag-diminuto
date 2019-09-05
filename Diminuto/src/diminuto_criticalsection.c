/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2017 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_criticalsection.h"

void diminuto_criticalsection_cleanup(void * voidp)
{
    pthread_mutex_t * mutexp = (pthread_mutex_t *)voidp;
    pthread_mutex_unlock(mutexp);
}
