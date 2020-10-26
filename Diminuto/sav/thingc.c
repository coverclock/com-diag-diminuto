//* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2020 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 */

#include "com/diag/diminuto/diminuto_cxxcapi.h"
#include "thingc.h"

CXXCINLINE int logi(Thing * that) { return thing_log(that); }

int logc(Thing * that) { return logi(that); }
