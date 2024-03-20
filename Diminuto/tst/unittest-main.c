/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the main feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the main feature.
 */

#include "com/diag/diminuto/diminuto_main.h"
#include "com/diag/diminuto/diminuto_unittest.h"

int main(int argc, char ** argv)
{
    SETLOGMASK();

    {
    const char * program;

        TEST();

        program = diminuto_main_program_f(argv[0]);
        CHECKPOINT("\"%s\" \"%s\"\n", argv[0], program);
        ASSERT(strcmp(program, "unittest-main") == 0);

        STATUS();
    }

    {
    const char * program;

        TEST();

        program = diminuto_main_program();
        CHECKPOINT("\"%s\" \"%s\"\n", argv[0], program);
        ASSERT(strcmp(program, "unittest-main") == 0);

        STATUS();
    }

    EXIT();
}

