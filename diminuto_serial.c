/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_serial.h"
#include "diminuto_log.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <termio.h>

int diminuto_serial_raw(int fd)
{
    int rc = -1;
    struct termios tios = { 0 };

    do {

	    if (!isatty(fd)) {
            errno = EINVAL;
            diminuto_perror("diminuto_serial_raw: isatty");
            break;
        }

        if (tcgetattr(fd, &tios) < 0) {
            diminuto_perror("diminuto_serial_raw: tcgetattr");
            break;
        }

        /*
         * Taken right from termios(3) "Raw mode".
         */
        tios.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON );
        tios.c_oflag &= ~OPOST;
        tios.c_cflag &= ~(CSIZE | PARENB);
        tios.c_cflag |= (CS8 | CREAD | CLOCAL);
        tios.c_cc[VTIME] = 0;
        tios.c_cc[VMIN] = 1;

        if (tcflush(fd, TCIFLUSH) < 0) {
            diminuto_perror("diminuto_serial_raw: tcflush");
            /* Proceed anyway. */
        }

        if (tcsetattr(fd, TCSANOW, &tios) < 0) {
            diminuto_perror("diminuto_serial_raw: tcsetattr");
            break;
        }

        rc = 0;

    } while (0);

    return rc;
}

int diminuto_serial_unbuffered(FILE * fp)
{
    int rc = -1;

    do {

	    if (!isatty(fileno(fp))) {
            errno = EINVAL;
            diminuto_perror("diminuto_serial_unbuffered: isatty");
            break;
        }

        if (setvbuf(fp, NULL, _IONBF, 0) != 0) {
            diminuto_perror("diminuto_serial_unbuffered: setvbuf");
            break;
        }

        rc = 0;

    } while (0);

    return rc;
}
