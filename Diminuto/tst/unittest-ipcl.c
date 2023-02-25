/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the IPC feature for Local sockets.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the IPC feature for Local (UNIX Domain) sockets.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_ipcl.h"
#include "com/diag/diminuto/diminuto_dump.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_alarm.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_countof.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

static const char LOCAL1[] = "/tmp/com-diag-diminuto-unittest-ipcl-1.sock";
static const char LOCAL2[] = "/tmp/com-diag-diminuto-unittest-ipcl-2.sock";
static const char * UNNAMED = DIMINUTO_IPCL_UNNAMED;
static const size_t TOTAL = 1024 * 1024 * 100;

int main(int argc, char * argv[])
{
    diminuto_ticks_t hertz;

    SETLOGMASK();

    hertz = diminuto_frequency();

    {
        TEST();

        EXPECT(!diminuto_ipcl_is_unnamed(LOCAL1));
        EXPECT(!diminuto_ipcl_is_unnamed(LOCAL2));
        EXPECT(diminuto_ipcl_is_unnamed(UNNAMED));
        EXPECT(!diminuto_ipcl_is_unnamed(NULL));

        STATUS();
    }

    {
        int rc;

        TEST();

        rc = diminuto_ipcl_compare("/unix.sock", "/unix.sock");
        COMMENT("rc=%d\n", rc);
        rc = diminuto_ipcl_compare("/unix.sock1", "/unix.sock2");
        COMMENT("rc=%d\n", rc);
        rc = diminuto_ipcl_compare("/unix.sock2", "/unix.sock1");
        COMMENT("rc=%d\n", rc);
        rc = diminuto_ipcl_compare(NULL, "/unix.sock");
        COMMENT("rc=%d\n", rc);
        rc = diminuto_ipcl_compare("/unix.sock", NULL);
        COMMENT("rc=%d\n", rc);
        rc = diminuto_ipcl_compare(NULL, NULL);
        COMMENT("rc=%d\n", rc);

        EXPECT(diminuto_ipcl_compare(LOCAL1, LOCAL1) == 0);
        EXPECT(diminuto_ipcl_compare(LOCAL1, LOCAL2) != 0);
        EXPECT(diminuto_ipcl_compare(LOCAL1, UNNAMED) != 0);
        EXPECT(diminuto_ipcl_compare(LOCAL1, NULL) != 0);

        EXPECT(diminuto_ipcl_compare(LOCAL2, LOCAL1) != 0);
        EXPECT(diminuto_ipcl_compare(LOCAL2, LOCAL2) == 0);
        EXPECT(diminuto_ipcl_compare(LOCAL2, UNNAMED) != 0);
        EXPECT(diminuto_ipcl_compare(LOCAL2, NULL) != 0);

        EXPECT(diminuto_ipcl_compare(UNNAMED, LOCAL1) != 0);
        EXPECT(diminuto_ipcl_compare(UNNAMED, LOCAL2) != 0);
        EXPECT(diminuto_ipcl_compare(UNNAMED, UNNAMED) == 0);
        EXPECT(diminuto_ipcl_compare(UNNAMED, NULL) != 0);

        EXPECT(diminuto_ipcl_compare(NULL, LOCAL1) != 0);
        EXPECT(diminuto_ipcl_compare(NULL, LOCAL2) != 0);
        EXPECT(diminuto_ipcl_compare(NULL, UNNAMED) != 0);
        EXPECT(diminuto_ipcl_compare(NULL, NULL) != 0);

        STATUS();
    }

#define CANONICALIZE(_PATH_, _FILE_, _RESULT_) \
    do { \
        static const char file[] = _FILE_; \
        const char * relative = (_PATH_); \
        diminuto_local_t absolute = { '\xa5', }; \
        char * result; \
        const char * printable; \
        size_t minimum = 0; \
        size_t actual = 0; \
        diminuto_local_buffer_t buffer = { '\0', }; \
        minimum = strlen(file); \
        result = diminuto_ipcl_path(relative, absolute, sizeof(absolute)); \
        actual = strlen(absolute); \
        printable = diminuto_ipcl_path2string(result, buffer, sizeof(buffer)); \
        EXPECT(printable != (const char *)0); \
        COMMENT("relative=\"%s\" absolute=\"%s\" %s", relative, printable, (result == (char *)0) ? "INVALID" : diminuto_ipcl_is_unnamed(result) ? "UNNAMED" : "PATH"); \
        EXPECT(\
            ((_RESULT_) && \
                (result == absolute) && \
                (absolute[0] == '/') && \
                (strlen(absolute) >= minimum) && \
                (strcmp(absolute + actual - minimum, file) == 0)) || \
            ((_RESULT_) && \
                (result == absolute) && \
                (relative[0] == '\0') && \
                (absolute[0] == '\0')) || \
            ((!(_RESULT_)) && \
                (result == (char *)0) && \
                (absolute[0] == '\xa5'))); \
    } while (0)

    {
        TEST();

        /*
         * See also the unit tests for FS Canonicalize and IPC Endpoint
         * which test the same underlying realpath(3)-based capability.
         */

        CANONICALIZE("/", "", 0); 
        CANONICALIZE("/.", "", 0); 
        CANONICALIZE("/..", "", 0); 
        CANONICALIZE("/tmp/", "", 0); 
        CANONICALIZE("/var/tmp/", "", 0); 
        CANONICALIZE("/var/tmp/../run/", "", 0); 
        CANONICALIZE("/var/does-not-exist/unix.sock", "", 0); 
        CANONICALIZE("/unix.sock", "/unix.sock", !0); 
        CANONICALIZE("/.unix.sock", "/.unix.sock", !0); 
        CANONICALIZE("/../unix.sock", "/unix.sock", !0); 
        CANONICALIZE("/tmp/unix.sock", "/unix.sock", !0); 
        CANONICALIZE("/var/tmp/unix.sock", "/unix.sock", !0); 
        CANONICALIZE("/var/tmp/../../run/unix.sock", "/unix.sock", !0); 
        CANONICALIZE("/var/tmp/../../run/./unix.sock", "/unix.sock", !0); 
        CANONICALIZE("unix.sock", "/unix.sock", !0); 
        CANONICALIZE(".unix.sock", "/.unix.sock", !0); 
        CANONICALIZE("./unix.sock", "/unix.sock", !0); 
        CANONICALIZE("../unix.sock", "/unix.sock", !0); 
        CANONICALIZE("", "", !0); 

        STATUS();
    }

    {
        char * path;
        diminuto_local_buffer_t buffer;

        TEST();

        /*
         * Special cases.
         */

        path = (char *)0;
        COMMENT("NULL before=NULL after=\"%s\"\n", diminuto_ipcl_path2string(path, buffer, sizeof(buffer)));
        EXPECT(strcmp(buffer, "*") == 0);

        path = "";
        COMMENT("EMPTY before=\"%s\" after=\"%s\"\n", path, diminuto_ipcl_path2string(path, buffer, sizeof(buffer)));
        EXPECT(strcmp(buffer, "") == 0);

        path = ".";
        COMMENT("DOT before=\"%s\" after=\"%s\"\n", path, diminuto_ipcl_path2string(path, buffer, sizeof(buffer)));
        EXPECT(strcmp(buffer + strlen(buffer) - strlen(path), path) == 0);

        path = "..";
        COMMENT("DOTDOT before=\"%s\" after=\"%s\"\n", path, diminuto_ipcl_path2string(path, buffer, sizeof(buffer)));
        EXPECT(strcmp(buffer + strlen(buffer) - strlen(path), path) == 0);

        path = "~";
        COMMENT("TILDE before=\"%s\" after=\"%s\"\n", path, diminuto_ipcl_path2string(path, buffer, sizeof(buffer)));
        EXPECT(strcmp(buffer + strlen(buffer) - strlen(path), path) == 0);

        path = "/";
        COMMENT("SLASH before=\"%s\" after=\"%s\"\n", path, diminuto_ipcl_path2string(path, buffer, sizeof(buffer)));
        EXPECT(strcmp(buffer, "*") == 0);

        buffer[0] = 0xa5;
        path = "/";
        COMMENT("ZERO before=\"%s\" after=0x%x\n", path, (uint8_t)*diminuto_ipcl_path2string(path, buffer, 0));
        EXPECT((uint8_t)buffer[0] == 0xa5U);

        path = "/";
        COMMENT("ONE before=\"%s\" after=\"%s\"\n", path, diminuto_ipcl_path2string(path, buffer, 1));
        EXPECT(strcmp(buffer, "") == 0);

        path = "/";
        COMMENT("TWO before=\"%s\" after=\"%s\"\n", path, diminuto_ipcl_path2string(path, buffer, 2));
        EXPECT(strcmp(buffer, "*") == 0);

        path = "/";
        COMMENT("THREE before=\"%s\" after=\"%s\"\n", path, diminuto_ipcl_path2string(path, buffer, 3));
        EXPECT(strcmp(buffer, "*") == 0);

        path = ".gitignore";
        COMMENT("DOTFILE before=\"%s\" after=\"%s\"\n", path, diminuto_ipcl_path2string(path, buffer, sizeof(buffer)));
        EXPECT(strcmp(buffer + strlen(buffer) - strlen(path), path) == 0);

        path = "./Makefile";
        COMMENT("DOTSLASHFILE before=\"%s\" after=\"%s\"\n", path, diminuto_ipcl_path2string(path, buffer, sizeof(buffer)));
        EXPECT(strcmp(buffer + strlen(buffer) - strlen(path) + sizeof("./") - 1, path + sizeof("./") - 1) == 0);

        path = "../Makefile";
        COMMENT("DOTDOTSLASHFILE before=\"%s\" after=\"%s\"\n", path, diminuto_ipcl_path2string(path, buffer, sizeof(buffer)));
        EXPECT(strcmp(buffer + strlen(buffer) - strlen(path) + sizeof("../") - 1, path + sizeof("../") - 1) == 0);

        path = "~/.profile";
        COMMENT("TILDESLASHFILE before=\"%s\" after=\"%s\"\n", path, diminuto_ipcl_path2string(path, buffer, sizeof(buffer)));
        EXPECT(strcmp(buffer, "*") == 0);

        STATUS();
    }

    {
        diminuto_local_t first;
        diminuto_local_t second;

        TEST();

        EXPECT(diminuto_ipcl_compare(diminuto_ipcl_path("", first, sizeof(first)), diminuto_ipcl_path("", second, sizeof(second))) == 0);
        EXPECT(diminuto_ipcl_compare(diminuto_ipcl_path("/tmp/unix.sock", first, sizeof(first)), diminuto_ipcl_path("/tmp/unix.sock", second, sizeof(second))) == 0);
        EXPECT(diminuto_ipcl_compare(diminuto_ipcl_path("./unix.sock", first, sizeof(first)), diminuto_ipcl_path("./unix.sock", second, sizeof(second))) == 0);
        EXPECT(diminuto_ipcl_compare(diminuto_ipcl_path("/run/../tmp/unix.sock", first, sizeof(first)), diminuto_ipcl_path("/tmp/unix.sock", second, sizeof(second))) == 0);
        EXPECT(diminuto_ipcl_compare(diminuto_ipcl_path("/tmp/unix.sock1", first, sizeof(first)), diminuto_ipcl_path("/tmp/unix.sock2", second, sizeof(second))) != 0);
        EXPECT(diminuto_ipcl_compare(diminuto_ipcl_path("/IMBAD/unix.sock", first, sizeof(first)), diminuto_ipcl_path("/tmp/unix.sock", second, sizeof(second))) != 0);
        EXPECT(diminuto_ipcl_compare(diminuto_ipcl_path("/tmp/unix.sock", first, sizeof(first)), diminuto_ipcl_path("/IMBAD/unix.sock", second, sizeof(second))) != 0);
        EXPECT(diminuto_ipcl_compare(diminuto_ipcl_path("/IMBAD/unix.sock", first, sizeof(first)), diminuto_ipcl_path("/IMBAD/unix.sock", second, sizeof(second))) != 0);

        STATUS();
    }

    {
        TEST();

        ADVISE(diminuto_ipcl_remove(LOCAL1) < 0);
        ADVISE(diminuto_ipcl_remove(LOCAL2) < 0);

        STATUS();
    }

    {
        int fd;
        diminuto_local_t local;

        TEST();

        COMMENT("endpoint=\"%s\"\n", LOCAL1);
        ASSERT((fd = diminuto_ipcl_datagram_peer(LOCAL1)) >= 0);
        EXPECT(diminuto_ipc_type(fd) == AF_LOCAL);
        EXPECT(diminuto_ipcl_nearend(fd, local, sizeof(local)) >= 0);
        COMMENT("datagram peer nearend=\"%s\"\n", local);
        EXPECT(diminuto_ipcl_farend(fd, local, sizeof(local)) < 0);
        EXPECT(diminuto_ipcl_close(fd) >= 0);
        EXPECT((fd = diminuto_ipcl_datagram_peer(LOCAL1)) < 0);
        EXPECT(diminuto_ipc_type(fd) == AF_UNSPEC);
        ASSERT(diminuto_ipcl_remove(LOCAL1) >= 0);
        EXPECT((fd = diminuto_ipcl_datagram_peer(LOCAL1)) >= 0);
        EXPECT(diminuto_ipc_type(fd) == AF_LOCAL);
        EXPECT(diminuto_ipcl_close(fd) >= 0);
        ASSERT(diminuto_ipcl_remove(LOCAL1) >= 0);

        STATUS();
    }

    {
        int fd;
        diminuto_local_t local;

        TEST();

        COMMENT("endpoint=\"%s\"\n", UNNAMED);
        ASSERT((fd = diminuto_ipcl_datagram_peer(UNNAMED)) >= 0);
        EXPECT(diminuto_ipc_type(fd) == AF_LOCAL);
        EXPECT(diminuto_ipcl_nearend(fd, local, sizeof(local)) >= 0);
        COMMENT("datagram peer nearend=\"%s\"\n", local);
        EXPECT(diminuto_ipcl_is_unnamed(local));
        EXPECT(diminuto_ipcl_farend(fd, local, sizeof(local)) < 0);
        EXPECT(diminuto_ipcl_close(fd) >= 0);
        EXPECT((fd = diminuto_ipcl_datagram_peer(UNNAMED)) >= 0);
        EXPECT(diminuto_ipc_type(fd) == AF_LOCAL);
        EXPECT(diminuto_ipcl_close(fd) >= 0);
        EXPECT((fd = diminuto_ipcl_datagram_peer(UNNAMED)) >= 0);
        EXPECT(diminuto_ipc_type(fd) == AF_LOCAL);
        EXPECT(diminuto_ipcl_close(fd) >= 0);

        STATUS();
    }

    {
        int fd;
        diminuto_local_t local;

        TEST();

        COMMENT("endpoint=\"%s\"\n", LOCAL1);
        ASSERT((fd = diminuto_ipcl_stream_provider(LOCAL1)) >= 0);
        EXPECT(diminuto_ipc_type(fd) == AF_LOCAL);
        EXPECT(diminuto_ipcl_nearend(fd, local, sizeof(local)) >= 0);
        COMMENT("stream provider nearend=\"%s\"\n", local);
        EXPECT(diminuto_ipcl_farend(fd, local, sizeof(local)) < 0);
        EXPECT(diminuto_ipcl_close(fd) >= 0);
        ASSERT((fd = diminuto_ipcl_stream_provider(LOCAL1)) < 0);
        EXPECT(diminuto_ipc_type(fd) == AF_UNSPEC);
        ASSERT(diminuto_ipcl_remove(LOCAL1) >= 0);
        ASSERT((fd = diminuto_ipcl_stream_provider(LOCAL1)) >= 0);
        EXPECT(diminuto_ipc_type(fd) == AF_LOCAL);
        EXPECT(diminuto_ipcl_close(fd) >= 0);
        ASSERT(diminuto_ipcl_remove(LOCAL1) >= 0);

        STATUS();
    }

    {
        int fd;

        TEST();

        COMMENT("endpoint=\"%s\"\n", UNNAMED);
        ASSERT((fd = diminuto_ipcl_stream_provider(UNNAMED)) < 0);

        STATUS();
    }

    {
        int fd;

        TEST();

        ASSERT((fd = diminuto_ipcl_stream_provider(LOCAL1)) >= 0);
        EXPECT(diminuto_ipc_type(fd) == AF_LOCAL);
        ASSERT(diminuto_ipcl_remove(LOCAL1) >= 0);

        EXPECT(diminuto_ipc_set_nonblocking(fd, !0) >= 0);
        EXPECT(diminuto_ipc_set_nonblocking(fd, 0) >= 0);

        EXPECT(diminuto_ipc_set_reuseaddress(fd, 0) >= 0);
        EXPECT(diminuto_ipc_set_reuseaddress(fd, !0) >= 0);

        EXPECT(diminuto_ipc_set_keepalive(fd, !0) >= 0);
        EXPECT(diminuto_ipc_set_keepalive(fd, 0) >= 0);

        if (geteuid() == 0) {
            EXPECT(diminuto_ipc_set_debug(fd, !0) >= 0);
            EXPECT(diminuto_ipc_set_debug(fd, 0) >= 0);
        }

        EXPECT(diminuto_ipc_set_linger(fd, hertz) >= 0);
        EXPECT(diminuto_ipc_set_linger(fd, 0) >= 0);

        EXPECT(diminuto_ipcl_close(fd) >= 0);

        STATUS();
    }

    {
        int fd1;
        int fd2;
        const char MSG1[] = "Chip Overclock";
        const char MSG2[] = "Digital Aggregates Corporation";
        char buffer[64];
        diminuto_local_t local1;
        diminuto_local_t local2;

        /*
         * UNNAMED can send more than once to LOCAL2. Note that unix(7)
         * says that the sending socket name "should not be inspected"
         * when it is unnamed. The near-end name appears to be the empty
         * string (""). The receiver can't know ahead of time - just from
         * this API anyway - that there is no far-end name.
         */

        TEST();

        EXPECT((fd1 = diminuto_ipcl_datagram_peer(UNNAMED)) >= 0);
        EXPECT(diminuto_ipcl_nearend(fd1, local1, sizeof(local1)) >= 0);
        COMMENT("local1=\"%s\"\n", local1);
        EXPECT(diminuto_ipcl_is_unnamed(local1));

        EXPECT((fd2 = diminuto_ipcl_datagram_peer(LOCAL2)) >= 0);
        EXPECT(diminuto_ipcl_nearend(fd2, local2, sizeof(local2)) >= 0);
        COMMENT("local2=\"%s\"\n", local2);
        EXPECT(diminuto_ipcl_compare(local2, LOCAL2) == 0);

        /* This only works because the kernel buffers socket data. */

        ASSERT((diminuto_ipcl_datagram_send(fd1, MSG1, sizeof(MSG1), LOCAL2)) == sizeof(MSG1));
        ASSERT((diminuto_ipcl_datagram_receive(fd2, buffer, sizeof(buffer))) == sizeof(MSG1));
        EXPECT(strcmp(buffer, MSG1) == 0);

        ASSERT((diminuto_ipcl_datagram_send(fd1, MSG2, sizeof(MSG2), LOCAL2)) == sizeof(MSG2));
        ASSERT((diminuto_ipcl_datagram_receive(fd2, buffer, sizeof(buffer))) == sizeof(MSG2));
        EXPECT(strcmp(buffer, MSG2) == 0);

        EXPECT(diminuto_ipcl_close(fd1) >= 0);
        EXPECT(diminuto_ipcl_close(fd2) >= 0);

        ASSERT(diminuto_ipcl_remove(LOCAL2) >= 0);

        STATUS();
    }

    {
        int fd1;
        int fd2;
        const char MSG1[] = "Chip Overclock";
        const char MSG2[] = "Digital Aggregates Corporation";
        char buffer[64];
        diminuto_local_t local1;
        diminuto_local_t local2;
        diminuto_local_t local3;
        diminuto_local_t local4;

        /*
         * LOCAL1 can send to LOCAL2, and LOCAL2 can reply using
         * an explicit path.
         */

        TEST();

        ASSERT((fd1 = diminuto_ipcl_datagram_peer(LOCAL1)) >= 0);
        EXPECT(diminuto_ipcl_nearend(fd1, local1, sizeof(local1)) >= 0);
        COMMENT("local1=\"%s\"\n", local1);
        EXPECT(diminuto_ipcl_compare(local1, LOCAL1) == 0);

        ASSERT((fd2 = diminuto_ipcl_datagram_peer(LOCAL2)) >= 0);
        EXPECT(diminuto_ipcl_nearend(fd2, local2, sizeof(local2)) >= 0);
        COMMENT("local2=\"%s\"\n", local2);
        EXPECT(diminuto_ipcl_compare(local2, LOCAL2) == 0);

        /* This only works because the kernel buffers socket data. */

        ASSERT((diminuto_ipcl_datagram_send(fd1, MSG1, sizeof(MSG1), LOCAL2)) == sizeof(MSG1));
        ASSERT((diminuto_ipcl_datagram_receive_generic(fd2, buffer, sizeof(buffer), local3, sizeof(local3), 0)) == sizeof(MSG1));
        COMMENT("local3=\"%s\"\n", local3);
        EXPECT(diminuto_ipcl_compare(local3, LOCAL1) == 0);
        EXPECT(strcmp(buffer, MSG1) == 0);

        ASSERT((diminuto_ipcl_datagram_send(fd2, MSG2, sizeof(MSG2), LOCAL1)) == sizeof(MSG2));
        ASSERT((diminuto_ipcl_datagram_receive_generic(fd1, buffer, sizeof(buffer), local4, sizeof(local4), 0)) == sizeof(MSG2));
        COMMENT("local4=\"%s\"\n", local4);
        EXPECT(diminuto_ipcl_compare(local4, LOCAL2) == 0);
        EXPECT(strcmp(buffer, MSG2) == 0);

        EXPECT(diminuto_ipcl_close(fd1) >= 0);
        EXPECT(diminuto_ipcl_close(fd2) >= 0);

        ASSERT(diminuto_ipcl_remove(LOCAL1) >= 0);
        ASSERT(diminuto_ipcl_remove(LOCAL2) >= 0);

        STATUS();
    }

    {
        int fd1;
        int fd2;
        const char MSG1[] = "Chip Overclock";
        const char MSG2[] = "Digital Aggregates Corporation";
        char buffer[64];
        diminuto_local_t local1;
        diminuto_local_t local2;
        diminuto_local_t local3;
        diminuto_local_t local4;

        /*
         * LOCAL1 can send to LOCAL2, and LOCAL2 can reply using
         * an implicit path; this only works because the original
         * sender is named.
         */

        TEST();

        ASSERT((fd1 = diminuto_ipcl_datagram_peer(LOCAL1)) >= 0);
        EXPECT(diminuto_ipcl_nearend(fd1, local1, sizeof(local1)) >= 0);
        COMMENT("local1=\"%s\"\n", local1);
        EXPECT(diminuto_ipcl_compare(local1, LOCAL1) == 0);

        ASSERT((fd2 = diminuto_ipcl_datagram_peer(LOCAL2)) >= 0);
        EXPECT(diminuto_ipcl_nearend(fd2, local2, sizeof(local2)) >= 0);
        COMMENT("local2=\"%s\"\n", local2);
        EXPECT(diminuto_ipcl_compare(local2, LOCAL2) == 0);

        /* This only works because the kernel buffers socket data. */

        ASSERT((diminuto_ipcl_datagram_send(fd1, MSG1, sizeof(MSG1), LOCAL2)) == sizeof(MSG1));
        ASSERT((diminuto_ipcl_datagram_receive_generic(fd2, buffer, sizeof(buffer), local3, sizeof(local3), 0)) == sizeof(MSG1));
        COMMENT("local3=\"%s\"\n", local3);
        EXPECT(diminuto_ipcl_compare(local3, LOCAL1) == 0);
        EXPECT(strcmp(buffer, MSG1) == 0);

        ASSERT((diminuto_ipcl_datagram_send(fd2, MSG2, sizeof(MSG2), local3)) == sizeof(MSG2));
        ASSERT((diminuto_ipcl_datagram_receive_generic(fd1, buffer, sizeof(buffer), local4, sizeof(local4), 0)) == sizeof(MSG2));
        COMMENT("local4=\"%s\"\n", local4);
        EXPECT(diminuto_ipcl_compare(local4, LOCAL2) == 0);
        EXPECT(strcmp(buffer, MSG2) == 0);

        EXPECT(diminuto_ipcl_close(fd1) >= 0);
        EXPECT(diminuto_ipcl_close(fd2) >= 0);

        ASSERT(diminuto_ipcl_remove(LOCAL1) >= 0);
        ASSERT(diminuto_ipcl_remove(LOCAL2) >= 0);

        STATUS();
    }

    {
        int fd;
        char buffer[1];
        diminuto_local_t rendezvous = "CHANGED";
        diminuto_local_t local = "UNCHANGED";

        TEST();

        ASSERT((fd = diminuto_ipcl_datagram_peer(UNNAMED)) >= 0);
        EXPECT(diminuto_ipc_type(fd) == AF_LOCAL);
        EXPECT(diminuto_ipcl_nearend(fd, rendezvous, sizeof(rendezvous)) >= 0);
        COMMENT("rendezvous=\"%s\"", rendezvous);
        EXPECT(diminuto_ipcl_is_unnamed(rendezvous));
        EXPECT(diminuto_ipc_set_nonblocking(fd, !0) >= 0);

        ASSERT((diminuto_ipcl_datagram_receive_generic(fd, buffer, sizeof(buffer), local, sizeof(local), 0)) < 0);
        EXPECT(errno == EAGAIN);
        COMMENT("local=\"%s\"", local);
        EXPECT(diminuto_ipcl_compare(local, "UNCHANGED") == 0);

        EXPECT(diminuto_ipcl_close(fd) >= 0);

        STATUS();
    }

    {
        int fd;
        char buffer[1];
        diminuto_local_t rendezvous = "CHANGED";
        diminuto_local_t local = "UNCHANGED";

        TEST();

        ASSERT((fd = diminuto_ipcl_datagram_peer(UNNAMED)) >= 0);
        EXPECT(diminuto_ipc_type(fd) == AF_LOCAL);
        EXPECT(diminuto_ipcl_nearend(fd, rendezvous, sizeof(rendezvous)) >= 0);
        COMMENT("rendezvous=\"%s\"", rendezvous);
        EXPECT(diminuto_ipcl_is_unnamed(rendezvous));

        ASSERT((diminuto_ipcl_datagram_receive_generic(fd, buffer, sizeof(buffer), local, sizeof(local), MSG_DONTWAIT)) < 0);
        EXPECT(errno == EAGAIN);
        COMMENT("local=\"%s\"", local);
        EXPECT(diminuto_ipcl_compare(local, "UNCHANGED") == 0);

        EXPECT(diminuto_ipcl_close(fd) >= 0);

        STATUS();
    }

    {
        int fd;
        char buffer[1];
        diminuto_local_t rendezvous = "CHANGED";
        diminuto_local_t local = "UNCHANGED";
        diminuto_sticks_t before;
        diminuto_sticks_t after;

        TEST();

        ASSERT((fd = diminuto_ipcl_datagram_peer(UNNAMED)) >= 0);
        EXPECT(diminuto_ipc_type(fd) == AF_LOCAL);
        EXPECT(diminuto_ipcl_nearend(fd, rendezvous, sizeof(rendezvous)) >= 0);
        COMMENT("rendezvous=\"%s\"", rendezvous);
        EXPECT(diminuto_ipcl_is_unnamed(rendezvous));

        EXPECT(diminuto_alarm_install(0) >= 0);
        EXPECT(diminuto_timer_oneshot(2000000ULL) == 0);
        EXPECT((before = diminuto_time_elapsed()) >= 0);

        ASSERT((diminuto_ipcl_datagram_receive_generic(fd, buffer, sizeof(buffer), local, sizeof(local), 0)) < 0);
        EXPECT(errno == EINTR);
        COMMENT("local=\"%s\"", local);
        EXPECT(diminuto_ipcl_compare(local, "UNCHANGED") == 0);

        EXPECT((after = diminuto_time_elapsed()) >= 0);
        EXPECT(diminuto_timer_oneshot(0ULL) >= 0);
        CHECKPOINT("elapsed %lld - %lld = %lld\n", (long long int)after, (long long int)before, (long long int)(after - before));
        EXPECT(diminuto_alarm_check());
        EXPECT((after - before) >= 2000000LL);

        EXPECT(diminuto_ipcl_close(fd) >= 0);

        STATUS();
    }

    {
        diminuto_local_t rendezvous;
        diminuto_local_t local;
        int service;
        pid_t pid;

        TEST();

        /*
         * Named consumer.
         */

        ASSERT((service = diminuto_ipcl_stream_provider(LOCAL1)) >= 0);
        EXPECT(diminuto_ipcl_nearend(service, rendezvous, sizeof(rendezvous)) >= 0);
        COMMENT("rendezvous=\"%s\"\n", rendezvous);
        EXPECT(diminuto_ipcl_compare(rendezvous, LOCAL1) == 0);

        EXPECT((pid = fork()) >= 0);

        if (pid != 0) {

            int producer;
            int status;

            local[0] = '\0';
            ASSERT((producer = diminuto_ipcl_stream_accept_generic(service, local, sizeof(local))) >= 0);
            COMMENT("local=\"%s\"\n", local);
            EXPECT(diminuto_ipcl_compare(local, LOCAL2) == 0);

            local[0] = '\0';
            EXPECT(diminuto_ipcl_nearend(producer, local, sizeof(local)) >= 0);
            COMMENT("local=\"%s\"\n", local);
            EXPECT(diminuto_ipcl_compare(local, LOCAL1) == 0);

            local[0] = '\0';
            EXPECT(diminuto_ipcl_farend(producer, local, sizeof(local)) >= 0);
            COMMENT("local=\"%s\"\n", local);
            EXPECT(diminuto_ipcl_compare(local, LOCAL2) == 0);

            diminuto_delay(hertz / 1000, !0);

            EXPECT(diminuto_ipcl_close(producer) >= 0);

            /*
             * If you don't wait for the child to exit, it may not yet have
             * closed its end of the socket thereby releasing the bound IP
             * address by the time the next unit test begins.
             */

            EXPECT(waitpid(pid, &status, 0) == pid);
            CHECKPOINT("pid=%d status=%d\n", pid, status);
            EXPECT(WIFEXITED(status));
            EXPECT(WEXITSTATUS(status) == 0);

            EXPECT(diminuto_ipcl_close(service) >= 0);

        } else {

            int consumer;

            EXPECT(diminuto_ipcl_close(service) >= 0);

            diminuto_delay(hertz / 1000, !0);

            ASSERT((consumer = diminuto_ipcl_stream_consumer_generic(LOCAL1, LOCAL2)) >= 0);

            diminuto_delay(hertz / 1000, !0);

            EXPECT(diminuto_ipcl_close(consumer) >= 0);

            EXIT();

        }

        ASSERT(diminuto_ipcl_remove(LOCAL1) >= 0);
        ASSERT(diminuto_ipcl_remove(LOCAL2) >= 0);

        STATUS();
    }

    {
        diminuto_local_t rendezvous;
        diminuto_local_t local;
        int service;
        pid_t pid;

        TEST();

        /*
         * Unnamed consumer.
         */

        ASSERT((service = diminuto_ipcl_stream_provider(LOCAL1)) >= 0);
        EXPECT(diminuto_ipcl_nearend(service, rendezvous, sizeof(rendezvous)) >= 0);
        COMMENT("rendezvous=\"%s\"\n", rendezvous);
        EXPECT(diminuto_ipcl_compare(rendezvous, LOCAL1) == 0);

        EXPECT((pid = fork()) >= 0);

        if (pid != 0) {

            int producer;
            int status;

            strcpy(local, "UNCHANGED");
            ASSERT((producer = diminuto_ipcl_stream_accept_generic(service, local, sizeof(local))) >= 0);
            COMMENT("local=\"%s\"\n", local);
            ADVISE(diminuto_ipcl_is_unnamed(local)); /* Unexpected. */

            strcpy(local, "UNCHANGED");
            EXPECT(diminuto_ipcl_nearend(producer, local, sizeof(local)) >= 0);
            COMMENT("local=\"%s\"\n", local);
            EXPECT(diminuto_ipcl_compare(local, LOCAL1) == 0); /* Expected. */

            strcpy(local, "UNCHANGED");
            EXPECT(diminuto_ipcl_farend(producer, local, sizeof(local)) >= 0);
            COMMENT("local=\"%s\"\n", local);
            ADVISE(diminuto_ipcl_is_unnamed(local)); /* Unexpected. */

            diminuto_delay(hertz / 1000, !0);

            EXPECT(diminuto_ipcl_close(producer) >= 0);

            /*
             * If you don't wait for the child to exit, it may not yet have
             * closed its end of the socket thereby releasing the bound IP
             * address by the time the next unit test begins.
             */

            EXPECT(waitpid(pid, &status, 0) == pid);
            CHECKPOINT("pid=%d status=%d\n", pid, status);
            EXPECT(WIFEXITED(status));
            EXPECT(WEXITSTATUS(status) == 0);

            EXPECT(diminuto_ipcl_close(service) >= 0);

        } else {

            int consumer;

            EXPECT(diminuto_ipcl_close(service) >= 0);

            diminuto_delay(hertz / 1000, !0);

            ASSERT((consumer = diminuto_ipcl_stream_consumer(LOCAL1)) >= 0);
            EXPECT(diminuto_ipc_type(consumer) == AF_LOCAL);

            diminuto_delay(hertz / 1000, !0);

            EXPECT(diminuto_ipcl_close(consumer) >= 0);

            EXIT();

        }

        ASSERT(diminuto_ipcl_remove(LOCAL1) >= 0);

        STATUS();
    }

    {
        int service;
        pid_t pid;

        TEST();

        /*
         * Streamlined.
         */

        ASSERT((service = diminuto_ipcl_stream_provider(LOCAL1)) >= 0);

        EXPECT((pid = fork()) >= 0);

        if (pid != 0) {

            int producer;
            int status;

            ASSERT((producer = diminuto_ipcl_stream_accept(service)) >= 0);

            diminuto_delay(hertz / 1000, !0);

            EXPECT(diminuto_ipcl_close(producer) >= 0);

            /*
             * If you don't wait for the child to exit, it may not yet have
             * closed its end of the socket thereby releasing the bound IP
             * address by the time the next unit test begins.
             */

            EXPECT(waitpid(pid, &status, 0) == pid);
            CHECKPOINT("pid=%d status=%d\n", pid, status);
            EXPECT(WIFEXITED(status));
            EXPECT(WEXITSTATUS(status) == 0);

            EXPECT(diminuto_ipcl_close(service) >= 0);

        } else {

            int consumer;

            EXPECT(diminuto_ipcl_close(service) >= 0);

            diminuto_delay(hertz / 1000, !0);

            ASSERT((consumer = diminuto_ipcl_stream_consumer(LOCAL1)) >= 0);
            EXPECT(diminuto_ipc_type(consumer) == AF_LOCAL);

            diminuto_delay(hertz / 1000, !0);

            EXPECT(diminuto_ipcl_close(consumer) >= 0);

            EXIT();

        }

        ASSERT(diminuto_ipcl_remove(LOCAL1) >= 0);

        STATUS();
    }

    {
        int service;
        pid_t pid;

        TEST();

        /*
         * Reversed,
         */

        EXPECT((service = diminuto_ipcl_stream_provider(LOCAL1)) >= 0);

        EXPECT((pid = fork()) >= 0);

        if (pid == 0) {

            int producer;

            EXPECT((producer = diminuto_ipcl_stream_accept(service)) >= 0);

            diminuto_delay(hertz / 1000, !0);

            EXPECT(diminuto_ipcl_close(producer) >= 0);
            EXPECT(diminuto_ipcl_close(service) >= 0);

            EXPECT(diminuto_ipcl_remove(LOCAL1) >= 0);

            EXIT();

        } else {

            int consumer;
            int status;

            EXPECT(diminuto_ipcl_close(service) >= 0);

            diminuto_delay(hertz / 1000, !0);

            EXPECT((consumer = diminuto_ipcl_stream_consumer(LOCAL1)) >= 0);
            EXPECT(diminuto_ipc_type(consumer) == AF_LOCAL);

            EXPECT(diminuto_ipcl_close(consumer) >= 0);

            /*
             * If you don't wait for the child to exit, it may not yet have
             * closed its end of the socket thereby releasing the bound IP
             * address by the time the next unit test begins.
             */

            EXPECT(waitpid(pid, &status, 0) == pid);
            CHECKPOINT("pid=%d status=%d\n", pid, status);
            EXPECT(WIFEXITED(status));
            EXPECT(WEXITSTATUS(status) == 0);
        }

        STATUS();
    }

    /*
     * There's a subtle design flaw in the next unit test which I first ran
     * first about fifteen years ago when doing socket programming under SunOS.
     * It is possible for the two processes below to deadlock, one waiting
     * for space to become available on the output side, one waiting for data
     * to become available on the input side. I'm not concerned about this
     * unit test, but it shows how difficult socket programming in the real
     * world can be. Real world applications should use select(2) or its
     * equivalent to determine if output buffer space is available in the
     * kernel. See diminuto_mux.c and its unit test unittest-mux.c which
     * uses select(2) to multiplex the diminuto_ipc feature in just this way.
     */

    {
        const char * pointer;
        diminuto_local_t provider;
        diminuto_local_t source;
        diminuto_local_t path;
        int service;
        pid_t pid;

        TEST();

        ASSERT((pointer = diminuto_ipcl_path(LOCAL1, provider, sizeof(provider))) != (const char *)0);
        ASSERT((service = diminuto_ipcl_stream_provider_generic(pointer, 16)) >= 0);
        EXPECT(diminuto_ipcl_nearend(service, source, sizeof(source)) >= 0);
        EXPECT(diminuto_ipcl_compare(source, provider) == 0);

        ASSERT((pid = fork()) >= 0);

        if (pid) {

            int producer;
            uint8_t output[256];
            uint8_t input[256];
            uint8_t datum;
            ssize_t sent;
            ssize_t received;
            ssize_t used;
            ssize_t available;
            ssize_t inputqueued;
            ssize_t outputqueued;
            uint8_t * here;
            uint8_t * there;
            uint8_t * current;
            size_t totalsent;
            size_t totalreceived;
            int status;

            ASSERT((producer = diminuto_ipcl_stream_accept_generic(service, path, sizeof(path))) >= 0);

            /*
             * We can remove the UNIX domain socket from the file
             * system as long as no other consumer wants to
             * connect with us. The accepted socket stays open
             * until we're done with it.
             */

            ASSERT(diminuto_ipcl_remove(LOCAL1) >= 0);
            ASSERT(diminuto_ipcl_remove(LOCAL1) < 0);

            here = output;
            used = sizeof(output);
            sent = 0;

            there = input;
            available = sizeof(input);
            received = 0;

            totalsent = 0;
            totalreceived = 0;

            datum = 0;
            do {
                output[datum] = datum;
            } while ((++datum) > 0);

            memset(input, 0, sizeof(input));

            do {

                if (totalsent < TOTAL) {

                    if (used > (TOTAL - totalsent)) {
                        used = TOTAL - totalsent;
                    }

                    ASSERT((sent = diminuto_ipcl_stream_write_generic(producer, here, 1, used)) > 0);
                    ASSERT(sent <= used);

                    EXPECT((outputqueued = diminuto_ipc_stream_get_pending(producer)) >= 0);

                    totalsent += sent;
                    COMMENT("producer sent %zd %zd %zu\n", sent, outputqueued, totalsent);

                    here += sent;
                    used -= sent;

                    if (used == 0) {
                        here = output;
                        used = sizeof(output);
                    }

                }

                current = there;

                if (available > (TOTAL - totalreceived)) {
                    available = TOTAL - totalreceived;
                }

                EXPECT((inputqueued = diminuto_ipc_stream_get_available(producer)) >= 0);

                ASSERT((received = diminuto_ipcl_stream_read(producer, there, available)) > 0);
                ASSERT(received <= available);

                totalreceived += received;
                COMMENT("producer received %zd %zd %zu\n", received, inputqueued, totalreceived);

                there += received;
                available -= received;

                while (current < there) {
                    ASSERT(*(current++) == (datum++));
                }

                if (available == 0) {
                    there = input;
                    available = sizeof(input);
                    memset(input, 0, sizeof(input));
                }

            } while (totalreceived < TOTAL);

            COMMENT("totalsent=%zu totalreceived=%zu\n", totalsent, totalreceived);
            EXPECT(totalsent == totalreceived);

            EXPECT(diminuto_ipcl_shutdown(producer) >= 0);
            EXPECT(diminuto_ipcl_shutdown(service) >= 0);

            ASSERT(diminuto_ipcl_close(producer) >= 0);
            ASSERT(diminuto_ipcl_close(service) >= 0);

            EXPECT(waitpid(pid, &status, 0) == pid);
            CHECKPOINT("pid=%d status=%d\n", pid, status);
            EXPECT(WIFEXITED(status));
            EXPECT(WEXITSTATUS(status) == 0);

        } else {

            int consumer;
            uint8_t buffer[64];
            ssize_t sent;
            ssize_t received;
            size_t totalsent;
            size_t totalreceived;
            ssize_t inputqueued;
            ssize_t outputqueued;

            ASSERT(diminuto_ipcl_close(service) >= 0);

            diminuto_delay(hertz / 1000, !0);

            ASSERT((consumer = diminuto_ipcl_stream_consumer(source)) >= 0);
            EXPECT(diminuto_ipc_type(consumer) == AF_LOCAL);

            totalreceived = 0;
            totalsent = 0;

            while (!0) {

                EXPECT((inputqueued = diminuto_ipc_stream_get_available(consumer)) >= 0);

                ASSERT((received = diminuto_ipcl_stream_read(consumer, buffer, sizeof(buffer))) >= 0);
                ASSERT(received <= sizeof(buffer));

                totalreceived += received;
                COMMENT("consumer received %zd %zd %zu\n", received, inputqueued, totalreceived);

                if (received == 0) {
                    break;
                }

                sent = 0;
                while (sent < received) {
                    ASSERT((sent = diminuto_ipcl_stream_write_generic(consumer,  buffer + sent, 1, received - sent)) > 0);
                    ASSERT(sent <= received);

                    EXPECT((outputqueued = diminuto_ipc_stream_get_pending(consumer)) >= 0);

                    totalsent += sent;
                    COMMENT("consumer sent %zd %zd %zu\n", sent, outputqueued, totalsent);

                    received -= sent;
                }
            }

            EXPECT(inputqueued == 0);

            EXPECT(diminuto_ipcl_shutdown(consumer) >= 0);

            ASSERT(diminuto_ipcl_close(consumer) >= 0);

            EXIT();
        }

        STATUS();
    }

    {
        int fd;
        diminuto_local_t local;

        TEST();

        COMMENT("endpoint=\"%s\"\n", LOCAL1);
        ASSERT((fd = diminuto_ipcl_packet_provider(LOCAL1)) >= 0);
        EXPECT(diminuto_ipc_type(fd) == AF_LOCAL);
        EXPECT(diminuto_ipcl_nearend(fd, local, sizeof(local)) >= 0);
        COMMENT("packet provider nearend=\"%s\"\n", local);
        EXPECT(diminuto_ipcl_farend(fd, local, sizeof(local)) < 0);
        EXPECT(diminuto_ipcl_close(fd) >= 0);
        ASSERT((fd = diminuto_ipcl_packet_provider(LOCAL1)) < 0);
        EXPECT(diminuto_ipc_type(fd) == AF_UNSPEC);
        ASSERT(diminuto_ipcl_remove(LOCAL1) >= 0);
        ASSERT((fd = diminuto_ipcl_packet_provider(LOCAL1)) >= 0);
        EXPECT(diminuto_ipc_type(fd) == AF_LOCAL);
        EXPECT(diminuto_ipcl_close(fd) >= 0);
        ASSERT(diminuto_ipcl_remove(LOCAL1) >= 0);

        STATUS();
    }

    {
        diminuto_local_t rendezvous;
        diminuto_local_t local;
        int service;
        pid_t pid;

        TEST();

        /*
         * Named consumer.
         */

        ASSERT((service = diminuto_ipcl_packet_provider(LOCAL1)) >= 0);
        EXPECT(diminuto_ipcl_nearend(service, rendezvous, sizeof(rendezvous)) >= 0);
        COMMENT("rendezvous=\"%s\"\n", rendezvous);
        EXPECT(diminuto_ipcl_compare(rendezvous, LOCAL1) == 0);

        EXPECT((pid = fork()) >= 0);

        if (pid != 0) {

            int producer;
            int status;

            local[0] = '\0';
            ASSERT((producer = diminuto_ipcl_packet_accept_generic(service, local, sizeof(local))) >= 0);
            COMMENT("local=\"%s\"\n", local);
            EXPECT(diminuto_ipcl_compare(local, LOCAL2) == 0);

            local[0] = '\0';
            EXPECT(diminuto_ipcl_nearend(producer, local, sizeof(local)) >= 0);
            COMMENT("local=\"%s\"\n", local);
            EXPECT(diminuto_ipcl_compare(local, LOCAL1) == 0);

            local[0] = '\0';
            EXPECT(diminuto_ipcl_farend(producer, local, sizeof(local)) >= 0);
            COMMENT("local=\"%s\"\n", local);
            EXPECT(diminuto_ipcl_compare(local, LOCAL2) == 0);

            diminuto_delay(hertz / 1000, !0);

            EXPECT(diminuto_ipcl_close(producer) >= 0);

            /*
             * If you don't wait for the child to exit, it may not yet have
             * closed its end of the socket thereby releasing the bound IP
             * address by the time the next unit test begins.
             */

            EXPECT(waitpid(pid, &status, 0) == pid);
            CHECKPOINT("pid=%d status=%d\n", pid, status);
            EXPECT(WIFEXITED(status));
            EXPECT(WEXITSTATUS(status) == 0);

            EXPECT(diminuto_ipcl_close(service) >= 0);

        } else {

            int consumer;

            EXPECT(diminuto_ipcl_close(service) >= 0);

            diminuto_delay(hertz / 1000, !0);

            ASSERT((consumer = diminuto_ipcl_packet_consumer_generic(LOCAL1, LOCAL2)) >= 0);

            diminuto_delay(hertz / 1000, !0);

            EXPECT(diminuto_ipcl_close(consumer) >= 0);

            EXIT();

        }

        ASSERT(diminuto_ipcl_remove(LOCAL1) >= 0);
        ASSERT(diminuto_ipcl_remove(LOCAL2) >= 0);

        STATUS();
    }

    {
        const char * pointer;
        diminuto_local_t provider;
        diminuto_local_t source;
        diminuto_local_t path;
        int service;
        pid_t pid;

        TEST();

        ASSERT((pointer = diminuto_ipcl_path(LOCAL1, provider, sizeof(provider))) != (const char *)0);
        ASSERT((service = diminuto_ipcl_packet_provider_generic(pointer, 16)) >= 0);
        EXPECT(diminuto_ipcl_nearend(service, source, sizeof(source)) >= 0);
        EXPECT(diminuto_ipcl_compare(source, provider) == 0);

        ASSERT((pid = fork()) >= 0);

        if (pid) {

            int producer;
            uint8_t output[256];
            uint8_t input[countof(output)];
            uint8_t datum;
            ssize_t sent;
            ssize_t received;
            size_t totalsent;
            size_t totalreceived;
            int status;
            struct msghdr message;
            struct iovec vector[1];

            ASSERT((producer = diminuto_ipcl_packet_accept_generic(service, path, sizeof(path))) >= 0);

            /*
             * We can remove the UNIX domain socket from the file
             * system as long as no other consumer wants to
             * connect with us. The accepted socket stays open
             * until we're done with it.
             */

            ASSERT(diminuto_ipcl_remove(LOCAL1) >= 0);
            ASSERT(diminuto_ipcl_remove(LOCAL1) < 0);

            sent = 0;
            received = 0;

            totalsent = 0;
            totalreceived = 0;

            datum = 0;
            do {
                output[datum] = datum;
            } while ((++datum) > 0);

            memset(input, 0, sizeof(input));

            do {

                if (totalsent < TOTAL) {

                    vector[0].iov_base = output;
                    vector[0].iov_len = sizeof(output);

                    message.msg_name = (void *)0;
                    message.msg_namelen = 0;
                    message.msg_iov = vector;
                    message.msg_iovlen = countof(vector);
                    message.msg_control = (void *)0;
                    message.msg_controllen = 0;
                    message.msg_flags = 0;

                    ASSERT((sent = diminuto_ipcl_packet_send(producer, &message)) > 0);

                    totalsent += message.msg_iov[0].iov_len;
                    COMMENT("producer sent %zd %zd %zu\n", sent, message.msg_iov[0].iov_len, totalsent);

                }

                vector[0].iov_base = input;
                vector[0].iov_len = sizeof(input);

                message.msg_name = (void *)0;
                message.msg_namelen = 0;
                message.msg_iov = vector;
                message.msg_iovlen = countof(vector);
                message.msg_control = (void *)0;
                message.msg_controllen = 0;
                message.msg_flags = 0;

                ASSERT((received = diminuto_ipcl_packet_receive(producer, &message)) > 0);

                totalreceived += message.msg_iov[0].iov_len;
                COMMENT("producer received %zd %zd %zu\n", received, message.msg_iov[0].iov_len, totalreceived);

                ASSERT(memcmp(input, output, sizeof(input)) == 0);
                memset(input, 0, sizeof(input));

            } while (totalreceived < TOTAL);

            COMMENT("totalsent=%zu totalreceived=%zu\n", totalsent, totalreceived);
            EXPECT(totalsent == totalreceived);

            EXPECT(diminuto_ipcl_shutdown(producer) >= 0);
            EXPECT(diminuto_ipcl_shutdown(service) >= 0);

            ASSERT(diminuto_ipcl_close(producer) >= 0);
            ASSERT(diminuto_ipcl_close(service) >= 0);

            EXPECT(waitpid(pid, &status, 0) == pid);
            CHECKPOINT("pid=%d status=%d\n", pid, status);
            EXPECT(WIFEXITED(status));
            EXPECT(WEXITSTATUS(status) == 0);

        } else {

            int consumer;
            uint8_t buffer[256];
            ssize_t sent;
            ssize_t received;
            size_t totalsent;
            size_t totalreceived;
            struct msghdr message;
            struct iovec vector[1];

            ASSERT(diminuto_ipcl_close(service) >= 0);

            diminuto_delay(hertz / 1000, !0);

            ASSERT((consumer = diminuto_ipcl_packet_consumer(source)) >= 0);
            EXPECT(diminuto_ipc_type(consumer) == AF_LOCAL);

            sent = 0;
            received = 0;

            totalreceived = 0;
            totalsent = 0;

            while (!0) {

                vector[0].iov_base = buffer;
                vector[0].iov_len = sizeof(buffer);

                message.msg_name = (void *)0;
                message.msg_namelen = 0;
                message.msg_iov = vector;
                message.msg_iovlen = countof(vector);
                message.msg_control = (void *)0;
                message.msg_controllen = 0;
                message.msg_flags = 0;

                ASSERT((received = diminuto_ipcl_packet_receive(consumer, &message)) >= 0);

                if (received > 0) { totalreceived += message.msg_iov[0].iov_len; }
                COMMENT("consumer received %zd %zd %zu\n", received, message.msg_iov[0].iov_len, totalreceived);

                if (received == 0) {
                    break;
                }

                vector[0].iov_base = buffer;
                vector[0].iov_len = sizeof(buffer);

                message.msg_name = (void *)0;
                message.msg_namelen = 0;
                message.msg_iov = vector;
                message.msg_iovlen = countof(vector);
                message.msg_control = (void *)0;
                message.msg_controllen = 0;
                message.msg_flags = 0;

                ASSERT((sent = diminuto_ipcl_packet_send(consumer, &message)) > 0);

                totalsent += message.msg_iov[0].iov_len;
                COMMENT("consumer sent %zd %zd %zu\n", sent, message.msg_iov[0].iov_len, totalsent);

            }

            EXPECT(diminuto_ipcl_shutdown(consumer) >= 0);

            ASSERT(diminuto_ipcl_close(consumer) >= 0);

            EXIT();
        }

        STATUS();
    }

    EXIT();
}
