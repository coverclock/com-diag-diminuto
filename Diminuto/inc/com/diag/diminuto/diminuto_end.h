/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief When included ends a section of C linkage.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * This header file can included from both C++ and C translation units.
 * If included in a C++ program, ends a C linkage section.
 * If included in a C program, does nothing.
 *
 * This file may be included more than once.
 */

#if defined(__cplusplus)
}
#endif
