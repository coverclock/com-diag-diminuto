/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2021 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the array_t feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the types feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_types.h"
#include <stdint.h>

int main() {

    struct Test {
        uint8_t align1;
        diminuto_align2_t align2;
        diminuto_align4_t align4;
        diminuto_align8_t align8;
    };
    struct Test test
#if defined(__GNUC__)
        __attribute__ ((aligned(8)))
#endif
        ;
    struct Test * tp;

    SETLOGMASK();

    {
        TEST();

        ASSERT(sizeof(uint8_t) == 1);
        ASSERT(sizeof(diminuto_align2_t) == 0);
        ASSERT(sizeof(diminuto_align4_t) == 0);
        ASSERT(sizeof(diminuto_align8_t) == 0);

        STATUS();
    }

    {
        TEST();

        ASSERT(sizeof(test) == 8);
        ASSERT(sizeof(test.align1) == 1);
        ASSERT(sizeof(test.align2) == 0);
        ASSERT(sizeof(test.align4) == 0);
        ASSERT(sizeof(test.align8) == 0);

        STATUS();
    }

    {
        TEST();

        ASSERT((((uintptr_t)(&test)) & 0x7) == 0);
        ASSERT((((uintptr_t)(&test.align1)) & 0x7) == 0);
        ASSERT((((uintptr_t)(&test.align2)) & 0x1) == 0);
        ASSERT((((uintptr_t)(&test.align4)) & 0x3) == 0);
        ASSERT((((uintptr_t)(&test.align8)) & 0x7) == 0);

        STATUS();
    }

    {
        TEST();

        ASSERT((tp = malloc(sizeof(struct Test))) != (struct Test *)0);

        STATUS();
    }

    {
        TEST();

        ASSERT(sizeof(tp->align1) == 1);
        ASSERT(sizeof(tp->align2) == 0);
        ASSERT(sizeof(tp->align4) == 0);
        ASSERT(sizeof(tp->align8) == 0);
        ASSERT(sizeof(*tp) == 8);

        STATUS();
    }

    {
        TEST();

        ASSERT((((uintptr_t)(tp)) & 0x7) == 0);
        ASSERT((((uintptr_t)(&(tp->align1))) & 0x7) == 0);
        ASSERT((((uintptr_t)(&(tp->align2))) & 0x1) == 0);
        ASSERT((((uintptr_t)(&(tp->align4))) & 0x3) == 0);
        ASSERT((((uintptr_t)(&(tp->align8))) & 0x7) == 0);

        STATUS();
    }

    {
        TEST();

        free(tp);

        STATUS();
    }

    {
        TEST();

        ASSERT(DIMINUTO_IPV4_SIZE >= sizeof("999.999.999.999"));
        ASSERT(DIMINUTO_IPV6_SIZE >= sizeof("FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF"));
        ASSERT((size_t)DIMINUTO_IP_SIZE >= (size_t)DIMINUTO_IPV4_SIZE);
        ASSERT((size_t)DIMINUTO_IP_SIZE >= (size_t)DIMINUTO_IPV6_SIZE);
        ASSERT(sizeof(diminuto_ipv4_buffer_t) >= DIMINUTO_IPV4_SIZE);
        ASSERT(sizeof(diminuto_ipv6_buffer_t) >= DIMINUTO_IPV6_SIZE);
        ASSERT(sizeof(diminuto_ip_buffer_t) >= DIMINUTO_IP_SIZE);
        ASSERT(sizeof(diminuto_ip_buffer_t) >= sizeof(diminuto_ipv4_buffer_t));
        ASSERT(sizeof(diminuto_ip_buffer_t) >= sizeof(diminuto_ipv6_buffer_t));

        STATUS();
    }

    EXIT();
}
