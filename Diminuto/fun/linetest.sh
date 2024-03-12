#!/bin/bash
# Copyright 2024 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# ABSTRACT
#
# This is built for a specific test fixture I breadboarded up.
# After the values of input lines 22 and 27 are read and displayed,
# and the LEDs on output lines 16, 20, and 21 cycle on and off,
# press the button on line 27 to advance the 3-bit binary counter
# displayed on the LEDs, or press the button pn line 22 simultaneously
# with line 27 to exit. You will probably have to run this as root.
#
# SEE ALSO
#
# hardware_test_fixture.sh
#
# USAGE
#
# linetest
#

PROGRAM=$(basename $0)
DEBUG=$1

. $(readlink -e $(dirname ${0})/../fun)/hardware_test_fixture

. $(readlink -e $(dirname ${0})/../bin)/setup

echo "${PROGRAM}: SANITY TEST"

echo "${PROGRAM}: Initializing." 

linetool ${DEBUG} -D ${HARDWARE_TEST_FIXTURE_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_1} -o -x -c
linetool ${DEBUG} -D ${HARDWARE_TEST_FIXTURE_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_2} -o -x -c
linetool ${DEBUG} -D ${HARDWARE_TEST_FIXTURE_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_3} -o -x -c
sleep 1

PIN22=$(linetool ${DEBUG} -D ${HARDWARE_TEST_FIXTURE_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_BUT_LOW} -i -L -x -r)
echo "${PROGRAM}: Button ${HARDWARE_TEST_FIXTURE_PIN_BUT_LOW} is ${PIN22}"

PIN27=$(linetool ${DEBUG} -D ${HARDWARE_TEST_FIXTURE_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_BUT_HIGH} -i -H -x -r)
echo "${PROGRAM}: Button ${HARDWARE_TEST_FIXTURE_PIN_BUT_HIGH} is ${PIN27}"

echo "${PROGRAM}: OUTPUT TEST"

echo "${PROGRAM}: Cycling LEDs ${HARDWARE_TEST_FIXTURE_PIN_LED_1} ${HARDWARE_TEST_FIXTURE_PIN_LED_2} ${HARDWARE_TEST_FIXTURE_PIN_LED_3}."

linetool ${DEBUG} -D ${HARDWARE_TEST_FIXTURE_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_1} -o -x -s
sleep 1
linetool ${DEBUG} -D ${HARDWARE_TEST_FIXTURE_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_2} -o -x -s
sleep 1
linetool ${DEBUG} -D ${HARDWARE_TEST_FIXTURE_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_3} -o -x -s
sleep 1

linetool ${DEBUG} -D ${HARDWARE_TEST_FIXTURE_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_1} -o -x -c
sleep 1
linetool ${DEBUG} -D ${HARDWARE_TEST_FIXTURE_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_2} -o -x -c
sleep 1
linetool ${DEBUG} -D ${HARDWARE_TEST_FIXTURE_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_3} -o -x -c
sleep 1

echo "${PROGRAM}: DEBOUNCING TEST"

echo "${PROGRAM}: Press button ${HARDWARE_TEST_FIXTURE_PIN_BUT_HIGH} to advance."
echo "${PROGRAM}: Press button ${HARDWARE_TEST_FIXTURE_PIN_BUT_LOW} SIMULTANEOUSLY to exit."

VALUE=0
linetool ${DEBUG} -D ${HARDWARE_TEST_FIXTURE_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_BUT_HIGH} -i -H -x -b ${HARDWARE_TEST_FIXTURE_DEBOUNCE_CUE_USEC} | while read PIN27; do
	PIN22=$(linetool ${DEBUG} -D ${HARDWARE_TEST_FIXTURE_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_BUT_LOW} -i -L -x -r)
	if [[ ${PIN22} -ne 0 ]]; then
		linetool ${DEBUG} -D ${HARDWARE_TEST_FIXTURE_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_1} -o -x -c
		linetool ${DEBUG} -D ${HARDWARE_TEST_FIXTURE_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_2} -o -x -c
		linetool ${DEBUG} -D ${HARDWARE_TEST_FIXTURE_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_3} -o -x -c
		break
	elif [[ ${PIN27} -ne 0 ]]; then
		VALUE=$((${VALUE} + 1))
		TEMP=${VALUE}
		PIN16=$((${TEMP} % 2))
		TEMP=$((${TEMP} / 2))
		PIN20=$((${TEMP} % 2))
		TEMP=$((${TEMP} / 2))
		PIN21=$((${TEMP} % 2))
		linetool ${DEBUG} -D ${HARDWARE_TEST_FIXTURE_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_1} -o -x -w ${PIN16}
		linetool ${DEBUG} -D ${HARDWARE_TEST_FIXTURE_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_2} -o -x -w ${PIN20}
		linetool ${DEBUG} -D ${HARDWARE_TEST_FIXTURE_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_3} -o -x -w ${PIN21}
	else
		:
	fi
done

echo "${PROGRAM}: MULTIPLEXING TEST"

echo "${PROGRAM}: Press button ${HARDWARE_TEST_FIXTURE_PIN_BUT_HIGH} to advance."
echo "${PROGRAM}: Press button ${HARDWARE_TEST_FIXTURE_PIN_BUT_LOW} SIMULTANEOUSLY to exit."

VALUE=0
linetool ${DEBUG} -D ${HARDWARE_TEST_FIXTURE_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_BUT_HIGH} -i -H -B -1 -U -M ${HARDWARE_TEST_FIXTURE_DEBOUNCE_DEV_USEC} -x -m ${HARDWARE_TEST_FIXTURE_DEBOUNCE_MUX_USEC} | while read PIN27; do
	PIN22=$(linetool ${DEBUG} -D ${HARDWARE_TEST_FIXTURE_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_BUT_LOW} -i -L -x -r)
	if [[ ${PIN22} -ne 0 ]]; then
		linetool ${DEBUG} -D ${HARDWARE_TEST_FIXTURE_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_1} -o -x -c
		linetool ${DEBUG} -D ${HARDWARE_TEST_FIXTURE_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_2} -o -x -c
		linetool ${DEBUG} -D ${HARDWARE_TEST_FIXTURE_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_3} -o -x -c
		break
	elif [[ ${PIN27} -ne 0 ]]; then
		VALUE=$((${VALUE} + 1))
		TEMP=${VALUE}
		PIN16=$((${TEMP} % 2))
		TEMP=$((${TEMP} / 2))
		PIN20=$((${TEMP} % 2))
		TEMP=$((${TEMP} / 2))
		PIN21=$((${TEMP} % 2))
		linetool ${DEBUG} -D ${HARDWARE_TEST_FIXTURE_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_1} -o -x -w ${PIN16}
		linetool ${DEBUG} -D ${HARDWARE_TEST_FIXTURE_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_2} -o -x -w ${PIN20}
		linetool ${DEBUG} -D ${HARDWARE_TEST_FIXTURE_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_3} -o -x -w ${PIN21}
	else
		:
	fi
done

echo "${PROGRAM}: Exiting."

exit 0
