/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_UNICODE_
#define _H_COM_DIAG_DIMINUTO_UNICODE_

/**
 * @file
 * @copyright Copyright 2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Common Unicode characters I like to use.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Hazer <https://github.com/coverclock/com-diag-hazer>
 * @details
 */

#include <wchar.h>

/**
 * @def COM_DIAG_DIMINUTO_UNICODE_DEGREE
 * Define the value that is used for the degree symbol. By default this
 * is the wide character Unicode for the degree symbol, but can be
 * defined at compile time to be something else like '*' to avoid UTF-8.
 * (I'm not convinced this is correct for big endian architectures.)
 */
#define COM_DIAG_DIMINUTO_UNICODE_DEGREE 0xb0U

/**
 * This is the Unicode for the degree symbol.
 */
static const wint_t DIMINUTO_UNICODE_DEGREE = COM_DIAG_DIMINUTO_UNICODE_DEGREE;

/**
 * @def COM_DIAG_DIMINUTO_UNICODE_PLUSMINUS
 * Define the value that is used for the plus minus symbol. By default this
 * is the wide character Unicode for the plus minus symbol, but can be
 * defined at compile time to be something else like '~' to avoid UTF-8.
 * (I'm not convinced this is correct for big endian architectures.)
 */
#define COM_DIAG_DIMINUTO_UNICODE_PLUSMINUS 0xb1U

/**
 * This is the Unicode for the plus minus symbol.
 */
static const wint_t DIMINUTO_UNICODE_PLUSMINUS = COM_DIAG_DIMINUTO_UNICODE_PLUSMINUS;

#endif
