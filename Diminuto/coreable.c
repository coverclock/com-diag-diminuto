/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * USAGE
 *
 * coreable executablebinary
 *
 * EXAMPLES
 *
 * coreable executablebinary
 *
 * ABSTRACT
 *
 * Runs the specfied executable binary and enables a full core dump to be
 * generated in the event that it fails.
 *
 * NOTES
 *
 * Ported (barely) from Desperado.
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>

int main(int argc, char **argv) {
    int rc = 1;
    if (argc >= 2) {
        struct rlimit limit;
        if (-1 == getrlimit(RLIMIT_CORE, &limit)) {
            perror(argv[0]);
        } else {
            limit.rlim_cur = limit.rlim_max;
            if (-1 == setrlimit(RLIMIT_CORE, &limit)) {
                perror(argv[0]);
            }
        }
        rc = execvp(argv[1], &(argv[1]));
    } 
    return rc;
}
