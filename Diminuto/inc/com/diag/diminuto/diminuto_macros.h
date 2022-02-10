/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_MACROS_
#define _H_COM_DIAG_DIMINUTO_MACROS_

/**
 * @file
 * @copyright Copyright 2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implement IF ELSE, FOR EACH, and APPLY preprocessor macros.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * Implement IF ELSE, FOR EACH, and APPLY preprocessor macros as described by
 * Jonathan Heathcoat of BBC R&D.
 *
 * Important safety tip: if you have a compile error in the code generated
 * by the user-specified macro you pass as the first argument to APPLY,
 * the recursive action of FOREACH will create an explosion of error
 * output from the compiler. The root cause will typically be in the
 * very first line of error output.
 *
 * To avoid collisions with the application in the C preprocessor name
 * space, macros that are intended to be used by the application have
 * relatively simple names, while those that are used internally have
 * names beginning with the prefix DIMINUTO_MACROS_. Some of the macros
 * are exposed with simple names mostly for unit testing. Even the simple
 * names are made longer to prevent collisions.
 *
 * I really admire Dr. Heathcoat; not only am I pretty sure I would never
 * have figured this out, I'm pretty certain I would never have thought
 * is was possible in the first place.
 *
 * See the unit test for some examples of how to use these macros. Crank up
 * the log level to see more output. E.g.
 *
 *     COM_DIAG_DIMINUTO_LOG_MASK=0xff unittest-macros
 *
 * Yeah, I know the name of this feature - "macros" - isn't very imaginative.
 * As soon as I think of a more descriptive name ("mindblown" didn't seem
 * appropriate) I'll change it.
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

#define DIMINUTO_MACROS_EMPTY()

#define DIMINUTO_MACROS_IS_PROBE(...) TAKE_SECOND(__VA_ARGS__, 0)

#define DIMINUTO_MACROS_PROBE() ~, 1

#define DIMINUTO_MACROS_LOGICAL_NOT_0 DIMINUTO_MACROS_PROBE()

/*******************************************************************************
 * PUBLIC
 ******************************************************************************/

/**
 * @def LOGICAL_NOT
 * Generate the logical inverse of the argument @a _X_: 1 for 0, 0 for not 0.
 */
#define LOGICAL_NOT(_X_) DIMINUTO_MACROS_IS_PROBE(CONCATENATE(DIMINUTO_MACROS_LOGICAL_NOT_, _X_))

/**
 * @def MAKE_BOOLEAN
 * Generate the boolean of the argument @a _X_: 0 for 0, 1 for not 0.
 */
#define MAKE_BOOLEAN(_X_) LOGICAL_NOT(LOGICAL_NOT(_X_))

/*******************************************************************************
 * PRIVATE
 ******************************************************************************/

#define DIMINUTO_MACROS_IF_1_ELSE(...)
#define DIMINUTO_MACROS_IF_0_ELSE(...) __VA_ARGS__

#define DIMINUTO_MACROS_IF_1(...) __VA_ARGS__ DIMINUTO_MACROS_IF_1_ELSE
#define DIMINUTO_MACROS_IF_0(...)             DIMINUTO_MACROS_IF_0_ELSE

#define DIMINUTO_MACROS_IF_ELSE(_CONDITION_) CONCATENATE(DIMINUTO_MACROS_IF_, _CONDITION_)

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
#define IF_ELSE(_CONDITION_) DIMINUTO_MACROS_IF_ELSE(MAKE_BOOLEAN(_CONDITION_))

/*******************************************************************************
 * PRIVATE
 ******************************************************************************/

#define DIMINUTO_MACROS_FOREACH1024(...) DIMINUTO_MACROS_FOREACH512(DIMINUTO_MACROS_FOREACH512(__VA_ARGS__))
#define DIMINUTO_MACROS_FOREACH512(...) DIMINUTO_MACROS_FOREACH256(DIMINUTO_MACROS_FOREACH256(__VA_ARGS__))
#define DIMINUTO_MACROS_FOREACH256(...) DIMINUTO_MACROS_FOREACH128(DIMINUTO_MACROS_FOREACH128(__VA_ARGS__))
#define DIMINUTO_MACROS_FOREACH128(...) DIMINUTO_MACROS_FOREACH64(DIMINUTO_MACROS_FOREACH64(__VA_ARGS__))
#define DIMINUTO_MACROS_FOREACH64(...) DIMINUTO_MACROS_FOREACH32(DIMINUTO_MACROS_FOREACH32(__VA_ARGS__))
#define DIMINUTO_MACROS_FOREACH32(...) DIMINUTO_MACROS_FOREACH16(DIMINUTO_MACROS_FOREACH16(__VA_ARGS__))
#define DIMINUTO_MACROS_FOREACH16(...) DIMINUTO_MACROS_FOREACH8(DIMINUTO_MACROS_FOREACH8(__VA_ARGS__))
#define DIMINUTO_MACROS_FOREACH8(...) DIMINUTO_MACROS_FOREACH4(DIMINUTO_MACROS_FOREACH4(__VA_ARGS__))
#define DIMINUTO_MACROS_FOREACH4(...) DIMINUTO_MACROS_FOREACH2(DIMINUTO_MACROS_FOREACH2(__VA_ARGS__))
#define DIMINUTO_MACROS_FOREACH2(...) DIMINUTO_MACROS_FOREACH1(DIMINUTO_MACROS_FOREACH1(__VA_ARGS__))
#define DIMINUTO_MACROS_FOREACH1(...) __VA_ARGS__

#define DIMINUTO_MACROS_DEFER2(_MACRO_) _MACRO_ DIMINUTO_MACROS_EMPTY DIMINUTO_MACROS_EMPTY()()

#define DIMINUTO_MACROS_END() 0

#define DIMINUTO_MACROS_APPLY() APPLY

/*******************************************************************************
 * PUBLIC
 ******************************************************************************/

/**
 * @def HAS_ARGUMENTS
 * Generate 1 if the macro has arguments, 0 otherwise.
 */
#define HAS_ARGUMENTS(...) MAKE_BOOLEAN(TAKE_FIRST(DIMINUTO_MACROS_END __VA_ARGS__)())

/**
 * @def FOREACH
 * Generate code iteratively (actually, recursively) driving the APPLY macro until
 * all arguments are consumed.
 */
#define FOREACH(...) DIMINUTO_MACROS_FOREACH1024(__VA_ARGS__)

/**
 * @def APPLY
 * Generate code when driven by FOREACH by applying the macro in the first
 * argument @a _MACRO_ to the first @a _FIRST_ and every successive
 * argument in the variable length argument list.
 */
#define APPLY(_MACRO_, _FIRST_, ...) _MACRO_(_FIRST_) IF_ELSE(HAS_ARGUMENTS(__VA_ARGS__))(DIMINUTO_MACROS_DEFER2(DIMINUTO_MACROS_APPLY)()(_MACRO_, __VA_ARGS__))()

/*******************************************************************************
 * END
 ******************************************************************************/

#endif
