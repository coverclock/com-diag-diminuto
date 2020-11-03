#!/bin/bash
# Copyright 2018 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# ABSTRACT
#
# Run the pwmthread program on a specified GPIO pin with a duty cycle from
# 0 to 100.
#
# USAGE
#
# pwmthreads [ GPIOPIN [ DELAYSECONDS ] ]
#
# SEE ALSO
#
# hardware_test_fixture.sh
#

. $(readlink -e $(dirname ${0})/../fun)/hardware_test_fixture

. $(readlink -e $(dirname ${0})/../bin)/setup

PGM=$(basename $0)
PIN=${1:-${HARDWARE_TEST_FIXTURE_PIN_PWM_LED}}
SEC=${2:-3}
DUTY=0;
PCT=100;
DIR="tmp"

mkdir -p ${DIR}

while [[ ${DUTY} -le ${PCT} ]]; do
	pwmthread ${PIN} ${DUTY} &
	PID=$!
	trap "kill ${PID}; exit" 1 2 3 15
	echo ${PGM}: ${PIN} ${SEC} ${DUTY} ${PID} 1>&2
	sleep ${SEC}
	kill -15 ${PID}
	#wait ${PID}
	wait
	DUTY=$((${DUTY} + 1))
done

pintool -p ${PIN} -n 2> /dev/null

exit 0
