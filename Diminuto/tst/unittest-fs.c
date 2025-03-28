/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2015-2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the File System (FS) feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the File System (FS) feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_types.h"
#include "com/diag/diminuto/diminuto_log.h"
#include "com/diag/diminuto/diminuto_fs.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <pthread.h>

static int callback(void * vp, const char * name, const char * path, size_t depth, const struct stat * statp)
{
    if (vp == (void *)0) {
        return -123;
    } else if (*(const char *)vp == '\0') {
        return -456;
    } else if (strcmp(path, (const char *)vp) == 0) {
        return 123;
    } else {
        return 0;
    }
}

int main(int argc, char * argv[])
{
    int debug;

    SETLOGMASK();

    if (argc <= 1) {
        /* Do nothing. */
    } else if  (strcmp(argv[1], "-d") != 0) {
        /* Do nothing. */
    } else { 
        debug = diminuto_fs_debug(!0);
    }

    {
        TEST();

        EXPECT(sizeof(diminuto_path_t) >= PATH_MAX);
        EXPECT(sizeof(diminuto_local_t) >= NAME_MAX);

        STATUS();
    }

    {
        mode_t mode;
        diminuto_fs_type_t type = DIMINUTO_FS_TYPE_NONE;
        int ii;

        TEST();

        for (ii = 0x0; ii <= 0xf; ++ii) {
            mode = ii << 12;
            type = diminuto_fs_mode2type(mode);
            CHECKPOINT("mode=0%06o type='%c'\n", ii << 12, type);
            EXPECT(type != DIMINUTO_FS_TYPE_NONE);
            EXPECT(type != DIMINUTO_FS_TYPE_UNKNOWN);
        }

        STATUS();
    }

    {
        int rc;

        TEST();

        rc = diminuto_fs_walk(".", callback, (void *)0);
        EXPECT(rc == -123);

        STATUS();
    }

    {
        int rc;

        TEST();

        rc = diminuto_fs_walk(".", callback, "");
        EXPECT(rc == -456);

        STATUS();
    }

    {
        int rc;

        TEST();

        rc = diminuto_fs_walk("/usr/local/bin", callback, "/usr/local/bin");
        EXPECT(rc == 123);

        STATUS();
    }

    {
        int rc;

        TEST();

        rc = diminuto_fs_walk("/bin", callback, "/bin/true");
        /*
         * Ubuntu MATE 19.10 soft links /bin to /usr/bin.
         */
        if (rc == 0) {
            rc = diminuto_fs_walk("/usr/bin", callback, "/usr/bin/true");
        }
        EXPECT(rc == 123);

        STATUS();
    }

    {
        int rc;

        TEST();

        rc = diminuto_fs_walk("/usr/local", callback, "/usr/local/bin");
        EXPECT(rc == 123);

        STATUS();
    }

    {
        int rc;

        TEST();

        rc = diminuto_fs_walk("/usr/local", callback, "/usr/local/include/com-diag-diminuto-fs-non-existent");
        EXPECT(rc == 0);

        STATUS();
    }

    {
        int rc;

        TEST();

        rc = diminuto_fs_mkdir_p("", 0755, 0);
        ASSERT(rc == 0);

        STATUS();
    }

    {
        int rc;
        int debug;

        TEST();

        debug = diminuto_fs_debug(!0);
        rc = diminuto_fs_mkdir_p("", 0755, !0);
        ASSERT(rc == 0);
        diminuto_fs_debug(debug);

        STATUS();
    }

    {
        int rc;
        int debug;

        TEST();

        debug = diminuto_fs_debug(!0);
        rc = diminuto_fs_mkdir_p("/", 0755, 0);
        ASSERT(rc == 0);
        diminuto_fs_debug(debug);

        STATUS();
    }

    {
        int rc;
        int debug;

        TEST();

        debug = diminuto_fs_debug(!0);
        rc = diminuto_fs_mkdir_p("/", 0755, !0);
        ASSERT(rc == 0);
        diminuto_fs_debug(debug);

        STATUS();
    }

    {
        int rc;
        int debug;
        diminuto_fs_type_t type;

        TEST();

        debug = diminuto_fs_debug(!0);
        rc = diminuto_fs_mkdir_p("unittest-fs", 0755, 0);
        ASSERT(rc == 0);
        type = diminuto_fs_type("unittest-fs");
        ASSERT(type == DIMINUTO_FS_TYPE_NONE);
        diminuto_fs_debug(debug);

        STATUS();
    }

    {
        int rc;
        int debug;
        diminuto_fs_type_t type;

        TEST();

        debug = diminuto_fs_debug(!0);
        rc = diminuto_fs_mkdir_p("unittest-fs", 0755, !0);
        ASSERT(rc == 0);
        type = diminuto_fs_type("unittest-fs");
        ASSERT(type == DIMINUTO_FS_TYPE_DIRECTORY);
        rc = rmdir("unittest-fs");
        ASSERT(rc == 0);
        diminuto_fs_debug(debug);

        STATUS();
    }

    {
        int rc;
        int debug;
        diminuto_fs_type_t type;

        TEST();

        debug = diminuto_fs_debug(!0);
        rc = diminuto_fs_mkdir_p("./unittest-fs", 0755, 0);
        ASSERT(rc == 0);
        type = diminuto_fs_type("./unittest-fs");
        ASSERT(type == DIMINUTO_FS_TYPE_NONE);
        diminuto_fs_debug(debug);

        STATUS();
    }

    {
        int rc;
        int debug;
        diminuto_fs_type_t type;

        TEST();

        debug = diminuto_fs_debug(!0);
        rc = diminuto_fs_mkdir_p("./unittest-fs", 0755, !0);
        ASSERT(rc == 0);
        type = diminuto_fs_type("./unittest-fs");
        ASSERT(type == DIMINUTO_FS_TYPE_DIRECTORY);
        rc = rmdir("./unittest-fs");
        ASSERT(rc == 0);
        diminuto_fs_debug(debug);

        STATUS();
    }

    {
        int rc;
        int debug;
        diminuto_fs_type_t type;

        TEST();

        debug = diminuto_fs_debug(!0);
        rc = diminuto_fs_mkdir_p("/tmp/unittest-fs", 0755, 0);
        ASSERT(rc == 0);
        type = diminuto_fs_type("/tmp/unittest-fs");
        ASSERT(type == DIMINUTO_FS_TYPE_NONE);
        diminuto_fs_debug(debug);

        STATUS();
    }

    {
        int rc;
        int debug;
        diminuto_fs_type_t type;

        TEST();

        debug = diminuto_fs_debug(!0);
        rc = diminuto_fs_mkdir_p("/tmp/unittest-fs", 0755, !0);
        ASSERT(rc == 0);
        type = diminuto_fs_type("/tmp/unittest-fs");
        ASSERT(type == DIMINUTO_FS_TYPE_DIRECTORY);
        rc = rmdir("/tmp/unittest-fs");
        ASSERT(rc == 0);
        diminuto_fs_debug(debug);

        STATUS();
    }

    {
        int rc;
        int debug;
        diminuto_fs_type_t type;

        TEST();

        debug = diminuto_fs_debug(!0);
        rc = diminuto_fs_mkdir_p("unittest-fs/unittest-fs/unittest-fs", 0755, 0);
        ASSERT(rc == 0);
        type = diminuto_fs_type("unittest-fs");
        ASSERT(type == DIMINUTO_FS_TYPE_DIRECTORY);
        type = diminuto_fs_type("unittest-fs/unittest-fs");
        ASSERT(type == DIMINUTO_FS_TYPE_DIRECTORY);
        type = diminuto_fs_type("unittest-fs/unittest-fs/unittest-fs");
        ASSERT(type == DIMINUTO_FS_TYPE_NONE);
        rc = rmdir("unittest-fs/unittest-fs");
        ASSERT(rc == 0);
        rc = rmdir("unittest-fs");
        ASSERT(rc == 0);
        diminuto_fs_debug(debug);

        STATUS();
    }

    {
        int rc;
        int debug;
        diminuto_fs_type_t type;

        TEST();

        debug = diminuto_fs_debug(!0);
        rc = diminuto_fs_mkdir_p("unittest-fs/unittest-fs/unittest-fs", 0755, !0);
        ASSERT(rc == 0);
        type = diminuto_fs_type("unittest-fs/unittest-fs/unittest-fs");
        ASSERT(type == DIMINUTO_FS_TYPE_DIRECTORY);
        type = diminuto_fs_type("unittest-fs/unittest-fs");
        ASSERT(type == DIMINUTO_FS_TYPE_DIRECTORY);
        type = diminuto_fs_type("unittest-fs");
        ASSERT(type == DIMINUTO_FS_TYPE_DIRECTORY);
        rc = rmdir("unittest-fs/unittest-fs/unittest-fs");
        ASSERT(rc == 0);
        rc = rmdir("unittest-fs/unittest-fs");
        ASSERT(rc == 0);
        rc = rmdir("unittest-fs");
        ASSERT(rc == 0);
        diminuto_fs_debug(debug);

        STATUS();
    }

    {
        int rc;
        int debug;
        diminuto_fs_type_t type;

        TEST();

        debug = diminuto_fs_debug(!0);
        rc = diminuto_fs_mkdir_p("unittest-fs/unittest-fs/unittest-fs/", 0755, 0);
        ASSERT(rc == 0);
        type = diminuto_fs_type("unittest-fs/unittest-fs/unittest-fs");
        ASSERT(type == DIMINUTO_FS_TYPE_DIRECTORY);
        type = diminuto_fs_type("unittest-fs/unittest-fs");
        ASSERT(type == DIMINUTO_FS_TYPE_DIRECTORY);
        type = diminuto_fs_type("unittest-fs");
        ASSERT(type == DIMINUTO_FS_TYPE_DIRECTORY);
        rc = rmdir("unittest-fs/unittest-fs/unittest-fs");
        ASSERT(rc == 0);
        rc = rmdir("unittest-fs/unittest-fs");
        ASSERT(rc == 0);
        rc = rmdir("unittest-fs");
        ASSERT(rc == 0);
        diminuto_fs_debug(debug);

        STATUS();
    }

    {
        int rc;
        int debug;
        diminuto_fs_type_t type;

        TEST();

        debug = diminuto_fs_debug(!0);
        rc = diminuto_fs_mkdir_p("unittest-fs/unittest-fs/unittest-fs/", 0755, !0);
        ASSERT(rc == 0);
        type = diminuto_fs_type("unittest-fs/unittest-fs/unittest-fs");
        ASSERT(type == DIMINUTO_FS_TYPE_DIRECTORY);
        type = diminuto_fs_type("unittest-fs/unittest-fs");
        ASSERT(type == DIMINUTO_FS_TYPE_DIRECTORY);
        type = diminuto_fs_type("unittest-fs");
        ASSERT(type == DIMINUTO_FS_TYPE_DIRECTORY);
        rc = rmdir("unittest-fs/unittest-fs/unittest-fs");
        ASSERT(rc == 0);
        rc = rmdir("unittest-fs/unittest-fs");
        ASSERT(rc == 0);
        rc = rmdir("unittest-fs");
        ASSERT(rc == 0);
        diminuto_fs_debug(debug);

        STATUS();
    }

    /*
     * In the tests below I deliberately use _POSIX_PATH_MAX instead of
     * PATH_MAX. See also the unit test suite for the IPC Endpoint function,xi
     * which uses the` FS Canonicalize function and has its own long list of
     * unit tests.
     */

    {
        int rc;
        char * path;
        char buffer[_POSIX_PATH_MAX];

        TEST();

        path = "";
        buffer[0] = '!';
        rc = diminuto_fs_canonicalize(path, buffer, 0);
        ASSERT(rc < 0);
        ASSERT(buffer[0] == '!');

        STATUS();
    }

    {
        int rc;
        char * path;
        char buffer[_POSIX_PATH_MAX];

        TEST();

        path = "/tmp/file";
        buffer[0] = '!';
        CHECKPOINT("relative=\"%s\"", path);
        rc = diminuto_fs_canonicalize(path, buffer, 0);
        ASSERT(rc < 0);
        ASSERT(buffer[0] == '!');

        STATUS();
    }

    {
        int rc;
        char * path;
        char buffer[_POSIX_PATH_MAX];

        TEST();

        path = "/tmp/file";
        buffer[0] = '!';
        CHECKPOINT("relative=\"%s\"", path);
        rc = diminuto_fs_canonicalize(path, buffer, 2);
        ASSERT(rc < 0);
        ASSERT(buffer[0] == '!');

        STATUS();
    }

    {
        int rc;
        char * path;
        char buffer[_POSIX_PATH_MAX];

        TEST();

        path = "/tmp/file";
        buffer[0] = '!';
        CHECKPOINT("relative=\"%s\"", path);
        rc = diminuto_fs_canonicalize(path, buffer, sizeof(buffer));
        ASSERT(rc == 0);
        CHECKPOINT("absolute=\"%s\"", buffer);
        ASSERT(strcmp(buffer, path) == 0);

        STATUS();
    }

    {
        int rc;
        char * path;
        char buffer[_POSIX_PATH_MAX];

        TEST();

        path = "/file";
        buffer[0] = '!';
        CHECKPOINT("relative=\"%s\"", path);
        rc = diminuto_fs_canonicalize(path, buffer, sizeof(buffer));
        ASSERT(rc == 0);
        CHECKPOINT("absolute=\"%s\"", buffer);
        ASSERT(strcmp(buffer, path) == 0);

        STATUS();
    }

    {
        int rc;
        char * path;
        char buffer[_POSIX_PATH_MAX];

        TEST();

        path = "/tmp/";
        buffer[0] = '!';
        CHECKPOINT("relative=\"%s\"", path);
        rc = diminuto_fs_canonicalize(path, buffer, sizeof(buffer));
        ASSERT(rc == 0);
        CHECKPOINT("absolute=\"%s\"", buffer);
        ASSERT(strcmp(buffer, path) == 0);

        STATUS();
    }

    {
        int rc;
        char * path;
        char buffer[_POSIX_PATH_MAX];

        TEST();

        path = ".//file";
        buffer[0] = '!';
        CHECKPOINT("relative=\"%s\"", path);
        rc = diminuto_fs_canonicalize(path, buffer, sizeof(buffer));
        ASSERT(rc == 0);
        CHECKPOINT("absolute=\"%s\"", buffer);
        ASSERT(buffer[0] == '/');

        STATUS();
    }

    {
        int rc;
        char * path;
        char buffer[_POSIX_PATH_MAX];

        TEST();

        path = "..//file";
        buffer[0] = '!';
        CHECKPOINT("relative=\"%s\"", path);
        rc = diminuto_fs_canonicalize(path, buffer, sizeof(buffer));
        ASSERT(rc == 0);
        CHECKPOINT("absolute=\"%s\"", buffer);
        ASSERT(buffer[0] == '/');

        STATUS();
    }

    {
        int rc;
        char * path;
        char buffer[_POSIX_PATH_MAX];

        TEST();

        path = "/var/tmp/../run/./file";
        buffer[0] = '!';
        CHECKPOINT("relative=\"%s\"", path);
        rc = diminuto_fs_canonicalize(path, buffer, sizeof(buffer));
        ASSERT(rc == 0);
        CHECKPOINT("absolute=\"%s\"", buffer);
        ASSERT(buffer[0] == '/');

        STATUS();
    }

    {
        int rc;
        char * path;
        char buffer[_POSIX_PATH_MAX];

        TEST();

        path = "..///./file";
        buffer[0] = '!';
        CHECKPOINT("relative=\"%s\"", path);
        rc = diminuto_fs_canonicalize(path, buffer, sizeof(buffer));
        ASSERT(rc == 0);
        CHECKPOINT("absolute=\"%s\"", buffer);
        ASSERT(buffer[0] == '/');

        STATUS();
    }

    {
        int rc;
        char * path;
        char buffer[_POSIX_PATH_MAX];

        TEST();

        path = "/";
        buffer[0] = '!';
        CHECKPOINT("relative=\"%s\"", path);
        rc = diminuto_fs_canonicalize(path, buffer, sizeof(buffer));
        ASSERT(rc == 0);
        CHECKPOINT("absolute=\"%s\"", buffer);
        ASSERT(strcmp(buffer, path) == 0);

        STATUS();
    }

    {
        int rc;
        char * path;
        char buffer[_POSIX_PATH_MAX];

        TEST();

        path = "/.";
        buffer[0] = '!';
        CHECKPOINT("relative=\"%s\"", path);
        rc = diminuto_fs_canonicalize(path, buffer, sizeof(buffer));
        ASSERT(rc == 0);
        CHECKPOINT("absolute=\"%s\"", buffer);
        ASSERT(strcmp(buffer, "/") == 0);

        STATUS();
    }

    {
        int rc;
        char * path;
        char buffer[_POSIX_PATH_MAX];

        TEST();

        path = "/..";
        buffer[0] = '!';
        CHECKPOINT("relative=\"%s\"", path);
        rc = diminuto_fs_canonicalize(path, buffer, sizeof(buffer));
        ASSERT(rc == 0);
        CHECKPOINT("absolute=\"%s\"", buffer);
        ASSERT(strcmp(buffer, "/") == 0);

        STATUS();
    }

    {
        int rc;
        char * path;
        char buffer[_POSIX_PATH_MAX];

        TEST();

        path = "/../file";
        buffer[0] = '!';
        CHECKPOINT("relative=\"%s\"", path);
        rc = diminuto_fs_canonicalize(path, buffer, sizeof(buffer));
        ASSERT(rc == 0);
        CHECKPOINT("absolute=\"%s\"", buffer);
        ASSERT(strcmp(buffer, "/file") == 0);

        STATUS();
    }

    {
        int rc;
        char * path;
        char buffer[_POSIX_PATH_MAX];

        TEST();

        path = "/./.file";
        buffer[0] = '!';
        CHECKPOINT("relative=\"%s\"", path);
        rc = diminuto_fs_canonicalize(path, buffer, sizeof(buffer));
        ASSERT(rc == 0);
        CHECKPOINT("absolute=\"%s\"", buffer);
        ASSERT(strcmp(buffer, "/.file") == 0);

        STATUS();
    }

    {
        int rc;
        char * path;
        char buffer[_POSIX_PATH_MAX];
        pid_t pid;
        char string[_POSIX_PATH_MAX];

        TEST();

        path = "/proc/self";
        buffer[0] = '!';
        CHECKPOINT("relative=\"%s\"", path);
        rc = diminuto_fs_expand(path, buffer, sizeof(buffer));
        ASSERT(rc == 0);
        CHECKPOINT("absolute=\"%s\"", buffer);
        pid = getpid();
        (void)snprintf(string, sizeof(string), "%llu", (diminuto_llu_t)pid);
        ASSERT(strncmp(string, buffer, sizeof(string)));

        STATUS();
    }

    {
        int rc;
        char * path;
        char buffer[_POSIX_PATH_MAX];
        pid_t pid;
        pthread_t thread;
        char string[_POSIX_PATH_MAX];

        TEST();

        path = "/proc/thread-self";
        buffer[0] = '!';
        CHECKPOINT("relative=\"%s\"", path);
        rc = diminuto_fs_expand(path, buffer, sizeof(buffer));
        ASSERT(rc == 0);
        CHECKPOINT("absolute=\"%s\"", buffer);
        pid = getpid();
        thread = pthread_self();
        (void)snprintf(string, sizeof(string), "%llu/task/%llu", (diminuto_llu_t)pid, (diminuto_llu_t)thread);
        ASSERT(strncmp(string, buffer, sizeof(string)));

        STATUS();
    }

    {
        int rc;
        char * path;
        char buffer[_POSIX_PATH_MAX];
        char expected[_POSIX_PATH_MAX];
        pid_t pid;

        TEST();

        path = "/dev/fd";
        buffer[0] = '!';
        CHECKPOINT("relative=\"%s\"", path);
        rc = diminuto_fs_expand(path, buffer, sizeof(buffer));
        ASSERT(rc == 0);
        CHECKPOINT("absolute=\"%s\"", buffer);
        pid = getpid();
        ASSERT(pid > 0);
        (void)snprintf(expected, sizeof(expected), "/proc/%d/fd", pid);
        expected[sizeof(expected) - 1] = '\0';
        CHECKPOINT("expected=\"%s\"", expected);
        ASSERT(strcmp(buffer, expected) == 0);

        STATUS();
    }

    diminuto_fs_debug(debug);

    EXIT();
}
