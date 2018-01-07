/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014-2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_serializedsection.h"

int main(void)
{
    diminuto_serialized_section_spinlock_t lock1 = 0;
    diminuto_serialized_section_spinlock_t lock2 = 0;
    int zero = 0;

    ASSERT(lock1 == zero);
    ASSERT(lock2 == zero);

    DIMINUTO_SERIALIZED_SECTION_BEGIN(&lock1);

    	int zero = 0;

    	ASSERT(lock1 != zero);
        ASSERT(lock2 == zero);

        DIMINUTO_SERIALIZED_SECTION_BEGIN(&lock2);

        	int zero = 0;

        	ASSERT(lock1 != zero);
            ASSERT(lock2 != zero);

        DIMINUTO_SERIALIZED_SECTION_END;

        ASSERT(lock1 != zero);
        ASSERT(lock2 == zero);

    DIMINUTO_SERIALIZED_SECTION_END;

    ASSERT(lock1 == zero);
    ASSERT(lock2 == zero);

    EXIT();
}
