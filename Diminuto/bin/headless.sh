#!/bin/bash
# Copyright 2019 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# ABSTRACT
#
# Uses the inotify tools to watch a log file produced by something
# like Hazer's  gpstool running in "headless" (-H FILE) mode in which
# the latest full screen update is written to a file using the Diminuto
# Observation feature. See the Hazer/bin/base.sh and Hazer/bin/rover.sh
# for examples of this. So you might use the command
#
# headless out/host/log/base
#
# to watch the full screen updates performed by the base script. Multiple
# headless instantiations for the same output file can be run at one time.
# Exiting the script directly or indirectly (e.g. by logging off or exiting
# a terminal emulator) should have no effect on the application instance
# running in the background.
#
# This script traps SIGHUP and creates a timestamped copy of the next
# headless file being watched in the same directory. A HUP (hangup)
# signal can be easily sent from the bash command line.
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

clear
while MOVED=$(inotifywait -e moved_to ${DIRECTORY} 2> /dev/null); do
  if [[ "${MOVED}" == "${TARGET}" ]]; then
    if [[ "${CHECKPOINT}" == "Y" ]]; then
	TEMPORARY=$(mktemp ${CANONICAL}-XXXXXXXXXX)
	cp ${CANONICAL} ${TEMPORARY}
	mv ${TEMPORARY} ${CANONICAL}-$(date -u '+%Y%m%dT%H%M%SZ')
	CHECKPOINT=N
    fi
    clear
    stdbuf -o0 cat ${CANONICAL}
  fi
done

exit 1
