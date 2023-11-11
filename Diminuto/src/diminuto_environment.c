/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright -2023 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Environment feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Environment feature.
 */

#include "com/diag/diminuto/diminuto_environment.h"
#include <stdlib.h>

diminuto_readerwriter_t diminuto_environment_readerwriter = DIMINUTO_READERWRITER_INITIALIZER;

char * diminuto_environment_get(const char * name)
{
    char * result = (char *)0;

    DIMINUTO_ENVIRONMENT_READER_BEGIN;
        result = getenv(name);
    DIMINUTO_ENVIRONMENT_READER_END;

    return result;
}

int diminuto_environment_put(char * string)
{
    int result = -1;

    DIMINUTO_ENVIRONMENT_WRITER_BEGIN;
        result = putenv(string);
    DIMINUTO_ENVIRONMENT_WRITER_END;

    return result;
}

int diminuto_environment_set(const char * name, const char * value, int overwrite)
{
    int result = -1;

    DIMINUTO_ENVIRONMENT_WRITER_BEGIN;
        result = setenv(name, value, overwrite);
    DIMINUTO_ENVIRONMENT_WRITER_END;

    return result;
}

int diminuto_environment_unset(const char * name)
{
    int result = -1;

    DIMINUTO_ENVIRONMENT_WRITER_BEGIN;
        result = unsetenv(name);
    DIMINUTO_ENVIRONMENT_WRITER_END;

    return result;
}
