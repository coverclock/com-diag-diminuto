/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _COM_DIAG_DIMINUTO_CXX_THING_HPP_
#define _COM_DIAG_DIMINUTO_CXX_THING_HPP_

/**
 * @file
 * @copyright Copyright 2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the C++ header file for the CXXCAPI test.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the C++ header file for the CXXCAPI test.
 */

#if defined(__cplusplus)

/**
 * This is the definition of Thing that C++ programs will see.
 * It is a conventional definition of a C++ class.
 */
class Thing {

private:

    /**
     * Hold an arbitary integer in the Thing.
     */
    int data;

public:

    /**
     * Construct a Thing with default data.
     */
    Thing();

    /**
     * Construct a Thing with parm as its data.
     */
    Thing(int parm);

    /**
     * Destruct a Thing.
     */
    ~Thing();

    /**
     * Set a new value in the Thing and return its prior value.
     * @param arg is the new value of the Thing.
     * @return the prior value of the Thing.
     */
    inline int set(int arg) { int prior = this->data; this->data = arg; return prior; }

    /**
     * Log the value of the Thing using the C-based Diminuto logging facility
     * @return the current value of the Thing.
     */
    int log();

};

#else

/**
 * This is the definition of Thing that C programs will see.
 * It declares an "opaque" structure, whose definition is not
 * complete, and a type that resolves to that structure. Neither
 * can be used to declare a new variablea. But you can use them
 * to declare a pointer to the struct, without the compiler knowing
 * what's inside the struct or how big it is.
 */
struct Thing;

/**
 * This is the definition of Thing that C programs will see.
 * It declares an "opaque" structure, whose definition is not
 * complete, and a type that resolves to that structure. Neither
 * can be used to declare a new variablea. But you can use them
 * to declare a pointer to the struct, without the compiler knowing
 * what's inside the struct or how big it is.
 */
typedef struct Thing Thing;

#endif

/**
 * These function prototypes will be seen by both C and C++
 * programs. Both will have C-linkage, and be callable by
 * either C or C++ code, even though they have completely
 * different definitions of Thing.
 * @param that points to a Thing in either C or C++;
 * @return the result returned by the Thing::log() function.
 */
CXXCAPI int thing_log(Thing * that);

/**
 * These function prototypes will be seen by both C and C++
 * programs. Both will have C-linkage, and be callable by
 * either C or C++ code, even though they have completely
 * different definitions of Thing.
 * @param that points to a Thing in either C or C++.
 * @return the pointer to the C or C++ Thing.
 */
CXXCINLINE Thing * thing_get(Thing * that) { return that; }

#endif
