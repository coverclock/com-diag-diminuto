/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010-2020 Digital Aggregates Corporation, Colorado, USA.
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
    CHECKPOINT("f%d\n", 9);
    rc = diminuto_stacktrace();
    CHECKPOINT("f%d %d\n", 9, rc);
    return rc;
}

int f8(void) {
    int rc;
    CHECKPOINT("f%d\n", 8);
    rc = f9();
    CHECKPOINT("f%d %d\n", 8, rc);
    return rc;
}

int f7(void) {
    int rc;
    CHECKPOINT("f%d\n", 7);
    rc = f8();
    CHECKPOINT("f%d %d\n", 7, rc);
    return rc;
}

int f6(void) {
    int rc;
    CHECKPOINT("f%d\n", 6);
    rc = f7();
    CHECKPOINT("f%d %d\n", 6, rc);
    return rc;
}

int f5(void)
{
    int rc;
    CHECKPOINT("f%d\n", 5);
    rc = f6();
    CHECKPOINT("f%d %d\n", 5, rc);
    return rc;
}

int f4(void)
{
    int rc;
    CHECKPOINT("f%d\n", 4);
    rc = f5();
    CHECKPOINT("f%d %d\n", 4, rc);
    return rc;
}

int f3(void)
{
    int rc;
    CHECKPOINT("f%d\n", 3);
    rc = f4();
    CHECKPOINT("f%d %d\n", 3, rc);
    return rc;
}

int f2(void)
{
    int rc;
    CHECKPOINT("f%d\n", 2);
    rc = f3();
    CHECKPOINT("f%d %d\n", 2, rc);
    return rc;
}

int f1(void)
{
    int rc;
    CHECKPOINT("f%d\n", 1);
    rc = f2();
    CHECKPOINT("f%d %d\n", 1, rc);
    return rc;
}

int f0(void)
{
    int rc;
    CHECKPOINT("f%d\n", 0);
    rc = f1();
    CHECKPOINT("f%d %d\n", 0, rc);
    return rc;
}

int main(int argc, char ** argv)
{
    int count;

    SETLOGMASK();

    CHECKPOINT("main\n");

    count = f0();

    CHECKPOINT("main %d\n", count);

    ASSERT(count >= 0);

    EXIT();
}
