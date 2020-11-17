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
 * COM_DIAG_DIMINUTO_MACROS_. (Some of these macros are exposed with simple names
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

/**
 * @def EMPTY
 * Generate no code.
 */
#define EMPTY()

/**
 * @def CONCATENATE
 * Concatenate two arguments @a _A_ and @a _B_ into a single token.
 */
#define CONCATENATE(_A_, _B_) _A_ ## _B_

/**
 * @def TAKE_FIRST
 * Generate the first argument @a _FIRST_ in a variable length argument list.
 */
#define TAKE_FIRST(_FIRST_, ...) _FIRST_

/**
 * @def TAKE_SECOND
 * Ignore the first argument @a _FIRST_ and return the second argument
 * @a _SECOND_ in a variable length argument list.
 */
#define TAKE_SECOND(_FIRST_, _SECOND_, ...) _SECOND_

/*******************************************************************************
 * PRIVATE
 ******************************************************************************/

#define COM_DIAG_DIMINUTO_MACROS_IS_COM_DIAG_DIMINUTO_MACROS_PROBE(...) TAKE_SECOND(__VA_ARGS__, 0)

#define COM_DIAG_DIMINUTO_MACROS_PROBE() ~, 1

#define COM_DIAG_DIMINUTO_MACROS_NOT_0 COM_DIAG_DIMINUTO_MACROS_PROBE()

/*******************************************************************************
 * PUBLIC
 ******************************************************************************/

/**
 * @def NOT
 * Generate the logical inverse of the argument @a _X_: 1 for 0, 0 for not 0.
 */
#define NOT(_X_) COM_DIAG_DIMINUTO_MACROS_IS_COM_DIAG_DIMINUTO_MACROS_PROBE(CONCATENATE(COM_DIAG_DIMINUTO_MACROS_NOT_, _X_))

/**
 * @def BOOLIFY
 * Generate the boolean of the argument @a _X_: 0 for 0, 1 for not 0.
 */
#define BOOLIFY(_X_) NOT(NOT(_X_))

/*******************************************************************************
 * PRIVATE
 ******************************************************************************/

#define COM_DIAG_DIMINUTO_MACROS_IF_1_ELSE(...)
#define COM_DIAG_DIMINUTO_MACROS_IF_0_ELSE(...) __VA_ARGS__

#define COM_DIAG_DIMINUTO_MACROS_IF_1(...) __VA_ARGS__ COM_DIAG_DIMINUTO_MACROS_IF_1_ELSE
#define COM_DIAG_DIMINUTO_MACROS_IF_0(...)             COM_DIAG_DIMINUTO_MACROS_IF_0_ELSE

#define COM_DIAG_DIMINUTO_MACROS_IF_ELSE(_CONDITION_) CONCATENATE(COM_DIAG_DIMINUTO_MACROS_IF_, _CONDITION_)

/*******************************************************************************
 * PUBLIC
 ******************************************************************************/

/**
 * @def IF_ELSE
 * If argument @a _CONDITION_ is true, generate the first following
 * parenthesized expression, otherwise generate the second following
 * parenthesized expression. Note that neither parenthesized expression
 * are actually referenced in the macro.
 */
#define IF_ELSE(_CONDITION_) COM_DIAG_DIMINUTO_MACROS_IF_ELSE(BOOLIFY(_CONDITION_))

/*******************************************************************************
 * PRIVATE
 ******************************************************************************/

#define COM_DIAG_DIMINUTO_MACROS_FORALL1024(...) COM_DIAG_DIMINUTO_MACROS_FORALL512(COM_DIAG_DIMINUTO_MACROS_FORALL512(__VA_ARGS__))
#define COM_DIAG_DIMINUTO_MACROS_FORALL512(...) COM_DIAG_DIMINUTO_MACROS_FORALL256(COM_DIAG_DIMINUTO_MACROS_FORALL256(__VA_ARGS__))
#define COM_DIAG_DIMINUTO_MACROS_FORALL256(...) COM_DIAG_DIMINUTO_MACROS_FORALL128(COM_DIAG_DIMINUTO_MACROS_FORALL128(__VA_ARGS__))
#define COM_DIAG_DIMINUTO_MACROS_FORALL128(...) COM_DIAG_DIMINUTO_MACROS_FORALL64(COM_DIAG_DIMINUTO_MACROS_FORALL64(__VA_ARGS__))
#define COM_DIAG_DIMINUTO_MACROS_FORALL64(...) COM_DIAG_DIMINUTO_MACROS_FORALL32(COM_DIAG_DIMINUTO_MACROS_FORALL32(__VA_ARGS__))
#define COM_DIAG_DIMINUTO_MACROS_FORALL32(...) COM_DIAG_DIMINUTO_MACROS_FORALL16(COM_DIAG_DIMINUTO_MACROS_FORALL16(__VA_ARGS__))
#define COM_DIAG_DIMINUTO_MACROS_FORALL16(...) COM_DIAG_DIMINUTO_MACROS_FORALL8(COM_DIAG_DIMINUTO_MACROS_FORALL8(__VA_ARGS__))
#define COM_DIAG_DIMINUTO_MACROS_FORALL8(...) COM_DIAG_DIMINUTO_MACROS_FORALL4(COM_DIAG_DIMINUTO_MACROS_FORALL4(__VA_ARGS__))
#define COM_DIAG_DIMINUTO_MACROS_FORALL4(...) COM_DIAG_DIMINUTO_MACROS_FORALL2(COM_DIAG_DIMINUTO_MACROS_FORALL2(__VA_ARGS__))
#define COM_DIAG_DIMINUTO_MACROS_FORALL2(...) COM_DIAG_DIMINUTO_MACROS_FORALL1(COM_DIAG_DIMINUTO_MACROS_FORALL1(__VA_ARGS__))
#define COM_DIAG_DIMINUTO_MACROS_FORALL1(...) __VA_ARGS__

#define COM_DIAG_DIMINUTO_MACROS_DEFER(_MACRO_) _MACRO_ EMPTY EMPTY()()

#define COM_DIAG_DIMINUTO_MACROS_END() 0

#define COM_DIAG_DIMINUTO_MACROS_APPLY() APPLY

/*******************************************************************************
 * PUBLIC
 ******************************************************************************/

/**
 * @def HAS_ARGUMENTS
 * Generate 1 if the macro has arguments, 0 otherwise.
 */
#define HAS_ARGUMENTS(...) BOOLIFY(TAKE_FIRST(COM_DIAG_DIMINUTO_MACROS_END __VA_ARGS__)())

/**
 * @def FORALL
 * Generate code iteratively (actually, recursively) driving the APPLY macro until
 * all arguments are consumed.
 */
#define FORALL(...) COM_DIAG_DIMINUTO_MACROS_FORALL1024(__VA_ARGS__)

/**
 * @def APPLY
 * Generate code when driven by FORALL by applying the macro in the first
 * argument @a _MACRO_ to the first @a _FIRST_ and every successive
 * argument in the variable length argument list.
 */
#define APPLY(_MACRO_, _FIRST_, ...) _MACRO_(_FIRST_) IF_ELSE(HAS_ARGUMENTS(__VA_ARGS__))(COM_DIAG_DIMINUTO_MACROS_DEFER(COM_DIAG_DIMINUTO_MACROS_APPLY)()(_MACRO_, __VA_ARGS__))()

/*******************************************************************************
 * END
 ******************************************************************************/

#endif
