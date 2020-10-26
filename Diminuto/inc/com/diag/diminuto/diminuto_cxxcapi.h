/* vim: set ts=4 expandtab shiftwidth=4: */
#ifndef _COM_DIAG_DIMINUTO_CXXCAPI_H_
#define _COM_DIAG_DIMINUTO_CXXCAPI_H_

/**
 * @file
 *
 * Copyright 2006-2020 Digital Aggregates Corporation, Colorado, USA.<BR>
 * This file is part of the Digital Aggregates Grandote library.<BR>
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
 * other. See the [Tt]hing* files in the sav directory for an example
 * of how this works.
 *
 * CXXCAPI was originally developed for the Digital Aggregates
 * Desperado project, starting as far back as 2005. A subset of
 * Desperado, including CXXCAPI, eventually became the Digital
 * Aggregates Grandote project.
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
# define CXXCTYPE(_NAMESPACE_, _TYPE_) CXXCTOKEN(_NAMESPACE_)CXXCTOKEN(_TYPE_)
#else
/**
 *  @def CXXCTYPE
 *
 *  This symbol is used with type references in header files included in
 *  both C and C++ translation units to define the type as to whether it
 *  has a namespace prefix. C++ symbols will, C symbols will not. Pronounced
 *  "sexy-type".
 */
# define CXXCTYPE(_NAMESPACE_, _TYPE_) _TYPE_
#endif

#endif
