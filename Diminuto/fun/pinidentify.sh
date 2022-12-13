#!/bin/bash
# Copyright 2022 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto

PROGRAM=$(basename $0)
USEC=${1:-5000000}

. $(readlink -e $(dirname ${0})/../bin)/setup

echo "${PROGRAM}: 12"
pintool -p 12 -x -o -s -u ${USEC} -c -e
echo "${PROGRAM}: 21"
pintool -p 21 -x -o -s -u ${USEC} -c -e
echo "${PROGRAM}: 20"
pintool -p 20 -x -o -s -u ${USEC} -c -e
echo "${PROGRAM}: 16"
pintool -p 16 -x -o -s -u ${USEC} -c -e

exit 0
