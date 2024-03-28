#!/bin/bash
# Copyright 2024 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto

PROGRAM=$(basename $0)

. $(readlink -e $(dirname ${0})/../bin)/setup
. $(readlink -e $(dirname ${0})/../fun)/hardware_test_fixture

export COM_DIAG_DIMINUTO_LOG_MASK="~0"

linetool -D ${HARDWARE_TEST_FIXTURE_GPIO_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_BUT_HIGH} -i -H -B -M ${HARDWARE_TEST_FIXTURE_DEBOUNCE_DEV_USEC} -x -1 -m ${HARDWARE_TEST_FIXTURE_TIMEOUT_MUX_USEC} -n | ( exit )

# SIGPIPE won't be detected by linetool until it tries to write
# to the orphaned pipe after the consumer has exited. Since bash
# starts the processes in the pipeline from right to left, the
# pipeline consumer likely (but I suspect not for certain) exits
# before linetool writes its first output (as a result of the -1
# option. This results in a SIGPIPE. If linetool writes before 
# the consumer exits, the producer process won't get a SIGPIPE
# until there is a state change on the line and the producer writes
# again. (SIGPIPE is signal 13.)

exit 0
