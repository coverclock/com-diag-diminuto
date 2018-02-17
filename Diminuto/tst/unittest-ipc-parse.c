/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_ipc.h"

int main(int argc, char * argv[])
{
	diminuto_ipc_parse_t parse;
	int rc;

    SETLOGMASK();

    TEST();

    rc = diminuto_ipc_parse("80", &parse);

    rc = diminuto_ipc_parse(":http", &parse);

    rc = diminuto_ipc_parse("localhost", &parse);

    rc = diminuto_ipc_parse("49152", &parse);

    rc = diminuto_ipc_parse(":49152", &parse);

    rc = diminuto_ipc_parse("localhost:49152", &parse);

    rc = diminuto_ipc_parse("google.com", &parse);

    rc = diminuto_ipc_parse("google.com:80", &parse);

    rc = diminuto_ipc_parse("google.com:http", &parse);

    rc = diminuto_ipc_parse("172.217.1.206", &parse);

    rc = diminuto_ipc_parse("172.217.1.206:80", &parse);

    rc = diminuto_ipc_parse("172.217.1.206:http", &parse);

    rc = diminuto_ipc_parse("[0:0:0:0:ffff:172.217.1.206]:49152", &parse);

    rc = diminuto_ipc_parse("[::ffff:172.217.1.206]:49152", &parse);

    rc = diminuto_ipc_parse("[::ffff:172.217.1.206]:http", &parse);

    rc = diminuto_ipc_parse("[2607:f8b0:400f:805::200e]:49152", &parse);

    rc = diminuto_ipc_parse("[2607:f8b0:400f:805::200e]:http", &parse);

    STATUS();

    EXIT();
}
