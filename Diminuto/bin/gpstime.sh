#!/bin/bash
# Copyright 2023 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
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

LEPLST="
1981:06:30:+1
1982:06:30:+1
1983:06:30:+1
1985:06:30:+1
1987:12:30:+1
1989:12:30:+1
1990:12:30:+1
1992:06:30:+1
1993:06:30:+1
1994:06:30:+1
1995:12:30:+1
1997:06:30:+1
1998:12:30:+1
2005:12:30:+1
2008:12:30:+1
2012:06:30:+1
2015:06:30:+1
2016:12:30:+1
"

PGMNAM=$(basename $0)

if [[ $# == 0 ]]; then
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
    LEPSEC=$(posixtime ${YEAR} ${MONTH} ${DAY})
    if (( $? != 0 )); then
        echo "${PGMNAM}: ${YEAR} ${MONTH} ${DAY} failed!" 1>&2
        exit 1
    fi
    if (( ${POSSEC} < ${LEPSEC} )); then
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

echo ${PGMNAM}: ${YY}-${MM}-${DD}T${HH}:${NN}:${SS}${FS}${TZ} ${GPSEPO}epo ${OFFSEC}off ${GPSSEC}${FS}gps $(iso8601 ${EFFSEC}) ${GPSWNO}wno ${GPSTOW}${FS}tow $(dhhmmss ${GPSTOW}) 1>&2

echo ${GPSSEC}${FS} ${GPSWNO} ${GPSTOW}${FS}
