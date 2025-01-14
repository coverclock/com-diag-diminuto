#!/bin/bash -x
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
TMPFIL="${TMP}/${PGM}.txt"
PIPFIL="${TMP}/${PGM}.fifo"
DATFIL=${1:-"./lesser.txt"}

rm -f ${TMPFIL}
rm -f ${PIPFIL}

trap "rm -f ${TMPFIL} ${PIPFIL}; exit 9" 1 2 3 15

TEST=1
echo "${PGM} TEST ${TEST}:" 1>&2
filepoller < ${DATFIL} > ${TMPFIL}
diff ${DATFIL} ${TMPFIL}
if (( $? != 0 )); then
    echo "${PGM} TEST ${TEST} FAILED!" 1>&2
    exit ${TEST}
fi
rm -f ${TMPFIL}

TEST=2
echo "${PGM} TEST ${TEST}:" 1>&2
cat ${DATFIL} | filepoller > ${TMPFIL}
diff ${DATFIL} ${TMPFIL}
if (( $? != 0 )); then
    echo "${PGM} TEST ${TEST} FAILED!" 1>&2
    exit ${TEST}
fi
rm -f ${TMPFIL}

TEST=3
echo "${PGM} TEST ${TEST}:" 1>&2
cat ${DATFIL} | filepoller | cat > ${TMPFIL}
diff ${DATFIL} ${TMPFIL}
if (( $? != 0 )); then
    echo "${PGM} TEST ${TEST} FAILED!" 1>&2
    exit ${TEST}
fi
rm -f ${TMPFIL}

TEST=4
echo "${PGM} TEST ${TEST}:" 1>&2
rm -f ${PIPFIL}
mkfifo ${PIPFIL}
filepoller < ${PIPFIL} > ${TMPFIL} &
cat ${DATFIL} > ${PIPFIL}
diff ${DATFIL} ${TMPFIL}
if (( $? != 0 )); then
    echo "${PGM} TEST ${TEST} FAILED!" 1>&2
    exit ${TEST}
fi
rm -f ${TMPFIL}
rm -f ${PIPFIL}

TEST=5
echo "${PGM} TEST ${TEST}:" 1>&2
( sleep 10; cat ${DATFIL} ) | filepoller 15000000000 > ${TMPFIL} &
THAT=$(pgrep filepoller)
sleep ${TEST}
kill -SIGALRM ${THAT}
wait ${THAT}
diff ${DATFIL} ${TMPFIL}
if (( $? != 0 )); then
    echo "${PGM} TEST ${TEST} FAILED!" 1>&2
    exit ${TEST}
fi
rm -f ${TMPFIL}

# To test the serial device handling, you can try this interactive
# test, which blocks the process for a bit and lets characters build
# up in the hardware queue of the UART or the software buffer of the
# device driver. Change the '5' seconds to something bigger for more
# data.
#
# (sleep 5; filepoller)

echo "${PGM} SUCCESS." 1>&2

exit 0
