/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2014 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Release feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Release feature.
 *
 * The purpose of this translation unit is to embed the release string
 * inside the library or shared object. The object module will be statically
 * linked into an application only if the translation unit makes explicit
 * references to the storage here as external references.
 */

#include "com/diag/diminuto/diminuto_release.h"

const char COM_DIAG_DIMINUTO_RELEASE_KEYWORD[] = "COM_DIAG_DIMINUTO_RELEASE=" COM_DIAG_DIMINUTO_RELEASE;
const char * COM_DIAG_DIMINUTO_RELEASE_VALUE = &COM_DIAG_DIMINUTO_RELEASE_KEYWORD[sizeof("COM_DIAG_DIMINUTO_RELEASE=") - 1];
