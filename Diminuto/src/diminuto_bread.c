/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2021 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Buffered Read (BREAD) feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 */

#include <errno.h>
#include <string.h>
#include "com/diag/diminuto/diminuto_log.h"
#include "diminuto_bread.h"

ssize_t diminuto_bread_read(diminuto_bread_t * sp, void * bp, size_t ll)
{
    ssize_t rr = 0;
    size_t cc = 0;
    size_t pp = 0;

    while (ll > 0) {

        /*
         * We know that the caller buffer is greater than zero length, and
         * it has not been filled up.
         */

        while (ll > 0) {
            cc = diminuto_bread_consumeable(sp);
            if (cc == 0) {
                break;
            }
            if (cc > ll) {
                cc = ll;
            }
            memcpy(bp, sp->bread_consumer, cc);
            diminuto_bread_consumed(sp, cc);
            bp += cc;
            ll -= cc;
            rr += cc;
        }

        /*
         * We know either the caller buffer has been filled up,
         * or all the data in the feature buffer has been exhausted,
         * or there was no data in the feature buffer.
         */

        if (rr > 0) {
            /*
             * As long as we returned a non-zero amount of data,
             * we choose not to block the caller on a read (assuming
             * the abstract read functions blocks).
             */
            break;
        }

        /*
         * We know the caller buffer still has no data in it, which
         * must mean the feature buffer was empty.
         */

        pp = diminuto_bread_produceable(sp);
        cc = (*(sp->bread_read))(sp->bread_void, sp->bread_producer, pp);
        if (cc < 0) {
            diminuto_perror("diminuto_bread_read");
            rr = -1;
            break;
        } else if (cc == 0) {
            break;
        } else {
            diminuto_bread_produced(sp, cc);
        }

        /*
         * We know that the abstract read function put some data into
         * the feature buffer, so we can try again.
         */

    }

    return rr;
}

void diminuto_bread_dump(const diminuto_bread_t * sp)
{
    DIMINUTO_LOG_DEBUG("bread@%p: read=%p void=%p begin=%p\n", sp, sp->bread_read, sp->bread_void, sp->bread_begin);
    DIMINUTO_LOG_DEBUG("bread@%p: end=%zd producer=%zd consumer=%zd used=%zu free=%zu\n", sp, sp->bread_end - sp->bread_begin, sp->bread_producer - sp->bread_begin, sp->bread_consumer - sp->bread_begin, sp->bread_used, sp->bread_free);
    DIMINUTO_LOG_DEBUG("bread@%p: produceable=%zu consumeable=%zu\n", sp, diminuto_bread_produceable(sp), diminuto_bread_consumeable(sp));
}
