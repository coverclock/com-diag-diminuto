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
        diminuto_path_t path;
        diminuto_line_offset_t line;
        int inverted;

        TEST();

        /*
         * I only use /dev/gpiochip0 because I figure if a target has any
         * GPIO controllers at all it will have that one, and parse requires
         * that the file exists.
         */

        device = diminuto_line_parse("/dev/gpiochip0", path, sizeof(path), &line, &inverted);
        ASSERT(device == (char *)0);
        ASSERT(errno == ENODATA);

        device = diminuto_line_parse("/dev/gpiochip0:", path, sizeof(path), &line, &inverted);
        ASSERT(device == (char *)0);
        ASSERT(errno == EINVAL);

        device = diminuto_line_parse("/dev/gpiochip0:invalid", path, sizeof(path), &line, &inverted);
        ASSERT(device == (char *)0);
        ASSERT(errno == EINVAL);

        device = diminuto_line_parse("/dev/gpiochip0:4294967296", path, sizeof(path), &line, &inverted);
        ASSERT(device == (char *)0);
        ASSERT(errno == ERANGE);

        device = diminuto_line_parse("/dev/gpiochip0:-4294967296", path, sizeof(path), &line, &inverted);
        ASSERT(device == (char *)0);
        ASSERT(errno == ERANGE);

        device = diminuto_line_parse("/dev/gpiochip0:-18", path, sizeof(path), &line, &inverted);
        ASSERT(device != (char *)0);
        ASSERT(strcmp(device, "/dev/gpiochip0") == 0);
        ASSERT(line == 18);
        ASSERT(inverted);
        NOTIFY("device \"%s\" line %u\n", device, line);

        device = diminuto_line_parse("/dev/gpiochip0:19", path, sizeof(path), &line, &inverted);
        ASSERT(device != (char *)0);
        ASSERT(strcmp(device, "/dev/gpiochip0") == 0);
        ASSERT(line == 19);
        ASSERT(!inverted);
        NOTIFY("device \"%s\" line %u\n", device, line);

        device = diminuto_line_parse("/dev/gpiochip0:0", path, sizeof(path), &line, &inverted);
        ASSERT(device != (char *)0);
        ASSERT(strcmp(device, "/dev/gpiochip0") == 0);
        ASSERT(line == 0);
        ASSERT(!inverted);
        NOTIFY("device \"%s\" line %u\n", device, line);

        device = diminuto_line_parse("/dev/gpiochip0:-0", path, sizeof(path), &line, &inverted);
        ASSERT(device != (char *)0);
        ASSERT(strcmp(device, "/dev/gpiochip0") == 0);
        ASSERT(line == 0);
        ASSERT(inverted);
        NOTIFY("device \"%s\" line %u\n", device, line);

        device = diminuto_line_parse("/dev/gpiochip0:+0", path, sizeof(path), &line, &inverted);
        ASSERT(device != (char *)0);
        ASSERT(strcmp(device, "/dev/gpiochip0") == 0);
        ASSERT(line == 0);
        ASSERT(!inverted);
        NOTIFY("device \"%s\" line %u\n", device, line);

        device = diminuto_line_parse("/dev/gpiochip0:0x14", path, sizeof(path), &line, &inverted);
        ASSERT(device != (char *)0);
        ASSERT(strcmp(device, "/dev/gpiochip0") == 0);
        ASSERT(line == 20);
        ASSERT(!inverted);
        NOTIFY("device \"%s\" line %u\n", device, line);

        device = diminuto_line_parse("/dev/gpiochip0:-0x15", path, sizeof(path), &line, &inverted);
        ASSERT(device != (char *)0);
        ASSERT(strcmp(device, "/dev/gpiochip0") == 0);
        ASSERT(line == 21);
        ASSERT(inverted);
        NOTIFY("device \"%s\" line %u\n", device, line);

        device = diminuto_line_parse("/dev/gpiochip0:+0x16", path, sizeof(path), &line, &inverted);
        ASSERT(device != (char *)0);
        ASSERT(strcmp(device, "/dev/gpiochip0") == 0);
        ASSERT(line == 22);
        ASSERT(!inverted);
        NOTIFY("device \"%s\" line %u\n", device, line);

        device = diminuto_line_parse("/dev/gpiochip0:027", path, sizeof(path), &line, &inverted);
        ASSERT(device != (char *)0);
        ASSERT(strcmp(device, "/dev/gpiochip0") == 0);
        ASSERT(line == 23);
        ASSERT(!inverted);
        NOTIFY("device \"%s\" line %u\n", device, line);

        device = diminuto_line_parse("/dev/gpiochip0:-030", path, sizeof(path), &line, &inverted);
        ASSERT(device != (char *)0);
        ASSERT(strcmp(device, "/dev/gpiochip0") == 0);
        ASSERT(line == 24);
        ASSERT(inverted);
        NOTIFY("device \"%s\" line %u\n", device, line);

        device = diminuto_line_parse("/dev/gpiochip0:+031", path, sizeof(path), &line, &inverted);
        ASSERT(device != (char *)0);
        ASSERT(strcmp(device, "/dev/gpiochip0") == 0);
        ASSERT(line == 25);
        ASSERT(!inverted);
        NOTIFY("device \"%s\" line %u\n", device, line);

        STATUS();
    }

/*
 * Raspberry Pi 5, Debian "bookworm", Linux 6.1
 * ============================================
 *
 * gpstool ... -I "/dev/gpiochip4:18" -p "/dev/gpiochip4:16" ...
 *
 *  Option -I "/dev/gpiochip4:18"
 *  Option -p "/dev/gpiochip4:16"
 *  Strobe Line (4) "/dev/gpiochip4:16" "/dev/gpiochip4" 16
 *  1pps Line (5) "/dev/gpiochip4:18" "/dev/gpiochip4" 18
 *
 * gpioinfo
 *
 *  gpiochip4 - 54 lines:
 *   line  16:      "PIN36"    "gpstool"  output  active-high [used]
 *   line  18:      "PIN12"    "gpstool"   input  active-high [used]
 *
 * unittest-line PIN12 PIN36 INVALID
 *
 *  name "PIN12" device "/dev/gpiochip4" line 18
 *  name "PIN36" device "/dev/gpiochip4" line 16
 *  name "INVALID"
 *
 * Raspberry Pi 4, Debian "bullseye", Linux 6.1
 * ============================================
 *
 * gpioinfo
 *
 *  gpiochip0 - 58 lines:
 *   line  16:     "GPIO16"       unused  output  active-high
 *   line  18:     "GPIO18"       unused   input  active-high
 *
 * unittest-line GPIO16 GPIO18
 *
 *  name "GPIO16" device "/dev/gpiochip0" line 16
 *  name "GPIO18" device "/dev/gpiochip0" line 18
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
