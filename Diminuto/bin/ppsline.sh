#!/bin/bash
# Copyright 2024 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# ABSTRACT
#
# Uses the Diminuto linetool to select(2) on the specified GPIO line (18 on my
# lab fixture) to read the GPS device (MakerFocus USB-Port-GPS xQuectel L80-R
# on my lab fixture) PPS signal. This is just for testing. In production, the
# GPS NMEA stream and PPS signal is fed into gpsd which in turn feeds into
# ntpd.
#
# USAGE
#
# pps [ GPIODEVICE:GPIOLINE ]
#

LINE=${1:-"/dev/gpiochip4:18"}

linetool -P ${LINE} -i -H -R -F -x -M 0 -m -1
