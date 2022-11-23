/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_ISACONSTANT_
#define _H_COM_DIAG_DIMINUTO_ISACONSTANT_

/**
 * @file
 * @copyright Copyright 2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements a compile-time offsetof operator.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Is A Constant feature determines if its argument is an
 * literal integer constant or something else. This freaks me out
 * a little. I haven't quite figured out yet why this works,
 * but I assume it has to do with C's implicit conversion rules
 * and implicit literal constant types. The core of the expression
 * has to do with a cast to a (void *) pointer (which doesn't compile
 * for floats and doubles, either literals or variables), then asking
 * what the sizeof() the dereferenced pointer is. Remarkably, this
 * works for any size literal integer, including longs and long longs.
 * See the associated unit test for lots of examples.
 *
 * At the time of this writing, the default dialect of C implemented
 * by gcc 11.3.0 is the GNU dialect of ISO C17.
 *
 * REFERENCE
 *
 * Fe'lix Cloutier (fay59), "Quirks of C",
 * https://gist.github.com/fay59/5ccbe684e6e56a7df8815c3486568f01
 * (from Martin Uecker via the Linux Kernel mailing list)
 */

/**
 * @def diminuto_isaconstant
 * Return true of @a _ARGUMENT_ is an integer constant, false otherwise.
 */
#define diminuto_isaconstant(_ARGUMENT_) (sizeof(int) == sizeof(*(1 ? ((void *)((_ARGUMENT_) * 0l)) : (int *)1)))

/*
 * (sizeof(int) == sizeof(*(1 ? ((void *)((4)   * 0l)) : (int*)1)))
 * (sizeof(int) == sizeof(*(1 ? ((void *)((foo) * 0l)) : (int*)1)))
 */

#ifndef isaconstant
    /**
     * @def isaconstant
     * Return true of @a _ARGUMENT_ is an integer constant, false otherwise.
     */
#   define isaconstant(_ARGUMENT_) diminuto_isaconstant(_ARGUMENT_)
#endif

#endif
