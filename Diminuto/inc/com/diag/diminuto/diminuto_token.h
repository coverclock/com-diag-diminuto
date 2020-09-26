/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_TOKEN_
#define _H_COM_DIAG_DIMINUTO_TOKEN_

/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA.
 * Licensed under the terms in LICENSE.txt.
 *
 * The Token feature pre-processes and then stringifies a preprocessor symbol.
 * This is a common trick trick to get C to turn the value of a preprocessor
 * symbol into a printable C-string at compile time.
 */

/**
 * @def DIMINUTO_TOKEN_STRING
 * Stringify the preprocessor symbol (or really, any argument) @a _ARG_.
 */
#define DIMINUTO_TOKEN_STRING(_ARG_) #_ARG_

/**
 * @def DIMINUTO_TOKEN_TOKEN
 * Stringify and then tokenize the preprocessor symbol (or really, any argument)
 * @a _ARG_.
 */
#define DIMINUTO_TOKEN_TOKEN(_ARG_) DIMINUTO_TOKEN_STRING(_ARG_)

#endif
