/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2009-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the Uninterruptible Section feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the Uninterruptible Section feature.
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_alarm.h"
#include "com/diag/diminuto/diminuto_interrupter.h"
#include "com/diag/diminuto/diminuto_timer.h"
#include "com/diag/diminuto/diminuto_time.h"
#include "com/diag/diminuto/diminuto_delay.h"
#include "com/diag/diminuto/diminuto_frequency.h"
#include "com/diag/diminuto/diminuto_uninterruptiblesection.h"
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char ** argv)
{

    {
        diminuto_ticks_t hertz;
        sigset_t mask;
        pid_t pid;
        int rc;

        TEST();

        ASSERT((hertz = diminuto_frequency()) > 0);
        ASSERT((pid = getpid()) >= 0);

        ASSERT(diminuto_alarm_install(0) == 0);
        ASSERT(diminuto_interrupter_install(0) == 0);

        ASSERT(sigemptyset(&mask) == 0);
        ASSERT(!sigismember(&mask, SIGTERM));
        ASSERT(!sigismember(&mask, SIGALRM));
        ASSERT(!sigismember(&mask, SIGCHLD));
        ASSERT(!sigismember(&mask, SIGINT));

        ASSERT(pthread_sigmask(0, (sigset_t *)0, &mask) == 0);
        ASSERT(!sigismember(&mask, SIGTERM));
        ASSERT(!sigismember(&mask, SIGALRM));
        ASSERT(!sigismember(&mask, SIGCHLD));
        ASSERT(!sigismember(&mask, SIGINT));

        ASSERT(diminuto_timer_periodic(hertz / 20) >= 0);
        ASSERT(diminuto_delay(hertz, !0) > 0);
        ASSERT(diminuto_alarm_check());
        ASSERT(diminuto_timer_periodic(0) >= 0);
        (void)diminuto_alarm_check();

        ASSERT(diminuto_interrupter_signal(pid) == 0);
        ASSERT(diminuto_interrupter_check());

        rc = 0;

        DIMINUTO_UNINTERRUPTIBLE_SECTION_BEGIN(SIGTERM, SIGALRM, SIGCHLD);

            rc = 1;

            ASSERT(pthread_sigmask(0, (sigset_t *)0, &mask) == 0);
            ASSERT(sigismember(&mask, SIGTERM));
            ASSERT(sigismember(&mask, SIGALRM));
            ASSERT(sigismember(&mask, SIGCHLD));
            ASSERT(!sigismember(&mask, SIGINT));

            ASSERT(diminuto_timer_periodic(hertz / 20) >= 0);
            ASSERT(diminuto_delay(hertz, !0) == 0);
            ASSERT(!diminuto_alarm_check());
            ASSERT(diminuto_timer_periodic(0) >= 0);
            (void)diminuto_alarm_check();

            ASSERT(diminuto_interrupter_signal(pid) == 0);
            ASSERT(diminuto_interrupter_check());

            DIMINUTO_UNINTERRUPTIBLE_SECTION_BEGIN(SIGINT);

                ASSERT(rc == 1);
                rc = 2;

                ASSERT(pthread_sigmask(0, (sigset_t *)0, &mask) == 0);
                ASSERT(sigismember(&mask, SIGTERM));
                ASSERT(sigismember(&mask, SIGALRM));
                ASSERT(sigismember(&mask, SIGCHLD));
                ASSERT(sigismember(&mask, SIGINT));

                ASSERT(diminuto_timer_periodic(hertz / 20) >= 0);
                ASSERT(diminuto_delay(hertz, !0) == 0);
                ASSERT(!diminuto_alarm_check());
                ASSERT(diminuto_timer_periodic(0) >= 0);
                (void)diminuto_alarm_check();

                ASSERT(diminuto_interrupter_signal(pid) == 0);
                ASSERT(!diminuto_interrupter_check());

            DIMINUTO_UNINTERRUPTIBLE_SECTION_END;

            ASSERT(rc == 2);

            ASSERT(pthread_sigmask(0, (sigset_t *)0, &mask) == 0);
            ASSERT(sigismember(&mask, SIGTERM));
            ASSERT(sigismember(&mask, SIGALRM));
            ASSERT(sigismember(&mask, SIGCHLD));
            ASSERT(!sigismember(&mask, SIGINT));

            ASSERT(diminuto_timer_periodic(hertz / 20) >= 0);
            ASSERT(diminuto_delay(hertz, !0) == 0);
            ASSERT(!diminuto_alarm_check());
            ASSERT(diminuto_timer_periodic(0) >= 0);
            (void)diminuto_alarm_check();

            ASSERT(diminuto_interrupter_signal(pid) == 0);
            ASSERT(diminuto_interrupter_check());

        DIMINUTO_UNINTERRUPTIBLE_SECTION_END;

        ASSERT(rc == 2);

        ASSERT(pthread_sigmask(0, (sigset_t *)0, &mask) == 0);
        ASSERT(!sigismember(&mask, SIGTERM));
        ASSERT(!sigismember(&mask, SIGALRM));
        ASSERT(!sigismember(&mask, SIGCHLD));
        ASSERT(!sigismember(&mask, SIGINT));

        ASSERT(diminuto_timer_periodic(hertz / 20) >= 0);
        ASSERT(diminuto_delay(hertz, !0) > 0);
        ASSERT(diminuto_alarm_check());
        ASSERT(diminuto_timer_periodic(0) >= 0);
        (void)diminuto_alarm_check();

        ASSERT(diminuto_interrupter_signal(pid) == 0);
        ASSERT(diminuto_interrupter_check());

        STATUS();
    }

    {
        diminuto_ticks_t hertz;
        sigset_t mask;
        pid_t pid;
        int rc;

        TEST();

        ASSERT((hertz = diminuto_frequency()) > 0);
        ASSERT((pid = getpid()) >= 0);

        ASSERT(diminuto_alarm_install(0) == 0);
        ASSERT(diminuto_interrupter_install(0) == 0);

        ASSERT(sigemptyset(&mask) == 0);
        ASSERT(!sigismember(&mask, SIGTERM));
        ASSERT(!sigismember(&mask, SIGALRM));
        ASSERT(!sigismember(&mask, SIGCHLD));
        ASSERT(!sigismember(&mask, SIGINT));

        ASSERT(pthread_sigmask(0, (sigset_t *)0, &mask) == 0);
        ASSERT(!sigismember(&mask, SIGTERM));
        ASSERT(!sigismember(&mask, SIGALRM));
        ASSERT(!sigismember(&mask, SIGCHLD));
        ASSERT(!sigismember(&mask, SIGINT));

        ASSERT(diminuto_timer_periodic(hertz / 20) >= 0);
        ASSERT(diminuto_delay(hertz, !0) > 0);
        ASSERT(diminuto_alarm_check());
        ASSERT(diminuto_timer_periodic(0) >= 0);
        (void)diminuto_alarm_check();

        ASSERT(diminuto_interrupter_signal(pid) == 0);
        ASSERT(diminuto_interrupter_check());

        rc = 0;

        DIMINUTO_UNINTERRUPTIBLE_SECTION_TRY(SIGTERM, SIGALRM, SIGCHLD);

            rc = 1;

            ASSERT(pthread_sigmask(0, (sigset_t *)0, &mask) == 0);
            ASSERT(sigismember(&mask, SIGTERM));
            ASSERT(sigismember(&mask, SIGALRM));
            ASSERT(sigismember(&mask, SIGCHLD));
            ASSERT(!sigismember(&mask, SIGINT));

            ASSERT(diminuto_timer_periodic(hertz / 20) >= 0);
            ASSERT(diminuto_delay(hertz, !0) == 0);
            ASSERT(!diminuto_alarm_check());
            ASSERT(diminuto_timer_periodic(0) >= 0);
            (void)diminuto_alarm_check();

            ASSERT(diminuto_interrupter_signal(pid) == 0);
            ASSERT(diminuto_interrupter_check());

            DIMINUTO_UNINTERRUPTIBLE_SECTION_TRY(SIGINT);

                ASSERT(rc == 1);
                rc = 2;

                ASSERT(pthread_sigmask(0, (sigset_t *)0, &mask) == 0);
                ASSERT(sigismember(&mask, SIGTERM));
                ASSERT(sigismember(&mask, SIGALRM));
                ASSERT(sigismember(&mask, SIGCHLD));
                ASSERT(sigismember(&mask, SIGINT));

                ASSERT(diminuto_timer_periodic(hertz / 20) >= 0);
                ASSERT(diminuto_delay(hertz, !0) == 0);
                ASSERT(!diminuto_alarm_check());
                ASSERT(diminuto_timer_periodic(0) >= 0);
                (void)diminuto_alarm_check();

                ASSERT(diminuto_interrupter_signal(pid) == 0);
                ASSERT(!diminuto_interrupter_check());

            DIMINUTO_UNINTERRUPTIBLE_SECTION_END;

            ASSERT(rc == 2);

            ASSERT(pthread_sigmask(0, (sigset_t *)0, &mask) == 0);
            ASSERT(sigismember(&mask, SIGTERM));
            ASSERT(sigismember(&mask, SIGALRM));
            ASSERT(sigismember(&mask, SIGCHLD));
            ASSERT(!sigismember(&mask, SIGINT));

            ASSERT(diminuto_timer_periodic(hertz / 20) >= 0);
            ASSERT(diminuto_delay(hertz, !0) == 0);
            ASSERT(!diminuto_alarm_check());
            ASSERT(diminuto_timer_periodic(0) >= 0);
            (void)diminuto_alarm_check();

            ASSERT(diminuto_interrupter_signal(pid) == 0);
            ASSERT(diminuto_interrupter_check());

        DIMINUTO_UNINTERRUPTIBLE_SECTION_END;

        ASSERT(rc == 2);

        ASSERT(pthread_sigmask(0, (sigset_t *)0, &mask) == 0);
        ASSERT(!sigismember(&mask, SIGTERM));
        ASSERT(!sigismember(&mask, SIGALRM));
        ASSERT(!sigismember(&mask, SIGCHLD));
        ASSERT(!sigismember(&mask, SIGINT));

        ASSERT(diminuto_timer_periodic(hertz / 20) >= 0);
        ASSERT(diminuto_delay(hertz, !0) > 0);
        ASSERT(diminuto_alarm_check());
        ASSERT(diminuto_timer_periodic(0) >= 0);
        (void)diminuto_alarm_check();

        ASSERT(diminuto_interrupter_signal(pid) == 0);
        ASSERT(diminuto_interrupter_check());

        STATUS();
    }

    EXIT();
}
