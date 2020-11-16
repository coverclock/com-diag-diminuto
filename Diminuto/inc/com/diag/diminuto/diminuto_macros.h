/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_MACROS_
#define _H_COM_DIAG_DIMINUTO_MACROS_

/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * REFERENCES
 *
 * J. Heathcote, "C Pre-Processor Magic", 2020,
 * <http://jhnet.co.uk/articles/cpp_magic>
 */

#define CAT(_A_, _B_) _A_ ## _B_

#define SECOND(_A_, _B_, ...) _B_
#define IS_PROBE(...) SECOND(__VA_ARGS__, 0)
#define PROBE() ~, 1

#define NOT(_X_) IS_PROBE(CAT(_NOT_, _X_))
#define _NOT_0 PROBE()

#define BOOL(_X_) NOT(NOT(_X_))

#define _IF_1_ELSE(...)
#define _IF_0_ELSE(...) __VA_ARGS__

#define _IF_1(...) __VA_ARGS__ _IF_1_ELSE
#define _IF_0(...)             _IF_0_ELSE

#define _IF_ELSE(_CONDITION_) CAT(_IF_, _CONDITION_)

#define IF_ELSE(_CONDITION_) _IF_ELSE(BOOL(_CONDITION_))

#endif
