#!/bin/bash
# Copyright 2018 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# ABSTRACT
#
# This uses a serial loopback test fixture that I built.
#
# USAGE
#
# lbkttest [ DEVICE [ SPEED [ MODULOREPORT ] ] ]
#

. $(readlink -e $(dirname ${0})/../bin)/setup

DEVICE=${1:-"/dev/ttyUSB0"}
RATE=${2:-115200}
MODULO=${3:-10240}

exec serialtool -D ${DEVICE} -b ${RATE} -8 -n -1 -l -B -M ${MODULO}
