#!/bin/bash
# Copyright 2023 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# ABSTRACT
#
# Given a time stamp in [ YYYY [ MM [ DD [ HH [ NN [ SS.SS [ TZ ] ] ] ] ] ] ]
# format, emits to standard output the number of seconds since the POSIX
# (UNIX) epoch 1970-01-01. Additional information is emitted to standard error.
# Does not account for DST. This can be adjusted for by the user by adjusting
# the time zone offset. If no argument is given, it uses the current date, time
# and time zone.
#
# USAGE
#
# posixtime [ YEAR [ MONTH [ DAY [ HOUR [ MINUTE [ SECOND.[FRACTION] [ -TIME:ZONE ] ] ] ] ] ] ]
#
# EXAMPLES
#
# posixtime
#
# posixtime 1970 01 01
#
# posixtime 1980 01 06
#
# posixtime 2023 08 10 11 16 00.999 -07:00
#

PGMNAM=$(basename $0)

if [[ $# == 0 ]]; then
    set -- $(date -u +"%Y %m %d %H %M %S.%N %:z")
fi

# It's impossible for all of the positional parameters to be unset
# (see above), but this reminds me what the POSIX epoch is.

YY=${1:-"1970"}
MM=${2:-"01"}
DD=${3:-"01"}
HH=${4:-"00"}
NN=${5:-"00"}
SS=${6:-"00"}
TZ=${7:-"-00:00"}

if [[ "${SS%%\.*}" == "${SS}" ]]; then
    FORMAT="%s"
else
    FORMAT="%s.%N"
fi

POSSEC=$(date -d "${YY}-${MM}-${DD} ${HH}:${NN}:${SS} ${TZ}" +"${FORMAT}")

echo ${PGMNAM}: ${YY}-${MM}-${DD}T${HH}:${NN}:${SS}${TZ} ${POSSEC}sec $(iso8601 ${POSSEC}) 1>&2

echo ${POSSEC}
