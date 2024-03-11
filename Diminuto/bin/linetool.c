/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2014-2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Manipulate GPIO pins using the ioctl ABI.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * EXAMPLES
 *
 * linetool -D /dev/gpiochip4 -p 20 -i -x -b 10000<BR>
 * linetool -D /dev/gpiochip4 -p 20 -i -B -x -U -M<BR>
 * linetool -D /dev/gpiochip4 -p 20 -i -B -U -x -m 1000000<BR>
 * linetool -D /dev/gpiochip4 -p 160 -o -H -x -r -s -r -u 5000000 -r -c -r -p 161 -x -h -r -w 0 -r -u 5000000 -r -w 1 -r -p 160 -n -p 161 -n<BR>
 *
 * ABSTRACT
 *
 * Allows manipulation of general purpose input/output (GPIO) pins using the
 * lower level calls of the line facility. Uses the iotcl GPIO driver interface.
 * Probably must be run as root or, on some platforms (e.g. Raspian on the
 * Raspberry Pi) as a user in the gpio group (e.g. pi).
 *
 * Note that although the syntax of linetool looks a lot like taht of pintool,
 * the semantics are completely different. See the scripts that use linetool
 * for examples.
 *
 * Note that how a particularly GPIO line may be configured and what operations
 * may be performed on it depends on the underlying hardware and how its device
 * driver is implemented and configured.
 *
 * The utility strace(1) was useful when testing this.
 */

#include "com/diag/diminuto/diminuto_line.h"
#include "com/diag/diminuto/diminuto_number.h"
#include "com/diag/diminuto/diminuto_string.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_mux.h"
#include "com/diag/diminuto/diminuto_poll.h"
#include "com/diag/diminuto/diminuto_cue.h"
#include "com/diag/diminuto/diminuto_daemon.h"
#include "com/diag/diminuto/diminuto_error.h"
#include "com/diag/diminuto/diminuto_minmaxof.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>

static void usage(const char * program)
{
    fprintf(stderr, "usage: %s [ -d ] [ -S ] [ [ -D DEVICE ] [ -p LINE ] [ -P PATH:LINE ] ] [ -x ] [ -i | -o ] [ -h | -l ] [ -N | -R | -F | -B ] [ -1 ] [ -b USECONDS ] [ -X COMMAND ] [ -r | -m USECONDS | -M | -b USECONDS | -w BOOLEAN | -s | -c ] [ -U ] [ -t | -f ] [ -u USECONDS ] [ -n | -e ] [ ... ]\n", program);
    fprintf(stderr, "       -1              Read LINE initially when multiplexing.\n");
    fprintf(stderr, "       -B              Configure LINE edge to both.\n");
    fprintf(stderr, "       -D DEVICE       Close and use DEVICE:LINE subsequently.\n");
    fprintf(stderr, "       -F              Configure context edge to falling.\n");
    fprintf(stderr, "       -H              Configure context active to high.\n");
    fprintf(stderr, "       -L              Configure context active to low.\n");
    fprintf(stderr, "       -M              Multiplex upon edge.\n");
    fprintf(stderr, "       -N              Configure context edge to none.\n");
    fprintf(stderr, "       -P DEVICE:LINE  Close and use DEVICE:LINE subsequently.\n");
    fprintf(stderr, "       -R              Configure context edge to rising.\n");
    fprintf(stderr, "       -S              Daemonize.\n");
    fprintf(stderr, "       -U              Filter out non-unique edge changes.\n");
    fprintf(stderr, "       -X COMMAND      Execute COMMAND DEVICE LINE STATE PRIOR in shell when multiplexed edge changes.\n");
    fprintf(stderr, "       -b USECONDS     Poll with debounce every USECONDS (try 10000) microseconds.\n");
    fprintf(stderr, "       -c              Clear LINE by writing 0.\n");
    fprintf(stderr, "       -d              Enable debug mode.\n");
    fprintf(stderr, "       -e              Close LINE but keep context.\n");
    fprintf(stderr, "       -f              Proceed if the last LINE state was 0.\n");
    fprintf(stderr, "       -h              Configure context direction to output and active to high.\n");
    fprintf(stderr, "       -i              Configure context direction to input.\n");
    fprintf(stderr, "       -l              Configure context direction to output and active to low.\n");
    fprintf(stderr, "       -m USECONDS     Multiplex upon LINE edge or until USECONDS microseconds.\n");
    fprintf(stderr, "       -n              Close LINE and clear context.\n");
    fprintf(stderr, "       -o              Configure context direction to output.\n");
    fprintf(stderr, "       -p LINE         Close and use DEVICE:LINE subsequently.\n");
    fprintf(stderr, "       -r              Read LINE and print to standard output.\n");
    fprintf(stderr, "       -s              Set LINE by writing !0.\n");
    fprintf(stderr, "       -t              Proceed if the last LINE state was !0.\n");
    fprintf(stderr, "       -u USECONDS     Delay for USECONDS microseconds.\n");
    fprintf(stderr, "       -w BOOLEAN      Write BOOLEAN to LINE.\n");
    fprintf(stderr, "       -x              Configure LINE with context and open.\n");
    fprintf(stderr, "       -?              Print menu.\n");
}

int main(int argc, char * argv[])
{
    int xc = 0;
    const char * program = "linetool";
    int done = 0;
    int fail = 0;
    int error = 0;
    int debug = 0;
    int first = 0;
    int unique = 0;
    int state = 0;
    int prior = 0;
    int active = 0;
    int edge = 0;
    uint64_t flags = 0x0;
    const char * path = (const char *)0;
    unsigned int line = maximumof(unsigned int);
    int sline = -1;
    const char * command = (const char *)0;
    diminuto_unsigned_t uvalue = 0;
    diminuto_signed_t svalue = -1;
    diminuto_mux_t mux = { 0 };
    diminuto_ticks_t ticks = 0;
    diminuto_sticks_t sticks = -2;
    diminuto_cue_state_t cue = { 0 };
    int fd = -1;
    int ready = -1;
    int nfds = 0;
    int rc = -1;
    char sopt[2] = { '\0', '\0' };
    char buffer[1024] = { '\0' };
    int opt = '\0';
    extern char * optarg;

    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

    diminuto_line_consumer(__FILE__);

    while ((opt = getopt(argc, argv, "1BD:FHLMNP:RSUX:b:cdefhilm:nop:rstu:vw:x?")) >= 0) {

        sopt[0] = opt;

        uvalue = 0;
        svalue = -1;

        switch (opt) {

        case '1':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            first = !0;
            break;

        case 'B':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            flags |= DIMINUTO_LINE_FLAG_EDGE_RISING;
            flags |= DIMINUTO_LINE_FLAG_EDGE_FALLING;
            if (debug) { fprintf(stderr, "%s (\"%s\" %u 0x%llx)\n", program, (path == (const char *)0) ? "" : path, line, (unsigned long long)flags); }
            break;

        case 'D':
            if (debug) { fprintf(stderr, "%s -%c \"%s\"\n", program, opt, optarg); }
            path = optarg;
            if (debug) { fprintf(stderr, "%s (\"%s\" %u 0x%llx)\n", program, (path == (const char *)0) ? "" : path, line, (unsigned long long)flags); }
            if ((fd = diminuto_line_close(fd)) >= 0) {
                fail = !0;
            }
            break;

        case 'F':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            flags &= ~DIMINUTO_LINE_FLAG_EDGE_RISING;
            flags |= DIMINUTO_LINE_FLAG_EDGE_FALLING;
            if (debug) { fprintf(stderr, "%s (\"%s\" %u 0x%llx)\n", program, (path == (const char *)0) ? "" : path, line, (unsigned long long)flags); }
            break;

        case 'H':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            flags &= ~DIMINUTO_LINE_FLAG_ACTIVE_LOW;
            if (debug) { fprintf(stderr, "%s (\"%s\" %u 0x%llx)\n", program, (path == (const char *)0) ? "" : path, line, (unsigned long long)flags); }
            break;

        case 'L':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            flags |= DIMINUTO_LINE_FLAG_ACTIVE_LOW;
            if (debug) { fprintf(stderr, "%s (\"%s\" %u 0x%llx)\n", program, (path == (const char *)0) ? "" : path, line, (unsigned long long)flags); }
            break;

        case 'N':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            flags &= ~DIMINUTO_LINE_FLAG_EDGE_RISING;
            flags &= ~DIMINUTO_LINE_FLAG_EDGE_FALLING;
            if (debug) { fprintf(stderr, "%s (\"%s\" %u 0x%llx)\n", program, (path == (const char *)0) ? "" : path, line, (unsigned long long)flags); }
            break;

        case 'P':
            if (debug) { fprintf(stderr, "%s -%c %s\n", program, opt, optarg); }
            path = diminuto_line_parse(optarg, &sline);
            if (sline < 0) {
                flags |= DIMINUTO_LINE_FLAG_ACTIVE_LOW;
                line = -sline;
            } else {
                line = sline;
            }
            if (debug) { fprintf(stderr, "%s (\"%s\" %u 0x%llx)\n", program, (path == (const char *)0) ? "" : path, line, (unsigned long long)flags); }
            if ((fd = diminuto_line_close(fd)) >= 0) {
                fail = !0;
                break;
            }
            break;

        case 'R':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            flags |= DIMINUTO_LINE_FLAG_EDGE_RISING;
            flags &= ~DIMINUTO_LINE_FLAG_EDGE_FALLING;
            if (debug) { fprintf(stderr, "%s (\"%s\" %u 0x%llx)\n", program, (path == (const char *)0) ? "" : path, line, (unsigned long long)flags); }
            break;

        case 'S':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            rc = diminuto_daemon(program);
            if (rc < 0) {
                fail = !0;
            }
            break;

        case 'U':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            unique = !0;
            break;

        case 'X':
            if (debug) { fprintf(stderr, "%s -%c \"%s\"\n", program, opt, optarg); }
            command = optarg;
            break;

        case 'b':
            if (debug) { fprintf(stderr, "%s -%c %s\n", program, opt, optarg); }
            if ((*diminuto_number_unsigned(optarg, &uvalue) != '\0')) {
                diminuto_perror(optarg);
                error = !0;
                break;
            }
            ticks = uvalue;
            ticks *= diminuto_frequency();
            ticks /= 1000000;
            if (fd < 0) {
                errno = EBADF;
                diminuto_perror(sopt);
                fail = !0;
                break;
            }
            if ((state = diminuto_line_get(fd)) < 0) {
                fail = !0;
                break;
            }
            printf("%d\n", state);
            fflush(stdout);
            diminuto_cue_init(&cue, state);
            while (!0) {
                prior = state;
                diminuto_delay(ticks, 0);
                if ((state = diminuto_line_get(fd)) < 0) {
                    fail = !0;
                    break;
                }
                state = diminuto_cue_debounce(&cue, !!state);
                if (state != prior) {
                    printf("%d\n", state);
                    fflush(stdout);
                }
            }
            if (fail) {
                break;
            }
            break;

        case 'c':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            state = 0;
            if (fd < 0) {
                errno = EBADF;
                diminuto_perror(sopt);
                fail = !0;
            } else if (diminuto_line_put(fd, state) < 0) {
                fail = !0;
            } else {
                /* Do nothing. */
            }
            break;

        case 'd':
            debug = !0;
            fprintf(stderr, "%s -%c\n", program, opt);
            fprintf(stderr, "%s (\"%s\" %u 0x%llx)\n", program, (path == (const char *)0) ? "" : path, line, (unsigned long long)flags);
            break;

        case 'e':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            if ((fd = diminuto_line_close(fd)) >= 0) {
                fail = !0;
            }
            break;

        case 'f':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            done = !!state;
            break;

        case 'h':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            flags = DIMINUTO_LINE_FLAG_OUTPUT;
            flags &= ~DIMINUTO_LINE_FLAG_ACTIVE_LOW;
            if (debug) { fprintf(stderr, "%s (\"%s\" %u 0x%llx)\n", program, (path == (const char *)0) ? "" : path, line, (unsigned long long)flags); }
            break;

        case 'i':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            flags |= DIMINUTO_LINE_FLAG_INPUT;
            if (debug) { fprintf(stderr, "%s (\"%s\" %u 0x%llx)\n", program, (path == (const char *)0) ? "" : path, line, (unsigned long long)flags); }
            break;

        case 'l':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            flags = DIMINUTO_LINE_FLAG_OUTPUT;
            flags |= DIMINUTO_LINE_FLAG_ACTIVE_LOW;
            if (debug) { fprintf(stderr, "%s (\"%s\" %u 0x%llx)\n", program, (path == (const char *)0) ? "" : path, line, (unsigned long long)flags); }
            break;

        case 'm':
            if (debug) { fprintf(stderr, "%s -%c %s\n", program, opt, optarg); }
            if ((*diminuto_number_signed(optarg, &svalue) != '\0')) {
                diminuto_perror(optarg);
                error = !0;
                break;
            } else if (svalue > 0) {
                sticks = svalue;
                sticks *= diminuto_frequency();
                sticks /= 1000000;
            } else {
                sticks = svalue;
            }
            /* Fall through. */

        case 'M':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            if (fd < 0) {
                errno = EBADF;
                diminuto_perror(sopt);
                fail = !0;
                break;
            }
            prior = -1;
            if (!first) {
                /* Do nothing. */
            } else if ((state = diminuto_line_get(fd)) < 0) {
                fail = !0;
                break;
            } else {
                state = !!state;
                printf("%d\n", state);
                fflush(stdout);
                prior = state;
            }
            diminuto_mux_init(&mux);
            if (diminuto_mux_register_interrupt(&mux, fd) < 0) {
                fail = !0;
                break;
            }
            while (!0) {
                if ((nfds = diminuto_mux_wait(&mux, sticks)) < 0) {
                    fail = !0;
                    break;
                }
                if (nfds > 0) {
                    while (!0) {
                        if ((ready = diminuto_mux_ready_read(&mux)) < 0) {
                            fail = !0;
                            break;
                        } else if (ready != fd) {
                            errno = EBADF;
                            diminuto_perror("diminuto_mux_ready_read");
                            fail = !0;
                            break;
                        } else if ((edge = diminuto_line_read(fd)) == 0) {
                            fail = !0;
                            break;
                        } else {
                            state = (edge < 0) ? 0 /* FALLING */ : !0 /* RISING */;
                            active = (!unique) || (prior < 0) || (prior != state);
                            if (!active) {
                                /* Do nothing. */
                            } else if (command != (const char *)0) {
                                snprintf(buffer, sizeof(buffer), "%s %s %u %d %d", command, path, line, state, prior);
                                buffer[sizeof(buffer) - 1] = '\0';
                                rc = diminuto_system(buffer);
                                if (rc < 0) {
                                    fail = !0;
                                    break;
                                }
                            } else {
                                printf("%d\n", state);
                                fflush(stdout);
                            }
                            prior = state;
                        }
                    }
                    if (fail) {
                        break;
                    }
                } else if ((state = diminuto_line_get(fd)) < 0) {
                    fail = !0;
                    break;
                } else {
                    state = !!state;
                    active = (!unique) || (prior < 0) || (prior != state);
                    if (!active) {
                        /* Do nothing. */
                    } else if (command != (const char *)0) {
                        snprintf(buffer, sizeof(buffer), "%s %s %d %d %d", command,path, line, state, prior);
                        buffer[sizeof(buffer) - 1] = '\0';
                        rc = diminuto_system(buffer);
                        if (rc < 0) {
                            fail = !0;
                            break;
                        }
                    } else {
                        printf("%d\n", state);
                        fflush(stdout);
                    }
                    prior = state;
                }
            }
            if (diminuto_mux_unregister_interrupt(&mux, fd) < 0) {
                fail = !0;
                break;
            }
            if (fail) {
                break;
            }
            break;

        case 'n':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            path = (const char *)0;
            line = maximumof(typeof(line));
            flags = 0x0;
            if (debug) { fprintf(stderr, "%s (\"%s\" %u 0x%llx)\n", program, (path == (const char *)0) ? "" : path, line, (unsigned long long)flags); }
            if ((fd = diminuto_line_close(fd)) >= 0) {
                fail = !0;
            }
            break;

        case 'o':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            flags |= DIMINUTO_LINE_FLAG_OUTPUT;
            if (debug) { fprintf(stderr, "%s (\"%s\" %u 0x%llx)\n", program, (path == (const char *)0) ? "" : path, line, (unsigned long long)flags); }
            break;

        case 'p':
            if (debug) { fprintf(stderr, "%s -%c %s\n", program, opt, optarg); }
            if (*diminuto_number_signed(optarg, &svalue) != '\0') {
                diminuto_perror(optarg);
                error = !0;
            } else if (svalue > maximumof(typeof(sline))) {
                errno = ERANGE;
                diminuto_perror(optarg);
                error = !0;
            } else {
                sline = svalue;
                if (sline < 0) {
                    flags |= DIMINUTO_LINE_FLAG_ACTIVE_LOW;
                    line = - sline;
                } else {
                    line = sline;
                }
                if (debug) { fprintf(stderr, "%s (\"%s\" %u 0x%llx)\n", program, (path == (const char *)0) ? "" : path, line, (unsigned long long)flags); }
                if ((fd = diminuto_line_close(fd)) >= 0) {
                    fail = !0;
                }
            }
            break;

        case 'r':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            if (fd < 0) {
                errno = EBADF;
                diminuto_perror(sopt);
                fail = !0;
            } else if ((state = diminuto_line_get(fd)) < 0) {
                fail = !0;
            } else {
                printf("%d\n", state);
            }
            break;

        case 's':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            state = !0;
            if (fd < 0) {
                errno = EBADF;
                diminuto_perror(sopt);
                fail = !0;
            } else if (diminuto_line_put(fd, state) < 0) {
                fail = !0;
            } else {
                /* Do nothing. */
            }
            break;

        case 't':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            done = (state == 0);
            break;

        case 'u':
            if (debug) { fprintf(stderr, "%s -%c %s\n", program, opt, optarg); }
            if ((*diminuto_number_unsigned(optarg, &uvalue) != '\0')) {
                diminuto_perror(optarg);
                error = !0;
            } else {
                ticks = uvalue;
                ticks *= diminuto_frequency();
                ticks /= 1000000;
                diminuto_delay(ticks, 0);
            }
            break;

        case 'w':
            if (debug) { fprintf(stderr, "%s -%c %s\n", program, opt, optarg); }
            if (*diminuto_number_unsigned(optarg, &uvalue) != '\0') {
                diminuto_perror(optarg);
                error = !0;
            } else if (fd < 0) {
                errno = EBADF;
                diminuto_perror(sopt);
                fail = !0;
            } else if (diminuto_line_put(fd, state = !!uvalue) < 0) {
                fail = !0;
            } else {
                /* Do nothoing. */
            }
            break;

        case 'x':
            if (debug) { fprintf(stderr, "%s -%c\n", program, opt); }
            if (fd >= 0) {
                errno = EBADF;
                diminuto_perror(sopt);
                fail = !0;
            } else if ((fd = diminuto_line_open(path, line, flags)) < 0) {
                fail = !0;
            } else {
                /* Do nothing. */
            }
            break;

        case '?':
            usage(program);
            break;

        default:
            error = !0;
            break;

        }

        if (error) {
            usage(program);
            xc = 1;
            break;
        }

        if (fail) {
            xc = 2;
            break;
        }

        if (done) {
            break;
        }

    }

    (void)diminuto_line_close(fd);

    return xc;
}
