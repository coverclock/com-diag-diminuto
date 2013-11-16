/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include <stdio.h>
#include <string.h>
#include "com/diag/diminuto/diminuto_unittest.h"

int main(int argc, char ** argv)
{
	FILE * fp;
	char command[80] = { '\0' } ;
	int ii;
    int ch;

    strncat(command, "phex", sizeof(command) - strlen(command) - 1);

    for (ii = 1; ii < argc; ++ii) {
        strncat(command, " ", sizeof(command) - strlen(command) - 1);
    	strncat(command, argv[ii], sizeof(command) - strlen(command) - 1);
     }

    fprintf(stderr, "command: \"%s\"\n", command);

    fp = popen(command, "w");
    ASSERT(fp != (FILE *)0);

    fputc('\n', stdout);
    fputc('\n', stderr);

    for (ch = 0; ch < 256; ++ch) { fputc(ch, fp); }

    fflush(fp);

    EXIT();
}

