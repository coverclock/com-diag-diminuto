/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2016 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Revision feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Revision feature.
 *
 * The purpose of this translation unit is to embed the revision string
 * inside the library or shared object. The object module will be statically
 * linked into an application only if the translation unit makes explicit
 * references to the storage here as external references.
 */

#include "com/diag/diminuto/diminuto_revision.h"

const char COM_DIAG_DIMINUTO_REVISION_KEYWORD[] = "COM_DIAG_DIMINUTO_REVISION=" COM_DIAG_DIMINUTO_REVISION;
const char * COM_DIAG_DIMINUTO_REVISION_VALUE = &COM_DIAG_DIMINUTO_REVISION_KEYWORD[sizeof("COM_DIAG_DIMINUTO_REVISION=") - 1];
