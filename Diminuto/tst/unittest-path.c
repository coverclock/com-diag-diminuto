/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Path feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Path feature.
 *
 * It is useful to run this under valgrind to insure that it doesn't have
 * any memory leaks.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_path.h"
#include "com/diag/diminuto/diminuto_platform.h"

static const char PATH[] = "/usr/sbin:/usr/bin:/sbin:/bin:/system/bin:/system/xbin";

int main(int argc, char ** argv)
{
    char * result;
    char * alternative;

    /*
     * I tried to pick stuff that an embedded Linux system was likely to have
     * so that this could be run on the target. Platforms like Android and
     * Cygwin put these executables in different places from Ubuntu.
     */

    result = diminuto_path_scan(PATH, "ls");
    ASSERT(result != (char *)0);
    EXPECT((strcmp(result, "/bin/ls") == 0) || (strcmp(result, "/system/bin/ls") == 0) || (strcmp(result, "/usr/bin/ls") == 0));
    free(result);

    result = diminuto_path_find("PATH", "ls");
    ASSERT(result != (char *)0);
    EXPECT((strcmp(result, "/bin/ls") == 0) || (strcmp(result, "/system/bin/ls") == 0) || (strcmp(result, "/usr/bin/ls") == 0));
    free(result);

    result = diminuto_path_find("PATH", "rm");
    ASSERT(result != (char *)0);
    EXPECT((strcmp(result, "/bin/rm") == 0) || (strcmp(result, "/system/bin/rm") == 0) || (strcmp(result, "/usr/bin/rm") == 0));
    free(result);

    result = diminuto_path_find("PATH", "head");
    ASSERT(result != (char *)0);
    EXPECT((strcmp(result, "/usr/bin/head") == 0) || (strcmp(result, "/system/xbin/head") == 0));
    free(result);

    /*
     * This next one has the added benefit that it tests soft links. It has
     * the added deficit that we don't really know what the result should
     * be. Cygwin calls its Shared Objects DLLs (for Dynamic Link Library) to
     * accomodate Windows.
     */

    result = diminuto_path_find("LD_LIBRARY_PATH", "libdiminuto.so");
    alternative = diminuto_path_find("LD_LIBRARY_PATH", "libdiminuto.dll");
    ASSERT((result != (char *)0) || (alternative != (char *)0));
    free(result);
    free(alternative);

    /*
     * And of course some should fail.
     */

    result = diminuto_path_scan(PATH, "COM_DIAG_DIMINUTO_NOTFOUND");
    ASSERT(result == (char *)0);

    result = diminuto_path_scan((const char *)0, "ls");
    ASSERT(result == (char *)0);

    result = diminuto_path_scan(PATH, (const char *)0);
    ASSERT(result == (char *)0);

    result = diminuto_path_scan("/COM_DIAG_DIMINUTO_NOTFOUND/sbin:/COM_DIAG_DIMINUTO_NOTFOUND/bin", "ls");
    ASSERT(result == (char *)0);

    result = diminuto_path_find("PATH", "COM_DIAG_DIMINUTO_NOTFOUND");
    ASSERT(result == (char *)0);

    result = diminuto_path_find("COM_DIAG_DIMINUTO_NOTFOUND", "ls");
    ASSERT(result == (char *)0);

    result = diminuto_path_find((const char *)0, "ls");
    ASSERT(result == (char *)0);

    result = diminuto_path_find("PATH", (const char *)0);
    ASSERT(result == (char *)0);

    result = diminuto_path_find((const char *)0, (const char *)0);
    ASSERT(result == (char *)0);

    EXIT();
}
