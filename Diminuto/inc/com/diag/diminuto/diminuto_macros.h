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

/*******************************************************************************
 *
 ******************************************************************************/

#define CAT(_A_, _B_) _A_ ## _B_

#define FIRST(_FIRST_, ...) _FIRST_

#define SECOND(_FIRST_, _SECOND_, ...) _SECOND_

#define IS_PROBE(...) SECOND(__VA_ARGS__, 0)

#define PROBE() ~, 1

#define _NOT_0 PROBE()

#define NOT(_X_) IS_PROBE(CAT(_NOT_, _X_))

#define BOOL(_X_) NOT(NOT(_X_))

#define EMPTY()

/*******************************************************************************
 *
 ******************************************************************************/

#define _IF_1_ELSE(...)
#define _IF_0_ELSE(...) __VA_ARGS__

#define _IF_1(...) __VA_ARGS__ _IF_1_ELSE
#define _IF_0(...)             _IF_0_ELSE

#define _IF_ELSE(_CONDITION_) CAT(_IF_, _CONDITION_)

#define IF_ELSE(_CONDITION_) _IF_ELSE(BOOL(_CONDITION_))

/*******************************************************************************
 *
 ******************************************************************************/

#define EVAL(...) EVAL1024(__VA_ARGS__)
#define EVAL1024(...) EVAL512(EVAL512(__VA_ARGS__))
#define EVAL512(...) EVAL256(EVAL256(__VA_ARGS__))
#define EVAL256(...) EVAL128(EVAL128(__VA_ARGS__))
#define EVAL128(...) EVAL64(EVAL64(__VA_ARGS__))
#define EVAL64(...) EVAL32(EVAL32(__VA_ARGS__))
#define EVAL32(...) EVAL16(EVAL16(__VA_ARGS__))
#define EVAL16(...) EVAL8(EVAL8(__VA_ARGS__))
#define EVAL8(...) EVAL4(EVAL4(__VA_ARGS__))
#define EVAL4(...) EVAL2(EVAL2(__VA_ARGS__))
#define EVAL2(...) EVAL1(EVAL1(__VA_ARGS__))
#define EVAL1(...) __VA_ARGS__

#define DEFER1(_M_) _M_ EMPTY()
#define DEFER2(_M_) _M_ EMPTY EMPTY()()
#define DEFER3(_M_) _M_ EMPTY EMPTY EMPTY()()()
#define DEFER4(_M_) _M_ EMPTY EMPTY EMPTY EMPTY()()()()

#define _END_OF_ARGUMENTS_() 0

#define HAS_ARGS(...) BOOL(FIRST(_END_OF_ARGUMENTS_ __VA_ARGS__)())

#define MAP(_M_, _FIRST_, ...) _M_(_FIRST_) IF_ELSE(HAS_ARGS(__VA_ARGS__))(DEFER2(_MAP)()(_M_, __VA_ARGS__))(/**/)

#define _MAP() MAP

/*******************************************************************************
 *
 ******************************************************************************/

#endif
