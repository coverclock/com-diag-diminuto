#!/bin/bash
# Copyright 2023 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# ABSTRACT
#
# Convert a time stamp into a year, day of the year, and seconds from the start
# of the day. Jilian days are one-based, so the first day of a year is 1.
# Note that this isn't literally using the Julian calendar.
#
# USAGE
#
# juliantime [ YEAR [ MONTH [ DAY [ HOUR [ MINUTE [ SECOND.[FRACTION] [ -TIME:ZONE ] ] ] ] ] ] ]
#
# EXAMPLES
#
# juliantime
#
# juliantime 2023
#
# juliantime 2023 08 24
#
# juliantime 2023 08 24 14 35 59
#
# juliantime 2023 08 24 14 35 59.123456789
#
# juliantime 2023 08 24 14 35 59.123456789 -00:00
#

PGMNAM=$(basename $0)

if (( $# == 0 )); then
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

FS=${SS#*.}
if [[ "${FS}" == "${SS}" ]]; then
    FS=""
else
    FS=".${FS}"
    SS=${SS%%.*}
fi

POSSEC=$(posixtime ${YY} ${MM} ${DD} ${HH} ${NN} ${SS} ${TZ})
if (( $? != 0 )); then
    echo "${PGMNAM}: ${YY} ${MM} ${DD} ${HH} ${NN} ${SS} ${TZ} failed!" 1>&2
    exit 1
fi

ANNSEC=$(posixtime ${YY} 01 01 00 00 00 -00:00)
if (( $? != 0 )); then
    echo "${PGMNAM}: ${YY} 01 01 00 00 00 -00:00 failed!" 1>&2
    exit 1
fi

OFFSEC=$(( ${POSSEC} - ${ANNSEC} ))
DAYSEC=$(( 24 * 60 * 60 ))
JULOFF=$(( ${OFFSEC} / ${DAYSEC} ))
JULDAY=$(( ${JULOFF} + 1 ))
JULSEC=$(( ${OFFSEC} % ${DAYSEC} ))

echo ${PGMNAM}: ${YY}-${MM}-${DD}T${HH}:${NN}:${SS}${FS}${TZ} ${YY}year ${JULDAY}day ${JULSEC}${FS}sec 1>&2

echo ${YY} ${JULDAY} ${JULSEC}${FS}
