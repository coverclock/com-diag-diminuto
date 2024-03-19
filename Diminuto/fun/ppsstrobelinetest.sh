#!/bin/bash
# Copyright 2024 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-hazer
#
# USAGE
#
# ppstrobelinetest.sh [ INPUTDEVICE [ INPUTLINE [ OUTPUTDEVICE [ OUTPUTLINE ] ] ] ]
#
# SEE ALSO
#
# hardware_test_fixture.sh
#

. $(readlink -e $(dirname ${0})/../fun)/hardware_test_fixture
. $(readlink -e $(dirname ${0})/../bin)/setup

PROGRAM=$(basename $0)
INPUTDEVICE=${1:-${HARDWARE_TEST_FIXTURE_GPIO_DEVICE}}
INPUTLINE=${2:-${HARDWARE_TEST_FIXTURE_PIN_PPS}}
OUTPUTDEVICE=${3:-${HARDWARE_TEST_FIXTURE_GPIO_DEVICE}}
OUTPUTLINE=${4:-${HARDWARE_TEST_FIXTURE_PIN_LED_1}}

linetool -D ${INPUTDEVICE} -p ${INPUTLINE} -i -H -R -F -M 0 -x -m -1 |
	while read VALUE; do
		echo ${PROGRAM}: ${VALUE} 1>&2
		if [[ ${VALUE} -eq 0 ]]; then
			linetool -D ${OUTPUTDEVICE} -p ${OUTPUTLINE} -o -H -x -c
		else
			linetool -D ${OUTPUTDEVICE} -p ${OUTPUTLINE} -o -H -x -s
		fi
	done

exit 0
