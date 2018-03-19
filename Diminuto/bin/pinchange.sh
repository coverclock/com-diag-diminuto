#!/bin/bash
# Copyright 2018 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in README.h
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto

# This is built for a specific test fixture I breadboarded up.
# This runs the pintool in multiplex mode, monitoring edge
# changes on pin 22, and running this script with arguments
# when such a change occurs. So you run this script *without*
# arguments, it runs pintool, and when pintool detects an
# edge change on pin 22, it runs this script *with* arguments.
# You will probably have to run this as root. (You can add
# an argument - anything will work, e.g. "pinchange daemon"
# - when you run this command and it will run as a daemon;
# you'll need to kill it explicitly to get rid of it.)

PROGRAM=$(basename $0)
ROOT=$(readlink -e $(dirname ${0})/../bin)
PIN=22

. ${ROOT}/setup

if [[ $# -eq 0 ]]; then
    pintool -p ${PIN} -n 2> /dev/null
    pintool -p ${PIN} -x -i -H
    exec pintool -p ${PIN} -X ${ROOT}/pinchange -B -U -M
elif [[ $# -eq 1 ]]; then
    pintool -p ${PIN} -n 2> /dev/null
    pintool -p ${PIN} -x -i -H
    exec pintool -p ${PIN} -X ${ROOT}/pinchange -B -U -S -M
elif [[ $# -eq 3 ]]; then
    PIN=${1:-"-"}
    STATE=${2:-"-"}
    PRIOR=${3:-"-"}
    exec ${ROOT}/log -n "${PROGRAM} ${PIN} ${STATE} ${PRIOR}" 1>&2
else
    echo "usage: ${PROGRAM} [ PIN STATE PRIOR ]" 1>&2
    exit 1
fi
