/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2013 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock (coverclock@diag.com)<BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 */

#include "diminuto_pin.h"
#include "com/diag/diminuto/diminuto_pin.h"
#include "com/diag/diminuto/diminuto_log.h"
#include <errno.h>

static const char SYS_CLASS_GPIO_EXPORT[] = "/sys/class/gpio/export";
static const char SYS_CLASS_GPIO_UNEXPORT[] = "/sys/class/gpio/unexport";
static const char SYS_CLASS_GPIO_DIRECTION[] = "/sys/class/gpio/gpio%u/direction";
static const char SYS_CLASS_GPIO_VALUE[] = "/sys/class/gpio/gpio%u/value";

static const char TMP_CLASS_GPIO_EXPORT[] = "/tmp/class/gpio/export";
static const char TMP_CLASS_GPIO_UNEXPORT[] = "/tmp/class/gpio/unexport";
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

int diminuto_pin_export(int pin)
{
	int rc = -1;
	FILE * fp = (FILE *)0;

	do {

		if (pin < 0) {
			errno = EINVAL;
			break;
		}

		if ((fp = fopen(debug ? TMP_CLASS_GPIO_EXPORT : SYS_CLASS_GPIO_EXPORT, "a")) == (FILE *)0) {
			break;
		}

		if (fseek(fp, 0L, SEEK_SET) < 0) {
			break;
		}

		if (fprintf(fp, "%d\n", pin) < 0) {
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
		diminuto_perror("diminuto_pin_export");
	}

	return rc;
}

int diminuto_pin_unexport(int pin)
{
	int rc = -1;
	FILE * fp = (FILE *)0;

	do {

		if (pin < 0) {
			errno = EINVAL;
			break;
		}

		if ((fp = fopen(debug ? TMP_CLASS_GPIO_UNEXPORT : SYS_CLASS_GPIO_UNEXPORT, "a")) == (FILE *)0) {
			break;
		}

		if (fseek(fp, 0L, SEEK_SET) < 0) {
			break;
		}

		if (fprintf(fp, "%d\n", pin) < 0) {
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
		diminuto_perror("diminuto_pin_unexport");
	}

	return rc;
}

int diminuto_pin_direction(int pin, int output)
{
	int rc = -1;
	FILE * fp = (FILE *)0;
	char filename[sizeof(SYS_CLASS_GPIO_DIRECTION) + sizeof("2147483647") + 1];

	do {

		if (pin < 0) {
			errno = EINVAL;
			break;
		}

		snprintf(filename, sizeof(filename), debug ? TMP_CLASS_GPIO_DIRECTION : SYS_CLASS_GPIO_DIRECTION, pin);

		if ((fp = fopen(filename, "a")) == (FILE *)0) {
			break;
		}

		if (fseek(fp, 0L, SEEK_SET) < 0) {
			break;
		}

		if (fputs(output ? "out\n" : "in\n", fp) < 0) {
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
		diminuto_perror("diminuto_pin_direction");
	}

	return rc;
}

FILE * diminuto_pin_open(int pin)
{
	FILE * fp = (FILE *)0;
	char filename[sizeof(SYS_CLASS_GPIO_DIRECTION) + sizeof("2147483647") + 1];

	do {

		if (pin < 0) {
			errno = EINVAL;
			break;
		}

		snprintf(filename, sizeof(filename), debug ? TMP_CLASS_GPIO_VALUE : SYS_CLASS_GPIO_VALUE, pin);

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
	fp = diminuto_pin_close(fp);
	diminuto_pin_unexport(pin);

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
