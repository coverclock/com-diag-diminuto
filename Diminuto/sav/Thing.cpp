//* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 */

#include "com/diag/diminuto/diminuto_cxxcapi.h"

#include "Thing.hpp"

#include "com/diag/diminuto/diminuto_begin.h"
#   include "com/diag/diminuto/diminuto_log.h"
#   include "thingc.h"
#include "com/diag/diminuto/diminuto_end.h"

Thing::Thing() { this->data = 0; }

Thing::Thing(int parm) { this->data = parm; }

Thing::~Thing() { }

int Thing::log() { DIMINUTO_LOG_NOTICE("this=%p data=0x%x null=%p\n", this, this->data, NULL); return this->data; }

CXXCAPI int thing_log(Thing * that) { return that->log(); }

int main(void)
{

    Thing thing(0xdeadbeef);

    thing.log();

    thing.set(0xc0edbabe);

    thing.log();

    int result = logc(&thing);

    DIMINUTO_LOG_NOTICE("result=0x%x\n", result);
}
