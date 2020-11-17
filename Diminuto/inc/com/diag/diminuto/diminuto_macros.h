/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_MACROS_
#define _H_COM_DIAG_DIMINUTO_MACROS_

/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implement IF ELSE, EVAL, and APPLY preprocessor macros.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * Implement IF ELSE, EVAL, and APPLY preprocessor macros as described by
 * Jonathan Heathcoat of BBC R&D.
 *
 * To avoid collisions with the application in the C preprocessor name space,
 * macros that are intended to be used by the application have relatively simple
 * names, while those that are used internally have names beginning with
 * COM_DIAG_DIMINUTO_.
 *
 * REFERENCES
 *
 * J. Heathcote, "C Pre-Processor Magic", 2020,
 * <http://jhnet.co.uk/articles/cpp_magic>
 */

/*******************************************************************************
 *
 ******************************************************************************/

#define EMPTY()

#define CAT(_A_, _B_) _A_ ## _B_

#define FIRST(_FIRST_, ...) _FIRST_

#define SECOND(_FIRST_, _SECOND_, ...) _SECOND_

/*******************************************************************************
 *
 ******************************************************************************/

#define COM_DIAG_DIMINUTO_IS_COM_DIAG_DIMINUTO_PROBE(...) SECOND(__VA_ARGS__, 0)

#define COM_DIAG_DIMINUTO_PROBE() ~, 1

#define COM_DIAG_DIMINUTO_NOT_0 COM_DIAG_DIMINUTO_PROBE()

/*******************************************************************************
 *
 ******************************************************************************/

#define NOT(_X_) COM_DIAG_DIMINUTO_IS_COM_DIAG_DIMINUTO_PROBE(CAT(COM_DIAG_DIMINUTO_NOT_, _X_))

#define BOOL(_X_) NOT(NOT(_X_))

/*******************************************************************************
 *
 ******************************************************************************/

#define COM_DIAG_DIMINUTO_IF_1_ELSE(...)
#define COM_DIAG_DIMINUTO_IF_0_ELSE(...) __VA_ARGS__

#define COM_DIAG_DIMINUTO_IF_1(...) __VA_ARGS__ COM_DIAG_DIMINUTO_IF_1_ELSE
#define COM_DIAG_DIMINUTO_IF_0(...)             COM_DIAG_DIMINUTO_IF_0_ELSE

#define COM_DIAG_DIMINUTO_IF_ELSE(_CONDITION_) CAT(COM_DIAG_DIMINUTO_IF_, _CONDITION_)

/*******************************************************************************
 *
 ******************************************************************************/

#define IF_ELSE(_CONDITION_) COM_DIAG_DIMINUTO_IF_ELSE(BOOL(_CONDITION_))

/*******************************************************************************
 *
 ******************************************************************************/

#define FORALL(...) FORALL1024(__VA_ARGS__)
#define FORALL1024(...) FORALL512(FORALL512(__VA_ARGS__))
#define FORALL512(...) FORALL256(FORALL256(__VA_ARGS__))
#define FORALL256(...) FORALL128(FORALL128(__VA_ARGS__))
#define FORALL128(...) FORALL64(FORALL64(__VA_ARGS__))
#define FORALL64(...) FORALL32(FORALL32(__VA_ARGS__))
#define FORALL32(...) FORALL16(FORALL16(__VA_ARGS__))
#define FORALL16(...) FORALL8(FORALL8(__VA_ARGS__))
#define FORALL8(...) FORALL4(FORALL4(__VA_ARGS__))
#define FORALL4(...) FORALL2(FORALL2(__VA_ARGS__))
#define FORALL2(...) FORALL1(FORALL1(__VA_ARGS__))
#define FORALL1(...) __VA_ARGS__

#define DEFER1(_M_) _M_ EMPTY()
#define DEFER2(_M_) _M_ EMPTY EMPTY()()
#define DEFER3(_M_) _M_ EMPTY EMPTY EMPTY()()()
#define DEFER4(_M_) _M_ EMPTY EMPTY EMPTY EMPTY()()()()

#define COM_DIAG_DIMINUTO_END_OF_ARGUMENTS_() 0

#define HAS_ARGS(...) BOOL(FIRST(COM_DIAG_DIMINUTO_END_OF_ARGUMENTS_ __VA_ARGS__)())

#define COM_DIAG_DIMINUTO_APPLY() APPLY

/*******************************************************************************
 *
 ******************************************************************************/

#define APPLY(_M_, _FIRST_, ...) _M_(_FIRST_) IF_ELSE(HAS_ARGS(__VA_ARGS__))(DEFER2(COM_DIAG_DIMINUTO_APPLY)()(_M_, __VA_ARGS__))(/**/)

/*******************************************************************************
 *
 ******************************************************************************/

#endif
