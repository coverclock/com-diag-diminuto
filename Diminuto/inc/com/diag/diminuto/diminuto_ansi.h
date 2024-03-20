/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_ANSI_
#define _H_COM_DIAG_DIMINUTO_ANSI_

/**
 * @file
 * @copyright Copyright 2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Code generators for ANSI escape sequences I like to use.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Hazer <https://github.com/coverclock/com-diag-diminuto>
 * @details
 */

/**
 * @def DIMINUTO_ANSI_POSITION_CURSOR
 * Position cursor.
 */
#define DIMINUTO_ANSI_POSITION_CURSOR(_ROW_, _COLUMN_) "\033[" #_ROW_ ";" #_COLUMN_ "H"

/**
 * @def DIMINUTO_ANSI_ERASE_SCREEN
 * Erase to end of screen.
 */
#define DIMINUTO_ANSI_ERASE_SCREEN "\033[0J"

/**
 * @def DIMINUTO_ANSI_ERASE_LINE
 * Erase to end of line.
 */
#define DIMINUTO_ANSI_ERASE_LINE "\033[0K"

#endif
