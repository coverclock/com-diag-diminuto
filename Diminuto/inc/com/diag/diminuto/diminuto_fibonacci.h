/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_FIBONACCI_
#define _H_COM_DIAG_DIMINUTO_FIBONACCI_

/**
 * @file
 * @copyright Copyright 2015-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements a Fibonacci sequence generator.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Fibonacci feature implements a Fibonacci-sequence generator. This
 * has turned out to be useful, in the author's experience, for a variety
 * of resource allocation strategies.
 *
 * Generate the Fibonacci sequence: x[i+2] = x[i+1] + x[i+0]. The modern
 * sequence, in which x[0] = 0 and x[1] = 1, starts at 0, where as the classic
 * Fibonacci sequence, in which x[0] = 1 and x[1] = 1, starts at 1. The
 * advanced Fibonacci sequence, in which x[0] = 1 and x[1] = 2, is frequently
 * more useful in resource allocation scenarios. The generator can be
 * initialized for any sequence.
 *
 * The Fibonacci sequence is remarkably useful as a way to scale successive time
 * delays during error recovery, or buffer allocations after memory exhaustion,
 * and the like. It grows slowly at first, and then more quickly, but not so
 * quickly that the values get crazy big too soon.
 *
 * REFERENCES
 *
 * Wikipedia, "Fibonacci Number", http://en.wikipedia.org/wiki/Fibonacci_number
 *
 * Chip Overclock, "Fibonacci Scaling",
 * http://coverclock.blogspot.com/2011/12/fibonacci-scaling_09.html
 */

#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_swap.h"
#include "com/diag/diminuto/diminuto_minmaxof.h"

/**
 * Each Fibonacci number is of this integer type. This works for a lot of
 * applications, but if it doesn't work for you, feel free to try a narrower
 * (uint16_t) or wider (uint64_t) integer type.
 */
typedef uint32_t diminuto_fibonacci_value_t;

/**
 * The Fibonacci generator has to maintain the last two values as its state.
 * This structure has to be initialized before use. There are functions provided
 * for this purpose.
 */
typedef struct DiminutoFibonacci {
    diminuto_fibonacci_value_t  x1; /**< Previous value. */
    diminuto_fibonacci_value_t  x0; /**< Next value. */
    diminuto_fibonacci_value_t  xm; /**< Maximum value. */
} diminuto_fibonacci_state_t;

/**
 * Initialize the Fibonacci generator state.
 * @param statep points to the generator state.
 * @param x0 is the initial value assigned to x[2].
 * @param x1 is the initial value assigned to x[1].
 * @param xm is the initial value assigned to xm, the maximum value of x.
 * @return a pointer to the generator state.
 */
static inline diminuto_fibonacci_state_t * diminuto_fibonacci_init(diminuto_fibonacci_state_t * statep, diminuto_fibonacci_value_t x0, diminuto_fibonacci_value_t x1, diminuto_fibonacci_value_t xm)
{
    statep->x0 = x0;
    statep->x1 = x1;
    statep->xm = xm;
    return statep;
}

/**
 * Release any resources held by the Finonacci object.
 * @param statep points to the object.
 * @return NULL if successful, a pointer to the object otherwise.
 */
static inline diminuto_fibonacci_state_t * diminuto_fibonacci_fini(diminuto_fibonacci_state_t * statep)
{
    return (diminuto_fibonacci_state_t *)0;
}

/**
 * Initialize the Fibonacci generator state to use the classic sequence that
 * begins with (1, 1).
 * @param statep points to the generator state.
 * @return a pointer to the generator state.
 */
static inline diminuto_fibonacci_state_t * diminuto_fibonacci_init_classic(diminuto_fibonacci_state_t * statep)
{
    return diminuto_fibonacci_init(statep, 1, 1, diminuto_maximumof(diminuto_fibonacci_value_t));
}

/**
 * Initialize the Fibonacci generator state to use the modern sequence that`
 * begins with (0, 1).
 * @param statep points to the generator state.
 * @return a pointer to the generator state.
 */
static inline diminuto_fibonacci_state_t * diminuto_fibonacci_init_modern(diminuto_fibonacci_state_t * statep)
{
    return diminuto_fibonacci_init(statep, 0, 1, diminuto_maximumof(diminuto_fibonacci_value_t));
}

/**
 * Initialize the Fibonacci generator state to use the advanced sequence that`
 * begins with (1, 2).
 * @param statep points to the generator state.
 * @return a pointer to the generator state.
 */
static inline diminuto_fibonacci_state_t * diminuto_fibonacci_init_advanced(diminuto_fibonacci_state_t * statep)
{
    return diminuto_fibonacci_init(statep, 1, 2, diminuto_maximumof(diminuto_fibonacci_value_t));
}

/**
 * Set the maximum value returned by the Fibonacci generator. The actual
 * maximum value will be the largest Fibonacci number smaller than the specified
 * maximum if the maximum itself is not a Fibonacci number.
 * @param statep points to the generator state.
 * @param xm is the new maximum value.
 * @return the prior maximum value.
 */
static inline diminuto_fibonacci_value_t diminuto_fibonacci_set_limit(diminuto_fibonacci_state_t * statep, diminuto_fibonacci_value_t xm)
{
    DIMINUTO_SWAP(xm, statep->xm);
    return xm;
}

/**
 * Generate the next Fibonacci number. The returned values keep increasing
 * along the classic (1, 1, 2, 3, 5, 8, 13, ...), the modern (0, 1, 1, 2, 3,
 * 5, 8, 13, ...), or the advanced (1, 2, 3, 5, 8, 13, ...) Fibonacci sequence,
 * depending on how the state structure was initialized, until the generated
 * value would exceed the maximum, at which point the largest possible Fibonacci
 * number less or equal to the maximum is returned for that and all subsequent
 * calls.
 * @param statep points to the generator state.
 * @return the next number in the Fibonacci sequence or the maximum possible.
 */
static inline diminuto_fibonacci_value_t diminuto_fibonacci_next(diminuto_fibonacci_state_t * statep)
{
    diminuto_fibonacci_value_t xn;

    xn = statep->x0;
    statep->x0 = statep->x1;
    if ((statep->xm - statep->x1) > xn) { statep->x1 += xn; }

    return xn;
}

#endif
