/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "com/diag/diminuto/diminuto_pin.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <errno.h>

static const char SYS_CLASS_GPIO_EXPORT[] = "/sys/class/gpio/export";
static const char SYS_CLASS_GPIO_DIRECTION[] = "/sys/class/gpio/gpio%u/direction";
static const char SYS_CLASS_GPIO_VALUE[] = "/sys/class/gpio/gpio%u/value";

static const char TMP_CLASS_GPIO_EXPORT[] = "/tmp/class/gpio/export";
static const char TMP_CLASS_GPIO_DIRECTION[] = "/tmp/class/gpio/gpio%u/direction";
static const char TMP_CLASS_GPIO_VALUE[] = "/tmp/class/gpio/gpio%u/value";

static int debug = 0;

int diminuto_pin_debug(int enable)
{
	int prior;
	prior = debug;
	debug = enable;
	return prior;
}

FILE * diminuto_pin_open(int pin, int output)
{
	int success = 0;
	FILE * fp = (FILE *)0;
	const char * path;
	char filename[sizeof(SYS_CLASS_GPIO_DIRECTION) + sizeof("4294967295")];

	do {

		path = "pin";
		if (pin < 0) {
			break;
		}

		path = debug ? TMP_CLASS_GPIO_EXPORT : SYS_CLASS_GPIO_EXPORT;
		if ((fp = fopen(path, "w")) == (FILE *)0) {
			break;
		}

		if (fprintf(fp, "%u\n", pin) < 0) {
			break;
		}

		if (fclose(fp) == EOF) {
			break;
		}

		path = debug ? TMP_CLASS_GPIO_DIRECTION : SYS_CLASS_GPIO_DIRECTION;
		snprintf(filename, sizeof(filename), path, pin);
		if ((fp = fopen(filename, "w")) == (FILE *)0) {
			diminuto_perror(filename);
			break;
		}

		if (fputs(output ? "output\n" : "input\n", fp) == EOF) {
			break;
		}

		if (fclose(fp) == EOF) {
			break;
		}

		path = debug ? TMP_CLASS_GPIO_VALUE : SYS_CLASS_GPIO_VALUE;
		snprintf(filename, sizeof(filename), path, pin);
		if ((fp = fopen(filename, output ? "w" : "r")) == (FILE *)0) {
			break;
		}

		success = !0;

	} while (0);

	if (success) {
		/* Do nothing. */
	} else if (fp == (FILE *)0) {
		/* Do nothing. */
	} else {
		fclose(fp);
	}

	if (!success) {
		errno = ENOENT;
		diminuto_perror(path);
		fp = (FILE *)0;
	}

	return fp;
}

FILE * diminuto_pin_input(int pin)
{
	return diminuto_pin_open(pin, 0);
}

FILE * diminuto_pin_output(int pin)
{
	return diminuto_pin_open(pin, !0);
}

int diminuto_pin_set(FILE * fp, int assert)
{
	int rc = EOF;
	const char * value;

	do {

		if (fp == (FILE *)0) {
			break;
		}

		rewind(fp);

		value = assert ? "1\n" : "0\n";
		if (fputs(value, fp) == EOF) {
			break;
		}

		rc = 0;

	} while (0);

	if (rc < 0) {
		errno = EINVAL;
		diminuto_perror("diminuto_pin_set");
	}

	return rc;
}

int diminuto_pin_get(FILE * fp)
{
	int value = EOF;
	int rc;

	do {

		if (fp == (FILE *)0) {
			break;
		}

		rewind(fp);

		if ((rc = fscanf(fp, "%u", &value)) == EOF) {
			break;
		}

		if (rc < 1) {
			break;
		}

		value = value ? !0 : 0;

	} while (0);

	if (value < 0) {
		errno = EINVAL;
		diminuto_perror("diminuto_pin_get");
	}

	return value;
}
