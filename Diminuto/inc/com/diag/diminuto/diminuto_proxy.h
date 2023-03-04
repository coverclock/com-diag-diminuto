/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_PROXY_
#define _H_COM_DIAG_DIMINUTO_PROXY_

/**
 * @file
 * @copyright Copyright 2013-2015 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Provides a preprocessor framework for dependency injection.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Proxy feature provides a method of replacing what functions an
 * application uses at run-time. It is used by the Heap feature.
 * This is an example of how powerful the preprocessor can be for code
 * generation (but with great power comes great responsibility). These macros
 * implement a method for dependency injection, so that the underlying
 * implementation of a feature may be replaced at run time. Example: memory
 * allocation and free functions.
 */

#include "com/diag/diminuto/diminuto_criticalsection.h"

#define DIMINUTO_PROXY_POINTER_H(_PROXY_, _FUNCTION_, _RESULT_, _PARAMETERS_, _RETURN_, _ARGUMENTS_) \
    typedef _RESULT_ (diminuto_##_PROXY_##_##_FUNCTION_##_func_t) _PARAMETERS_; \
    extern diminuto_##_PROXY_##_##_FUNCTION_##_func_t * diminuto_##_PROXY_##_##_FUNCTION_##_func_p;

#define DIMINUTO_PROXY_POINTER_C(_PROXY_, _FUNCTION_, _RESULT_, _PARAMETERS_, _RETURN_, _ARGUMENTS_) \
    diminuto_##_PROXY_##_##_FUNCTION_##_func_t * diminuto_##_PROXY_##_##_FUNCTION_##_func_p = &_FUNCTION_;

#define DIMINUTO_PROXY_SETTOR_H(_PROXY_, _FUNCTION_, _RESULT_, _PARAMETERS_, _RETURN_, _ARGUMENTS_) \
    extern diminuto_##_PROXY_##_##_FUNCTION_##_func_t * diminuto_##_PROXY_##_##_FUNCTION_##_set(diminuto_##_PROXY_##_##_FUNCTION_##_func_t * now);

#define DIMINUTO_PROXY_SETTOR_C(_PROXY_, _FUNCTION_, _RESULT_, _PARAMETERS_, _RETURN_, _ARGUMENTS_) \
    diminuto_##_PROXY_##_##_FUNCTION_##_func_t * diminuto_##_PROXY_##_##_FUNCTION_##_set(diminuto_##_PROXY_##_##_FUNCTION_##_func_t * now) \
    { \
        static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; \
        diminuto_##_PROXY_##_##_FUNCTION_##_func_t * was; \
        DIMINUTO_CRITICAL_SECTION_BEGIN(&mutex); \
            was = diminuto_##_PROXY_##_##_FUNCTION_##_func_p; \
            diminuto_##_PROXY_##_##_FUNCTION_##_func_p = (now != (diminuto_##_PROXY_##_##_FUNCTION_##_func_t *)0) ? now : &_FUNCTION_; \
        DIMINUTO_CRITICAL_SECTION_END; \
        return was; \
    }

#define DIMINUTO_PROXY_FUNCTION_H(_PROXY_, _FUNCTION_, _RESULT_, _PARAMETERS_, _RETURN_, _ARGUMENTS_) \
    extern _RESULT_ diminuto_##_PROXY_##_##_FUNCTION_ _PARAMETERS_;

#define DIMINUTO_PROXY_FUNCTION_C(_PROXY_, _FUNCTION_, _RESULT_, _PARAMETERS_, _RETURN_, _ARGUMENTS_) \
    _RESULT_ diminuto_##_PROXY_##_##_FUNCTION_ _PARAMETERS_ \
    { \
        _RETURN_ (*diminuto_##_PROXY_##_##_FUNCTION_##_func_p)_ARGUMENTS_; \
    }

#define DIMINUTO_PROXY_FUNCTION_S(_PROXY_, _FUNCTION_, _RESULT_, _PARAMETERS_, _RETURN_, _ARGUMENTS_) \
    static DIMINUTO_PROXY_FUNCTION_C(_PROXY_, _FUNCTION_, _RESULT_, _PARAMETERS_, _RETURN_, _ARGUMENTS_)

#endif
