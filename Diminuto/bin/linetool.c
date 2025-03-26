/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2024-2025 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Manipulate GPIO pins using the ioctl ABI.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 *
 * EXAMPLES
 *
 * linetool -D /dev/gpiochip4 -p 20 -i -x -b 10000
 *
 * linetool -D /dev/gpiochip4 -p 21 -i -B -U -M 1000000 -x -m 10000
 *
 * linetool -D /dev/gpiochip4 -p 16 -o -H -x -r -s -r -u 5000000 -r -c -r -e -p 18 -x -r -w 0 -r -u 5000000 -r -w 1 -r -n
 * 
 * linetool -P /dev/./../dev/gpiochip0:-0x0 -O -e -O -n -O
 *
 * ABSTRACT
 *
 * Allows manipulation of general purpose input/output (GPIO) pins using the
 * lower level calls of the line facility. Uses the iotcl GPIO driver interface.
 * Probably must be run as root or, on some platforms (e.g. Raspian on the
 * Raspberry Pi) as a user in the gpio group (e.g. pi).
 *
 * N.B. Although the syntax of linetool looks a lot like taht of pintool,
 * the semantics are completely different. See the scripts that use linetool
 * (like linetest.sh) for examples.
 *
 * N.B. How a particularly GPIO line may be configured and what operations
 * may be performed on it depends on the underlying hardware and how its device
 * driver is implemented and configured. (I've used GPIO controllers on
 * ARM-based SOCs that had remarkably broad and versatile capabilities, like
 * supporting configuration of internal pull-up resisters with no external
 * hardware, but this won't be the case on all platforms.)
 *
 * The utility strace(1) was useful when testing this.
 */

#include "com/diag/diminuto/diminuto_line.h"
#include "com/diag/diminuto/diminuto_assert.h"
#include "com/diag/diminuto/diminuto_cue.h"
#include "com/diag/diminuto/diminuto_core.h"
#include "com/diag/diminuto/diminuto_daemon.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_error.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_main.h"
#include "com/diag/diminuto/diminuto_minmaxof.h"
#include "com/diag/diminuto/diminuto_mux.h"
#include "com/diag/diminuto/diminuto_number.h"
#include "com/diag/diminuto/diminuto_pipe.h"
#include "com/diag/diminuto/diminuto_string.h"
#include "com/diag/diminuto/diminuto_types.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

/*******************************************************************************
 * CODE GENERATORS
 ******************************************************************************/

#define DEBUGOPTION DIMINUTO_LOG_DEBUG("%s -%c\n", program, opt)

#define DEBUGSTRINGOPTION DIMINUTO_LOG_DEBUG("%s -%c \"%s\"\n", program, opt, optarg)

#define DEBUGNUMERICOPTION DIMINUTO_LOG_DEBUG("%s -%c %s\n", program, opt, optarg)

#define DEBUGCONTEXT DIMINUTO_LOG_DEBUG("%s { \"%s\" %u 0x%llx %u (%d) }\n", program, (path == (const char *)0) ? "" : path, line, (unsigned long long)flags, useconds, fd)

#define DEBUGEDGE DIMINUTO_LOG_DEBUG("%s <%d>\n", program, edge)

#define DEBUGSTATE DIMINUTO_LOG_DEBUG("%s (%d)\n", program, state)

#define DEBUGWAIT DIMINUTO_LOG_DEBUG("%s ?\n", program)

#define DEBUGPIPE DIMINUTO_LOG_DEBUG("%s %d!\n", program, SIGPIPE)

#define DEBUGEXIT DIMINUTO_LOG_DEBUG("%s %d %d %d %d\n", program, error, fail, done, xc)

/*******************************************************************************
 * USAGE
 ******************************************************************************/

static const char OPTIONS[] = "1BD:FHLM:NOP:RSUX:b:cefhilm:nop:rstu:vw:x?";

static const char USAGE[] = "[ -S ] "
                            "[ [ -D DEVICE -p LINE ] | [ -P DEVICE:LINE ] ] "
                            "[ -i ] [ -o ] "
                            "[ -H | -L ]"
                            "[ -h | -l ] "
                            "[ -N | -R | -F | -B ] "
                            "[ -M USECONDS ] "
                            "[ -x ] "
                            "[ -r | -w 0 | -w 1 | -s | -c | [ -1 ] [ -U ] [ -X COMMAND ] -m USECONDS | -b USECONDS ] "
                            "[ -t | -f ] "
                            "[ -u USECONDS ] "
                            "[ -n | -e ] "
                            "[ -O ] "
                            "[ ... ] ";

static void usage(const char * program)
{
    fprintf(stderr, "usage: %s %s\n", program, USAGE);
    fprintf(stderr, "       -1              Read LINE initially for -m.\n");
    fprintf(stderr, "       -B              Configure LINE edge to both.\n");
    fprintf(stderr, "       -D DEVICE       Close and configure context to DEVICE.\n");
    fprintf(stderr, "       -F              Configure context edge to falling.\n");
    fprintf(stderr, "       -H              Configure context active to high.\n");
    fprintf(stderr, "       -L              Configure context active to low.\n");
    fprintf(stderr, "       -M USECONDS     Configure context debounce period to USECONDS for -m (try 10000).\n");
    fprintf(stderr, "       -N              Configure context edge to none.\n");
    fprintf(stderr, "       -O              Print context to standard output.\n");
    fprintf(stderr, "       -P DEVICE:LINE  Close and configure context to DEVICE:LINE.\n");
    fprintf(stderr, "       -R              Configure context edge to rising.\n");
    fprintf(stderr, "       -S              Daemonize.\n");
    fprintf(stderr, "       -U              Filter non-unique LINE changes for for -m.\n");
    fprintf(stderr, "       -X COMMAND      Execute COMMAND DEVICE LINE STATE PRIOR in shell for -m.\n");
    fprintf(stderr, "       -b USECONDS     Poll with software debounce period of USECONDS (try 10000).\n");
    fprintf(stderr, "       -c              Clear LINE by writing 0.\n");
    fprintf(stderr, "       -e              Close LINE but keep context.\n");
    fprintf(stderr, "       -f              Proceed if the last LINE state was 0.\n");
    fprintf(stderr, "       -h              Configure context direction to output and active to high.\n");
    fprintf(stderr, "       -i              Configure context direction to input.\n");
    fprintf(stderr, "       -l              Configure context direction to output and active to low.\n");
    fprintf(stderr, "       -m USECONDS     Multiplex with driver debounce with a timeout of USECONDS microseconds (try 1000000).\n");
    fprintf(stderr, "       -n              Close LINE and clear context.\n");
    fprintf(stderr, "       -o              Configure context direction to output.\n");
    fprintf(stderr, "       -p LINE         Close and configure context to LINE.\n");
    fprintf(stderr, "       -r              Read LINE and print to standard output.\n");
    fprintf(stderr, "       -s              Set LINE by writing !0.\n");
    fprintf(stderr, "       -t              Proceed if the last LINE state was !0.\n");
    fprintf(stderr, "       -u USECONDS     Delay for USECONDS microseconds.\n");
    fprintf(stderr, "       -w STATE        Write STATE [ 0 | 1 ] to LINE.\n");
    fprintf(stderr, "       -x              Configure LINE with context and open.\n");
    fprintf(stderr, "       -?              Print menu.\n");
}

/*******************************************************************************
 * MAIN
 ******************************************************************************/

int main(int argc, char * argv[])
{
    int xc = 0;
    const char * program = "linetool";
    int done = 0;
    int fail = 0;
    int error = 0;
    int first = 0;
    int unique = 0;
    int state = 0;
    int prior = 0;
    int edge = 0;
    int inverted = 0;
    int effective = 0;
    int fd = -1;
    int ready = -1;
    int nfds = 0;
    int rc = -1;
    const char * command = (const char *)0;
    diminuto_unsigned_t uvalue = 0;
    diminuto_signed_t svalue = -1;
    diminuto_mux_t mux = { 0 };
    sigset_t blocked = { 0, };
    sigset_t original = { 0, };
    diminuto_ticks_t uticks = 0;
    diminuto_sticks_t sticks = -2;
    diminuto_cue_state_t cue = { 0 };
    char sopt[2] = { '\0', '\0' };
    diminuto_path_t pathname = { '\0', };
    int opt = '\0';
    extern char * optarg;
    /* CONTEXT BEGIN */
    const char * path = (const char *)0;
    diminuto_line_offset_t line = maximumof(diminuto_line_offset_t);
    diminuto_line_bits_t flags = 0x0;
    diminuto_line_duration_t useconds = 0;
    /* CONTEXT END */

    program = diminuto_main_program();
    diminuto_contract(program != (const char *)0);

    diminuto_log_open(program);
    (void)diminuto_log_setmask();

    rc = diminuto_core_enable();
    diminuto_contract(rc >= 0);

    /*
     * The SIGPIPE is not received until the application tries to
     * write to the orphaned pipe. This doesn't for -b until the
     * debounced state changes from its prior value, and not for -m
     * until the driver indicates a new event.
     */
    rc = diminuto_pipe_install(0);
    diminuto_contract(rc >= 0);

    (void)diminuto_line_consumer(__FILE__);

    while ((opt = getopt(argc, argv, OPTIONS)) >= 0) {

        if (diminuto_pipe_check()) {
            DEBUGPIPE;
            done = !0;
            break;
        }

        sopt[0] = opt;

        uvalue = 0;
        svalue = -1;

        switch (opt) {

        case '1':
            DEBUGOPTION;
            first = !0;
            break;

        case 'B':
            DEBUGOPTION;
            flags |= DIMINUTO_LINE_FLAG_EDGE_RISING;
            flags |= DIMINUTO_LINE_FLAG_EDGE_FALLING;
            DEBUGCONTEXT;
            break;

        case 'D':
            DEBUGSTRINGOPTION;
            path = optarg;
            DEBUGCONTEXT;
            break;

        case 'F':
            DEBUGOPTION;
            flags |= DIMINUTO_LINE_FLAG_EDGE_FALLING;
            DEBUGCONTEXT;
            break;

        case 'H':
            DEBUGOPTION;
            flags &= ~DIMINUTO_LINE_FLAG_ACTIVE_LOW;
            DEBUGCONTEXT;
            break;

        case 'L':
            DEBUGOPTION;
            flags |= DIMINUTO_LINE_FLAG_ACTIVE_LOW;
            DEBUGCONTEXT;
            break;

        case 'M':
            DEBUGNUMERICOPTION;
            if ((*diminuto_number_unsigned(optarg, &uvalue) != '\0')) {
                diminuto_perror(optarg);
                error = !0;
            } else if (uvalue > maximumof(typeof(useconds))) {
                errno = ERANGE;
                diminuto_perror(optarg);
                error = !0;
            } else {
                useconds = uvalue;
                DEBUGCONTEXT;
            }
            break;

        case 'N':
            DEBUGOPTION;
            flags &= ~DIMINUTO_LINE_FLAG_EDGE_RISING;
            flags &= ~DIMINUTO_LINE_FLAG_EDGE_FALLING;
            DEBUGCONTEXT;
            break;

        case 'O':
            DEBUGOPTION;
            printf("%s %u 0x%llx %u %d\n", (path == (const char *)0) ? "/dev/null" : path, line, (unsigned long long)flags, useconds, fd);
            fflush(stdout);
            DEBUGCONTEXT;
            break;

        case 'P':
            DEBUGSTRINGOPTION;
            path = diminuto_line_parse(optarg, pathname, sizeof(pathname), &line, &inverted);
            if (path == (const char *)0) {
                fail = !0;
                break;
            }
            if (inverted) {
                flags |= DIMINUTO_LINE_FLAG_ACTIVE_LOW;
            }
            DEBUGCONTEXT;
            break;

        case 'R':
            DEBUGOPTION;
            flags |= DIMINUTO_LINE_FLAG_EDGE_RISING;
            DEBUGCONTEXT;
            break;

        case 'S':
            DEBUGOPTION;
            rc = diminuto_daemon(program);
            if (rc < 0) {
                fail = !0;
            }
            break;

        case 'U':
            DEBUGOPTION;
            unique = !0;
            break;

        case 'X':
            DEBUGSTRINGOPTION;
            command = optarg;
            break;

        case 'b':
            DEBUGNUMERICOPTION;
            if ((*diminuto_number_unsigned(optarg, &uvalue) != '\0')) {
                diminuto_perror(optarg);
                error = !0;
                break;
            }
            uticks = diminuto_frequency_units2ticks(uvalue, 1000000);
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
            DEBUGSTATE;
            printf("%d\n", state);
            fflush(stdout);
            prior = state;
            diminuto_cue_init(&cue, state);
            while (!0) {
                if (diminuto_pipe_check()) {
                    DEBUGPIPE;
                    break;
                }
                diminuto_delay(uticks, 0);
                if ((state = diminuto_line_get(fd)) < 0) {
                    fail = !0;
                    break;
                }
                state = diminuto_cue_debounce(&cue, !!state);
                if (state != prior) {
                    DEBUGSTATE;
                    printf("%d\n", state);
                    fflush(stdout);
                    prior = state;
                }
            }
            break;

        case 'c':
            DEBUGOPTION;
            if (fd < 0) {
                errno = EBADF;
                diminuto_perror(sopt);
                fail = !0;
            } else if (diminuto_line_clear(fd) < 0) {
                fail = !0;
            } else {
                state = 0;
                DEBUGSTATE;
            }
            break;

        case 'e':
            DEBUGOPTION;
            DEBUGCONTEXT;
            /*
             * It is not an error to close a closed line.
             */
            if (fd < 0) {
                /* Do nothing. */
            } else if ((fd = diminuto_line_close(fd)) >= 0) {
                fail = !0;
            } else {
                /* Do nothing. */
            }
            DEBUGCONTEXT;
            break;

        case 'f':
            DEBUGOPTION;
            done = !!state;
            break;

        case 'h':
            DEBUGOPTION;
            flags = DIMINUTO_LINE_FLAG_OUTPUT;
            flags &= ~DIMINUTO_LINE_FLAG_ACTIVE_LOW;
            DEBUGCONTEXT;
            break;

        case 'i':
            DEBUGOPTION;
            flags |= DIMINUTO_LINE_FLAG_INPUT;
            DEBUGCONTEXT;
            break;

        case 'l':
            DEBUGOPTION;
            flags = DIMINUTO_LINE_FLAG_OUTPUT;
            flags |= DIMINUTO_LINE_FLAG_ACTIVE_LOW;
            DEBUGCONTEXT;
            break;

        case 'm':
            DEBUGNUMERICOPTION;
            if ((*diminuto_number_signed(optarg, &svalue) != '\0')) {
                diminuto_perror(optarg);
                error = !0;
                break;
            } else if (svalue > 0) {
                sticks = diminuto_frequency_units2ticks(svalue, 1000000);
            } else {
                /* Do nothing: 0 == poll, <0 == blocking. */
            }
            if (fd < 0) {
                errno = EBADF;
                diminuto_perror(sopt);
                fail = !0;
                break;
            }
            if ((rc = pthread_sigmask(SIG_BLOCK, (sigset_t *)0, &blocked)) != 0) {
                errno = rc;
                diminuto_perror("sigprocmask");
                fail = !0;
                break;
            } else if (sigaddset(&blocked, SIGPIPE) < 0) {
                diminuto_perror("sigaddset");
                fail = !0;
                break;
            } else if ((rc = pthread_sigmask(SIG_BLOCK, &blocked, &original)) != 0) {
                errno = rc;
                diminuto_perror("sigprocmask");
                fail = !0;
                break;
            } else if (diminuto_mux_init(&mux) != &mux) {
                fail = !0;
                break;
            } else if (diminuto_mux_register_read(&mux, fd) < 0) {
                fail = !0;
                break;
            } else if (diminuto_mux_unregister_signal(&mux, SIGPIPE) < 0) {
                fail = !0;
                break;
            } else {
#if 0
                extern int diminuto_pipe_debug;
                diminuto_pipe_debug = !0;
#endif
#if 0
                diminuto_mux_dump(&mux);
#endif
            }
            if (!first) {
                prior = -1;
            } else if ((state = diminuto_line_get(fd)) < 0) {
                fail = !0;
                break;
            } else {
                state = !!state;
                DEBUGSTATE;
                printf("%d\n", state);
                fflush(stdout);
                prior = state;
            }
            while (!0) {
                DEBUGWAIT;
                if (diminuto_pipe_check()) {
                    DEBUGPIPE;
                    break;
                } else if ((nfds = diminuto_mux_wait(&mux, sticks)) < 0) {
                    if (errno == EINTR) {
                        /* SIGPIPE presumably. */
                        diminuto_yield();
                        continue;
                    } else {
                        fail = !0;
                        break;
                    }
                } else if (nfds == 0) {
                    /* Timeout presumably. */
                    diminuto_yield();
                    continue;
                } else if ((ready = diminuto_mux_ready_read(&mux)) < 0) {
                    fail = !0;
                    break;
                } else if (ready != fd) {
                    errno = ENOENT;
                    diminuto_perror("diminuto_mux_ready_read");
                    fail = !0;
                    break;
                } else if ((edge = diminuto_line_read(fd)) < 0) {
                    switch (errno) {
                    case EINTR:
                    case EAGAIN:
                        /* SIGPIPE or something retryable. */
                        diminuto_yield();
                        continue;
                    default:
                        fail = !0;
                        break;
                    }
                    break;
                } else {
                    DEBUGEDGE;
                    state = !!edge;
                    effective = (!unique) || (prior < 0) || (prior != state);
                    if (!effective) {
                        /* Do nothing. */
                    } else if (command != (const char *)0) {
                        char buffer[1024] = { '\0' };
                        snprintf(buffer, sizeof(buffer), "%s %s %u %d %d", command, path, line, state, prior);
                        buffer[sizeof(buffer) - 1] = '\0';
                        rc = diminuto_system(buffer);
                        if (rc < 0) {
                            fail = !0;
                            break;
                        }
                    } else {
                        DEBUGSTATE;
                        printf("%d\n", state);
                        fflush(stdout);
                    }
                    prior = state;
                }
            }
            if ((rc = pthread_sigmask(SIG_BLOCK, &original, (sigset_t *)0)) != 0) {
                errno = rc;
                diminuto_perror("sigprocmask");
                fail = !0;
            }
            break;

        case 'n':
            DEBUGOPTION;
            DEBUGCONTEXT;
            path = (const char *)0;
            line = maximumof(typeof(line));
            flags = 0x0;
            useconds = 0;
            /*
             * It is not an error to close a closed line.
             */
            if (fd < 0) {
                /* Do nothing. */
            } else if ((fd = diminuto_line_close(fd)) >= 0) {
                fail = !0;
            } else {
                /* Do nothing. */
            }
            DEBUGCONTEXT;
            break;

        case 'o':
            DEBUGOPTION;
            flags |= DIMINUTO_LINE_FLAG_OUTPUT;
            DEBUGCONTEXT;
            break;

        case 'p':
            DEBUGNUMERICOPTION;
            /*
             * -p -0 is a valid parameter.
             */
            if (*optarg == '-') {
                inverted = !0;
                optarg += 1;
            } else {
                inverted = 0;
            }
            if (*diminuto_number_unsigned(optarg, &uvalue) != '\0') {
                diminuto_perror(optarg);
                error = !0;
            } else if (uvalue > maximumof(typeof(line))) {
                errno = ERANGE;
                diminuto_perror(optarg);
                error = !0;
            } else {
                line = uvalue;
                if (inverted) {
                    flags |= DIMINUTO_LINE_FLAG_ACTIVE_LOW;
                }
                DEBUGCONTEXT;
            }
            break;

        case 'r':
            DEBUGOPTION;
            if (fd < 0) {
                errno = EBADF;
                diminuto_perror(sopt);
                fail = !0;
            } else if ((state = diminuto_line_get(fd)) < 0) {
                fail = !0;
            } else {
                DEBUGSTATE;
                printf("%d\n", state);
            }
            break;

        case 's':
            DEBUGOPTION;
            if (fd < 0) {
                errno = EBADF;
                diminuto_perror(sopt);
                fail = !0;
            } else if (diminuto_line_set(fd) < 0) {
                fail = !0;
            } else {
                state = !0;
                DEBUGSTATE;
            }
            break;

        case 't':
            DEBUGOPTION;
            done = !state;
            break;

        case 'u':
            DEBUGNUMERICOPTION;
            if ((*diminuto_number_unsigned(optarg, &uvalue) != '\0')) {
                diminuto_perror(optarg);
                error = !0;
            } else {
                uticks = diminuto_frequency_units2ticks(uvalue, 1000000);
                diminuto_delay(uticks, 0);
            }
            break;

        case 'w':
            DEBUGNUMERICOPTION;
            if (*diminuto_number_unsigned(optarg, &uvalue) != '\0') {
                diminuto_perror(optarg);
                error = !0;
            } else if (fd < 0) {
                errno = EBADF;
                diminuto_perror(sopt);
                fail = !0;
            } else if (diminuto_line_put(fd, uvalue = !!uvalue) < 0) {
                fail = !0;
            } else {
                state = uvalue;
                DEBUGSTATE;
            }
            break;

        case 'x':
            DEBUGOPTION;
            DEBUGCONTEXT;
            if (fd >= 0) {
                errno = EBADF;
                diminuto_perror(sopt);
                fail = !0;
            } else if ((fd = diminuto_line_open_read(path, line, flags, useconds)) < 0) {
                fail = !0;
            } else {
                /* Do nothing. */
            }
            break;

        case '?':
        default:
            usage(program);
            return 1;
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

    DEBUGEXIT;

    return xc;
}
