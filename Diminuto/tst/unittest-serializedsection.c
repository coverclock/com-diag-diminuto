/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014-2015 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_serializedsection.h"

int main(void)
{
	diminuto_serializedsection_spinlock_t lock1 = 0;
	diminuto_serializedsection_spinlock_t lock2 = 0;
    ASSERT(lock1 == 0);
    ASSERT(lock2 == 0);
    DIMINUTO_SERIALIZED_SECTION_BEGIN(&lock1);
        ASSERT(lock1 != 0);
        ASSERT(lock2 == 0);
        DIMINUTO_SERIALIZED_SECTION_BEGIN(&lock2);
            ASSERT(lock1 != 0);
            ASSERT(lock2 != 0);
        DIMINUTO_SERIALIZED_SECTION_END;
        ASSERT(lock1 != 0);
        ASSERT(lock2 == 0);
    DIMINUTO_SERIALIZED_SECTION_END;
    ASSERT(lock1 == 0);
    ASSERT(lock2 == 0);
    EXIT();
}
