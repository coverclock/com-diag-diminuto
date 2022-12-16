#!/bin/bash
# Copyright 2018 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# ABSTRACT
#
# Run the pwmthread program on a specified GPIO pin with a duty cycle from
# 0% to 100%.
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

PGM=$(basename $0)
PIN=${1:-${HARDWARE_TEST_FIXTURE_PIN_PWM_LED}}
SEC=${2:-1}
MIN=0
MAX=255
DUTY=${MIN}

. $(readlink -e $(dirname ${0})/../bin)/setup

pintool -p ${PIN} -x -o -c -n 2> /dev/null

while [[ ${DUTY} -le ${MAX} ]]; do
	pwmthread ${PIN} ${DUTY} &
	PID=$!
	trap "kill ${PID}; exit" 1 2 3 15
	echo ${PGM}: pin ${PIN} sec ${SEC} dty ${DUTY} pid ${PID} 1>&2
	sleep ${SEC}
	kill -15 ${PID}
	wait
	DUTY=$((${DUTY} + 1))
done

while [[ ${DUTY} -ge ${MIN} ]]; do
	pwmthread ${PIN} ${DUTY} &
	PID=$!
	trap "kill ${PID}; exit" 1 2 3 15
	echo ${PGM}: pin ${PIN} sec ${SEC} dty ${DUTY} pid ${PID} 1>&2
	sleep ${SEC}
	kill -15 ${PID}
	wait
	DUTY=$((${DUTY} - 1))
done

pintool -p ${PIN} -x -o -c -n 2> /dev/null

exit 0
