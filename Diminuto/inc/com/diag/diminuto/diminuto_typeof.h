/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_TYPEOF_
#define _H_COM_DIAG_DIMINUTO_TYPEOF_

/**
 * @file
 * @copyright Copyright 2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements a typeof compile-time operator.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Type Of feature infers the type of the specified variable. The typeof
 * operator exists in GNU C but in ISO C is an extension called __typeof__.
 */

#if defined(__GNUC__)
    /**
     * @def diminuto_typeof
     * Given a variable @a _VARIABLE_ infer the type in a way that can be
     * used anywhere a typedef can be used.
     */
#   define diminuto_typeof(_VARIABLE_) typeof(_VARIABLE_)
#elif defined(typeof)
    /**
     * @def diminuto_typeof
     * Given a variable @a _VARIABLE_ infer the type in a way that can be
     * used anywhere a typedef can be used.
     */
#   define diminuto_typeof(_VARIABLE_) __typeof__(_VARIABLE_)
#else
    /**
     * @def diminuto_typeof
     * Given a variable @a _VARIABLE_ infer the type in a way that can be
     * used anywhere a typedef can be used.
     */
#   define diminuto_typeof(_VARIABLE_) __typeof__(_VARIABLE_)
    /**
     * @def typeof
     * Given a variable @a _VARIABLE_ infer the type in a way that can be
     * used anywhere a typedef can be used.
     */
#   define typeof(_VARIABLE_) __typeof__(_VARIABLE_)
#endif

#endif
