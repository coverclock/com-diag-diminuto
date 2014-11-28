/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_TOKEN_
#define _H_COM_DIAG_DIMINUTO_TOKEN_

/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * Pre-processes and then stringifies a preprocessor symbol. This is
 * a common compiler trick trick to get C to turn the value of a preprocessor
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
