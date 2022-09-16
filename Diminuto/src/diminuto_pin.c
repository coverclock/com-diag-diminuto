/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2013-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the implementation of the Pin feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the implementation of the Pin feature.
 *
 * DEPRECATED
 *
 * The Pin feature, which makes it easy to manipulate General Purpose
 * Input/Output (GPIO) pins, uses the deprecated /sysfs ABI. I'm looking
 * at the new ABIs. While the new ABIs offer a higher level interface, my
 * initial impression is that they don't easily support the kinds of weird
 * GPIO doings with special devices that I am often called upon to do.
 *
 * REFERENCES
 *
 * Linux 3.10.24, "GPIO Interfaces", Documentation/gpio.txt
 */

#include "com/diag/diminuto/diminuto_pin.h"
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_log.h"
#if 0
#   define COM_DIAG_DIMINUTO_DEBUG !0
#endif
#include "com/diag/diminuto/diminuto_debug.h"
#include <errno.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

diminuto_ticks_t diminuto_pin_delay = COM_DIAG_DIMINUTO_FREQUENCY / 4; /* 250ms */

int diminuto_pin_tries = 8; /* 2s */

static const char ROOT[] = "/sys";

static const char * root = ROOT;

const char DIMINUTO_PIN_ROOT_CLASS_GPIO_EXPORT[] = "%s/class/gpio/export";

const char DIMINUTO_PIN_ROOT_CLASS_GPIO_PIN_DIRECTION[] = "%s/class/gpio/gpio%u/direction";

const char DIMINUTO_PIN_ROOT_CLASS_GPIO_PIN_ACTIVELOW[] = "%s/class/gpio/gpio%u/active_low";

const char DIMINUTO_PIN_ROOT_CLASS_GPIO_PIN_EDGE[] = "%s/class/gpio/gpio%u/edge";

const char DIMINUTO_PIN_ROOT_CLASS_GPIO_PIN_VALUE[] = "%s/class/gpio/gpio%u/value";

const char DIMINUTO_PIN_ROOT_CLASS_GPIO_UNEXPORT[] = "%s/class/gpio/unexport";

const char * diminuto_pin_debug(const char * tmp)
{
    const char * prior;

    prior = root;
    root = (tmp != (const char *)0) ? tmp : ROOT;

    return prior;
}

int diminuto_pin_configure_generic(const char * format, int pin, const char * string, int ignore, int tries, diminuto_ticks_t delay)
{
    int result = -1;
    int rc = -1;
    diminuto_path_t filename = { '\0', };
    struct stat status = { 0, };
    FILE * fp = (FILE *)0;
    int tt = 0;

    do {

        DPRINTF("configure pin=%d\n", pin);

        if (pin < 0) {
            errno = EINVAL;
            diminuto_perror("diminuto_pin_configure_generic: pin");
            break;
        }

        snprintf(filename, sizeof(filename), format, root, pin);
        if (filename[sizeof(filename) - 1] != '\0') {
            filename[sizeof(filename) - 1] = '\0';
            errno = EINVAL;
            diminuto_perror(filename);
            break;
        }

        DPRINTF("configure filename=\"%s\"\n", filename);

        for (tt = tries; tt > 0; --tt) {

            DPRINTF("configure tries=%d\n", tt);

            if ((rc = stat(filename, &status)) >= 0) {
                /* Do nothing. */
            } else if (errno == ENOENT) {
                /* Do nothing. */
            } else if (errno == EPERM) {
                /* Do nothing. */
            } else if (errno == EACCES) {
                /* Do nothing. */
            } else {
                break;
            }

            DPRINTF("configure stat=%d errno=%d\n", rc, errno);

            if (rc >= 0) {
                /* Do nothing. */
            } else if (tt > 1) {
                diminuto_delay(diminuto_pin_delay, 0);
                continue;
            } else {
                break;
            }

            if ((fp = fopen(filename, "w")) != (FILE *)0) {
                /* Do nothing. */
            } else if (errno == ENOENT) {
                /* Do nothing. */
            } else if (errno == EPERM) {
                /* Do nothing. */
            } else if (errno == EACCES) {
                /* Do nothing. */
            } else {
                break;
            }

            DPRINTF("configure fopen=%p errno=%d\n", fp, errno);

            if (fp != (FILE *)0) {
                break;
            } else if (tt > 1) {
                diminuto_delay(diminuto_pin_delay, 0);
                continue;
            } else {
                break;
            }

        }

        if (fp == (FILE *)0) {
            diminuto_perror(filename);
            break;
        }

        DPRINTF("configure string=\"%s\"\n", string);

        if ((rc = fputs(string, fp)) != EOF) {
            /* Do nothing. */
        } else if (errno != EINVAL) {
            diminuto_perror(filename);
            break;
        } else if (ignore) {
            /* Do nothing. */
        } else {
            diminuto_perror(filename);
            break;
        }

        DPRINTF("configure fputs=%d errno=%d\n", rc, errno);

        if ((rc = fflush(fp)) != EOF) {
            /* Do nothing. */
        } else if (errno != EINVAL) {
            diminuto_perror(filename);
            break;
        } else if (ignore) {
            /* Do nothing. */
        } else {
            diminuto_perror(filename);
            break;
        }

        DPRINTF("configure fputs=%d errno=%d\n", rc, errno);

        result = 0;

    } while (0);

    if (fp == (FILE *)0) {
        /* Do nothing. */
    } else if (fclose(fp) != EOF) {
        /* Do nothing. */
    } else {
        diminuto_perror(filename);
        result = -1;
    }

    DPRINTF("configure result=%d\n", result);

    return result;
}

int diminuto_pin_port_generic(const char * format, int pin, int ignore)
{
    char buffer[sizeof("-9223372036854775807\n")];
    snprintf(buffer, sizeof(buffer), "%d\n", pin);
    return diminuto_pin_configure_generic(format, pin, buffer, ignore, 1, 0);
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

    return diminuto_pin_configure(DIMINUTO_PIN_ROOT_CLASS_GPIO_PIN_EDGE, pin, string);
}

FILE * diminuto_pin_open(int pin, int output)
{
    FILE * fp = (FILE *)0;
    diminuto_path_t filename;
    int rc = -1;
    struct stat status;
    const char * mode = (const char *)0;

    do {

        DPRINTF("open pin=%d\n", pin);

        if (pin < 0) {
            errno = EINVAL;
            diminuto_perror("diminuto_pin_open: pin");
            break;
        }

        snprintf(filename, sizeof(filename), DIMINUTO_PIN_ROOT_CLASS_GPIO_PIN_VALUE, root, pin);

        DPRINTF("open filename=\"%s\"\n", filename);

        if ((rc = stat(filename, &status)) < 0) {
            diminuto_perror(filename);
            break;
        }

        DPRINTF("open stat=%d\n", rc);

        if ((fp = fopen(filename, mode = output ? "w" : "r")) == (FILE *)0) {
            diminuto_perror(filename);
            break;
        }

        DPRINTF("open fopen=%p mode=\"%c\"\n", fp, mode[0]);

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
        diminuto_perror("diminuto_pin_close: fclose");
    }

    DPRINTF("close fclose=%p\n, fp");

    return fp;
}

FILE * diminuto_pin_setup(int pin, int output)
{
    FILE * fp = (FILE *)0;

    if (diminuto_pin_export(pin) < 0) {
        /* Do nothing. */
    } else if (diminuto_pin_direction(pin, output) < 0) {
        /* Do nothing. */
    } else {
        fp = diminuto_pin_open(pin, output);
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
 * standard I/O library function calls (which are based on the
 * examples in the documentation for the sysfs GPIO interface at
 * the time I originally wrote this code) to using straight system
 * calls. The former, when used at very high calling frequency in
 * the modulator feature, sometimes produced an EINVAL error. I have
 * yet to see the system calls do this under the same test load. The
 * new implementation also passes the pintest functional test.
 */

int diminuto_pin_put(FILE * fp, int assert)
{
    int result = -1;
    int rc = -1;
    ssize_t size = 0;
    const char * value = (const char *)0;

    do {

        int fd = -1;

        fd = fileno(fp);

        DPRINTF("put fp=%p fd=%d assert=%d\n", fp, fd, assert);

        if ((rc = lseek(fd, 0, SEEK_SET)) < 0) {
            diminuto_perror("diminuto_pin_put: lseek");
            break;
        }

        DPRINTF("put lseek=%d\n", rc);

        if ((size = write(fd, value = assert ? "1\n" : "0\n", sizeof("X\n") - 1)) < 0) {
            diminuto_perror("diminuto_pin_put: write");
            break;
        }

        DPRINTF("put write=%zd value=\"%c\"\n", size, value[0]);

        result = 0;

    } while (0);

    DPRINTF("put result=%d\n", result);

    return result;
}

int diminuto_pin_get(FILE * fp)
{
    int value = -1;
    int rc = -1;
    ssize_t size = 0;

    do {

        int fd = -1;
        char buffer[sizeof("X\n")] = { 0 };
        ssize_t size = 0;

        fd = fileno(fp);

        DPRINTF("get fp=%p fd=%d\n", fp, fd);

        if ((rc = lseek(fd, 0, SEEK_SET)) < 0) {
            diminuto_perror("diminuto_pin_get: lseek");
            break;
        }

        DPRINTF("get lseek=%d\n", rc);

        if ((size = read(fd, buffer, sizeof(buffer) - 1)) < 0) {
            diminuto_perror("diminuto_pin_get: read");
            break;
        }

        DPRINTF("get read=%zd\n", size);

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
            break;
        }

    } while (0);

    DPRINTF("get value=%d\n", value);

    return value;
}
