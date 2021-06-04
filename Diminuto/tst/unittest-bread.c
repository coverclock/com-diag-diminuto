/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2021 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Buffered Read (BREAD) feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Buffer feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_bread.h"
#include <string.h>
#include <errno.h>
#include "../src/diminuto_bread.h"

int main(int argc, char * argv[])
{
    int debug;

    SETLOGMASK();

    {
        diminuto_bread_t * sp;
        static const size_t SIZE = 64;
        static const size_t LENGTH = 32;

        TEST();

        sp = diminuto_bread_alloc((diminuto_bread_f *)0, (void *)0, SIZE);
        ASSERT(sp != (diminuto_bread_t *)0);
        diminuto_bread_dump(sp);
        EXPECT(diminuto_bread_produceable(sp) == SIZE);
        EXPECT(diminuto_bread_consumeable(sp) == 0);

        diminuto_bread_produced(sp, LENGTH);
        diminuto_bread_dump(sp);
        EXPECT(diminuto_bread_produceable(sp) == LENGTH);
        EXPECT(diminuto_bread_consumeable(sp) == LENGTH);

        diminuto_bread_produced(sp, LENGTH);
        diminuto_bread_dump(sp);
        EXPECT(diminuto_bread_produceable(sp) == 0);
        EXPECT(diminuto_bread_consumeable(sp) == SIZE);

        diminuto_bread_consumed(sp, LENGTH);
        diminuto_bread_dump(sp);
        EXPECT(diminuto_bread_produceable(sp) == LENGTH);
        EXPECT(diminuto_bread_consumeable(sp) == LENGTH);

        diminuto_bread_consumed(sp, LENGTH);
        diminuto_bread_dump(sp);
        EXPECT(diminuto_bread_produceable(sp) == SIZE);
        EXPECT(diminuto_bread_consumeable(sp) == 0);

        diminuto_bread_free(sp);

        STATUS();
    }

    EXIT();
}
