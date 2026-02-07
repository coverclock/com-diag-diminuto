/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2026 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Platform feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Platform feature.
 *
 * The purpose of this translation unit is to embed the platform string
 * inside the library or shared object. The resulting object module should
 * never be linked into an application unless the application explicitly
 * references the global variables.
 */

#include "com/diag/diminuto/diminuto_platform.h"
#include "com/diag/diminuto/diminuto_error.h"
#include <string.h>

const char COM_DIAG_DIMINUTO_PLATFORM_KEYWORD[] = "COM_DIAG_DIMINUTO_PLATFORM=" COM_DIAG_DIMINUTO_PLATFORM;
const char * COM_DIAG_DIMINUTO_PLATFORM_VALUE = &COM_DIAG_DIMINUTO_PLATFORM_KEYWORD[sizeof("COM_DIAG_DIMINUTO_PLATFORM=") - 1];

int diminuto_platform_system(struct utsname * bufferp)
{
    int rc = -1;

    if (bufferp == (struct utsname *)0) {
        errno = EINVAL;
        diminuto_perror("uname");
    } else {
        memset(bufferp, 0, sizeof(*bufferp));
        rc = uname(bufferp);
        if (rc < 0) {
            diminuto_perror("uname");
        } else {
            /* Do nothing: success. */
        }
    }

    return rc;
}
