#!/bin/bash
# Copyright 2020 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# Unexports all of the pins used by the hardware text fixture.

. $(readlink -e $(dirname ${0})/../fun)/hardware_test_fixture

. $(readlink -e $(dirname ${0})/../bin)/setup

pintool -p ${HARDWARE_TEST_FIXTURE_PIN_LED_1} -e
pintool -p ${HARDWARE_TEST_FIXTURE_PIN_LED_2} -e
pintool -p ${HARDWARE_TEST_FIXTURE_PIN_LED_3} -e
pintool -p ${HARDWARE_TEST_FIXTURE_PIN_BUT_LOW} -e
pintool -p ${HARDWARE_TEST_FIXTURE_PIN_BUT_HIGH} -e
pintool -p ${HARDWARE_TEST_FIXTURE_PIN_PWM_LED} -e
pintool -p ${HARDWARE_TEST_FIXTURE_PIN_PWM_ADC} -e
pintool -p ${HARDWARE_TEST_FIXTURE_PIN_INT_LUX} -e
pintool -p ${HARDWARE_TEST_FIXTURE_PIN_INT_ADC} -e
