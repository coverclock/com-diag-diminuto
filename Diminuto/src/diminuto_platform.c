/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 * The purpose of this translation unit is to embed the platform string
 * inside the library or shared object. The resulting object module should
 * never be linked into an application unless the application explicitly
 * references the global variables.
 */

#include "com/diag/diminuto/diminuto_platform.h"

const char COM_DIAG_DIMINUTO_PLATFORM_KEYWORD[] = "COM_DIAG_DIMINUTO_PLATFORM=" COM_DIAG_DIMINUTO_PLATFORM;
const char * COM_DIAG_DIMINUTO_PLATFORM_VALUE = &COM_DIAG_DIMINUTO_PLATFORM_KEYWORD[sizeof("COM_DIAG_DIMINUTO_PLATFORM=") - 1];
