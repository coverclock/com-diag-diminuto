/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010-2014 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the String feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the String feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_string.h"
#include <string.h>

int main(int argc, char ** argv)
{
    SETLOGMASK();

    {
        EXPECT(strcmp(diminuto_string_basename("/usr/bin/basename1"), "basename1") == 0);
        EXPECT(strcmp(diminuto_string_basename("bin/basename2"), "basename2") == 0);
        EXPECT(strcmp(diminuto_string_basename("/basename3"), "basename3") == 0);
        EXPECT(strcmp(diminuto_string_basename("basename4"), "basename4") == 0);
        EXPECT(strcmp(diminuto_string_basename(argv[0]), "unittest-string") == 0);
    }

    {
        char dest[] = { 0x01, 0x23, 0x45, 0x67, (char)0x89 };
        char * src = "ABC";
        EXPECT(diminuto_strscpy(dest, src, 0) == dest);
        EXPECT(dest[0] == (char)0x01);
        EXPECT(dest[1] == (char)0x23);
        EXPECT(dest[2] == (char)0x45);
        EXPECT(dest[3] == (char)0x67);
        EXPECT(dest[4] == (char)0x89);
        EXPECT(strcmp(src, "ABC") == 0);
    }

    {
        char dest[] = { 0x01, 0x23, 0x45, 0x67, (char)0x89 };
        char * src = "ABC";
        EXPECT(diminuto_strscpy(dest, src, 1) == dest);
        EXPECT(dest[0] == '\0');
        EXPECT(dest[1] == (char)0x23);
        EXPECT(dest[2] == (char)0x45);
        EXPECT(dest[3] == (char)0x67);
        EXPECT(dest[4] == (char)0x89);
        EXPECT(strcmp(dest, "") == 0);
        EXPECT(strlen(dest) == 0);
        EXPECT(strcmp(src, "ABC") == 0);
    }

    {
        char dest[] = { 0x01, 0x23, 0x45, 0x67, (char)0x89 };
        char * src = "ABC";
        EXPECT(diminuto_strscpy(dest, src, 2) == dest);
        EXPECT(dest[0] == 'A');
        EXPECT(dest[1] == '\0');
        EXPECT(dest[2] == (char)0x45);
        EXPECT(dest[3] == (char)0x67);
        EXPECT(dest[4] == (char)0x89);
        EXPECT(strcmp(dest, "A") == 0);
        EXPECT(strlen(dest) == 1);
        EXPECT(strcmp(src, "ABC") == 0);
    }

    {
        char dest[] = { 0x01, 0x23, 0x45, 0x67, (char)0x89 };
        char * src = "ABC";
        EXPECT(diminuto_strscpy(dest, src, 3) == dest);
        EXPECT(dest[0] == 'A');
        EXPECT(dest[1] == 'B');
        EXPECT(dest[2] == '\0');
        EXPECT(dest[3] == (char)0x67);
        EXPECT(dest[4] == (char)0x89);
        EXPECT(strcmp(dest, "AB") == 0);
        EXPECT(strlen(dest) == 2);
        EXPECT(strcmp(src, "ABC") == 0);
    }

    {
        char dest[] = { 0x01, 0x23, 0x45, 0x67, (char)0x89 };
        char * src = "ABC";
        EXPECT(diminuto_strscpy(dest, src, 4) == dest);
        EXPECT(dest[0] == 'A');
        EXPECT(dest[1] == 'B');
        EXPECT(dest[2] == 'C');
        EXPECT(dest[3] == '\0');
        EXPECT(dest[4] == (char)0x89);
        EXPECT(strcmp(dest, "ABC") == 0);
        EXPECT(strlen(dest) == 3);
        EXPECT(strcmp(src, "ABC") == 0);
    }

    {
        char dest[] = { 0x01, 0x23, 0x45, 0x67, (char)0x89 };
        char * src = "ABC";
        EXPECT(diminuto_strscpy(dest, src, sizeof(dest)) == dest);
        EXPECT(dest[0] == 'A');
        EXPECT(dest[1] == 'B');
        EXPECT(dest[2] == 'C');
        EXPECT(dest[3] == '\0');
        EXPECT(dest[4] == (char)0x89);
        EXPECT(strcmp(dest, "ABC") == 0);
        EXPECT(strlen(dest) == 3);
        EXPECT(strcmp(src, "ABC") == 0);
    }

    EXIT();
}
