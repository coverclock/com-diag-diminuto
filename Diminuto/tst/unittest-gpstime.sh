#!/bin/bash
# Copyright 2023 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
# Test gpstime.

LEPFIL="$(readlink -e $(dirname ${0}))/../bin/gpstime-list"

if [ -r ${LEPFIL} ]; then
    . ${LEPFIL}
elif [ -r ${LEPFIL}.sh ]; then
    . ${LEPFIL}.sh
else
    echo "${PGMNAM}: ${LEPFIL} failed!" 1>&2
    exit 1
fi

EXPECTED="0 0 0 0"
ACTUAL=$(gpstime 1980 01 06 00 00 00 +00:00)
if (( $? != 0 )); then
    exit 1
fi
if [[ "${ACTUAL}" != "${EXPECTED}" ]]; then
    echo ${ACTUAL} ${EXPECTED} 1>&2
    exit 2
fi

EXPECTED="0.000000000 0 0.000000000 0"
ACTUAL=$(gpstime 1980 01 06 00 00 00.000000000 +00:00)
if (( $? != 0 )); then
    exit 1
fi
if [[ "${ACTUAL}" != "${EXPECTED}" ]]; then
    echo ${ACTUAL} ${EXPECTED} 1>&2
    exit 2
fi

EXPECTED="0 0 0 0"
ACTUAL=$(gpstime 1980 01 06)
if (( $? != 0 )); then
    exit 1
fi
if [[ "${ACTUAL}" != "${EXPECTED}" ]]; then
    echo ${ACTUAL} ${EXPECTED} 1>&2
    exit 2
fi

EXPECTED="0.123456789 0 0.123456789 0"
ACTUAL=$(gpstime 1980 01 06 00 00 00.123456789 +00:00)
if (( $? != 0 )); then
    exit 1
fi
if [[ "${ACTUAL}" != "${EXPECTED}" ]]; then
    echo ${ACTUAL} ${EXPECTED} 1>&2
    exit 2
fi

EXPECTED="1376706930.551593964 2276 182130.551593964 18"
ACTUAL=$(gpstime 2023 08 22 08 35 12.551593964 +06:00)
if (( $? != 0 )); then
    exit 1
fi
if [[ "${ACTUAL}" != "${EXPECTED}" ]]; then
    echo ${ACTUAL} ${EXPECTED} 1>&2
    exit 2
fi

ACTUAL=$(gpstime 1979 01 05 23 59 59.999999999 +00:00)
if (( $? == 0 )); then
    exit 1
fi

DELTA=0
for LEPDAT in ${LEPLST}; do
    set -- $(echo ${LEPDAT} | tr ':' ' ')
    Y0=${1}
    M0=${2}
    D0=${3}
    L0=${4}
    Y1=${5}
    M1=${6}
    D1=${7}
    L1=${8}
    GA=$(gpstime ${Y0} ${M0} ${D0} 23 59 58)
    if (( $? != 0 )); then
        echo "${Y0} ${M0} ${D0} 23 59 58 failed!" 1>&2
        exit 1
    fi
    set -- ${GA}
    SA=${1}
    DA=${4}
    GB=$(gpstime ${Y0} ${M0} ${D0} 23 59 59)
    if (( $? != 0 )); then
        echo "${Y1} ${M1} ${D1} 23 59 59 failed!" 1>&2
        exit 1
    fi
    set -- ${GB}
    SB=${1}
    DB=${4}
    GC=$(gpstime ${Y1} ${M1} ${D1} 00 00 00)
    if (( $? != 0 )); then
        echo "${Y1} ${M1} ${D1} 23 59 59 failed!" 1>&2
        exit 1
    fi
    set -- ${GC}
    SC=${1}
    DC=${4}
    AB=$(( ${SB} - ${SA} ))
    if (( ${AB} != 1 )); then
        echo "23:59:58..23:59:59 failed!" 1>&2
	exit 2
    fi
    BC=$(( ${SC} - ${SB} ))
    if (( ${BC} != 2 )); then
        echo "23:59:59..00:00:00 failed!" 1>&2
	exit 2
    fi
    if (( ${DA} != ${DELTA} )); then
        echo "${DA} ${DELTA} failed!" 1>&2
	exit 2
    fi
    if (( ${DB} != ${DELTA} )); then
        echo "${DB} ${DELTA} failed!" 1>&2
	exit 2
    fi
    DELTA=$(( ${DELTA} + 1 ))
    if (( ${DC} != ${DELTA} )); then
        echo "${DC} ${DELTA} failed!" 1>&2
	exit 2
    fi
done

echo "$(basename $0): PASSED." 1>&2
exit 0
