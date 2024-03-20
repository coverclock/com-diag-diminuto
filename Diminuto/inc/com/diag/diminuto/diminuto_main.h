/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_MAIN_
#define _H_COM_DIAG_DIMINUTO_MAIN_

/**
 * @file
 * @copyright Copyright 2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Typical infrastructure for main program.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Hazer <https://github.com/coverclock/com-diag-diminuto>
 * @details
 */

#include <string.h>

const char * diminuto_main_program_f(const char * argvzero) {
    const char * program = (const char *)0;

    program = ((program = strrchr(argvzero, '/')) == (char *)0) ? argvzero : program + 1;

    return program;
}

#define diminuto_main_program() diminuto_main_program_f(argv[0])

#endif
