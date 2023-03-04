/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _H_COM_DIAG_DIMINUTO_NEWDELETE_
#define _H_COM_DIAG_DIMINUTO_NEWDELETE_

/**
 * @file
 * @copyright Copyright 2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Provide new, delete, constructor, and destructor macros.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * THIS IS A WORK IN PROGRESS.
 *
 * REFERENCES
 *
 * GNU, THE C PREPROCESSOR, "Variadic Macros", 3.6, Free Software Foundations,
 * 2023
 */

#include "com/diag/diminuto/diminuto_heap.h"

/**
 * @def diminuto_ctor
 * Construct an existing instance of an object given its type, a pointer to
 * it, and an optional list of arguments. Return a pointer to the object.
 */
#define diminuto_ctor(_TYPE_, _PNTR_, ...) (_TYPE_##_ctor(_PNTR_, ##__VA_ARGS__))

/**
 * @def diminuto_dtor
 * Destruct an instance of an object given its type and a pointer to it.
 * Return a pointer to the object.
 */
#define diminuto_dtor(_TYPE_, _PNTR_)  (_TYPE_##_dtor(_PNTR_))

/**
 * @def diminuto_new
 * Allocate a new instance of an object given its type, and construct it
 * given an optional list of arguments. Return a pointer to the object.
 */
#define diminuto_new(_TYPE_, ...) (diminuto_ctor(_TYPE_, (_TYPE_ *)diminuto_heap_calloc(1, sizeof(_TYPE_)), ##__VA_ARGS__))

/**
 * @def diminuto_delete
 * Destruct an instance of an object given its type and a pointer to it,
 * and deallocate the memory associated with the object. Return void.
 */
#define diminuto_delete(_TYPE_, _PNTR_) (diminuto_heap_free(diminuto_dtor(_TYPE_, _PNTR_)))

#endif
