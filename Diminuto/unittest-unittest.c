/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2009 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_unittest.h"
#include "diminuto_coreable.h"

int main(int argc, char ** argv)
{
    EXPECT(1);
    EXPECT(0);
    ASSERT(1);
    ASSERT(0);

    return errors > 255 ? 255 : errors;
}

