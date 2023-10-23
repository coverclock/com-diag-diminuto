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
 * This defines a globally available thread mutex for use for serializing
 * operations involving the shell environment. This is because operations
 * that read the environment may core dump if another thread in the same
 * processes is modifying the environment e.g. via setenv(3). The most
 * general solution would be a reader-writer lock, since concurrent readers,
 * e.g. getenv(3) or operations that do getenv(3) under the hood e.g.
 * getaddrinfo(3), do not conflict. But for now I'm going with a simpler
 * mutex.
 */

#include "com/diag/diminuto/diminuto_criticalsection.h"

/**
 * This is the mutex used to serialize access to the global shell environment.
 */
extern pthread_mutex_t diminuto_environment_mutex;

#endif