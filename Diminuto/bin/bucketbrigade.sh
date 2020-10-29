#!/bin/bash
# Copyright 2020 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
# Use Diminuto's serialtool to read the output from a GNSS device on
# one serial port, then direct that output to another serial device,
# perhaps to be consumed by some other application or device.

SOURCE=${1:-"/dev/ttyUSB0"}
SOURCING=${2:-115200}
SINK=${3:-"/dev/ttyUSB1"}
SINKING=${4:-115200}

. $(readlink -e $(dirname ${0})/../bin)/setup

serialtool -D ${SOURCE} -b ${SOURCING} -8 -n -1 -l | serialtool -D ${SINK} -b ${SINKING} -8 -n -1 -m

exit 0
