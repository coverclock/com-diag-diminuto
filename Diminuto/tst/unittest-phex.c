/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Print Hex (PHex) feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Print Hex (Phex) feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <string.h>

int main(int argc, char ** argv)
{
    FILE * fp;
    char command[80] = { '\0' } ;
    int ii;
    int ch;

    SETLOGMASK();

    strncat(command, "phex", sizeof(command) - strlen(command) - 1);

    for (ii = 1; ii < argc; ++ii) {
        strncat(command, " ", sizeof(command) - strlen(command) - 1);
        strncat(command, argv[ii], sizeof(command) - strlen(command) - 1);
     }

    CHECKPOINT("command: \"%s\"\n", command);

    fp = popen(command, "w");
    ASSERT(fp != (FILE *)0);

    for (ch = 0; ch < 256; ++ch) { fputc(ch, fp); }

    fflush(fp);

    ii = pclose(fp);
    ASSERT(ii >= 0);

    EXIT();
}

