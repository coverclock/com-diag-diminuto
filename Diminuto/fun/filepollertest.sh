#!/bin/bash
# Copyright 2025 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# ABSTRACT
#
# Run the filepoller against several test scenarios.
#
# USAGE
#
# filepollertest
#
# SEE ALSO
#
# filepoller
#
# unittest-file-poller
#

. $(readlink -e $(dirname ${0})/../bin)/setup

PGM=$(basename $0)
TMP=${TMPDIR:-"/tmp"}
TMPFIL="${TMP}/${NAME}.txt"
PIPFIL="${TMP}/${NAME}.fifo"
DATFIL=${1:-"./lesser.txt"}

echo "${PGM} TEST 1:" 1>&2
filepoller < ${DATFIL} > ${TMPFIL}
diff ${DATFIL} ${TMPFIL}
if (( $? != 0 )); then
    echo "${PGM} TEST 1 FAILED!" 1>&2
    exit 1
fi
rm -f ${TMPFIL}

echo "${PGM} TEST 2:" 1>&2
cat ${DATFIL} | filepoller > ${TMPFIL}
diff ${DATFIL} ${TMPFIL}
if (( $? != 0 )); then
    echo "${PGM} TEST 2 FAILED!" 1>&2
    exit 2
fi
rm -f ${TMPFIL}

echo "${PGM} TEST 3:" 1>&2
rm -f ${PIPFIL}
mkfifo ${PIPFIL}
filepoller < ${PIPFIL} > ${TMPFIL} &
PID=$!
cat ${DATFIL} > ${PIPFIL}
diff ${DATFIL} ${TMPFIL}
if (( $? != 0 )); then
    echo "${PGM} TEST 3 FAILED!" 1>&2
    exit 3
fi
rm -f ${TMPFIL}
rm -f ${PIPFIL}

# To test the serial device handling, you can try this interactive
# test, which blocks the process for a bit and lets characters build
# up in the hardware queue of the UART.
#
# (sleep 5; filepoller)

echo "${PGM} SUCCESS." 1>&2

exit 0
