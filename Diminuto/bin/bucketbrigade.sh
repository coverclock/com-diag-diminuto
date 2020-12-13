#!/bin/bash
# Copyright 2020 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# ABSTRACT
#
# Use Diminuto's serialtool to read the output from a GNSS device on
# one serial port, then direct that output to another serial device,
# perhaps to be consumed by some other application or device.
#
# USAGE
#
# bucketbrigade SOURCEDEVICE SOURCESPEED SINKDEVICE SINKSPEED
#
# EXAMPLES
#
# bucketbrigade /dev/ttyACM0 9600 /dev/ttyUSB0 9600
#

SOURCE=${1:-"/dev/ttyUSB0"}
SOURCING=${2:-115200}
SINK=${3:-"/dev/ttyUSB1"}
SINKING=${4:-115200}

serialtool -D ${SOURCE} -b ${SOURCING} -8 -n -1 -l | serialtool -D ${SINK} -b ${SINKING} -8 -n -1 -m

exit 0
