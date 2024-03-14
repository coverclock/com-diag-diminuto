#!/bin/bash
# Copyright 2018 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# ABSTRACT
#
# I use a NaviSys Technology GR-701W USB GPS device to test
# the Diminuto Serial feature's support of data carrier detect
# (DCD). The GR supports one pulse per second (1PPS) by toggling
# DCD on the USB simulated serial connection. On my Raspberry Pi
# the GR will typically enumerate as /dev/ttyUSB0, and runs at
# 9600 8N1. There will be some garbage characters initially that
# I believe are due to data being decoded at the wrong baud rate
# before serialtool has established the serial port parameters.
#
# USAGE
#
# dcdtest [ DEVICE [ SPEED ] ]
#

. $(readlink -e $(dirname ${0})/../bin)/setup

DEVICE=${1:-"/dev/ttyUSB0"}
RATE=${2:-9600}

exec serialtool -D ${DEVICE} -b ${RATE} -8 -n -1 -m -c
