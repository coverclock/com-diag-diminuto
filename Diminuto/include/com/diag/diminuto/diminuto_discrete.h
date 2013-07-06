/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_DISCRETE_
#define _H_COM_DIAG_DIMINUTO_DISCRETE_

/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Arvada CO 80001-0587 USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * Jack Gannsle, "A Guide to Debouncing, or How To Debounce a Contact in Two
 * Easy Pages", 2008-06, http://www.ganssle.com/debouncing.htm
 *
 * Jack Gannsle, "A Guide to Debouncing - Part 2, or How To Debounce a Contact
 * in Two Easy Pages", 2008-06, http://www.ganssle.com/debouncing-pt2.htm
 *
 * Jack Ganssle, "My Favorite Software Debouncers", 2004-06-16,
 * http://www.embedded.com/electronics-blogs/break-points/4024981/My-favorite-software-debouncers
 *
 * Dr. Marty, "The Best Switch Debounce Routine Ever", 2009-05-20,
 * http://drmarty.blogspot.com/2009/05/best-switch-debounce-routine-ever.html
 *
 * Jack Ganssle, "A Guide to Debouncing", 2008-06,
 * http://www.eng.utah.edu/~cs5780/debouncing.pdf
 */

#include "com/diag/diminuto/diminuto_types.h"

/**
 * This holds the state for the debouncer.
 */
typedef struct DiminutoDiscreteState {
	int8_t a;
	int8_t b;
	int8_t c;
	int8_t r;
	int8_t p;
} diminuto_discrete_state_t;

/**
 * Initialize the discrete state structure to either high/true/asserted or
 * low/false/deasserted.
 * @param statep points to the discrete state structure.
 * @param initial is the initial value, 0 for false, !0 for true.
 */
extern void diminuto_discrete_initialize(diminuto_discrete_state_t * statep, int initial);

/**
 * Feed the next successive discrete sample to the debounce algorithm and
 * return the current debounced state, high/true/asserted or
 * low/false/deasserted.
 * @param statep points to the discrete state structure.
 * @param input is the next input, 0 for false, !0 for true.
 * @return the debouncer output, 0 for false, !0 for true.
 */
extern int diminuto_discrete_debounce(diminuto_discrete_state_t * statep, int input);

/**
 * Return the most recent computed debounced state without altering the state.
 * @param statep points to the discrete state structure.
 * @return the debounced state, 0 for false, !0 for true.
 */
static inline int diminuto_discrete_state(const diminuto_discrete_state_t * statep) {
	return statep->r;
}

/**
 * These are all the possible return values for the edge detector.
 */
typedef enum DiminutoDiscreteEdge {
	LOW		= (0 << 1) | 0, /* 0 */
	RISING	= (0 << 1) | 1, /* 1 */
	HIGH	= (1 << 1) | 1, /* 3 */
	FALLING	= (1 << 1) | 0, /* 2 */
} diminuto_discrete_edge_t;

/**
 * Detect edges from the debounced output without altering the state.
 * @param statep points to the discrete state structure.
 * @return a enumeration indicating low, rising, high, or falling.
 */
extern diminuto_discrete_edge_t diminuto_discrete_edge(const diminuto_discrete_state_t * statep);

#endif
