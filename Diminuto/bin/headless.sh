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
# a terminal emulator) should have no effect on the gpstool instance running
# in the background.
#
# The optional LIMIT parameter is to deal with the vagaries of various
# terminal emulators; I'm lookin' at you, Chromebook's Beagle Term.
#
# This script traps SIGHUP and creates a timestamped copy of the next
# headless file being watched in the same directory. A HUP (hangup)
# signal can be easily sent from the bash command line by suspending the
# script (
#
# DEPENDENCIES
#
# sudo apt-get install inotify-tools
#

PROGRAM=$(basename ${0})
HEADLESS=${1:-"/dev/null"}
LIMIT=${2:-$(($(stty size | cut -d ' ' -f 1) - 2))}

CANONICAL=$(readlink -f ${HEADLESS})
DIRECTORY=$(dirname ${CANONICAL})
FILE=$(basename ${CANONICAL})
TARGET="${DIRECTORY}/ MOVED_TO ${FILE}"
CHECKPOINT=N

test -d ${DIRECTORY} || exit 1

trap "CHECKPOINT=Y" SIGHUP

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
    awk '
      begin   { inp="INP [   ]"; out="OUT [   ]"; arm=1; }
      /^INP / { inp=substr($0,0,79); arm=1; next; }
      /^OUT / { out=substr($0,0,79); arm=1; next; }
              { if (arm!=0) { print inp; print out; arm=0; } print $0; next; }
      end     { if (arm!=0) { print inp; print out; arm=0; } }
    ' ${CANONICAL} | head -n ${LIMIT}
  fi
done

exit 0
