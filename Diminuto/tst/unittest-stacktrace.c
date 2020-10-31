/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010-2014 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Stack Trace feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Stack Trace feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_stacktrace.h"
#include "com/diag/diminuto/diminuto_log.h"

int f9(void) {
    int rc;
    DIMINUTO_LOG_DEBUG("%s f%d\n", DIMINUTO_LOG_HERE, 9);
    rc = diminuto_stacktrace();
    DIMINUTO_LOG_DEBUG("%s f%d %d\n", DIMINUTO_LOG_HERE, 9, rc);
    return rc;
}

int f8(void) {
    int rc;
    DIMINUTO_LOG_DEBUG("%s f%d\n", DIMINUTO_LOG_HERE, 8);
    rc = f9();
    DIMINUTO_LOG_DEBUG("%s f%d %d\n", DIMINUTO_LOG_HERE, 8, rc);
    return rc;
}

int f7(void) {
    int rc;
    DIMINUTO_LOG_DEBUG("%s f%d\n", DIMINUTO_LOG_HERE, 7);
    rc = f8();
    DIMINUTO_LOG_DEBUG("%s f%d %d\n", DIMINUTO_LOG_HERE, 7, rc);
    return rc;
}

int f6(void) {
    int rc;
    DIMINUTO_LOG_DEBUG("%s f%d\n", DIMINUTO_LOG_HERE, 6);
    rc = f7();
    DIMINUTO_LOG_DEBUG("%s f%d %d\n", DIMINUTO_LOG_HERE, 6, rc);
    return rc;
}

int f5(void)
{
    int rc;
    DIMINUTO_LOG_DEBUG("%s f%d\n", DIMINUTO_LOG_HERE, 5);
    rc = f6();
    DIMINUTO_LOG_DEBUG("%s f%d %d\n", DIMINUTO_LOG_HERE, 5, rc);
    return rc;
}

int f4(void)
{
    int rc;
    DIMINUTO_LOG_DEBUG("%s f%d\n", DIMINUTO_LOG_HERE, 4);
    rc = f5();
    DIMINUTO_LOG_DEBUG("%s f%d %d\n", DIMINUTO_LOG_HERE, 4, rc);
    return rc;
}

int f3(void)
{
    int rc;
    DIMINUTO_LOG_DEBUG("%s f%d\n", DIMINUTO_LOG_HERE, 3);
    rc = f4();
    DIMINUTO_LOG_DEBUG("%s f%d %d\n", DIMINUTO_LOG_HERE, 3, rc);
    return rc;
}

int f2(void)
{
    int rc;
    DIMINUTO_LOG_DEBUG("%s f%d\n", DIMINUTO_LOG_HERE, 2);
    rc = f3();
    DIMINUTO_LOG_DEBUG("%s f%d %d\n", DIMINUTO_LOG_HERE, 2, rc);
    return rc;
}

int f1(void)
{
    int rc;
    DIMINUTO_LOG_DEBUG("%s f%d\n", DIMINUTO_LOG_HERE, 1);
    rc = f2();
    DIMINUTO_LOG_DEBUG("%s f%d %d\n", DIMINUTO_LOG_HERE, 1, rc);
    return rc;
}

int f0(void)
{
    int rc;
    DIMINUTO_LOG_DEBUG("%s f%d\n", DIMINUTO_LOG_HERE, 0);
    rc = f1();
    DIMINUTO_LOG_DEBUG("%s f%d %d\n", DIMINUTO_LOG_HERE, 0, rc);
    return rc;
}

int main(int argc, char ** argv)
{
    int count;

    SETLOGMASK();

    DIMINUTO_LOG_DEBUG("%s main\n", DIMINUTO_LOG_HERE);

    count = f0();

    DIMINUTO_LOG_DEBUG("%s main %d\n", DIMINUTO_LOG_HERE, count);

    ASSERT(count >= 0);

    EXIT();
}
