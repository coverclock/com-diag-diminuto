/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Dump feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Dump feature.
 */

#include "com/diag/diminuto/diminuto_dump.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_types.h"
#include <stdio.h>

int main(void)
{
    uint8_t data[256];
    int ch;
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

    diminuto_dump(diminuto_log_stream(), data, sizeof(data));

    return 0;
}
