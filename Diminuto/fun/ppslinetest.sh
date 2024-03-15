#!/bin/bash
# Copyright 2024 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-hazer
#
# ABSTRACT
#
# Uses the Diminuto linetool to select(2) on the specified GPIO pin (18 on my
# lab fixture) to read the GPS device (MakerFocus USB-Port-GPS xQuectel L80-R
# on my lab fixture) PPS signal. This is just for testing. In production, the
# GPS NMEA stream and PPS signal is fed into gpsd which in turn feeds into
# ntpd.
#
# USAGE
#
# ppslinetest [ GPIODEV [ GPIOPIN ] ]
#
# SEE ALSO
#
# hardware_test_fixture.sh
#

. $(readlink -e $(dirname ${0})/../fun)/hardware_test_fixture

. $(readlink -e $(dirname ${0})/../bin)/setup

DEVICE=${1:-${HARDWARE_TEST_FIXTURE_DEVICE}}
LINE=${2:-${HARDWARE_TEST_FIXTURE_PIN_PPS}}

linetool -D ${DEVICE} -p ${LINE} -i -H -R -M 0 -x -m -1
