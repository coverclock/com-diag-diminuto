/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013-2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in LICENSE.txt<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * https://github.com/coverclock/com-diag-diminuto<BR>
 *
 * REFERENCES
 *
 * "GPIO Interfaces", Documentation/gpio.txt, Linux 3.10.24
 */

#include "com/diag/diminuto/diminuto_pin.h"
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <errno.h>
#include <sys/param.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>

static const char ROOT[] = "/sys";

static const char * root = ROOT;

static const char ROOT_CLASS_GPIO_EXPORT[] = "%s/class/gpio/export";
static const char ROOT_CLASS_GPIO_PIN_DIRECTION[] = "%s/class/gpio/gpio%u/direction";
static const char ROOT_CLASS_GPIO_PIN_ACTIVELOW[] = "%s/class/gpio/gpio%u/active_low";
static const char ROOT_CLASS_GPIO_PIN_EDGE[] = "%s/class/gpio/gpio%u/edge";
static const char ROOT_CLASS_GPIO_PIN_VALUE[] = "%s/class/gpio/gpio%u/value";
static const char ROOT_CLASS_GPIO_UNEXPORT[] = "%s/class/gpio/unexport";

const char * diminuto_pin_debug(const char * tmp)
{
	const char * prior;

	prior = root;
	root = (tmp != (const char *)0) ? tmp : ROOT;

	return prior;
}

static int diminuto_pin_configure_conditional(const char * format, int pin, const char * string, int ignore)
{
	int rc = -1;
	char filename[PATH_MAX];
	FILE * fp = (FILE *)0;

	do {

		if (pin < 0) {
			errno = EINVAL;
			diminuto_perror("diminuto_pin_configure_conditional: pin");
			break;
		}

		snprintf(filename, sizeof(filename), format, root, pin);

		if ((fp = fopen(filename, "w")) == (FILE *)0) {
			break;
		}

		if (fputs(string, fp) >= 0) {
			/* Do nothing. */
		} else if (errno != EINVAL) {
			diminuto_perror("diminuto_pin_configure_conditional: fputs");
			break;
		} else if (ignore) {
			/* Do nothing. */
		} else {
			diminuto_perror("diminuto_pin_configure_conditional: fputs");
			break;
		}

		if (fflush(fp) != EOF) {
			/* Do nothing. */
		} else if (errno != EINVAL) {
			diminuto_perror("diminuto_pin_configure_conditional: fflush");
			break;
		} else if (ignore) {
			/* Do nothing. */
		} else {
			diminuto_perror("diminuto_pin_configure_conditional: fflush");
			break;
		}

		rc = 0;

	} while (0);

	if (fp == (FILE *)0) {
		/* Do nothing. */
	} else if (fclose(fp) == 0) {
		/* Do nothing. */
	} else {
		diminuto_perror("diminuto_pin_configure_conditional: fclose");
		rc = -1;
	}

	return rc;
}

static inline int diminuto_pin_configure(const char * format, int pin, const char * string)
{
	return diminuto_pin_configure_conditional(format, pin, string, 0);
}

static int diminuto_pin_port_conditional(const char * format, int pin, int ignore)
{
	char buffer[sizeof("-9223372036854775807\n")];
	snprintf(buffer, sizeof(buffer), "%d\n", pin);
	return diminuto_pin_configure_conditional(format, pin, buffer, ignore);
}

static int diminuto_pin_port(const char * format, int pin)
{
	return diminuto_pin_port_conditional(format, pin, 0);
}

int diminuto_pin_export(int pin)
{
	int rc = 0;

	rc = diminuto_pin_port(ROOT_CLASS_GPIO_EXPORT, pin);
	if (rc < 0) {
		diminuto_perror("diminuto_pin_export");
	}

	return rc;
}

int diminuto_pin_unexport(int pin)
{
	int rc = 0;

	rc = diminuto_pin_port_conditional(ROOT_CLASS_GPIO_UNEXPORT, pin, 0);
	if (rc < 0) {
		diminuto_perror("diminuto_pin_unexport");
	}

	return rc;
}

int diminuto_pin_unexport_ignore(int pin)
{
	int rc = 0;

	rc = diminuto_pin_port_conditional(ROOT_CLASS_GPIO_UNEXPORT, pin, !0);
	if (rc < 0) {
		diminuto_perror("diminuto_pin_unexport_ignore");
	}

	return rc;
}

int diminuto_pin_active(int pin, int high)
{
	int rc = 0;

	rc = diminuto_pin_configure(ROOT_CLASS_GPIO_PIN_ACTIVELOW, pin, high ? "0\n" : "1\n");
	if (rc < 0) {
		diminuto_perror("diminuto_pin_active");
	}

	return rc;
}

int diminuto_pin_edge(int pin, diminuto_pin_edge_t edge)
{
	int rc = 0;
	const char * string = (const char *)0;

	switch (edge) {
	case DIMINUTO_PIN_EDGE_NONE:	string = "none\n";		break;
	case DIMINUTO_PIN_EDGE_RISING:	string = "rising\n";	break;
	default:
	case DIMINUTO_PIN_EDGE_FALLING:	string = "falling\n";	break;
	case DIMINUTO_PIN_EDGE_BOTH:	string = "both\n";		break;
	}

	rc = diminuto_pin_configure(ROOT_CLASS_GPIO_PIN_EDGE, pin, string);
	if (rc < 0) {
		diminuto_perror("diminuto_pin_edge");
	}

	return rc;
}

int diminuto_pin_direction(int pin, int output)
{
	int rc = 0;

	rc = diminuto_pin_configure(ROOT_CLASS_GPIO_PIN_DIRECTION, pin, output ? "out\n" : "in\n");
	if (rc < 0) {
		diminuto_perror("diminuto_pin_direction");
	}

	return rc;
}

int diminuto_pin_initialize(int pin, int high)
{
	int rc = 0;

	rc = diminuto_pin_configure(ROOT_CLASS_GPIO_PIN_DIRECTION, pin, high ? "high\n" : "low\n");
	if (rc < 0) {
		diminuto_perror("diminuto_pin_initialize");
	}

	return rc;
}

FILE * diminuto_pin_open(int pin)
{
	FILE * fp = (FILE *)0;
	char filename[PATH_MAX];

	do {

		if (pin < 0) {
			errno = EINVAL;
			diminuto_perror("diminuto_pin_open: pin");
			break;
		}

		snprintf(filename, sizeof(filename), ROOT_CLASS_GPIO_PIN_VALUE, root, pin);

		if ((fp = fopen(filename, "r+")) == (FILE *)0) {
			diminuto_perror("diminuto_pin_open: fopen");
			break;
		}

	} while (0);

	return fp;
}

FILE * diminuto_pin_close(FILE * fp)
{
	if (fp == (FILE *)0) {
		/* Do nothing. */
	} else if (fclose(fp) == 0) {
		fp = (FILE *)0;
	} else {
		diminuto_perror("diminuto_pin_close");
	}

	return fp;
}

FILE * diminuto_pin_input(int pin)
{
	FILE * fp = (FILE *)0;

	if (diminuto_pin_export(pin) < 0) {
		/* Do nothing. */
	} else if (diminuto_pin_direction(pin, 0) < 0) {
		/* Do nothing. */
	} else if ((fp = diminuto_pin_open(pin)) == (FILE *)0) {
		/* Do nothing. */
	} else {
		/* Do nothing. */
	}

	return fp;
}

FILE * diminuto_pin_output(int pin)
{
	FILE * fp = (FILE *)0;

	if (diminuto_pin_export(pin) < 0) {
		/* Do nothing. */
	} else if (diminuto_pin_direction(pin, !0) < 0) {
		/* Do nothing. */
	} else if ((fp = diminuto_pin_open(pin)) == (FILE *)0) {
		/* Do nothing. */
	} else {
		/* Do nothing. */
	}

	return fp;
}

FILE * diminuto_pin_unused(FILE * fp, int pin)
{
	if (fp == (FILE *)0) {
		/* Do nothing. */
	} else if (diminuto_pin_unexport(pin) < 0) {
		/* Do nothing. */
	} else if (diminuto_pin_close(fp) != (FILE *)0) {
		/* Do nothing. */
	} else {
		fp = (FILE *)0;
	}

	return fp;
}

/*
 * The put/get functions below were altered from using the buffered
 * standard I/O library function calls (which are right out of the
 * examples in the documentation for the /proc GPIO interface at
 * the time I originally wrote this code) to using straight system
 * calls. The former, when used at very high calling frequency in
 * the modulator feature, sometimes produced an EINVAL error. I have
 * yet to see the system calls do this under the same test load. The
 * new implementation also passes the pintest functional test.
 */

int diminuto_pin_put(FILE * fp, int assert)
{
	int rc = -1;

	do {

#if 0
		if (fseek(fp, 0L, SEEK_SET) < 0) {
			diminuto_perror("diminuto_pin_put: fseek");
			break;
		}

		if (fputs(assert ? "1\n" : "0\n", fp) == EOF) {
			diminuto_perror("diminuto_pin_put: fputs");
			break;
		}

		if (fflush(fp) == EOF) {
			diminuto_perror("diminuto_pin_put: fflush");
			break;
		}
#else
		int fd = -1;

		fd = fileno(fp);

		if (lseek(fd, 0, SEEK_SET) < 0) {
			diminuto_perror("diminuto_pin_put: lseek");
			break;
		}

		if (write(fd, assert ? "1\n" : "0\n", sizeof("X\n") - 1) < 0) {
			diminuto_perror("diminuto_pin_put: write");
			break;
		}
#endif
		rc = 0;

	} while (0);

	return rc;
}

int diminuto_pin_get(FILE * fp)
{
	int value = -1;
	int rc = -1;

	do {

#if 0
		if (fseek(fp, 0, SEEK_SET) < 0) {
			diminuto_perror("diminuto_pin_get: fseek");
			break;
		}

		if ((rc = fscanf(fp, "%d\n", &value)) == EOF) {
			diminuto_perror("diminuto_pin_get: fscanf");
			break;
		}

		if (rc < 1) {
			errno = EAGAIN;
			diminuto_perror("diminuto_pin_get: fscanf");
			break;
		}

		value = !!value;
#else
		int fd = -1;
		char buffer[3] = { 0 };
		ssize_t size = 0;

		fd = fileno(fp);

		if (lseek(fd, 0, SEEK_SET) < 0) {
			diminuto_perror("diminuto_pin_get: lseek");
			break;
		}

		if ((size = read(fd, buffer, sizeof(buffer) - 1)) < 0) {
			diminuto_perror("diminuto_pin_get: read");
			break;
		}

		if (size == 0) {
			errno = EAGAIN;
			diminuto_perror("diminuto_pin_get: read");
			break;
		}

		if (buffer[0] == '0') {
			value = 0;
		} else if (buffer[0] == '1') {
			value = !0;
		} else {
			errno = EBADMSG;
			diminuto_perror("diminuto_pin_get: read");
		}
#endif

		rc = 0;

	} while (0);

	return value;
}
