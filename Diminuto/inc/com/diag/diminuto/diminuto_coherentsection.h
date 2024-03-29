/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_COHERENTSECTION_
#define _H_COM_DIAG_DIMINUTO_COHERENTSECTION_

/**
 * @file
 * @copyright Copyright 2018-2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements macros to acquire and release coherent memory sections.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * EXPERIMENTAL
 * The Coherent Section feature uses the Barrier feature to
 * provide consistent memory acquire/release semantics using
 * bracketing macros.
 */

#include "com/diag/diminuto/diminuto_barrier.h"

/**
 * @def DIMINUTO_COHERENT_SECTION_BEGIN
 * Begin a code section that has a read (acquire) memory barrier at the
 * beginning and a write (release) memory barrier at the end.
 */
#define DIMINUTO_COHERENT_SECTION_BEGIN \
    do { \
        diminuto_barrier_acquire(); \
        do { \
            ((void)0)

/**
 * @def DIMINUTO_COHERENT_SECTION_END
 * End a code section that has a read (acquire) memory barrier at the
 * beginning and a write (release) memory barrier at the end.
 */
#define DIMINUTO_COHERENT_SECTION_END \
        } while (0); \
        diminuto_barrier_release(); \
    } while (0)

#endif
