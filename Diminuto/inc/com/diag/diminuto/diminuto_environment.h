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
 *
 * This defines the Environment feature's API.
 */

#include "com/diag/diminuto/diminuto_criticalsection.h"

/**
 * This is the mutex used to serialize access to the global shell environment.
 */
extern pthread_mutex_t diminuto_environment_mutex;

#endif
