/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the unit test for the Ring feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_ring.h"

int main(void)
{
    SETLOGMASK();

    {
        diminuto_ring_t ring = DIMINUTO_RING_INITIALIZER(7);

        TEST();

        ASSERT(ring.capacity == 7);
        ASSERT(ring.measure == 0);
        ASSERT(ring.producer == 0);
        ASSERT(ring.consumer == 0);

        ASSERT(diminuto_ring_consumable(&ring) == 0);
        ASSERT(diminuto_ring_producable(&ring) == 7);

        STATUS();
    }

    {
        diminuto_ring_t ring;

        TEST();

        ASSERT(diminuto_ring_init(&ring, 11) == &ring);

        ASSERT(ring.capacity == 11);
        ASSERT(ring.measure == 0);
        ASSERT(ring.producer == 0);
        ASSERT(ring.consumer == 0);

        ASSERT(diminuto_ring_consumable(&ring) == 0);
        ASSERT(diminuto_ring_producable(&ring) == 11);

        ASSERT(diminuto_ring_fini(&ring) == (diminuto_ring_t *)0);

        STATUS();
    }

    {
        diminuto_ring_t ring;
        int ii;

        TEST();

        ASSERT(diminuto_ring_init(&ring, 3) == &ring);

        for (ii = 0; ii < 13; ++ii) {

            ASSERT(diminuto_ring_consumable(&ring) == 0);
            ASSERT(diminuto_ring_producable(&ring) == 3);

            ASSERT(diminuto_ring_preview(&ring) < 0);
            ASSERT(diminuto_ring_consume(&ring) < 0);

            ASSERT(diminuto_ring_consumable(&ring) == 0);
            ASSERT(diminuto_ring_producable(&ring) == 3);

            ASSERT(diminuto_ring_produce(&ring) == 0);

            ASSERT(diminuto_ring_consumable(&ring) == 1);
            ASSERT(diminuto_ring_producable(&ring) == 2);

            ASSERT(diminuto_ring_produce(&ring) == 1);

            ASSERT(diminuto_ring_consumable(&ring) == 2);
            ASSERT(diminuto_ring_producable(&ring) == 1);

            ASSERT(diminuto_ring_produce(&ring) == 2);

            ASSERT(diminuto_ring_consumable(&ring) == 3);
            ASSERT(diminuto_ring_producable(&ring) == 0);

            ASSERT(diminuto_ring_produce(&ring) < 0);

            ASSERT(diminuto_ring_consumable(&ring) == 3);
            ASSERT(diminuto_ring_producable(&ring) == 0);

            ASSERT(diminuto_ring_preview(&ring) == 0);
            ASSERT(diminuto_ring_consume(&ring) == 0);

            ASSERT(diminuto_ring_consumable(&ring) == 2);
            ASSERT(diminuto_ring_producable(&ring) == 1);

            ASSERT(diminuto_ring_preview(&ring) == 1);
            ASSERT(diminuto_ring_consume(&ring) == 1);

            ASSERT(diminuto_ring_consumable(&ring) == 1);
            ASSERT(diminuto_ring_producable(&ring) == 2);

            ASSERT(diminuto_ring_preview(&ring) == 2);
            ASSERT(diminuto_ring_consume(&ring) == 2);
        }

        ASSERT(diminuto_ring_fini(&ring) == (diminuto_ring_t *)0);

        STATUS();
    }

    {
        diminuto_ring_t ring;
        int ii;

        TEST();

        ASSERT(diminuto_ring_init(&ring, 6) == &ring);

        for (ii = 0; ii < 17; ++ii) {

            ASSERT(diminuto_ring_consumable(&ring) == 0);
            ASSERT(diminuto_ring_producable(&ring) == 6);

            ASSERT(diminuto_ring_preview(&ring) < 0);
            ASSERT(diminuto_ring_consumes(&ring, 2) < 0);

            ASSERT(diminuto_ring_consumable(&ring) == 0);
            ASSERT(diminuto_ring_producable(&ring) == 6);

            ASSERT(diminuto_ring_produces(&ring, 3) == 0);

            ASSERT(diminuto_ring_consumable(&ring) == 3);
            ASSERT(diminuto_ring_producable(&ring) == 3);

            ASSERT(diminuto_ring_preview(&ring) == 0);
            ASSERT(diminuto_ring_consumes(&ring, 2) == 0);

            ASSERT(diminuto_ring_consumable(&ring) == 1);
            ASSERT(diminuto_ring_producable(&ring) == 5);

            ASSERT(diminuto_ring_produces(&ring, 3) == 3);

            ASSERT(diminuto_ring_consumable(&ring) == 4);
            ASSERT(diminuto_ring_producable(&ring) == 2);

            ASSERT(diminuto_ring_preview(&ring) == 2);
            ASSERT(diminuto_ring_consumes(&ring, 2) == 2);

            ASSERT(diminuto_ring_consumable(&ring) == 2);
            ASSERT(diminuto_ring_producable(&ring) == 4);

            ASSERT(diminuto_ring_preview(&ring) == 4);
            ASSERT(diminuto_ring_consumes(&ring, 2) == 4);

        }

        ASSERT(diminuto_ring_fini(&ring) == (diminuto_ring_t *)0);

        STATUS();
    }

    EXIT();
}
