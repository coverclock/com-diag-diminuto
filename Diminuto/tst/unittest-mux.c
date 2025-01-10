/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2025 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Mux feature>
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Mux feature>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_mux.h"
#include <string.h>

int main(int argc, char ** argv)
{
    SETLOGMASK();

    {
        diminuto_mux_t mux0;
        diminuto_mux_t mux1 = DIMINUTO_MUX_INIT;

        TEST();

        memset(&mux0, 0, sizeof(mux0)); 
        ASSERT(sizeof(mux0) == sizeof(mux1));
        ASSERT(memcmp(&mux0, &mux1, sizeof(mux0)) == 0);

        STATUS();

    }

    EXIT();
}
