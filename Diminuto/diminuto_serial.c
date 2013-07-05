/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_serial.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <termio.h>

int diminuto_serial_set(int fd, int bitspersecond, int databits, int paritybit, int stopbits, int modemcontrol)
{
	int rc = -1;
	int error = 0;
	struct termios tios = { 0 };
	int speed;
	int data;
	int parity;
	int stop;
	int clocal;
	int hupcl;

	do {

		switch (bitspersecond) {
		case 0:
			speed = B0;
			break;
		case 50:
			speed = B50;
			break;
		case 75:
			speed = B75;
			break;
		case 110:
			speed = B110;
			break;
		case 134:
			speed = B134;
			break;
		case 150:
			speed = B150;
			break;
		case 200:
			speed = B200;
			break;
		case 300:
			speed = B300;
			break;
		case 600:
			speed = B600;
			break;
		case 1200:
			speed = B1200;
			break;
		case 1800:
			speed = B1800;
			break;
		case 2400:
			speed = B2400;
			break;
		case 4800:
			speed = B4800;
			break;
		case 9600:
			speed = B9600;
			break;
		case 19200:
			speed = B19200;
			break;
		case 38400:
			speed = B38400;
			break;
		case 57600:
			speed = B57600;
			break;
		case 115200:
			speed = B115200;
			break;
		case 230400:
			speed = B230400;
			break;
		default:
			error = !0;
			errno = EINVAL;
			diminuto_perror("diminuto_serial_set: bitspersecond");
			break;
		}

		switch (databits) {
		case 5:
			data = CS5;
			break;
		case 6:
			data = CS6;
			break;
		case 7:
			data = CS7;
			break;
		case 8:
			data = CS8;
			break;
		default:
			error = !0;
			errno = EINVAL;
			diminuto_perror("diminuto_serial_set: databits");
			break;
		}

		if (parity == 0) {
			parity = 0;
		} else if ((parity % 2) == 1) {
			parity = PARENB | PARODD;
		} else {
			parity = PARENB;
		}

		switch (stopbits) {
		case 1:
			stop = 0;
			break;
		case 2:
			stop = CSTOPB;
			break;
		default:
			error = !0;
			errno = EINVAL;
			diminuto_perror("diminuto_serial_set: stopbits");
			break;
		}

		if (modemcontrol) {
			clocal = 0;
			hupcl = HUPCL;
		} else {
			clocal = CLOCAL;
			hupcl = 0;
		}

		if (error) {
			break;
		}

	    if (!isatty(fd)) {
            errno = ENOTTY;
            diminuto_perror("diminuto_serial_set: isatty");
            break;
        }

        if (tcgetattr(fd, &tios) < 0) {
            diminuto_perror("diminuto_serial_set: tcgetattr");
            break;
        }

        tios.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
        tios.c_oflag &= ~(OPOST);
        tios.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
        tios.c_cflag &= ~(CSIZE | PARENB | CSTOPB | CLOCAL | HUPCL);
        tios.c_cflag |= CREAD;
        tios.c_cflag |= data;
        tios.c_cflag |= parity;
        tios.c_cflag |= stopbits;
        tios.c_cflag |= clocal;
        tios.c_cflag |= hupcl;

        if (cfsetospeed(&tios, speed) < 0) {
        	diminuto_perror("diminuto_serial_set: cfsetospeed");
        	break;
        }

        if (cfsetispeed(&tios, speed) < 0) {
        	diminuto_perror("diminuto_serial_set: cfsetispeed");
        	break;
        }

        tios.c_cc[VTIME] = 0;
        tios.c_cc[VMIN] = 1;

        if (tcflush(fd, TCIFLUSH) < 0) {
        	diminuto_perror("diminuto_serial_set: tcflush");
        	/* Not fatal. */
        }

        if (tcsetattr(fd, TCSANOW, &tios) < 0) {
        	diminuto_perror("diminuto_serial_set: tcsetattr");
        	break;
        }

        rc = 0;

	} while (0);

	return rc;
}

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
