/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA.<BR>
 * Licensed under the terms in LICENSE.txt.<BR>
 *
 * Defines some simple tools to fascilite debugging of Works In Progress (WIP).
 * This file can be included more than once.
 */

#ifndef _H_COM_DIAG_DIMINUTO_DEBUG_
#define _H_COM_DIAG_DIMINUTO_DEBUG_
#include "com/diag/diminuto/diminuto_token.h"
#include <stdio.h>
#endif

#if defined(DPRINTF)
#   undef DPRINTF
#endif

#if defined(COM_DIAG_DIMINUTO_DEBUG)
#   define DPRINTF(...) ((void)fprintf(stderr, "DEBUG "  __FILE__ "@" DIMINUTO_TOKEN_TOKEN(__LINE__) ": " __VA_ARGS__))
#else
#   define DPRINTF(...) ((void)0)
#endif
