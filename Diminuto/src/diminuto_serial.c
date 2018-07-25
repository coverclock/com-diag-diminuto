/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010-2018 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 */

#include "com/diag/diminuto/diminuto_serial.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <stdio.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/serial.h>

int diminuto_serial_set(int fd, int bitspersecond, int databits, int paritybit, int stopbits, int modemcontrol, int xonxoff, int rtscts)
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
	int ixon;
	int ixoff;
	int crtscts;

	do {

		switch (bitspersecond) {
		case -1:
			break;
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
			bitspersecond = -1;
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

		if (paritybit == 0) {
			parity = 0;
		} else if ((paritybit % 2) != 0) {
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

		if (xonxoff) {
			ixon = IXON;
			ixoff = IXOFF;
		} else {
			ixon = 0;
			ixoff = 0;
		}

		if (rtscts) {
			crtscts = CRTSCTS;
		} else {
			crtscts = 0;
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

        tios.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON | IXOFF);
        tios.c_oflag &= ~(OPOST);
        tios.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
        tios.c_cflag &= ~(CSIZE | PARENB  | PARODD | CSTOPB | CLOCAL | HUPCL | CRTSCTS);

        tios.c_iflag |= ixon;
        tios.c_iflag |= ixoff;

        tios.c_cflag |= CREAD;
        tios.c_cflag |= data;
        tios.c_cflag |= parity;
        tios.c_cflag |= stopbits;
        tios.c_cflag |= clocal;
        tios.c_cflag |= hupcl;
        tios.c_cflag |= crtscts;

        if (bitspersecond >= 0) {
			if (cfsetospeed(&tios, speed) < 0) {
				diminuto_perror("diminuto_serial_set: cfsetospeed");
				break;
			}
			if (cfsetispeed(&tios, speed) < 0) {
				diminuto_perror("diminuto_serial_set: cfsetispeed");
				break;
			}
        }

        tios.c_cc[VTIME] = 0;
        tios.c_cc[VMIN] = 1;

        if (tcsetattr(fd, TCSANOW, &tios) < 0) {
        	diminuto_perror("diminuto_serial_set: tcsetattr");
        	break;
        }

        if (tcflush(fd, TCIOFLUSH) < 0) {
        	diminuto_perror("diminuto_serial_set: tcflush");
        	/* Not fatal. */
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

        if (tcsetattr(fd, TCSANOW, &tios) < 0) {
            diminuto_perror("diminuto_serial_raw: tcsetattr");
            break;
        }

        if (tcflush(fd, TCIOFLUSH) < 0) {
            diminuto_perror("diminuto_serial_raw: tcflush");
            /* Proceed anyway. */
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

int diminuto_serial_wait(int fd)
{
	int rc = -1;

	do {

	    if (!isatty(fd)) {
            errno = EINVAL;
            diminuto_perror("diminuto_serial_wait: isatty");
            break;
        }

	    if (ioctl(fd, TIOCMIWAIT, TIOCM_CD) < 0) {
            diminuto_perror("diminuto_serial_wait: ioctl(TIOCMIWAIT)");
            break;
	    }

	    /*
	     * Ubuntu 16.04.4 "xenial"
	     * Linux 4.13.0-39
	     * Navisys GR-701W
	     * U-Blox 7
	     * Prolific pl2303
	     * ioctl TIOCMIWAIT TIOCM_CD requires HUPCL modem control be enabled.
	     * ioctl TIOCMIWAIT TIOCM_CD appears to work on a /dev/ttyUSB serial device.
	     * ioctl TIOCGICOUNT fails errno=25 ENOTTY "Inappropriate ioctl for device".
	     */

		rc = 0;

	} while (0);

	return rc;
}

int diminuto_serial_status(int fd)
{
	int rc = -1;
	int status = -2;

	do {

	    if (!isatty(fd)) {
            errno = EINVAL;
            diminuto_perror("diminuto_serial_status: isatty");
            break;
        }

	    if (ioctl(fd, TIOCMGET, &status) < 0) {
            diminuto_perror("diminuto_serial_status: ioctl(TIOCMGET)");
            break;
	    }

		rc = !!(status & TIOCM_CD);

	} while (0);

	return rc;

}

int diminuto_serial_available(int fd)
{
	int rc = -1;
	int bytes = -2;

	do {

		if (!isatty(fd)) {
            errno = EINVAL;
            diminuto_perror("diminuto_serial_available: isatty");
            break;
        }

	    if (ioctl(fd, FIONREAD, &bytes) < 0) {
            diminuto_perror("diminuto_serial_available: ioctl(FIONREAD)");
            break;
	    }

	    rc = bytes;

	} while (0);

	return rc;
}
