#!/bin/bash
# Copyright 2018 Digital Aggregates Corporation, Colorado USA<BR>
# Licensed under the terms in LICENSE.txt<BR>
# Chip Overclock <coverclock@diag.com><BR>
# https://github.com/coverclock/com-diag-diminuto<BR>
 
# Defines the pinins and pinouts of the hardware test fixture
# on which I run the functional tests.

# SEE ALSO: hardware_test_fixture.sh

HARDWARE_TEST_FIXTURE_BUS_I2C=1
HARDWARE_TEST_FIXTURE_DEV_I2C_LUX=0x39
HARDWARE_TEST_FIXTURE_PIN_LED_1=16
HARDWARE_TEST_FIXTURE_PIN_LED_2=20
HARDWARE_TEST_FIXTURE_PIN_LED_3=21
HARDWARE_TEST_FIXTURE_PIN_BUT_LOW=22
HARDWARE_TEST_FIXTURE_PIN_BUT_HIGH=27
HARDWARE_TEST_FIXTURE_PIN_LED_PWM=12
HARDWARE_TEST_FIXTURE_PIN_INT_LUX=26