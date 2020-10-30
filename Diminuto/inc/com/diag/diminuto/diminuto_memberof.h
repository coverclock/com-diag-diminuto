/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_MEMBEROF_
#define _H_COM_DIAG_DIMINUTO_MEMBEROF_

/**
 * @file
 * @copyright Copyright 2015 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements a compile-time memberof operator.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Memmber Of feature creates a reference to a field in a structure
 * without actually having an instance of such a structure. Sounds
 * impossible, but useful in some limited contexts.
 */

/**
 * @def diminuto_memberof
 * Create a reference to a member named @a _MEMBER_ of an structure of type @a
 * _STRUCT_. This can be used to take the sizeof() a member of a structure
 * without having a pointer to such a structure.
 */
#define diminuto_memberof(_STRUCT_, _MEMBER_)   (((_STRUCT_ *)0)->_MEMBER_)

#ifndef memberof
#   define memberof(_STRUCT_, _MEMBER_) diminuto_memberof(_STRUCT_, _MEMBER_)
#endif

#endif
