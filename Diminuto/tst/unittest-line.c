/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of some of the Line feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of some of the Line feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_line.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static char * parm(const char * string)
{
    static char buffer[64];
    strcpy(buffer, string);
    return buffer;
}

int main(int argc, char ** argv)
{
    SETLOGMASK();

    {
        TEST();

        ASSERT(sizeof(diminuto_line_bits_t) == sizeof(uint64_t));
        ASSERT(sizeof(diminuto_line_offset_t) == sizeof(uint32_t));

        STATUS();
    }

    {
        const char * string;

        TEST();

        string = diminuto_line_consumer(__FILE__);
        ASSERT(string != (const char *)0);
        string = diminuto_line_consumer(string);
        ASSERT(strcmp(string, __FILE__) == 0);

        STATUS();
    }

    {
        char * device;
        diminuto_line_offset_t line;
        int inverted;

        TEST();

        device = diminuto_line_parse((char *)0, (diminuto_line_offset_t *)0, (int *)0);
        ASSERT(device == (char *)0);
        ASSERT(errno == ENODEV);

        device = diminuto_line_parse(parm("/dev/gpiochip0"), &line, &inverted);
        ASSERT(device == (char *)0);
        ASSERT(errno == ENODATA);

        device = diminuto_line_parse(parm("/dev/gpiochip1:"), &line, &inverted);
        ASSERT(device == (char *)0);
        ASSERT(errno == ENOENT);

        device = diminuto_line_parse(parm("/dev/gpiochip2:invalid"), &line, &inverted);
        ASSERT(device == (char *)0);
        ASSERT(errno == EINVAL);

        device = diminuto_line_parse(parm("/dev/gpiochip3:4294967296"), &line, &inverted);
        ASSERT(device == (char *)0);
        ASSERT(errno == ERANGE);

        device = diminuto_line_parse(parm("/dev/gpiochip4:-4294967296"), &line, &inverted);
        ASSERT(device == (char *)0);
        ASSERT(errno == ERANGE);

        device = diminuto_line_parse(parm("/dev/gpiochip5:-16"), (diminuto_line_offset_t *)0, (int *)0);
        ASSERT(device != (char *)0);
        ASSERT(strcmp(device, "/dev/gpiochip5") == 0);

        device = diminuto_line_parse(parm("/dev/gpiochip6:-17"), &line, (int *)0);
        ASSERT(device != (char *)0);
        ASSERT(strcmp(device, "/dev/gpiochip6") == 0);
        ASSERT(line == 17);

        device = diminuto_line_parse(parm("/dev/gpiochip7:-18"), &line, &inverted);
        ASSERT(device != (char *)0);
        ASSERT(strcmp(device, "/dev/gpiochip7") == 0);
        ASSERT(line == 18);
        ASSERT(inverted);

        device = diminuto_line_parse(parm("/dev/gpiochip8:19"), &line, &inverted);
        ASSERT(device != (char *)0);
        ASSERT(strcmp(device, "/dev/gpiochip8") == 0);
        ASSERT(line == 19);
        ASSERT(!inverted);

        device = diminuto_line_parse(parm("/dev/gpiochip9:0"), &line, &inverted);
        ASSERT(device != (char *)0);
        ASSERT(strcmp(device, "/dev/gpiochip9") == 0);
        ASSERT(line == 0);
        ASSERT(!inverted);

        device = diminuto_line_parse(parm("/dev/gpiochip10:-0"), &line, &inverted);
        ASSERT(device != (char *)0);
        ASSERT(strcmp(device, "/dev/gpiochip10") == 0);
        ASSERT(line == 0);
        ASSERT(inverted);

        device = diminuto_line_parse(parm("/dev/gpiochip11:+0"), &line, &inverted);
        ASSERT(device != (char *)0);
        ASSERT(strcmp(device, "/dev/gpiochip11") == 0);
        ASSERT(line == 0);
        ASSERT(!inverted);

        STATUS();
    }

    EXIT();
}
