#!/bin/bash
# Copyright 2024 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto

PROGRAM=$(basename $0)
DEBUG=$1

. $(readlink -e $(dirname ${0})/../bin)/setup
. $(readlink -e $(dirname ${0})/../fun)/hardware_test_fixture

linetool ${DEBUG} -D ${HARDWARE_TEST_FIXTURE_GPIO_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_BUT_HIGH} -i -H -R -M ${HARDWARE_TEST_FIXTURE_DEBOUNCE_DEV_USEC} -x -1 -m ${HARDWARE_TEST_FIXTURE_TIMEOUT_MUX_USEC} | while read VALUE; do
    echo ${PROGRAM} $$ ${VALUE} 1>&2
    break
done

# SIGPIPE won't be detected by linetool until it tries to write
# to the orphaned pipe after the consumer has exited.

exit 0
