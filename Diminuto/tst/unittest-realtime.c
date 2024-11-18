/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2024 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is the unit test for the Real Time feature supporting PREEMPT_RT.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is the unit test for the Real Time feature supporting PREEMPT_RT.
 */

#include <sched.h>
#include <unistd.h>
#include "com/diag/diminuto/diminuto_countof.h"
#include "com/diag/diminuto/diminuto_realtime.h"
#include "com/diag/diminuto/diminuto_unittest.h"
#include "diminuto_realtime.h"

int main(int argc, char * argv[])
{
    uid_t uid;
    uid_t euid;
    gid_t gid;
    gid_t egid;

    SETLOGMASK();

    {
        TEST();

        uid = getuid();
        ASSERT(uid >= 0);
        euid = geteuid();
        ASSERT(euid >= 0);
        gid = getgid();
        ASSERT(gid >= 0);
        egid = getegid();
        ASSERT(egid >= 0);

        CHECKPOINT("uid=%d euid=%d gid=%d egid=%d\n", uid, euid, gid, egid);

        STATUS();
    }

    {
        int rc;
        FILE * fp;

        TEST();

        rc = diminuto_realtime_is_supported_path((const char *)0);
        ASSERT(rc < 0);
        EXPECT(errno == EINVAL);

        rc = diminuto_realtime_is_supported_path("/proc/1/mem");
        EXPECT(rc < 0);
        EXPECT(((euid > 0) && (errno == EACCES)) || ((euid == 0) && (errno == EIO)));

        rc = diminuto_realtime_is_supported_path("./PREEMPT_RT-NOT-EXIST");
        ASSERT(rc == 0);

        ASSERT((fp = tmpfile()) != (FILE *)0);
        ASSERT((rc = fprintf(fp, "%d\n", -1)) > 0);
        rc = diminuto_realtime_is_supported_fp(fp, "PREEMPT_RT-1");
        ASSERT(rc == 0);

        ASSERT((fp = tmpfile()) != (FILE *)0);
        ASSERT((rc = fprintf(fp, "%d\n", 0)) > 0);
        rc = diminuto_realtime_is_supported_fp(fp, "PREEMPT_RT-2");
        ASSERT(rc == 0);

        ASSERT((fp = tmpfile()) != (FILE *)0);
        ASSERT((rc = fprintf(fp, "%d\n", 1)) > 0);
        rc = diminuto_realtime_is_supported_fp(fp, "PREEMPT_RT-3");
        ASSERT(rc > 0);

        rc = diminuto_realtime_is_supported();
        ASSERT(rc >= 0);

        CHECKPOINT("PREEMPT_RT is %ssupported.\n", rc ? "" : "not ");

        STATUS();
    }

    {
        int scheduler[] = {
#if defined(SCHED_NORMAL)
            SCHED_NORMAL,
#endif
#if defined(SCHED_OTHER)
            SCHED_OTHER,
#endif
#if defined(SCHED_FIFO)
            SCHED_FIFO,
#endif
#if defined(SCHED_RR)
            SCHED_RR,
#endif
#if defined(SCHED_BATCH)
            SCHED_BATCH,
#endif
#if defined(SCHED_ISO)
            SCHED_ISO,
#endif
#if defined(SCHED_IDLE)
            SCHED_IDLE,
#endif
#if defined(SCHED_DEADLINE)
            SCHED_DEADLINE,
#endif
        };
        const char * name[] = {
#if defined(SCHED_NORMAL)
            "SCHED_NORMAL",
#endif
#if defined(SCHED_OTHER)
            "SCHED_OTHER",
#endif
#if defined(SCHED_FIFO)
            "SCHED_FIFO",
#endif
#if defined(SCHED_RR)
            "SCHED_RR",
#endif
#if defined(SCHED_BATCH)
            "SCHED_BATCH",
#endif
#if defined(SCHED_ISO)
            "SCHED_ISO",
#endif
#if defined(SCHED_IDLE)
            "SCHED_IDLE",
#endif
#if defined(SCHED_DEADLINE)
            "SCHED_DEADLINE",
#endif
        };
        int ii;

        TEST();

        for (ii = 0; ii < countof(scheduler); ++ii) {
            CHECKPOINT("Scheduler [%d] %s (%d) Priority min %d max %d\n",
                ii,
                name[ii],
                scheduler[ii],
                sched_get_priority_min(scheduler[ii]),
                sched_get_priority_max(scheduler[ii]));
        }

        STATUS();
    }

    {
        pid_t pid;
        int rc;
        struct timespec ts = { 0, 0 };

        TEST();

        pid = getpid();
        ASSERT(pid >= 0);

        rc = sched_rr_get_interval(pid, &ts);
        ASSERT(rc == 0);

        CHECKPOINT("Round Robin Interval %llu.%09llus\n",
            (long long unsigned)ts.tv_sec,
            (long long unsigned)ts.tv_nsec);

        STATUS();
    }

    EXIT();
}
