#!/bin/bash
# Copyright 2018 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto

# This is built for a specific test fixture I breadboarded up.

. $(readlink -e $(dirname ${0})/../bin)/setup

PIN=${1:-16}
DUTY=0;
PCT=100;
SEC=2

while [[ ${DUTY} -le ${PCT} ]]; do
	pwmtest ${PIN} ${DUTY} &
	sleep ${SEC}
	kill $!
	DUTY=$((${DUTY} + 1))
done

exit 0