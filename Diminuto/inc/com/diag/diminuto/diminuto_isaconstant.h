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
 * literal integer constant or something else.
 *
 * See the associated unit test for lots of examples.
 *
 * At the time of this writing, the default dialect of C implemented
 * by gcc 11.3.0 is the GNU dialect of ISO C17.
 *
 * REFERENCES
 *
 * Fe'lix Cloutier (fay59), "Quirks of C",
 * https://gist.github.com/fay59/5ccbe684e6e56a7df8815c3486568f01
 * (from Martin Uecker via the Linux Kernel mailing list)
 *
 * ISO/IEC 9899, "Information technology - Programming languages - C",
 * 4th ed., ISO/IEC 9899:2018(E), `2018-07
 *
 * "sizeof", ISO/IEC 9899:2018(E), 6.5.3.4, pp. 64-65
 *
 * ":?", ISO/IEC 9899:2018(E), 6.5.15, pp. 71-72, "...if one operand
 * is a null pointer constant, the other has the type of the other
 * operand;"
 *
 * "constant expression", ISO/IEC 9899:2018(E), 6.6, pp. 76-77
 *
 * ((void *)((4) * 0l)) is a null pointer *constant*.
 *
 * ((void *)((foo) * 0l)) is _not_ a null pointer *constant*.
 */

/**
 * @def diminuto_isaconstant
 * Return true of @a _ARGUMENT_ is an integer constant, false otherwise.
 */
#define diminuto_isaconstant(_ARGUMENT_) (sizeof(int) == sizeof(*(1 ? ((void *)((_ARGUMENT_) * 0l)) : (int *)1)))

#ifndef isaconstant
    /**
     * @def isaconstant
     * Return true of @a _ARGUMENT_ is an integer constant, false otherwise.
     */
#   define isaconstant(_ARGUMENT_) diminuto_isaconstant(_ARGUMENT_)
#endif

#endif
