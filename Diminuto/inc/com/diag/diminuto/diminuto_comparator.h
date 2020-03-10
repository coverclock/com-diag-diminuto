/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_COMPARATOR_
#define _H_COM_DIAG_DIMINUTO_COMPARATOR_

/**
 * @file
 *
 * Copyright 2010, 2014 Digital Aggregates Corporation, Colorado, USA.
 * Licensed under the terms in LICENSE.txt.
 *
 * The Comparator feature provides a standard API, and usable examples,
 * for a comparator function used by other features as a callback.
 */

/**
 * This describes the prototype for a comparator function. Such functions are
 * used generically to compare two objects (whatever "comnpare" and "object"
 * means in the context of the application). Comparators are used by several
 * Diminuto features. Note that this is the type of the function, not the
 * type of a pointer to the function (although the latter is trivially
 * declared). The comparator logically subtracts the second object from the
 * first and returns the result: <0 if the first object is less than the second
 * object, 0 if the two objects are equal, and >0 if the first object is less
 * than the second object.
 */
typedef int (diminuto_comparator_t)(const void *, const void *);

/**
 * This is an implementation of a comparator function that compares two
 * C-style NUL-terminated strings. It returns <0 if the first string is
 * less than the second string, 0 if the two strings are equal (contain the
 * same characters), and >0 if the first string is greater than the second
 * string.
 * @param firstp is a pointer to the first string.
 * @param secondp is a pointer to the second string.
 * @return <0, 0, or >0.
 */
extern int diminuto_compare_strings(const void * firstp, const void * secondp);

/**
 * This is an implementation of a comparator function that compares two
 * pointers. It returns <0 if the first pointer has a lesser value than the
 * second pointer, 0 if the two pointer are equal (point to the same object),
 * and >0 if the first pointer is greater than the second pointer.
 * @param firstp is the first pointer.
 * @param secondp is the second pointer.
 * @return <0, 0, or >0.
 */
extern int diminuto_compare_pointers(const void * firstp, const void * secondp);

#endif
