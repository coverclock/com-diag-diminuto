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
# You will probably have to run this as root.

PROGRAM=$(basename $0)
ROOT=$(readlink -e $(dirname ${0})/../bin)
PIN=22

. ${ROOT}/setup

if [[ $# -eq 0 ]]; then
	pintool -p ${PIN} -n 2> /dev/null
	pintool -p ${PIN} -x -i -H
	exec pintool -p ${PIN} -X ${ROOT}/pinchange -B -U -M
elif [[ $# -eq 1 ]]; then
    echo "usage: ${PROGRAM} [ PIN STATE PRIOR ]" 1>&2
    exit 0
elif [[ $# -eq 2 ]]; then
    pintool -p ${PIN} -n 2> /dev/null
    pintool -p ${PIN} -x -i -H
    exec pintool -p ${PIN} -X ${ROOT}/pinchange -B -U -S -M
elif [[ $# -eq 3 ]]; then
	PIN=${1:-"-"}
	STATE=${2:-"-"}
	PRIOR=${3:-"-"}
	echo ${PROGRAM} ${PIN} ${STATE} ${PRIOR} 1>&2
	exit 0
else
	echo "usage: ${PROGRAM} [ PIN STATE PRIOR ]" 1>&2
	exit 1
fi
