/* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _THING_HPP_
#define _THING_HPP_

/*
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA
 * Licensed under the terms in README.h
 */

#if defined(__cplusplus)

/*
 * This is the definition of Thing that C++ programs will see.
 * It is a conventional definition of a C++ class.
 */

class Thing {

private:

    int data;

public:

    Thing();

    Thing(int parm);

    ~Thing();

    inline int set(int arg) { int prior = this->data; this->data = arg; return prior; }

    int log();

};

#else

/*
 * This is the definition of Thing that C programs will see.
 * It declares an "opaque" structure, whose definition is not
 * complete, and a type that resolves to that structure. Neither
 * can be used to declare a new variablea. But you can use them
 * to declare a pointer to the struct, without the compiler knowing
 * what's inside the struct or how big it is.
 */

struct Thing;

typedef struct Thing Thing;

#endif

/*
 * These function prototypes will be seen by both C and C++
 * programs. Both will have C-linkage, and be callable by
 * either C or C++ code, even though they have completely
 * different definitions of Thing.
 */

CXXCAPI int thing_log(Thing * that);

CXXCINLINE Thing * thing_get(Thing * that) { return that; }

#endif
