#!/bin/bash
# Copyright 2023 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# ABSTRACT
#
# Given a time stamp in [ YYYY [ MM [ DD [ HH [ NN [ SS.SS [ TZ ] ] ] ] ] ] ]
# format, emits to standard output [1] the number of seconds since the GPS
# epoch 1980-01-06, [2] the GPS Week Number (WNO), [3] the GPS Time Of
# Week (TOW) which is the number of seconds since the start of the current
# GPS week (which begins on Sunday), and [4] the number of seconds offset
# from UTC.  Additional information is emitted to standard error. If no
# argument is given, it uses the current date, time and time zone.
#
# USAGE
#
# gpstime [ YEAR [ MONTH [ DAY [ HOUR [ MINUTE [ SECOND.[FRACTION] [ -TIME:ZONE ] ] ] ] ] ] ]
#
# EXAMPLES
#
# gpstime
#
# gpstime 1980 01 06
#
# gpstime 2023 08 21 10 02 00 +06:00
#
# REFERENCES
#
# <https://en.wikipedia.org/wiki/Leap_second>
#
# <https://www.ietf.org/timezones/data/leap-seconds.list>
#
# <https://geodesy.noaa.gov/CORS/resources/gpscals.shtml>
#
# <http://leapsecond.com/java/gpsclock.htm>
#
# NOTE
#
# The list of leap seconds is hard coded and must be updated when a new leap
# second is added.

PGMNAM=$(basename $0)
LEPFIL="$(readlink -e $(dirname ${0}))/../bin/${PGMNAM%.sh}-list"

if [ -r ${LEPFIL} ]; then
    . ${LEPFIL}
elif [ -r ${LEPFIL}.sh ]; then
    . ${LEPFIL}.sh
else
    echo "${PGMNAM}: ${LEPFIL} failed!" 1>&2
    exit 1
fi

if (( $# == 0 )); then
    set -- $(date -u +"%Y %m %d %H %M %S.%N %:z")
fi

# It's impossible for all of the positional parameters to be unset
# (see above), but this reminds me what the GPS epoch is.

YY=${1:-"1980"}
MM=${2:-"01"}
DD=${3:-"06"}
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

# GPS Time epoch is 1980-01-06 00:00:00 +00:00 which is the first Sunday
# at the start of the first GPS week.

GPSEPO=$(posixtime 1980 01 06)
if (( $? != 0 )); then
    echo "${PGMNAM}: 1980 01 06 failed!" 1>&2
    exit 1
fi

if (( ${POSSEC} < ${GPSEPO} )); then
    echo "${PGMNAM}: (${POSSEC} >= ${GPSEPO}) failed!" 1>&2
    exit 1
fi

# GPS Time does not include the leap seconds that have occurred since
# its epoch (but it does include the ones that occurred before its epoch).
# This is a little counterintuitive, at least for me. It means GPS counts
# positive (added) leap seconds as regular seconds, putting GPS ahead of UTC.
# E.g. where UTC at the leap second would be 00:00:60, GPS is 00:01:00.

OFFSEC=0

for LEPDAT in ${LEPLST}; do
    set -- $(echo ${LEPDAT} | tr ':' ' ')
    YEAR=${1}
    MONTH=${2}
    DAY=${3}
    DELTA=${4}
    LEPSEC=$(posixtime ${YEAR} ${MONTH} ${DAY} 23 59 59)
    if (( $? != 0 )); then
        echo "${PGMNAM}: ${YEAR} ${MONTH} ${DAY} 23 59 59 failed!" 1>&2
        exit 1
    fi
    if (( ${POSSEC} <= ${LEPSEC} )); then
	break
    fi
    OFFSEC=$((${OFFSEC} + ${DELTA}))
done

EFFSEC=$((${POSSEC} + ${OFFSEC}))
GPSSEC=$((${EFFSEC} - ${GPSEPO}))

# GPS Time is kept as a week number (WNO) since its epoch, and the number
# of seconds that have elapsed since the start of current week, which is the
# time of week (TOW).

GPSWNO=$((${GPSSEC} / 604800))
GPSTOW=$((${GPSSEC} % 604800))

echo ${PGMNAM}: ${YY}-${MM}-${DD}T${HH}:${NN}:${SS}${FS}${TZ} ${GPSEPO}epo ${OFFSEC}off ${GPSSEC}${FS}gps $(iso8601 ${EFFSEC}${FS}) ${GPSWNO}wno ${GPSTOW}${FS}tow $(dhhmmss ${GPSTOW})${FS} 1>&2

echo ${GPSSEC}${FS} ${GPSWNO} ${GPSTOW}${FS} ${OFFSEC}
