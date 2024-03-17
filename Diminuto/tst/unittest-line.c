/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of some of the Line feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of some of the Line feature. Most of Line
 * accesses the GPIO device driver so needs hardware to test.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_line.h"
#include "com/diag/diminuto/diminuto_types.h"
#include <alloca.h>
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
        const char * device;
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

        device = diminuto_line_parse(parm("/dev/gpiochip12:0x14"), &line, &inverted);
        ASSERT(device != (char *)0);
        ASSERT(strcmp(device, "/dev/gpiochip12") == 0);
        ASSERT(line == 20);
        ASSERT(!inverted);

        device = diminuto_line_parse(parm("/dev/gpiochip13:025"), &line, &inverted);
        ASSERT(device != (char *)0);
        ASSERT(strcmp(device, "/dev/gpiochip13") == 0);
        ASSERT(line == 21);
        ASSERT(!inverted);

        STATUS();
    }

/*
 * Raspberry Pi 5, Debian "bookworm", Linux 6.1
 *
 * gpstool:
 *
 * gpstool ... -I "/dev/gpiochip4:18" -p "/dev/gpiochip4:16" ...
 *
 * gpioinfo:
 *
 *  :
 *  gpiochip4 - 54 lines:
 *  :
 *  line  16:      "PIN36" "src/diminuto_line.c" output active-high [used]
 *  :
 *  line  18:      "PIN12" "src/diminuto_line.c" input active-high [used]
 *  :
 *
 * unittest-line PIN12 PIN36:
 *
 *  :
 *  name "PIN12" device "/dev/gpiochip4" line 18
 *  name "PIN36" device "/dev/gpiochip4" line 16
 *  :
 *
 * Raspberry Pi 4, Debian "bullseye", Linux 6.1
 *
 * gpioinfo:
 *
 *  :
 *  gpiochip0 - 58 lines:
 *  :
 *  line  16:     "GPIO16"       unused  output  active-high
 *  :
 *  line  18:     "GPIO18"       unused   input  active-high
 *  :
 *
 * unittest-line GPIO16 GPIO18
 *
 *  :
 *  name "GPIO16" device "/dev/gpiochip0" line 16
 *  name "GPIO18" device "/dev/gpiochip0" line 18
 *  :
 */

    {
        const char * device;
        diminuto_line_offset_t line;
        char * buffer;
        int ii;

        TEST();

        buffer = (char *)alloca(sizeof(diminuto_path_t));
        ASSERT(buffer != (char *)0);

        for (ii = 1; ii < argc; ++ii) {
            device = diminuto_line_find(argv[ii], buffer, sizeof(diminuto_path_t), &line);
            if (device != (const char *)0) {
                NOTIFY("name \"%s\" device \"%s\" line %u\n", argv[ii], device, line);
            } else {
                NOTIFY("name \"%s\"\n", argv[ii]);
            }
        }

        STATUS();
    }

    EXIT();
}
