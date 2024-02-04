/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010-2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief Manipulate serial-ish ports.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
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
 * As an added bonus, serialtool can be used on other devices that are
 * not serial ports but which have serial input and output. serialtool detects
 * that the device is not a "tty" and bypasses the serial port configuration.
 * It otherwise acts as described above.
 *
 * EXAMPLES
 *
 * terminator:  serialtool -D /dev/ttyUSB0 -b 115200 -8 -1 -n -l -F -d<BR>
 * originator:  serialtool -D /dev/ttyS0 -b 115200 -8 -1 -n -l -B -d<BR>
 *
 * e.g. GPS:    serialtool -D /dev/ttyUSB0 -b 4800 -8 -1 -n -l -v
 *
 * NOTES
 *
 * You may need to either be root or be in the dialout group to access
 * serial ports.
 */

#include "com/diag/diminuto/diminuto_assert.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_fd.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_mux.h"
#include "com/diag/diminuto/diminuto_phex.h"
#include "com/diag/diminuto/diminuto_serial.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_types.h"
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

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
    int carrierdetect = 0;
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
    unsigned int seconds = 10; /* Same as expect(1). */
    int debug = 0;
    int verbose = 0;
    size_t maximum = 512;
    diminuto_mux_t mux = { 0 };
    void * buffer = (void *)0;
    ssize_t reads = 0;
    ssize_t writes = 0;
    int fds = 0;
    int ready = -1;
    int noinput = 0;
    int rawterminal = 0;
    int nonblocking = 0;
    int isaterminal = 0;
    int onlyaterminal = 0;
    size_t modulo = 0;

    diminuto_log_setmask();

    program = ((program = strrchr(argv[0], '/')) == (char *)0) ? argv[0] : program + 1;

    while ((opt = getopt(argc, argv, "125678?BD:FI:NM:OTb:cdehilmnoprst:v")) >= 0) {

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

        case 'M':
            modulo = strtoul(optarg, (char **)0, 0);
            break;

        case 'N':
            backward = 0;
            forward = 0;
            noinput = 0;
            nonblocking = !0;
            break;

        case 'O':
            nonblocking = !0;
            break;

        case 'T':
            onlyaterminal = !0;
            break;

        case 'b':
            bitspersecond = strtoul(optarg, (char **)0, 0);
            break;

        case 'c':
            modemcontrol = !0;
            carrierdetect = !0;
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

        case 'i':
            noinput = !0;
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

        case 'r':
            rawterminal = !0;
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
            fprintf(stderr, "usage: %s [ -1 | -2 ] [ -5 | -6 | -7 | -8 ] [ -B | -F | -N ] [ -O ] [ -I BYTES ] [ -D DEVICE ] [ -b BPS ] [ -c ] [ -d ] [ -e | -o | -n ] [ -h ] [ -s ] [ -l | -m ] [ -p ] [ -t SECONDS ] [ -i | -r ] [ -v ] [ -M MODULO ]\n", program);
            fprintf(stderr, "       -1          One stop bit.\n");
            fprintf(stderr, "       -2          Two stop bits.\n");
            fprintf(stderr, "       -5          Five data bits.\n");
            fprintf(stderr, "       -6          Six data bits.\n");
            fprintf(stderr, "       -7          Seven data bits.\n");
            fprintf(stderr, "       -8          Eight data bits.\n");
            fprintf(stderr, "       -B          Test loop back (send then receive).\n");
            fprintf(stderr, "       -F          Implement loop back (receive then send).\n");
            fprintf(stderr, "       -D DEVICE   Use DEVICE.\n");
            fprintf(stderr, "       -I BYTES    Set interactive buffer size to BYTES.\n");
            fprintf(stderr, "       -M MODULO   Report every MODULO characters.\n");
            fprintf(stderr, "       -N          Interactive mode (disables -B, -F, -i; implies -O).\n");
            fprintf(stderr, "       -O          Open in non-blocking mode (O_NONBLOCK).\n");
            fprintf(stderr, "       -T          Only permit device that is a TTY.\n");
            fprintf(stderr, "       -b BPS      Bits per second.\n");
            fprintf(stderr, "       -c          Block until DCD is asserted (implies -m, forbids -B, -F).\n");
            fprintf(stderr, "       -d          Emit characters on standard error using phex.\n");
            fprintf(stderr, "       -e          Even parity.\n");
            fprintf(stderr, "       -o          Odd parity.\n");
            fprintf(stderr, "       -n          No parity.\n");
            fprintf(stderr, "       -h          Hardware flow control (RTS/CTS).\n");
            fprintf(stderr, "       -i          Don't use stdin.\n");
            fprintf(stderr, "       -l          Local (no modem control).\n");
            fprintf(stderr, "       -m          Modem control.\n");
            fprintf(stderr, "       -p          Printable only ('!' to '~').\n");
            fprintf(stderr, "       -r          Place stdin and stdout in raw mode.\n");
            fprintf(stderr, "       -s          Software flow control (XON/XOFF).\n");
            fprintf(stderr, "       -t SECONDS  Timeout in SECONDS.\n");
            fprintf(stderr, "       -v          Print characters on standard error.\n");
            return 1;
            break;

        }

    }

    hertz = diminuto_frequency();

    action.sa_handler = handler;
    diminuto_contract(sigaction(SIGPIPE, &action, (struct sigaction *)0) >= 0);
    diminuto_contract(sigaction(SIGINT, &action, (struct sigaction *)0) >= 0);
    diminuto_contract(sigaction(SIGQUIT, &action, (struct sigaction *)0) >= 0);
    diminuto_contract(sigaction(SIGTERM, &action, (struct sigaction *)0) >= 0);
    diminuto_contract(sigaction(SIGHUP, &action, (struct sigaction *)0) >= 0);
    diminuto_contract(sigaction(SIGALRM, &action, (struct sigaction *)0) >= 0);

    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "%s %s %dbps %d%c%d %s %s %s %s %s %s %s %s %useconds %zubytes %zumodulo\n",
        forward ? "implement-loopback" : backward ? "test-loopback" : "interactive",
        device, bitspersecond, databits, "NOE"[paritybit], stopbits,
        onlyaterminal ? "ttyonly" : "anydevice",
        nonblocking ? "nonblocking" : "blocking",
        noinput ? "noinput" : "input",
        modemcontrol ? "modem" : "local",
        carrierdetect ? "dcd" : "nodcd",
        xonxoff ? "xonxoff" : "noswflow",
        rtscts ? "rtscts" : "nohwflow",
        printable ? "printable" : "all",
        seconds,
        maximum,
        modulo);

    fd = open(device, (O_RDWR | O_NOCTTY | (nonblocking ? O_NONBLOCK : 0)));
    diminuto_contract(fd >= 0);
    diminuto_contract(fd != STDIN_FILENO);
    diminuto_contract(fd != STDOUT_FILENO);

    rc = fcntl(fd, F_SETFL, (O_RDONLY | (nonblocking ? O_NONBLOCK : 0))); /* Because screen(1) does it. */
    diminuto_contract(rc == 0);

    isaterminal = diminuto_serial_valid(fd);
    diminuto_contract(isaterminal || (!onlyaterminal));

    if (isaterminal) {

        rc = ioctl(fd, TIOCEXCL);
        diminuto_contract(rc == 0);

        rc = ioctl(fd, TCFLSH, TCIOFLUSH);
        diminuto_contract(rc == 0);

        rc = diminuto_serial_set(fd, bitspersecond, databits, paritybit, stopbits, modemcontrol, xonxoff, rtscts);
        diminuto_contract(rc == 0);

        rc = diminuto_serial_raw(fd);
        diminuto_contract(rc == 0);

        bitspercharacter = 1 /* start bit */ + databits + ((paritybit != 0) ? 1 : 0) + stopbits;

        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "%s isatty\n", device);

    } else {

        databits = 8;
        paritybit = 0;
        stopbits = 0;

        bitspercharacter = 8;

        DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "%s !isatty\n", device);

    }

    if (rawterminal) {
        rc = diminuto_serial_raw(STDIN_FILENO);
        diminuto_contract(rc == 0);
        rc = diminuto_serial_raw(STDOUT_FILENO);
        diminuto_contract(rc == 0);
    }

    output = printable ? '!' : 0x00;

    count = 0;

    then = diminuto_time_elapsed();

    if (forward) {

        /*
         * Loop Forward: we receive a character and write that same character
         * back out.
         */

        fp = fdopen(fd, "r+");
        diminuto_contract(fp != (FILE *)0);

        rc = diminuto_serial_unbuffered(fp);
        diminuto_contract(rc == 0);

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
                if (modulo == 0) {
                    /* Do nothing. */
                } else if ((count % modulo) > 0) {
                    /* Do nothing. */
                } else {
                    now = diminuto_time_elapsed();
                    elapsed = (now - then) / (double)hertz;
                    bandwidth = (count * bitspercharacter) / elapsed;
                    DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "modulo %zubytes %.6lfs %dbaud %dbpC %.6lfbps\n", count, elapsed, bitspersecond, bitspercharacter, bandwidth);
                }
            } else if (done) {
                now = diminuto_time_elapsed();
                elapsed = (now - then) / (double)hertz;
                bandwidth = (count * bitspercharacter) / elapsed;
                DIMINUTO_LOG_NOTICE(DIMINUTO_LOG_HERE "done %zubytes %.6lfs %dbaud %dbpC %.6lfbps\n", count, elapsed, bitspersecond, bitspercharacter, bandwidth);
            } else if (curious) {
                now = diminuto_time_elapsed();
                elapsed = (now - then) / (double)hertz;
                bandwidth = (count * bitspercharacter) / elapsed;
                DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "curious %zubytes %.6lfs %dbaud %dbpC %.6lfbps\n", count, elapsed, bitspersecond, bitspercharacter, bandwidth);
                curious = 0;
            } else {
                now = diminuto_time_elapsed();
                elapsed = (now - then) / (double)hertz;
                bandwidth = (count * bitspercharacter) / elapsed;
                DIMINUTO_LOG_NOTICE(DIMINUTO_LOG_HERE "eof %zubytes %.6lfs %dbaud %dbpC %.6lfbps\n", count, elapsed, bitspersecond, bitspercharacter, bandwidth);
                done = !0;
            }
        }

    } else if (backward) {

        /*
         * Loop Backward: we write a character and expect to receive that same
         * character back. This is the default.
         */

        fp = fdopen(fd, "r+");
        diminuto_contract(fp != (FILE *)0);

        rc = diminuto_serial_unbuffered(fp);
        diminuto_contract(rc == 0);

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
                    if (modulo == 0) {
                        /* Do nothing. */
                    } else if ((count % modulo) > 0) {
                        /* Do nothing. */
                    } else {
                        now = diminuto_time_elapsed();
                        elapsed = (now - then) / (double)hertz;
                        bandwidth = (count * bitspercharacter) / elapsed;
                        DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "modulo %zubytes %.6lfs %dbaud %dbpC %.6lfbps\n", count, elapsed, bitspersecond, bitspercharacter, bandwidth);
                    }
                    break;
                } else if (alarmed) {
                    now = diminuto_time_elapsed();
                    elapsed = (now - then) / (double)hertz;
                    bandwidth = (count * bitspercharacter) / elapsed;
                    DIMINUTO_LOG_ERROR(DIMINUTO_LOG_HERE "timeout %zubytes %.6lfs %dbaud %dbpC %.6lfbps\n", count, elapsed, bitspersecond, bitspercharacter, bandwidth);
                    done = !0;
                    break;
                } else if (done) {
                    now = diminuto_time_elapsed();
                    elapsed = (now - then) / (double)hertz;
                    bandwidth = (count * bitspercharacter) / elapsed;
                    DIMINUTO_LOG_NOTICE(DIMINUTO_LOG_HERE "done %zubytes %.6lfs %dbaud %dbpC %.6lfbps\n", count, elapsed, bitspersecond, bitspercharacter, bandwidth);
                    break;
                } else if (curious) {
                    now = diminuto_time_elapsed();
                    elapsed = (now - then) / (double)hertz;
                    bandwidth = (count * bitspercharacter) / elapsed;
                    DIMINUTO_LOG_INFORMATION(DIMINUTO_LOG_HERE "curious %zubytes %.6lfs %dbaud %dbpC %.6lfbps\n", count, elapsed, bitspersecond, bitspercharacter, bandwidth);
                    curious = 0;
                } else {
                    now = diminuto_time_elapsed();
                    elapsed = (now - then) / (double)hertz;
                    bandwidth = (count * bitspercharacter) / elapsed;
                    DIMINUTO_LOG_NOTICE(DIMINUTO_LOG_HERE "eof %zubytes %.6lfs %dbaud %dbpC %.6lfbps\n", count, elapsed, bitspersecond, bitspercharacter, bandwidth);
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

        diminuto_contract(maximum > 0);
        buffer = malloc(maximum);
        diminuto_contract(buffer != (void *)0);
        diminuto_mux_init(&mux);
        diminuto_contract(fd != STDIN_FILENO);
        diminuto_contract(fd != STDOUT_FILENO);
        rc = diminuto_mux_register_read(&mux, fd);
        diminuto_contract(rc >= 0);
        if (!noinput) {
            rc = diminuto_mux_register_read(&mux, STDIN_FILENO);
            diminuto_contract(rc >= 0);
        }
        while (!done) {
            if (carrierdetect) {
                while (!0) {
                    rc = diminuto_serial_status(fd);
                    diminuto_contract(rc >= 0);
                    if (rc) {
                        break;
                    }
                    rc = diminuto_serial_wait(fd);
                    diminuto_contract(rc >= 0);
                }
            }
            fds = diminuto_mux_wait(&mux, -1);
            if (fds <= 0) {
                done = !0;
            }
            while (!done) {
                ready = diminuto_mux_ready_read(&mux);
                if (ready < 0) {
                    break;
                } else if (ready == fd) {
                    reads = diminuto_fd_read(fd, buffer, maximum);
                    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "read(%d)=%zu\n", fd, reads);
                    diminuto_contract(reads >= 0);
                    diminuto_contract(reads <= maximum);
                    if (reads == 0) {
                        done = !0;
                        break;
                    }
                    if (debug) {
                        char * bb;
                        for (bb = (char *)buffer, count = reads; count > 0; --count) {
                            diminuto_phex_emit(stderr, *(bb++), 72, 0, 0, 0, &current, &end, 0);
                        }
                        fputs("\n", stderr);
                    }
                    writes = diminuto_fd_write(STDOUT_FILENO, buffer, reads);
                    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "wrote(%d)=%zu\n", STDOUT_FILENO, writes);
                    diminuto_contract(writes == reads);
                } else if (ready == STDIN_FILENO) {
                    reads = diminuto_fd_read(STDIN_FILENO, buffer, maximum);
                    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "read(%d)=%zu\n", STDIN_FILENO, reads);
                    diminuto_contract(reads >= 0);
                    diminuto_contract(reads <= maximum);
                    if (reads == 0) {
                        done = !0;
                        break;
                    }
                    if (debug) {
                        char * bb;
                        for (bb = (char *)buffer, count = reads; count > 0; --count) {
                            diminuto_phex_emit(stderr, *(bb++), 72, 0, 0, 0, &current, &end, 0);
                        }
                        fputs("\n", stderr);
                    }
                    writes = diminuto_fd_write(fd, buffer, reads);
                    diminuto_contract(writes == reads);
                    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "wrote(%d)=%zu\n", fd, writes);
                } else {
                    diminuto_panic();
                }
            }
        }
        rc = diminuto_mux_unregister_read(&mux, STDIN_FILENO);
        diminuto_contract(rc >= 0);
        rc = diminuto_mux_unregister_read(&mux, fd);
        diminuto_contract(rc >= 0);

    }

    DIMINUTO_LOG_DEBUG(DIMINUTO_LOG_HERE "fini\n");

    exit(0);
}
