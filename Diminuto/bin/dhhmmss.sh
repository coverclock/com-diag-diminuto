#!/bin/bash
# Copyright 2019 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# ABSTRACT
#
# Convert an elapsed time duration in seconds into a day, hours
# minutes, seconds format.
#
# USAGE
#
# dhhmmss SECONDSELAPSED
#

ELAPSED=${1:-0}
DAYS=$(( ${ELAPSED} / 86400 ))
HOURS=$(( ${ELAPSED} % 86400 / 3600 ))
MINUTES=$(( ${ELAPSED} % 3600 / 60 ))
SECONDS=$(( ${ELAPSED} % 60 ))
printf "%d/%02d:%02d:%02d\n" ${DAYS} ${HOURS} ${MINUTES} ${SECONDS}
