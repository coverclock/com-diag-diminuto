#!/bin/bash
# Copyright 2019-2020 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# ABSTRACT
#
# Uses the inotify tool to watch for the indicated file to show
# up, then emits its canonical path name to standard outout. Also
# watches for a hangup (HUP) signal (SIGHUP) and when it sees it,
# creates a timestamped copy of the next indicated file in the
# same directory. Used to implement applications that run
# headless.
#
# USAGE
#
# headless OUTFILE [ PIDFILE ]
#
# EXAMPLE
#
# headless out/host/tmp/base.out
#
# DEPENDENCIES
#
# sudo apt-get install inotify-tools
#

PROGRAM=$(basename ${0})
HEADLESS=${1:-"/dev/null"}
PIDFIL=${2:-"./${PROGRAM}.pid"}

CANONICAL=$(readlink -f ${HEADLESS})
DIRECTORY=$(dirname ${CANONICAL})
FILE=$(basename ${CANONICAL})
TARGET="${DIRECTORY}/ MOVED_TO ${FILE}"
CHECKPOINT=N
SELF=$$

test -n "$(which inotifywait)" || exit 2
test -d ${DIRECTORY} || exit 2

echo ${SELF} > ${PIDFIL}
trap "CHECKPOINT=Y" SIGHUP
trap "rm -f ${PIDFIL}" EXIT

while MOVED=$(inotifywait -e moved_to ${DIRECTORY} 2> /dev/null); do
    if [[ "${MOVED}" == "${TARGET}" ]]; then
        if [[ "${CHECKPOINT}" == "Y" ]]; then
            TEMPORARY=$(mktemp ${CANONICAL}-XXXXXXXXXX)
            cp ${CANONICAL} ${TEMPORARY}
            mv ${TEMPORARY} ${CANONICAL}-$(date -u '+%Y%m%dT%H%M%SZ%N')
            CHECKPOINT=N
        fi
        echo ${CANONICAL}
    fi
done

exit 1
