/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013-2014 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * REFERENCES
 *
 * "GPIO Interfaces", Documentation/gpio.txt, Linux 3.10.24
 */

#include "com/diag/diminuto/diminuto_pin.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <errno.h>
#include <sys/param.h>

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
	root = (tmp != 0) ? tmp : ROOT;
	return prior;
}

static int diminuto_pin_configure(const char * format, int pin, const char * string, const char * name)
{
	int rc = -1;
	char filename[PATH_MAX];
	FILE * fp = (FILE *)0;

	do {

		if (pin < 0) {
			errno = EINVAL;
			break;
		}

		snprintf(filename, sizeof(filename), format, root, pin);

		if ((fp = fopen(filename, "w")) == (FILE *)0) {
			break;
		}

		if (fputs(string, fp) < 0) {
			break;
		}

		if (fflush(fp) == EOF) {
			break;
		}

		rc = 0;

	} while (0);

	if (fp == (FILE *)0) {
		/* Do nothing. */
	} else if (fclose(fp) == 0) {
		/* Do nothing. */
	} else {
		rc = -1;
	}

	if (rc != 0) {
		diminuto_perror(name);
	}

	return rc;
}

static int diminuto_pin_port(const char * format, int pin, const char * name)
{
	char buffer[sizeof("-9223372036854775807\n")];
	snprintf(buffer, sizeof(buffer), "%d\n", pin);
	return diminuto_pin_configure(format, pin, buffer, name);
}

int diminuto_pin_export(int pin)
{
	return diminuto_pin_port(ROOT_CLASS_GPIO_EXPORT, pin, "diminuto_pin_export");
}

int diminuto_pin_unexport(int pin)
{
	return diminuto_pin_port(ROOT_CLASS_GPIO_UNEXPORT, pin, "diminuto_pin_unexport");
}

int diminuto_pin_active(int pin, int high)
{
	return diminuto_pin_configure(ROOT_CLASS_GPIO_PIN_ACTIVELOW, pin, high ? "0\n" : "1\n", "diminuto_pin_active");
}

int diminuto_pin_edge(int pin, diminuto_pin_edge_t edge)
{
	const char * string;
	switch (edge) {
	case DIMINUTO_PIN_EDGE_NONE:	string = "none\n";		break;
	case DIMINUTO_PIN_EDGE_RISING:	string = "rising\n";		break;
	default:
	case DIMINUTO_PIN_EDGE_FALLING:	string = "falling\n";	break;
	case DIMINUTO_PIN_EDGE_BOTH:	string = "both\n";		break;
	}
	return diminuto_pin_configure(ROOT_CLASS_GPIO_PIN_EDGE, pin, string, "diminuto_pin_edge");
}

int diminuto_pin_direction(int pin, int output)
{
	return diminuto_pin_configure(ROOT_CLASS_GPIO_PIN_DIRECTION, pin, output ? "out\n" : "in\n", "diminuto_pin_direction");
}

int diminuto_pin_initialize(int pin, int high)
{
	return diminuto_pin_configure(ROOT_CLASS_GPIO_PIN_DIRECTION, pin, high ? "high\n" : "low\n", "diminuto_pin_initialize");
}

FILE * diminuto_pin_open(int pin)
{
	FILE * fp = (FILE *)0;
	char filename[PATH_MAX];

	do {

		if (pin < 0) {
			errno = EINVAL;
			break;
		}

		snprintf(filename, sizeof(filename), ROOT_CLASS_GPIO_PIN_VALUE, root, pin);

		if ((fp = fopen(filename, "r+")) == (FILE *)0) {
			break;
		}

	} while (0);

	if (fp == (FILE *)0) {
		diminuto_perror("diminuto_pin_open");
	}

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

int diminuto_pin_put(FILE * fp, int assert)
{
	int rc = -1;

	do {

		if (fseek(fp, 0L, SEEK_SET) < 0) {
			break;
		}

		if (fputs(assert ? "1\n" : "0\n", fp) == EOF) {
			break;
		}

		if (fflush(fp) == EOF) {
			break;
		}

		rc = 0;

	} while (0);

	if (rc != 0) {
		diminuto_perror("diminuto_pin_put");
	}

	return rc;
}

int diminuto_pin_get(FILE * fp)
{
	int value = -1;
	int rc;

	do {

		if (fseek(fp, 0, SEEK_SET) < 0) {
			break;
		}

		if ((rc = fscanf(fp, "%d\n", &value)) == EOF) {
			break;
		}

		if (rc < 1) {
			errno = EAGAIN;
			break;
		}

		value = !!value;

		rc = 0;

	} while (0);

	if (rc != 0) {
		diminuto_perror("diminuto_pin_get");
	}

	return value;
}

int diminuto_pin_set(FILE * fp)
{
	return diminuto_pin_put(fp, !0);
}

int diminuto_pin_clear(FILE * fp)
{
	return diminuto_pin_put(fp, 0);
}
