#!/bin/bash
# Copyright 2018 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto

# This is built for a specific test fixture I breadboarded up.
# After the values of input pins 22 and 27 are read and displayed,
# and the LEDs on output pins 16, 20, and 21 cycle on and off,
# press the button on pin 27 to advance the 3-bit binary counter
# displayed on the LEDs, or press the button pn pin 22 simultaneously
# with pin 27 to exit. You will probably have to run this as root.

. $(readlink -e $(dirname ${0})/../fun)/hardware_test_fixture

. $(readlink -e $(dirname ${0})/../bin)/setup

trap "pintool -p ${HARDWARE_TEST_FIXTURE_PIN_LED_1} -e; pintool -p ${HARDWARE_TEST_FIXTURE_PIN_LED_2} -e; pintool -p ${HARDWARE_TEST_FIXTURE_PIN_LED_3} -e; pintool -p ${HARDWARE_TEST_FIXTURE_PIN_PWM_LED} -e; pintool -p ${HARDWARE_TEST_FIXTURE_PIN_BUT_LOW} -e; pintool -p ${HARDWARE_TEST_FIXTURE_PIN_BUT_HIGH} -e" 0 2 3 15

pintool -p ${HARDWARE_TEST_FIXTURE_PIN_LED_1} -e -x -u 1000000 -o
pintool -p ${HARDWARE_TEST_FIXTURE_PIN_LED_2} -e -x -u 1000000 -o
pintool -p ${HARDWARE_TEST_FIXTURE_PIN_LED_3} -e -x -u 1000000 -o
pintool -p ${HARDWARE_TEST_FIXTURE_PIN_PWM_LED} -e -x -u 1000000 -o
pintool -p ${HARDWARE_TEST_FIXTURE_PIN_BUT_LOW} -e -x -u 1000000 -i -L
pintool -p ${HARDWARE_TEST_FIXTURE_PIN_BUT_HIGH} -e -x -u 1000000 -i -H

PIN22=$(pintool -p ${HARDWARE_TEST_FIXTURE_PIN_BUT_LOW} -r)
echo PIN22=${PIN22}
PIN27=$(pintool -p ${HARDWARE_TEST_FIXTURE_PIN_BUT_HIGH} -r)
echo PIN27=${PIN27}

pintool -p ${HARDWARE_TEST_FIXTURE_PIN_PWM_LED} -s

pintool -p ${HARDWARE_TEST_FIXTURE_PIN_LED_1} -s
sleep 1
pintool -p ${HARDWARE_TEST_FIXTURE_PIN_LED_2} -s
sleep 1
pintool -p ${HARDWARE_TEST_FIXTURE_PIN_LED_3} -s
sleep 1
pintool -p ${HARDWARE_TEST_FIXTURE_PIN_LED_1} -c
sleep 1
pintool -p ${HARDWARE_TEST_FIXTURE_PIN_LED_2} -c
sleep 1
pintool -p ${HARDWARE_TEST_FIXTURE_PIN_LED_3} -c
sleep 1

VALUE=0
pintool -p ${HARDWARE_TEST_FIXTURE_PIN_BUT_HIGH} -b 10000 | while read PIN27; do
	PIN22=$(pintool -p ${HARDWARE_TEST_FIXTURE_PIN_BUT_LOW} -r)
	if [[ ${PIN22} -ne 0 ]]; then
		pintool -p ${HARDWARE_TEST_FIXTURE_PIN_LED_1} -c
		pintool -p ${HARDWARE_TEST_FIXTURE_PIN_LED_2} -c
		pintool -p ${HARDWARE_TEST_FIXTURE_PIN_LED_3} -c
		break
	elif [[ ${PIN27} -ne 0 ]]; then
		VALUE=$((${VALUE} + 1))
		TEMP=${VALUE}
		PIN16=$((${TEMP} % 2))
		TEMP=$((${TEMP} / 2))
		PIN20=$((${TEMP} % 2))
		TEMP=$((${TEMP} / 2))
		PIN21=$((${TEMP} % 2))
		pintool -p ${HARDWARE_TEST_FIXTURE_PIN_LED_1} -w ${PIN16}
		pintool -p ${HARDWARE_TEST_FIXTURE_PIN_LED_2} -w ${PIN20}
		pintool -p ${HARDWARE_TEST_FIXTURE_PIN_LED_3} -w ${PIN21}
	else
		:
	fi
done

pintool -p ${HARDWARE_TEST_FIXTURE_PIN_PWM_LED} -c

exit 0
