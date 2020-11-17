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
 * COM_DIAG_DIMINUTO_. (Some of these macros are exposed with simple names
 * mostly for unit testing.)
 *
 * I really admire Dr. Heathcoat; not only am I pretty sure I would never have
 * figured this out, I'm pretty certain I would never thought to do it in the
 * first place.
 *
 * See the unit test for some examples of how to use these macros.
 *
 * REFERENCES
 *
 * J. Heathcote, "C Pre-Processor Magic", 2020,
 * <http://jhnet.co.uk/articles/cpp_magic>
 *
 * Wikipedia, "X-Macro", <https://en.wikipedia.org/wiki/X_Macro>
 *
 * Wikibooks, "C Programming/Preprocessor directives and macros",
 * <https://en.wikibooks.org/wiki/C_Programming/Preprocessor_directives_and_macros#X-Macros>
 */

/*******************************************************************************
 * PUBLIC
 ******************************************************************************/

#define EMPTY()

#define CONCATENATE(_A_, _B_) _A_ ## _B_

#define FIRST(_FIRST_, ...) _FIRST_

#define SECOND(_FIRST_, _SECOND_, ...) _SECOND_

/*******************************************************************************
 * PRIVATE
 ******************************************************************************/

#define COM_DIAG_DIMINUTO_IS_COM_DIAG_DIMINUTO_PROBE(...) SECOND(__VA_ARGS__, 0)

#define COM_DIAG_DIMINUTO_PROBE() ~, 1

#define COM_DIAG_DIMINUTO_NOT_0 COM_DIAG_DIMINUTO_PROBE()

/*******************************************************************************
 * PUBLIC
 ******************************************************************************/

#define NOT(_X_) COM_DIAG_DIMINUTO_IS_COM_DIAG_DIMINUTO_PROBE(CONCATENATE(COM_DIAG_DIMINUTO_NOT_, _X_))

#define BOOL(_X_) NOT(NOT(_X_))

/*******************************************************************************
 * PRIVATE
 ******************************************************************************/

#define COM_DIAG_DIMINUTO_IF_1_ELSE(...)
#define COM_DIAG_DIMINUTO_IF_0_ELSE(...) __VA_ARGS__

#define COM_DIAG_DIMINUTO_IF_1(...) __VA_ARGS__ COM_DIAG_DIMINUTO_IF_1_ELSE
#define COM_DIAG_DIMINUTO_IF_0(...)             COM_DIAG_DIMINUTO_IF_0_ELSE

#define COM_DIAG_DIMINUTO_IF_ELSE(_CONDITION_) CONCATENATE(COM_DIAG_DIMINUTO_IF_, _CONDITION_)

/*******************************************************************************
 * PUBLIC
 ******************************************************************************/

#define IF_ELSE(_CONDITION_) COM_DIAG_DIMINUTO_IF_ELSE(BOOL(_CONDITION_))

/*******************************************************************************
 * PRIVATE
 ******************************************************************************/

#define COM_DIAG_DIMINUTO_FORALL1024(...) COM_DIAG_DIMINUTO_FORALL512(COM_DIAG_DIMINUTO_FORALL512(__VA_ARGS__))
#define COM_DIAG_DIMINUTO_FORALL512(...) COM_DIAG_DIMINUTO_FORALL256(COM_DIAG_DIMINUTO_FORALL256(__VA_ARGS__))
#define COM_DIAG_DIMINUTO_FORALL256(...) COM_DIAG_DIMINUTO_FORALL128(COM_DIAG_DIMINUTO_FORALL128(__VA_ARGS__))
#define COM_DIAG_DIMINUTO_FORALL128(...) COM_DIAG_DIMINUTO_FORALL64(COM_DIAG_DIMINUTO_FORALL64(__VA_ARGS__))
#define COM_DIAG_DIMINUTO_FORALL64(...) COM_DIAG_DIMINUTO_FORALL32(COM_DIAG_DIMINUTO_FORALL32(__VA_ARGS__))
#define COM_DIAG_DIMINUTO_FORALL32(...) COM_DIAG_DIMINUTO_FORALL16(COM_DIAG_DIMINUTO_FORALL16(__VA_ARGS__))
#define COM_DIAG_DIMINUTO_FORALL16(...) COM_DIAG_DIMINUTO_FORALL8(COM_DIAG_DIMINUTO_FORALL8(__VA_ARGS__))
#define COM_DIAG_DIMINUTO_FORALL8(...) COM_DIAG_DIMINUTO_FORALL4(COM_DIAG_DIMINUTO_FORALL4(__VA_ARGS__))
#define COM_DIAG_DIMINUTO_FORALL4(...) COM_DIAG_DIMINUTO_FORALL2(COM_DIAG_DIMINUTO_FORALL2(__VA_ARGS__))
#define COM_DIAG_DIMINUTO_FORALL2(...) COM_DIAG_DIMINUTO_FORALL1(COM_DIAG_DIMINUTO_FORALL1(__VA_ARGS__))
#define COM_DIAG_DIMINUTO_FORALL1(...) __VA_ARGS__

#define COM_DIAG_DIMINUTO_DEFER1(_M_) _M_ EMPTY()
#define COM_DIAG_DIMINUTO_DEFER2(_M_) _M_ EMPTY EMPTY()()
#define COM_DIAG_DIMINUTO_DEFER3(_M_) _M_ EMPTY EMPTY EMPTY()()()
#define COM_DIAG_DIMINUTO_DEFER4(_M_) _M_ EMPTY EMPTY EMPTY EMPTY()()()()

#define COM_DIAG_DIMINUTO_END_OF_ARGUMENTS_() 0

#define COM_DIAG_DIMINUTO_APPLY() APPLY

/*******************************************************************************
 * PUBLIC
 ******************************************************************************/

#define HAS_ARGUMENTS(...) BOOL(FIRST(COM_DIAG_DIMINUTO_END_OF_ARGUMENTS_ __VA_ARGS__)())

#define FORALL(...) COM_DIAG_DIMINUTO_FORALL1024(__VA_ARGS__)

#define APPLY(_M_, _FIRST_, ...) _M_(_FIRST_) IF_ELSE(HAS_ARGUMENTS(__VA_ARGS__))(COM_DIAG_DIMINUTO_DEFER2(COM_DIAG_DIMINUTO_APPLY)()(_M_, __VA_ARGS__))(/**/)

/*******************************************************************************
 * END
 ******************************************************************************/

#endif
