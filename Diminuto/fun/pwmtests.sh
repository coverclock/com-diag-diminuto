#!/bin/bash
# Copyright 2018 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# ABSTRACT
#
# Run the pwmtest program on a GPIO pin with a duty cycle from 0 to 100.
#
# USAGE
#
# pwmtests [ DELAYSECONDS [ GPIOPIN ] ]
#
# SEE ALSO
#
# hardware_test_fixture.sh
#

. $(readlink -e $(dirname ${0})/../fun)/hardware_test_fixture

. $(readlink -e $(dirname ${0})/../bin)/setup

SEC=${1:-1}
PIN=${2:-${HARDWARE_TEST_FIXTURE_PIN_PWM_LED}}
DUTY=0;
PCT=100;

while [[ ${DUTY} -le ${PCT} ]]; do
	pwmtest ${PIN} ${DUTY} &
	sleep ${SEC}
	kill $!
	DUTY=$((${DUTY} + 1))
done

exit 0
