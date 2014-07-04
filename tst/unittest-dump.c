/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_dump.h"
#include <stdio.h>
#include <stdint.h>

int main(void)
{
    uint8_t data[256];
    int ch;
    const char * address;
    size_t offset;

    for (ch = 0; ch < 256; ++ch) { data[ch] = ch; }

    for (offset = 0; offset < 16; ++offset) {
        diminuto_dump_general(stdout, data + offset, sizeof(data) - (offset * 2), 0, '.', 0, 0, 0);
    }

    diminuto_dump_generic(stdout, data, sizeof(data), 1, '?', 0, 0, 2, 1, 16, "> ", " ", "<", '.', '_', ">\n");

    diminuto_dump_general(stdout, data, sizeof(data), 1, '_', 1, 0, 4);

    diminuto_dump(stdout, data, sizeof(data));

    diminuto_dump_virtual(stdout, data, sizeof(data), 0);

    diminuto_dump_bytes(stdout, data, sizeof(data));

    return 0;
}
