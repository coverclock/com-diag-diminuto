/* vi: set ts=4 expandtab shiftwidth=4: */
/**
 * @file
 * @copyright Copyright 2010-2022 Digital Aggregates Corporation, Colorado, USA.
 * @note Licensed under the terms in LICENSE.txt.
 * @brief This is a unit test of the memory Barrier feature.
 * @author Chip Overclock <mailto:coverclock@diag.com>
 * @see Diminuto <https://github.com/coverclock/com-diag-diminuto>
 * @details
 * This is a unit test of the memory Barrier feature.
 *
 * I'm still pondering how to actually functionally test
 * this code without some hardware debugging gear.
 *
 * Use the special make targets to see what the assembler looks
 * like:
 *
 * make out/host/obc/tst/unittest-barrier.c
 * make out/host/obc/tst/unittest-barrier.s
 */

#include "com/diag/diminuto/diminuto_unittest.h"
#include "com/diag/diminuto/diminuto_barrier.h"

/*
 * The following do_* functions exist so that this
 * code is more easily found in the resulting assembler
 * code.
 */

static volatile int lock = 0;

static int value = 0xdeadbeef;

void do_sync_synchronize()
{
#if defined(COM_DIAG_DIMINUTO_BARRIER)
    __sync_synchronize();
#endif
}

void do_sync_lock_test_and_set()
{
#if defined(COM_DIAG_DIMINUTO_ACQUIRE)
    __sync_lock_test_and_set(&lock, value);
#endif
}

void do_sync_lock_release()
{
#if defined(COM_DIAG_DIMINUTO_RELEASE)
    __sync_lock_release(&lock);
#endif
}

void do_functions()
{
    diminuto_barrier();
    diminuto_acquire();
    diminuto_release();
}

int main(void)
{
    volatile int reg;

    {
        TEST();

#if !defined(COM_DIAG_DIMINUTO_BARRIER)
        FAILURE();
#endif

#if !defined(COM_DIAG_DIMINUTO_ACQUIRE)
        FAILURE();
#endif

#if !defined(COM_DIAG_DIMINUTO_RELEASE)
        FAILURE();
#endif

        STATUS();
    }

    {
        TEST();

        /*
         * This is here mostly to keep the compiler from
         * optimizing out the function implementations.
         */

        printf("&do_sync_synchronize=%p\n", &do_sync_synchronize);
        do_sync_synchronize();

        printf("&lock=%p\n", &lock);
        printf("lock=0x%x=%u=%d\n", lock, lock, lock);

        printf("&do_sync_lock_test_and_set=%p\n", &do_sync_lock_test_and_set);
        do_sync_lock_test_and_set();

        printf("&lock=%p\n", &lock);
        printf("lock=0x%x=%u=%d\n", lock, lock, lock);

        printf("&do_sync_lock_release=%p\n", &do_sync_lock_release);
        do_sync_lock_release();

        printf("&lock=%p\n", &lock);
        printf("lock=0x%x=%u=%d\n", lock, lock, lock);

        printf("&do_functions=%p\n", &do_functions);
        do_functions();

        STATUS();
    }

    {
        TEST();
        diminuto_barrier();
        reg = 0;
        diminuto_barrier();
        reg = 1;
        diminuto_barrier();
        reg = 0;
        STATUS();
    }

    {
        TEST();
        diminuto_acquire();
        reg = 1;
        diminuto_release();
        STATUS();
    }

    {
        TEST();
        diminuto_barrier_f();
        reg = 0;
        diminuto_barrier_f();
        reg = 1;
        diminuto_barrier_f();
        reg = 0;
        STATUS();
    }

    {
        TEST();
        diminuto_acquire_f();
        reg = 1;
        diminuto_release_f();
        STATUS();
    }

    EXIT();
}
