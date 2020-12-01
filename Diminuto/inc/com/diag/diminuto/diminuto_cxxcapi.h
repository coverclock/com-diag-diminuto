/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_CXXCAPI_
#define _H_COM_DIAG_DIMINUTO_CXXCAPI_

/**
 * @file
 * @copyright Copyright 2006-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Defines the C++/C-language interoperability API.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * Defines the symbols and macros necessary for the Diminuto
 * C++/C-language Application Programming Interface (CXXCAPI).
 * The CXXCAPI provides tools to make it easier for C and C++
 * applications to interoperate, and for inline functions and
 * macros to be included in either C or C++ translations units.
 * (CXXCAPI is pronounced "sexy API".) You can use CXXCAPI to
 * integrate new C++ code with legacy C code bases, or vice versa.
 * For example, you can use it to integrate Diminuto itself into
 * an existing C++ code base. CXXCAPI allows you to pass pointers
 * to C++ objects into C code in a type safe manner, then pass
 * them back, and to call functions written one language in the
 * other.
 *
 * Note that if you mix C and C++ code, the main program must be
 * C++ to insure that the C++ run time system is included and
 * initialized correctly. This is particularly important for the
 * C++ static constructors being invoked before the main program begins.
 *
 * CXXCAPI was originally developed for the Digital Aggregates
 * Desperado project, starting as far back as 2005. A subset of
 * Desperado, including CXXCAPI, eventually became the Digital
 * Aggregates Grandote project.
 *
 * Because the unit test for CXXCAPI contains both C and C++ translation
 * units, it is in a seperate directory: cxx. The unit test is a tiny
 * example of how to mix C and C++ code, including handling C++ pointers
 * in C. You can run the CXXCAPI unit test from the main makefile
 *
 * make cxxcapi
 *
 * or alternatively by invoking the makefile
 *
 * make -C cxx
 *
 * in the cxx subdirectory.
 *
 * This header file can included from both C++ and C translation units.
 */

#if defined(NULL)
    /*
     * I have recently been disabused of the notion that NULL is now
     * a pre-defined symbol in later versions of the GCC compiler.
     */
#elif defined(__cplusplus)
    /**
     *  @def NULL
     *
     *  NULL is the value of the null pointer. The use of this manifest
     *  constant should be avoided in C++, but may be necessary when
     *  writing header files that may be included in either C or C++
     *  translation units.
     */
#   define NULL (0)
#else
    /**
     *  @def NULL
     *
     *  NULL is the value of the null pointer. The use of this manifest
     *  constant should be avoided in C++, but may be necessary when
     *  writing header files that may be included in either C or C++
     *  translation units.
     */
#   define NULL ((void *)0)
#endif

#if defined(__cplusplus)
    /**
     *  @def CXXCAPI
     *
     *  This symbol is used in both declarations and definitions of
     *  functions in C++ translation units that are callable from C
     *  translation units. Pronounced "sexy-API".
     */
#   define CXXCAPI extern "C"
#else
    /**
     *  @def CXXCAPI
     *
     *  This symbol is used in declarations of functions in C translation
     *  units that are defined in C++ translation units. Pronounced
     *  "sexy-API".
     */
#   define CXXCAPI extern
#endif

#if defined(__cplusplus)
    /*
     *  C++ allows inline keyword.
     */
#elif (defined(__STDC_VERSION__)&&((__STDC_VERSION__) > 199901L))
    /*
     *  ANSI C beyond 1999-01 allows inline keyword.
     */
#elif defined(inline)
    /*
     *  Platform has already defined inline keyword.
     */
#elif (defined(__GNUC__)&&defined(__GNUC_MINOR__)&&((((__GNUC__)*1000)+(__GNUC_MINOR__))>=2007))
    /**
     *  @def inline
     *
     *  This symbol allows applications to include system header
     *  files that define inline functions into a C translation
     *  units being compiled for strict ISO C compliance.
     */
#   define inline __inline__
#else
    /**
     *  @def inline
     *
     * This symbol is defined to be the empty string, causing
     * functions with which it is used in their declaration in
     * the manner of "static inline" to become static local
     * functions.
     */
#   define inline
#endif

#if defined(__cplusplus)
    /**
     *  @def CXXCINLINE
     *
     *  This symbol is used in both declarations and definitions of
     *  standalone functions in header files included in both C
     *  and C++ translation units to suggest that the compiler inline
     *  the function. Pronounced "sexy-inline".
     *
     *  Note that the use of an additional static keyword in this context
     *  has been deprecated by the later ANSI C++ standard but not having
     *  it produces warnings currently for compiles of C translation units.
     */
#   define CXXCINLINE inline
#else
    /**
     *  @def CXXCINLINE
     *
     *  This symbol is used in both declarations and definitions of
     *  standalone functions in header files included in both C
     *  and C++ translation units to suggest that the compiler inline
     *  the function. Pronounced "sexy-inline".
     *
     *  Note that the use of an additional static keyword in this context
     *  has been deprecated by the later ANSI C++ standard but not having
     *  it produces warnings currently for compiles of C translation units.
     */
#   define CXXCINLINE static inline
#endif

/**
 * @def CXXCTOKEN
 *
 * This exists solely to get around an issue with token replacement in the C
 * preprocessor. cpp from GCC 4.4.3 is seriously unhappy with using the
 * preprocessor pasting operator when one of the tokens is a namespace
 * with the double colon syntax.
 */
#define CXXCTOKEN(_TOKEN_) _TOKEN_

#if defined(__cplusplus)
    /**
     *  @def CXXCTYPE
     *
     *  This symbol is used with type references in header files included in
     *  both C and C++ translation units to define the type as to whether it
     *  has a namespace prefix. C++ symbols will, C symbols will not. Pronounced
     *  "sexy-type".
     */
#   define CXXCTYPE(_NAMESPACE_, _TYPE_) CXXCTOKEN(_NAMESPACE_)CXXCTOKEN(_TYPE_)
#else
    /**
     *  @def CXXCTYPE
     *
     *  This symbol is used with type references in header files included in
     *  both C and C++ translation units to define the type as to whether it
     *  has a namespace prefix. C++ symbols will, C symbols will not. Pronounced
     *  "sexy-type".
     */
#   define CXXCTYPE(_NAMESPACE_, _TYPE_) _TYPE_
#endif

#endif
