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
# NOTES
#
# The GPIO devices are different for the RPI4 versus the RPI5,
# The hardware test fixture script assumes the RPi5 but can be
# trivially edited to use the RPI4.
#
# The multiplexing test may take a second button press to exit.
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

. $(readlink -e $(dirname ${0})/../fun)/hardware_test_fixture

echo "${PROGRAM}: GPIO_DEVICE=${HARDWARE_TEST_FIXTURE_GPIO_DEVICE}" 1>&2
echo "${PROGRAM}: LED_1=${HARDWARE_TEST_FIXTURE_PIN_LED_1}" 1>&2
echo "${PROGRAM}: LED_2=${HARDWARE_TEST_FIXTURE_PIN_LED_2}" 1>&2
echo "${PROGRAM}: LED_3=${HARDWARE_TEST_FIXTURE_PIN_LED_3}" 1>&2
echo "${PROGRAM}: BUT_LOW=${HARDWARE_TEST_FIXTURE_PIN_BUT_LOW}" 1>&2
echo "${PROGRAM}: BUT_HIGH=${HARDWARE_TEST_FIXTURE_PIN_BUT_HIGH}" 1>&2

. $(readlink -e $(dirname ${0})/../bin)/setup

echo "${PROGRAM}: SANITY TEST" 1>&2

echo "${PROGRAM}: Initializing." 1>&2

linetool -D ${HARDWARE_TEST_FIXTURE_GPIO_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_1} -o -x -c
linetool -D ${HARDWARE_TEST_FIXTURE_GPIO_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_2} -o -x -c
linetool -D ${HARDWARE_TEST_FIXTURE_GPIO_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_3} -o -x -c
sleep 1

PIN22=$(linetool -D ${HARDWARE_TEST_FIXTURE_GPIO_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_BUT_LOW} -i -L -x -r)
echo "${PROGRAM}: Button ${HARDWARE_TEST_FIXTURE_PIN_BUT_LOW} is ${PIN22}" 1>&2

PIN27=$(linetool -D ${HARDWARE_TEST_FIXTURE_GPIO_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_BUT_HIGH} -i -H -x -r)
echo "${PROGRAM}: Button ${HARDWARE_TEST_FIXTURE_PIN_BUT_HIGH} is ${PIN27}" 1>&2

echo "${PROGRAM}: OUTPUT TEST" 1>&2

echo "${PROGRAM}: Cycling LEDs ${HARDWARE_TEST_FIXTURE_PIN_LED_1} ${HARDWARE_TEST_FIXTURE_PIN_LED_2} ${HARDWARE_TEST_FIXTURE_PIN_LED_3}." 1>&2

sleep 1
linetool -D ${HARDWARE_TEST_FIXTURE_GPIO_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_1} -o -x -s
sleep 1
linetool -D ${HARDWARE_TEST_FIXTURE_GPIO_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_2} -o -x -s
sleep 1
linetool -D ${HARDWARE_TEST_FIXTURE_GPIO_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_3} -o -x -s

sleep 1
linetool -D ${HARDWARE_TEST_FIXTURE_GPIO_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_1} -o -x -c
sleep 1
linetool -D ${HARDWARE_TEST_FIXTURE_GPIO_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_2} -o -x -c
sleep 1
linetool -D ${HARDWARE_TEST_FIXTURE_GPIO_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_3} -o -x -c

sleep 1

echo "${PROGRAM}: DEBOUNCING TEST" 1>&2

echo "${PROGRAM}: Press button ${HARDWARE_TEST_FIXTURE_PIN_BUT_HIGH} to advance." 1>&2
echo "${PROGRAM}: Press button ${HARDWARE_TEST_FIXTURE_PIN_BUT_LOW} SIMULTANEOUSLY to exit." 1>&2

VALUE=0
linetool -D ${HARDWARE_TEST_FIXTURE_GPIO_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_BUT_HIGH} -i -H -x -1 -b ${HARDWARE_TEST_FIXTURE_DEBOUNCE_LIB_USEC} | while read PIN27; do
	PIN22=$(linetool -D ${HARDWARE_TEST_FIXTURE_GPIO_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_BUT_LOW} -i -L -x -r)
	if [[ ${PIN22} -ne 0 ]]; then
		break
	elif [[ ${PIN27} -ne 0 ]]; then
		VALUE=$((${VALUE} + 1))
		TEMP=${VALUE}
		PIN16=$((${TEMP} % 2))
		TEMP=$((${TEMP} / 2))
		PIN20=$((${TEMP} % 2))
		TEMP=$((${TEMP} / 2))
		PIN21=$((${TEMP} % 2))
		linetool -D ${HARDWARE_TEST_FIXTURE_GPIO_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_1} -o -x -w ${PIN16}
		linetool -D ${HARDWARE_TEST_FIXTURE_GPIO_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_2} -o -x -w ${PIN20}
		linetool -D ${HARDWARE_TEST_FIXTURE_GPIO_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_3} -o -x -w ${PIN21}
	else
		:
	fi
done

linetool -D ${HARDWARE_TEST_FIXTURE_GPIO_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_1} -o -x -c
linetool -D ${HARDWARE_TEST_FIXTURE_GPIO_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_2} -o -x -c
linetool -D ${HARDWARE_TEST_FIXTURE_GPIO_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_3} -o -x -c

sleep 1

echo "${PROGRAM}: MULTIPLEXING TEST" 1>&2

echo "${PROGRAM}: Press button ${HARDWARE_TEST_FIXTURE_PIN_BUT_HIGH} to advance." 1>&2
echo "${PROGRAM}: Press button ${HARDWARE_TEST_FIXTURE_PIN_BUT_LOW} SIMULTANEOUSLY to exit." 1>&2
echo "${PROGRAM}: You might have to press button ${HARDWARE_TEST_FIXTURE_PIN_BUT_LOW} once more to exit." 1>&2

VALUE=0
linetool -D ${HARDWARE_TEST_FIXTURE_GPIO_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_BUT_HIGH} -i -H -R -M ${HARDWARE_TEST_FIXTURE_DEBOUNCE_DEV_USEC} -x -1 -m ${HARDWARE_TEST_FIXTURE_TIMEOUT_MUX_USEC} | while read PIN27; do
	PIN22=$(linetool -D ${HARDWARE_TEST_FIXTURE_GPIO_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_BUT_LOW} -i -L -x -r)
	if [[ ${PIN22} -ne 0 ]]; then
		break
	elif [[ ${PIN27} -ne 0 ]]; then
		VALUE=$((${VALUE} + 1))
		TEMP=${VALUE}
		PIN16=$((${TEMP} % 2))
		TEMP=$((${TEMP} / 2))
		PIN20=$((${TEMP} % 2))
		TEMP=$((${TEMP} / 2))
		PIN21=$((${TEMP} % 2))
		linetool -D ${HARDWARE_TEST_FIXTURE_GPIO_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_1} -o -x -w ${PIN16}
		linetool -D ${HARDWARE_TEST_FIXTURE_GPIO_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_2} -o -x -w ${PIN20}
		linetool -D ${HARDWARE_TEST_FIXTURE_GPIO_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_3} -o -x -w ${PIN21}
	else
		:
	fi
done

linetool -D ${HARDWARE_TEST_FIXTURE_GPIO_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_1} -o -x -c
linetool -D ${HARDWARE_TEST_FIXTURE_GPIO_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_2} -o -x -c
linetool -D ${HARDWARE_TEST_FIXTURE_GPIO_DEVICE} -p ${HARDWARE_TEST_FIXTURE_PIN_LED_3} -o -x -c

echo "${PROGRAM}: Exiting." 1>&2

exit 0
