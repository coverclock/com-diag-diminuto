/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the GNU GPL V2<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_coreable.h"
#include "diminuto_delay.h"
#include "diminuto_time.h"
#include <stdio.h>
#include <errno.h>

int main(int argc, char ** argv)
{
    uint32_t requested;
    uint64_t elapsed;
    uint32_t claimed;
    uint64_t measured;

    diminuto_coreable();

    printf("%20s %20s %20s\n", "requested", "claimed", "measured");

    for (requested = 0; requested < 10000000; requested += 1000000) {
        elapsed = diminuto_time();
        claimed = requested - diminuto_delay(requested, 0);
        measured = diminuto_time() - elapsed;
        printf("%20lu %20lu %20lu\n", requested, claimed, measured);
    }

    return 0;
}
