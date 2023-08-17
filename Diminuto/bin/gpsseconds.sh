#!/bin/bash
# Copyright 2023 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
# WORK IN PROGRESS

LEPLST="
1981:06:30:-1
1982:06:30:-1
1983:06:30:-1
1985:06:30:-1
1987:12:30:-1
1989:12:30:-1
1990:12:30:-1
1992:06:30:-1
1993:06:30:-1
1994:06:30:-1
1995:12:30:-1
1997:06:30:-1
1998:12:30:-1
2005:12:30:-1
2008:12:30:-1
2012:06:30:-1
2015:06:30:-1
2016:12:30:-1
"

PGMNAM=$(basename $0)

if [[ $# == 0 ]]; then
    set -- $(date +"%Y %m %d %H %M %S %:z")
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

GPSEPO=$(posixseconds 1980 01 06)
if (( $? != 0 )); then
    echo "${PGMNAM}: 1980 01 06 failed!" 1>&2
    exit 1
fi

POSSEC=$(posixseconds ${YY} ${MM} ${DD} ${HH} ${NN} ${SS} ${TZ})
if (( $? != 0 )); then
    echo "${PGMNAM}: ${YY} ${MM} ${DD} ${HH} ${NN} ${SS} ${TZ} failed!" 1>&2
    exit 1
fi

if (( ${POSSEC} < ${GPSEPO} )); then
    echo "${PGMNAM}: (${POSSEC} >= ${GPSEPO}) failed!" 1>&2
    exit 1
fi

GPSSEC=$((${POSSEC} - ${GPSEPO}))

for LEPDAT in ${LEPLST}; do
    set -- $(echo ${LEPDAT} | tr ':' ' ')
    YEAR=${1}
    MONTH=${2}
    DAY=${3}
    DELTA=${4}
    LEPSEC=$(posixseconds ${YEAR} ${MONTH} ${DAY})
    if (( $? != 0 )); then
        echo "${PGMNAM}: ${YEAR} ${MONTH} ${DAY} failed!" 1>&2
        exit 1
    fi
    if (( ${POSSEC} >= ${LEPSEC} )); then
        GPSSEC=$((${GPSSEC} ${DELTA}))
    fi
done

GPSWNO=$((${GPSSEC} / 604800))
GPSTOW=$((${GPSSEC} % 604800))

echo sec ${GPSSEC} wno ${GPSWNO} tow ${GPSTOW}
