//* vi: set ts=4 expandtab shiftwidth=4: */
#ifndef _THING_HPP_
#define _THING_HPP_

/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 */


#if defined(__cplusplus)

class Thing {

private:

    int data;

public:

    Thing();

    Thing(int parm);

    ~Thing();

    CXXCINLINE int set(int arg) { int prior = this->data; this->data = arg; return prior; }

    int log();

};

#else

struct Thing;

typedef struct Thing Thing;

#endif

CXXCAPI int thing_log(Thing * that);

#endif
