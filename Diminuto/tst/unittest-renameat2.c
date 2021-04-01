/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2018-2020 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the renameat2(2) feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the renameat2(2) feature.
 *
 * This is indeed a unit test of renameat2() but it is mostly done in support
 * of a blog article; the function/system call is tested in the unittest-lock.c
 * and unittest-hangup-*.c unit tests.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include "../src/diminuto_renameat2.h"

static int my_lock(const char * file) {
    static const char SUFFIX[] = "-lock-XXXXXX";
    pid_t pid = getpid();
    char * path = (char *) malloc(strlen(file) + sizeof(SUFFIX));
    strcpy(path, file);
    strcat(path, SUFFIX);
    int fd = mkstemp(path);
    FILE * fp = fdopen(fd, "w");
    fprintf(fp, "%d\n", pid);
    fclose(fp);
    int rc = renameat2(AT_FDCWD, path, AT_FDCWD, file, RENAME_NOREPLACE);
    if (rc < 0) {
        unlink(path);
    }
    free(path);
    return rc;
}

static int my_prelock(const char * file) {
    int fd = open(file, O_CREAT | O_EXCL | O_WRONLY, 0600);
    if (fd < 0) {
        return -1;
    }
    close(fd);
    return 0;
}

static int my_postlock(const char * file) {
    static const char SUFFIX[] = "-post-XXXXXX";
    pid_t pid = getpid();
    char * path = (char *) malloc(strlen(file) + sizeof(SUFFIX));
    strcpy(path, file);
    strcat(path, SUFFIX);
    int fd = mkstemp(path);
    FILE * fp = fdopen(fd, "w");
    fprintf(fp, "%d\n", pid);
    fclose(fp);
    int rc = renameat2(AT_FDCWD, path, AT_FDCWD, file, RENAME_EXCHANGE);
    unlink(path);
    free(path);
    return rc;
}

static int my_unlock(const char * file) {
    return unlink(file);
}

static pid_t my_locked(const char * file) {
    FILE * fp = fopen(file, "r");
    if (fp == NULL) {
        return -1;
    }
    pid_t pid = 0;
    fscanf(fp, "%d\n", &pid);
    fclose(fp);
    return pid;
}

static void test_lock() {
    static const char * LOCKFILE = "/tmp/unittest-renameat2-lock.pid";
    pid_t pid1 = getpid();
    ASSERT(pid1 >= 0);
    pid_t pid2 = my_locked(LOCKFILE);
    ASSERT(pid2 < 0);
    int rc = my_lock(LOCKFILE);
    ASSERT(rc == 0);
    pid_t pid3 = my_locked(LOCKFILE);
    ASSERT(pid3 > 0);
    ASSERT(pid1 == pid3);
    /* Try locking again: should fail. */
    rc = my_lock(LOCKFILE);
    ASSERT(rc < 0);
    rc = my_unlock(LOCKFILE);
    ASSERT(rc == 0);
    pid_t pid4 = my_locked(LOCKFILE);
    ASSERT(pid4 < 0);
    rc = my_unlock(LOCKFILE);
    ASSERT(rc < 0);
    pid_t pid5 = my_locked(LOCKFILE);
    ASSERT(pid5 < 0);
}

static void test_prepostlock() {
    static const char * LOCKFILE = "/tmp/unittest-renameat2-prepostlock.pid";
    pid_t pid1 = getpid();
    ASSERT(pid1 >= 0);
    pid_t pid2 = my_locked(LOCKFILE);
    ASSERT(pid2 < 0);
    int rc = my_postlock(LOCKFILE);
    ASSERT(rc < 0);
    rc = my_prelock(LOCKFILE);
    ASSERT(rc == 0);
    pid_t pid3 = my_locked(LOCKFILE);
    ASSERT(pid3 == 0);
    /* Try prelocking again: should fail. */
    rc = my_prelock(LOCKFILE);
    ASSERT(rc < 0);
    pid_t pid4 = my_locked(LOCKFILE);
    ASSERT(pid4 == 0);
    rc = my_postlock(LOCKFILE);
    ASSERT(rc == 0);
    pid_t pid5 = my_locked(LOCKFILE);
    ASSERT(pid5 > 0);
    ASSERT(pid1 == pid5);
    /* Try postlocking again: should succeed. */
    rc = my_postlock(LOCKFILE);
    ASSERT(rc == 0);
    pid_t pid6 = my_locked(LOCKFILE);
    ASSERT(pid6 > 0);
    ASSERT(pid1 == pid6);
    rc = my_unlock(LOCKFILE);
    ASSERT(rc == 0);
    pid_t pid7 = my_locked(LOCKFILE);
    ASSERT(pid7 < 0);
    rc = my_unlock(LOCKFILE);
    ASSERT(rc < 0);
    pid_t pid8 = my_locked(LOCKFILE);
    ASSERT(pid8 < 0);
}

int main(void) {
    SETLOGMASK();
    { TEST(); test_lock(); STATUS(); }
    { TEST(); test_prepostlock(); STATUS(); }
    EXIT();
}
