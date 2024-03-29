/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2015-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Fibonacci feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Fibonacci feature, which generates Fibinacci
 * numbers. The Fibonacci sequences are remarkably useful in backoff-retry
 * algorithms used in error recovery scenarios, and in cases where resources
 * must be dynamically allocated in progressively larger increments.
 */

#include <stdio.h>
#include "com/diag/diminuto/diminuto_fibonacci.h"
#include "com/diag/diminuto/diminuto_unittest.h"

int main(void)
{
    {
        diminuto_fibonacci_state_t state;
        diminuto_fibonacci_state_t * statep;
        diminuto_fibonacci_value_t x;
        diminuto_fibonacci_value_t y;
    
        TEST();
    
        statep = diminuto_fibonacci_init_modern(&state);
        ASSERT(statep == &state);
    
        ASSERT(statep->x0 == 0);
        ASSERT(statep->x1 == 1);
        ASSERT(statep->xm == (diminuto_fibonacci_value_t)~(diminuto_fibonacci_value_t)0);
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 0);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 1);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 1);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 2);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 3);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 5);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 8);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 13);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 21);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 34);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 55);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 89);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 144);
        y = x;
    
        while (!0) {
            x = diminuto_fibonacci_next(statep);
            if (x == y) { break; }
            y = x;
        }
        ASSERT(x == 2971215073UL); /* Number 47 */
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == y);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == y);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == y);
        y = x;
    
        STATUS();
    }

    {
        diminuto_fibonacci_state_t state;
        diminuto_fibonacci_state_t * statep;
        diminuto_fibonacci_value_t x;
        diminuto_fibonacci_value_t y;
    
        TEST();
    
        statep = diminuto_fibonacci_init_advanced(&state);
        ASSERT(statep == &state);
    
        ASSERT(statep->x0 == 1);
        ASSERT(statep->x1 == 2);
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 1);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 2);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 3);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 5);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 8);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 13);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 21);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 34);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 55);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 89);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 144);
        y = x;
    
        while (!0) {
            x = diminuto_fibonacci_next(statep);
            if (x == y) { break; }
            y = x;
        }
        ASSERT(x == 2971215073UL);
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == y);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == y);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == y);
        y = x;
    
        ASSERT(diminuto_fibonacci_fini(statep) == (diminuto_fibonacci_state_t *)0);
    
        STATUS();
    }

    {
        diminuto_fibonacci_state_t state;
        diminuto_fibonacci_state_t * statep;
        diminuto_fibonacci_value_t x;
        diminuto_fibonacci_value_t y;
    
        TEST();
    
        statep = diminuto_fibonacci_init_classic(&state);
        ASSERT(statep == &state);
    
        ASSERT(statep->x0 == 1);
        ASSERT(statep->x1 == 1);
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 1);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 1);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 2);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 3);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 5);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 8);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 13);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 21);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 34);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 55);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 89);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 144);
        y = x;
    
        while (!0) {
            x = diminuto_fibonacci_next(statep);
            if (x == y) { break; }
            y = x;
        }
        ASSERT(x == 2971215073UL);
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == y);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == y);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == y);
        y = x;
    
        ASSERT(diminuto_fibonacci_fini(statep) == (diminuto_fibonacci_state_t *)0);

        STATUS();
    }

    {
        diminuto_fibonacci_state_t state;
        diminuto_fibonacci_state_t * statep;
        diminuto_fibonacci_value_t x;
        diminuto_fibonacci_value_t y;
    
        TEST();
    
        statep = diminuto_fibonacci_init_modern(&state);
        ASSERT(statep == &state);
    
        ASSERT(statep->x0 == 0);
        ASSERT(statep->x1 == 1);
        ASSERT(statep->xm == (diminuto_fibonacci_value_t)~(diminuto_fibonacci_value_t)0);
    
        x = diminuto_fibonacci_set_limit(statep, 100);
        ASSERT(x == (diminuto_fibonacci_value_t)~(diminuto_fibonacci_value_t)0);
    
        ASSERT(statep->x0 == 0);
        ASSERT(statep->x1 == 1);
        ASSERT(statep->xm == 100);
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 0);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 1);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 1);
        y = x;
    
        x = diminuto_fibonacci_next(statep);
        ASSERT(x == 2);
        y = x;
    
        while (!0) {
            x = diminuto_fibonacci_next(statep);
            if (x == y) { break; }
            y = x;
        }
        ASSERT(x == 89);
    
        ASSERT(diminuto_fibonacci_fini(statep) == (diminuto_fibonacci_state_t *)0);
    
        STATUS();
    }

    EXIT();
}
