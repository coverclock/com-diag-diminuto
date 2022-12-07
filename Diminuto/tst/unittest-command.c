/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2021-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Command feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Command feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_command.h"

int main(int argc, char ** argv)
{
    static const char COMMAND[] = "/usr/local/bin/gpstool -D /dev/ttyUSB0 -b 9600 -8 -n -1 -H out/host/tmp/wheatstone.out -f 5 -F 1 -t 10 -O out/host/tmp/wheatstone.pid";
    static const char * ARGV[] = {
        "/usr/local/bin/gpstool",
        "-D",
        "/dev/ttyUSB0",
        "-b",
        "9600",
        "-8",
        "-n",
        "-1",
        "-H",
        "out/host/tmp/wheatstone.out",
        "-f",
        "5",
        "-F",
        "1",
        "-t",
        "10",
        "-O",
        "out/host/tmp/wheatstone.pid",
        (const char *)0,
    };
    char buffer[256];
    ssize_t total;

    diminuto_log_setmask();

    {
        total = diminuto_command_length(countof(ARGV) - 1, ARGV);
        ASSERT(total == (strlen(COMMAND) + 1));
    }

    {
        total = diminuto_command_length(countof(ARGV), ARGV);
        ASSERT(total == (strlen(COMMAND) + 1));
    }

    {
        total = diminuto_command_line(0, (const char **)0, (char *)0, 0);
        ASSERT(total == 0);
    }

    {
        buffer[0] = 0xa5;
        total = diminuto_command_line(0, (const char **)0, buffer, 1);
        ASSERT(total == 1);
        ASSERT(buffer[0] == '\0');
    }

    {
        buffer[0] = 0xa5;
        total = diminuto_command_line(countof(ARGV) - 1, ARGV, buffer, 0);
        ASSERT(total == 0);
        ASSERT(buffer[0] == (char)0xa5);
    }

    {
        buffer[0] = 0xa5;
        total = diminuto_command_line(countof(ARGV) - 1, ARGV, buffer, 1);
        ASSERT(total == 1);
        ASSERT(buffer[0] == '\0');
    }

    {
        total = diminuto_command_line(1, ARGV, buffer, sizeof(buffer));
        ASSERT(total == sizeof("/usr/local/bin/gpstool"));
        ASSERT(strncmp(buffer, "/usr/local/bin/gpstool", sizeof(buffer)) == 0);
    }

    {
        total = diminuto_command_line(2, ARGV, buffer, sizeof(buffer));
        ASSERT(total == sizeof("/usr/local/bin/gpstool -D"));
        ASSERT(strncmp(buffer, "/usr/local/bin/gpstool -D", sizeof(buffer)) == 0);
    }

    {
        total = diminuto_command_line(countof(ARGV), ARGV, buffer, sizeof("/usr/local/bin/gpstool"));
        ASSERT(total == sizeof("/usr/local/bin/gpstool"));
        ASSERT(strncmp(buffer, "/usr/local/bin/gpstool", sizeof(buffer)) == 0);
    }

    {
        total = diminuto_command_line(countof(ARGV), ARGV, buffer, sizeof("/usr/local/bin/gpstool "));
        ASSERT(total == sizeof("/usr/local/bin/gpstool "));
        ASSERT(strncmp(buffer, "/usr/local/bin/gpstool ", sizeof(buffer)) == 0);
    }

    {
        total = diminuto_command_line(countof(ARGV), ARGV, buffer, sizeof("/usr/local/bin/gpstool -D"));
        ASSERT(total == sizeof("/usr/local/bin/gpstool -D"));
        ASSERT(strncmp(buffer, "/usr/local/bin/gpstool -D", sizeof(buffer)) == 0);
    }

    {
        buffer[0] = '\0';
        CHECKPOINT("EXPECT \"%s\"[%zu][%zu]\n", COMMAND, sizeof(COMMAND), strlen(COMMAND));
        total = diminuto_command_line(countof(ARGV), ARGV, buffer, sizeof(buffer));
        CHECKPOINT("ACTUAL \"%s\"[%zu][%zu]\n", buffer, total, strlen(buffer));
        ASSERT(total == sizeof(COMMAND));
        ASSERT(strncmp(buffer, COMMAND, sizeof(buffer)) == 0);
    }

    {
        CHECKPOINT("EXPECT \"%s\"[%zu][%zu]\n", COMMAND, sizeof(COMMAND), strlen(COMMAND));
        total = diminuto_command_line(countof(ARGV) - 1, ARGV, buffer, sizeof(buffer));
        CHECKPOINT("ACTUAL \"%s\"[%zu][%zu]\n", buffer, total, strlen(buffer));
        ASSERT(total == sizeof(COMMAND));
        ASSERT(strncmp(buffer, COMMAND, sizeof(buffer)) == 0);
    }

    EXIT();
}
