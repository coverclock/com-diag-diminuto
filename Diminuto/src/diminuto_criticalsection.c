/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2017 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Critical Section feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Critical Section feature.
 */

#include "com/diag/diminuto/diminuto_criticalsection.h"

void diminuto_criticalsection_cleanup(void * voidp)
{
    pthread_mutex_t * mutexp = (pthread_mutex_t *)voidp;
    pthread_mutex_unlock(mutexp);
}
