/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_CUE_
#define _H_COM_DIAG_DIMINUTO_CUE_

/**
 * @file
 * @copyright Copyright 2015-2017 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Implements a digital signal debouncer and edge detector.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * The Cue feature provides a software debouncer and edge detector. It can be
 * used with the pin feature which manipulates GPIO pins using the sysfs
 * interface, but it can also be used independently. A cue can smooth out high
 * frequency events over a short duration. A polling time of 10ms works well
 * for many mechanical switches.
 *
 * The name was taken from "cue" as in "I missed my cue."
 *
 * Ported from the Pin feature of the deprecated Telegraph library.
 *
 * REFERENCES
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

/**
 * This structure holds the state for the debouncer and edge detector.
 * The debounce algorithm keeps three successive samples a, b, and c, the most
 * recent result r, plus the previously returned result p for use by the edge
 * detector. Most mechanical switches do well with a 100Hz (10ms period)
 * sampling frequency. It would be trivial to expand the algorithm to be based
 * on more samples, e.g. four or five or even more.
 */
typedef struct DiminutoCueState {
    unsigned int a : 1;
    unsigned int b : 1;
    unsigned int c : 1;
    unsigned int r : 1;
    unsigned int p : 1;
} diminuto_cue_state_t;

/**
 * Initialize the cue state structure to either high/true/asserted or
 * low/false/deasserted.
 * @param statep points to the cue state structure.
 * @param initial is the initial value, 0 for false, !0 for true.
 * @return a pointer ot the object or NULL if an error occurred.
 */
extern diminuto_cue_state_t *  diminuto_cue_init(diminuto_cue_state_t * statep, int initial);

/**
 * Release resources associatged with the cue.
 * @param statep points to the cue state structure.
 * @return NULL or a pointer ot the object an error occurred.
 */
static inline diminuto_cue_state_t *  diminuto_cue_fini(diminuto_cue_state_t * statep)
{
    return (diminuto_cue_state_t *)0;
}

/**
 * Feed the next successive cue sample to the debounce algorithm and
 * return the current debounced state, high/true/asserted or
 * low/false/deasserted. Typical switches will do well with a 100Hz (10ms)
 * sampling frequency.
 * @param statep points to the cue state structure.
 * @param input is the next input, 0 for false, !0 for true.
 * @return the debouncer output, 0 for false, !0 for true.
 */
extern int diminuto_cue_debounce(diminuto_cue_state_t * statep, int input);

/**
 * Return the most recent computed debounced state without altering the state.
 * @param statep points to the cue state structure.
 * @return the debounced state, 0 for false, !0 for true.
 */
static inline int diminuto_cue_state(const diminuto_cue_state_t * statep) {
    return statep->r;
}

/**
 * These are all the possible bits returned by the edge detector. The lowest
 * order bit represents the current state, and the next higher order bit
 * represents the prior state, and the highest order bit is one if the two lower
 * order bits are different.
 */
typedef enum DiminutoCueMask {
    DIMINUTO_CUE_MASK_CURRENT   = (1 << 0), /* 001b */
    DIMINUTO_CUE_MASK_PRIOR     = (1 << 1), /* 010b */
    DIMINUTO_CUE_MASK_CHANGING  = (1 << 2), /* 100b */
} diminuto_cue_mask_t;

/**
 * These are all the possible return values for the edge detector. The lowest
 * order bit represents the current state, and the next higher order bit
 * represents the prior state, and the highest order bit is one if the two lower
 * order bits are different. However, your application doesn't really need to
 * know that; it can just look at the enumerated values.
 */
typedef enum DiminutoCueEdge {
    DIMINUTO_CUE_EDGE_LOW       = (0 << 2) | (0 << 1) | (0 << 0), /* 000b = 0 */
    DIMINUTO_CUE_EDGE_RISING    = (1 << 2) | (0 << 1) | (1 << 0), /* 101b = 5 */
    DIMINUTO_CUE_EDGE_HIGH      = (0 << 2) | (1 << 1) | (1 << 0), /* 011b = 3 */
    DIMINUTO_CUE_EDGE_FALLING   = (1 << 2) | (1 << 1) | (0 << 0), /* 110b = 6 */
} diminuto_cue_edge_t;

/**
 * Detect edges from the debounced output without altering the state.
 * @param statep points to the cue state structure.
 * @return a enumeration indicating low, rising, high, or falling.
 */
extern diminuto_cue_edge_t diminuto_cue_edge(const diminuto_cue_state_t * statep);

/**
 * Detect rising edge from the raw input without altering the state.
 * Necessary for certain time-sensitive modulation schemes.
 * @param statep points to the cue state structure.
 * @return true if the raw undebounced input is rising.
 */
static inline int diminuto_cue_isrising(const diminuto_cue_state_t * statep)
{
    return ((statep->a != 0) && (statep->b == 0));
}

/**
 * Detect falling edge from the raw input without altering the state.
 * Necessary for certain time-sensitive modulation schemes.
 * @param statep points to the cue state structure.
 * @return true if the raw undebounced input is falling.
 */
static inline int diminuto_cue_isfalling(const diminuto_cue_state_t * statep)
{
    return ((statep->a == 0) && (statep->b != 0));
}

#if defined(COM_DIAG_DIMINUTO_DEPRECATED)
#   define diminuto_cue_is_rising diminuto_cue_isrising
#   define diminuto_cue_is_falling diminuto_cue_isfalling
#endif

#endif
