#!/bin/bash
# Copyright 2024 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-hazer
#
# ABSTRACT
#
# Uses the Diminuto linetool to strobe the specified GPIO pin (16 on my
# lab fixture). This is the line used on my GNSS test fixture to follow
# the PPS input from the GNSS device.
#
# USAGE
#
# strobelinetest [ GPIODEV [ GPIOPIN ] ]
#
# SEE ALSO
#
# hardware_test_fixture.sh
#

. $(readlink -e $(dirname ${0})/../fun)/hardware_test_fixture

. $(readlink -e $(dirname ${0})/../bin)/setup

DEVICE=${1:-${HARDWARE_TEST_FIXTURE_DEVICE}}
LINE=${2:-${HARDWARE_TEST_FIXTURE_PIN_LED_1}}

while true; do
	linetool -d -D ${DEVICE} -p ${LINE} -o -H -x -c
	sleep 1
	linetool -d -D ${DEVICE} -p ${LINE} -o -H -x -s
	sleep 1
done
