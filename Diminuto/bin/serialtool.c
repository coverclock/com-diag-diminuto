/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 *
 * Copyright 2010-2017 Digital Aggregates Corporation, Colorado, USA<BR>
 * Licensed under the terms in README.h<BR>
 * Chip Overclock <coverclock@diag.com><BR>
 * http://www.diag.com/navigation/downloads/Diminuto.html<BR>
 *
 * ABSTRACT
 *
 * serialtool is used to test serial port connectivity. It can be used
 * in loop back (-B), loop forward (-F), or interactive (default) mode.
 * In loop back mode, it tests a loopback by sending data to the serial
 * device and verifying that the same data is received from the serial
 * device. In loop forward mode, it implements a loopback by receiving data
 * from the serial device and sending it back to the serial device. In
 * interactive mode, it reads data from standard input and sends it to
 * the serial device, and receives data from the serial device and writes
 * it to standard output. It can configure the serial port baud rate,
 * data bits, stop bits, parity, modem control, and flow control.
 *
 * EXAMPLES
 *
 * terminator:  serialtool -D /dev/ttyUSB0 -b 115200 -8 -1 -n -l -F -d
 * originator:  serialtool -D /dev/ttyS0 -b 115200 -8 -1 -n -l -B -d
 *
 * e.g. GPS:    serialtool -D /dev/ttyUSB0 -b 4800 -8 -1 -n -l -v
 *
 * NOTES
 *
 * You will likely need to either be root or be in the dialout group to
 * access serial ports.
 */

#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_serial.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_phex.h"
#include "com/diag/diminuto/diminuto_fd.h"
#include "com/diag/diminuto/diminuto_mux.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#undef NDEBUG
#include <assert.h>
#include <stdlib.h>

static int done = 0;
static int curious = 0;
static int alarmed = 0;

static void handler(int signum)
{
    if (signum == SIGPIPE) {
        done = !0;
    } else if (signum == SIGINT) {
        done = !0;
    } else if (signum == SIGTERM) {
        done = !0;
    } else if (signum == SIGQUIT) {
        done = !0;
    } else if (signum == SIGHUP) {
        curious = !0;
    } else if (signum == SIGALRM) {
        alarmed = !0;
    } else {
        /* No nothing. */
    }
}

int main(int argc, char * argv[])
{
    int rc = -1;
    extern char * optarg;
    extern int optind;
    extern int opterr;
    extern int optopt;
    int opt;
    const char * program = (const char *)0;
    int fd = -1;
    int bitspersecond = 115200;
    int databits = 8;
    int paritybit = 0;
    int stopbits = 1;
    int modemcontrol = 0;
    int xonxoff = 0;
    int rtscts = 0;
    const char * device = "/dev/null";
    FILE * fp = (FILE *)0;
    int input = 0;
    int output = 0;
    size_t current = 0;
    int end = 0;
    diminuto_ticks_t now = 0;
    diminuto_ticks_t then = 0;
    diminuto_ticks_t hertz = 0;
    double elapsed = 0.0;
    size_t count = 0;
    int bitspercharacter = 0;
    int running = 0;
    double bandwidth = 0.0;
    int forward = 0;
    int backward = 0;
    struct sigaction action = { 0 };
    int printable = 0;
    unsigned int seconds = 1;
    int debug = 0;
    int verbose = 0;
    size_t maximum = 512;
    diminuto_mux_t mux = { 0 };
    void * buffer = (void *)0;
    ssize_t reads = 0;
    ssize_t writes = 0;
    int fds = 0;
    int ready = -1;

    diminuto_log_setmask();

    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

    while ((opt = getopt(argc, argv, "125678?BD:FI:b:dehlmnopst:v")) >= 0) {

        switch (opt) {

        case '1':
            stopbits = 1;
            break;

        case '2':
            stopbits = 2;
            break;

        case '5':
            databits = 5;
            break;

        case '6':
            databits = 6;
            break;

        case '7':
            databits = 7;
            break;

        case '8':
            databits = 8;
            break;

        case 'B':
            backward = !0;
            break;

        case 'D':
            device = optarg;
            break;

        case 'F':
            forward = !0;
            break;

        case 'I':
            maximum = strtoul(optarg, (char **)0, 0);
            break;

        case 'b':
            bitspersecond = strtoul(optarg, (char **)0, 0);
            break;

        case 'd':
            debug = !0;
            break;

        case 'e':
            paritybit = 2;
            break;

        case 'h':
            rtscts = !0;
            break;

        case 'l':
            modemcontrol = 0;
            break;

        case 'm':
            modemcontrol = !0;
            break;

        case 'n':
            paritybit = 0;
            break;

        case 'o':
            paritybit = 1;
            break;

        case 'p':
            printable = !0;
            break;

        case 's':
            xonxoff = !0;
            break;

        case 't':
            seconds = strtoul(optarg, (char **)0, 0);
            break;

        case 'v':
            verbose = !0;
            break;

        case '?':
            fprintf(stderr, "usage: %s [ -1 | -2 ] [ -5 | -6 | -7 | -8 ] [ -B | -F | -I BYTES ] [ -D DEVICE ] [ -b BPS ] [ -d ] [ -e | -o | -n ] [ -h ] [ -s ] [ -l | -m ] [ -p ] [ -t SECONDS ] [ -v ]\n", program);
            fprintf(stderr, "       -1          One stop bit.\n");
            fprintf(stderr, "       -2          Two stop bits.\n");
            fprintf(stderr, "       -5          Five data bits.\n");
            fprintf(stderr, "       -6          Six data bits.\n");
            fprintf(stderr, "       -7          Seven data bits.\n");
            fprintf(stderr, "       -8          Eight data bits.\n");
            fprintf(stderr, "       -B          Test loop back (send then receive).\n");
            fprintf(stderr, "       -F          Implement loop back (receive then send).\n");
            fprintf(stderr, "       -I BYTES    Set interactive buffer size to BYTES.\n");
            fprintf(stderr, "       -D DEVICE   Use DEVICE.\n");
            fprintf(stderr, "       -b BPS      Bits per second.\n");
            fprintf(stderr, "       -d          Emit characters on standard error using phex.\n");
            fprintf(stderr, "       -e          Even parity.\n");
            fprintf(stderr, "       -o          Odd parity.\n");
            fprintf(stderr, "       -n          No parity.\n");
            fprintf(stderr, "       -h          Hardware flow control (RTS/CTS).\n");
            fprintf(stderr, "       -s          Software flow control (XON/XOFF).\n");
            fprintf(stderr, "       -l          Local (no modem control).\n");
            fprintf(stderr, "       -m          Modem control.\n");
            fprintf(stderr, "       -p          Printable only ('!' to '~').\n");
            fprintf(stderr, "       -t SECONDS  Timeout in SECONDS.\n");
            fprintf(stderr, "       -v          Print characters on standard error.\n");
            return 1;
            break;

        }

    }

    hertz = diminuto_frequency();

    action.sa_handler = handler;
    assert(sigaction(SIGPIPE, &action, (struct sigaction *)0) >= 0);
    assert(sigaction(SIGINT, &action, (struct sigaction *)0) >= 0);
    assert(sigaction(SIGQUIT, &action, (struct sigaction *)0) >= 0);
    assert(sigaction(SIGTERM, &action, (struct sigaction *)0) >= 0);
    assert(sigaction(SIGHUP, &action, (struct sigaction *)0) >= 0);
    assert(sigaction(SIGALRM, &action, (struct sigaction *)0) >= 0);

    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "%s %s %dbps %d%c%d %s %s %s %s %useconds %ubytes\n", forward ? "implement-loopback" : backward ? "test-loopback" : "interactive", device, bitspersecond, databits, "NOE"[paritybit], stopbits, modemcontrol ? "modem" : "local", xonxoff ? "xonxoff" : "noswflow", rtscts ? "rtscts" : "nohwflow", printable ? "printable" : "all", seconds, maximum);

    fd = open(device, O_RDWR);
    assert(fd >= 0);

    rc = diminuto_serial_set(fd, bitspersecond, databits, paritybit, stopbits, modemcontrol, xonxoff, rtscts);
    assert(rc == 0);

    rc = diminuto_serial_raw(fd);
    assert(rc == 0);

    bitspercharacter = 1 + databits + ((paritybit != 0) ? 1 : 0) + stopbits;

    output = printable ? '!' : 0x00;

    count = 0;

    then = diminuto_time_elapsed();

    if (forward) {

        /*
         * Loop Forward: we receive a character and write that same character
         * back out.
         */

        fp = fdopen(fd, "r+");
        assert(fp != (FILE *)0);

        rc = diminuto_serial_unbuffered(fp);
        assert(rc == 0);

        while (!done) {
            input = fgetc(fp);
            if (input != EOF) {
                 if (!running) {
                     DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "running\n");
                    running = !0;
                }
                output = input;
                fputc(output, fp);
                ++count;
                if (debug) { diminuto_phex_emit(stderr, input, 72, 0, 0, 0, &current, &end, 0); }
                if (verbose) { fputc(input, stderr); }
            } else if (done) {
                now = diminuto_time_elapsed();
                elapsed = (now - then) / (double)hertz;
                bandwidth = (count * bitspercharacter) / elapsed;
                DIMINUTO_LOG_NOTICE(DIMINUTO_LOG_HERE "done %luB %.6lfs %dbaud %dbpC %.6lfbps\n", count, elapsed, bitspersecond, bitspercharacter, bandwidth);
            } else if (curious) {
                now = diminuto_time_elapsed();
                elapsed = (now - then) / (double)hertz;
                bandwidth = (count * bitspercharacter) / elapsed;
                DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "curious %luB %.6lfs %dbaud %dbpC %.6lfbps\n", count, elapsed, bitspersecond, bitspercharacter, bandwidth);
                curious = 0;
            } else {
                now = diminuto_time_elapsed();
                elapsed = (now - then) / (double)hertz;
                bandwidth = (count * bitspercharacter) / elapsed;
                DIMINUTO_LOG_NOTICE(DIMINUTO_LOG_HERE "eof %luB %.6lfs %dbaud %dbpC %.6lfbps\n", count, elapsed, bitspersecond, bitspercharacter, bandwidth);
                done = !0;
            }
        }

    } else if (backward) {

        /*
         * Loop Backward: we write a character and expect to receive that same
         * character back. This is the default.
         */

        fp = fdopen(fd, "r+");
        assert(fp != (FILE *)0);

        rc = diminuto_serial_unbuffered(fp);
        assert(rc == 0);

        while (!done) {
            fputc(output, fp);
            while (!0) {
                alarm(seconds);
                input = fgetc(fp);
                alarm(0);
                if (debug) { diminuto_phex_emit(stderr, input, 72, 0, 0, 0, &current, &end, 0); }
                if (verbose) { fputc(input, stderr); }
                if (input != EOF) {
                     if (!running) {
                         DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "running\n");
                         running = !0;
                     }
                    if (input != output) {
                        DIMINUTO_LOG_ERROR(DIMINUTO_LOG_HERE "mismatch 0x%x 0x%x\n", input, output);
                        done = !0;
                        break;
                    }
                    if (!printable) {
                          output = (output + 1) % (1 << (sizeof(char) * 8));
                    } else if ((++output) > '~') {
                        output = '!';
                    } else {
                        /* Do nothing. */
                    }
                    ++count;
                    break;
                } else if (alarmed) {
                    now = diminuto_time_elapsed();
                    elapsed = (now - then) / (double)hertz;
                    bandwidth = (count * bitspercharacter) / elapsed;
                    DIMINUTO_LOG_ERROR(DIMINUTO_LOG_HERE "timeout %luB %.6lfs %dbaud %dbpC %.6lfbps\n", count, elapsed, bitspersecond, bitspercharacter, bandwidth);
                    done = !0;
                    break;
                } else if (done) {
                    now = diminuto_time_elapsed();
                    elapsed = (now - then) / (double)hertz;
                    bandwidth = (count * bitspercharacter) / elapsed;
                    DIMINUTO_LOG_NOTICE(DIMINUTO_LOG_HERE "done %luB %.6lfs %dbaud %dbpC %.6lfbps\n", count, elapsed, bitspersecond, bitspercharacter, bandwidth);
                    break;
                } else if (curious) {
                    now = diminuto_time_elapsed();
                    elapsed = (now - then) / (double)hertz;
                    bandwidth = (count * bitspercharacter) / elapsed;
                    DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "curious %luB %.6lfs %dbaud %dbpC %.6lfbps\n", count, elapsed, bitspersecond, bitspercharacter, bandwidth);
                    curious = 0;
                } else {
                    now = diminuto_time_elapsed();
                    elapsed = (now - then) / (double)hertz;
                    bandwidth = (count * bitspercharacter) / elapsed;
                    DIMINUTO_LOG_NOTICE(DIMINUTO_LOG_HERE "eof %luB %.6lfs %dbaud %dbpC %.6lfbps\n", count, elapsed, bitspersecond, bitspercharacter, bandwidth);
                    done = !0;
                    break;
                }
            }
        }
                
    } else {

        /*
         * Interactive: data read from standard input is sent to device,
         * data received from device is written to standard output.
         */

        assert(maximum > 0);
        buffer = malloc(maximum);
        assert(buffer != (void *)0);
        diminuto_mux_init(&mux);
        assert(fd != STDIN_FILENO);
        assert(fd != STDOUT_FILENO);
        rc = diminuto_mux_register_read(&mux, fd);
        assert(rc >= 0);
        rc = diminuto_mux_register_read(&mux, STDIN_FILENO);
        assert(rc >= 0);
        while (!done) {
            fds = diminuto_mux_wait(&mux, -1);
            assert(fds > 0);
            while (!done) {
                ready = diminuto_mux_ready_read(&mux);
                if (ready < 0) {
                    break;
                } else if (ready == fd) {
                    reads = diminuto_fd_read(fd, buffer, maximum);
                    assert(reads >= 0);
                    assert(reads <= maximum);
                    if (reads == 0) {
                        done = !0;
                        break;
                    }
                    writes = diminuto_fd_write(STDOUT_FILENO, buffer, reads);
                    assert(writes == reads);
                    if (debug) {
                        char * bb;
                        for (bb = (char *)buffer; reads > 0; --reads) {
                            diminuto_phex_emit(stderr, *(bb++), 72, 0, 0, 0, &current, &end, 0);
                        }
                    }
                } else if (ready == STDIN_FILENO) {
                    reads = diminuto_fd_read(STDIN_FILENO, buffer, maximum);
                    assert(reads >= 0);
                    assert(reads <= maximum);
                    if (reads == 0) {
                        done = !0;
                        break;
                    }
                    writes = diminuto_fd_write(fd, buffer, reads);
                    assert(writes == reads);
                } else {
                    assert(0);
                }
            }
        }
        rc = diminuto_mux_unregister_read(&mux, STDIN_FILENO);
        assert(rc >= 0);
        rc = diminuto_mux_unregister_read(&mux, fd);
        assert(rc >= 0);

    }

    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "fini\n");

    exit(0);
}
