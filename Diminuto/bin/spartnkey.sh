#!/bin/bash
# Copyright 2023 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
# Converts the start and duration numbers from a Thingstream
# PointPerfect SPARTNKEY to GPS Week Number (WNO) and Time Of Week
# (TOW) that can be used for a UBX-RXM-SPARTNKEY message.
# WORK IN PROGRESS

PROGRAM=$(basename $0)

if (( $# == 0 )); then
    set -- $(date -u +"%Y %m %d %H %M %S")
fi

Y0=${1}
M0=${2}
D0=${3}
H0=${4}
N0=${5}
S0=${6}

POSIXTIME=$(posixtime ${Y0} ${M0} ${D0} ${H0} ${N0} ${S0})
if (( $? != 0 )); then
    echo "${PROGRAM}: ${Y0} ${M0} ${D0} ${H0} ${N0} ${S0} failed!" 1>&2
    exit 1
fi

OFFSEC=$((4 * 7 * 24 * 60 * 60))
EXPSEC=$((${POSIXTIME} + 1 - ${OFFSEC}))

EXPDAT=$(date -d "@${EXPSEC}" -u "+%Y %m %d %H %M %S")
if (( $? != 0 )); then
    echo "${PROGRAM}: ${EXPSEC} failed!" 1>&2
    exit 1
fi

set -- ${EXPDAT}
Y1=${1}
M1=${2}
D1=${3}
H1=${4}
N1=${5}
S1=${6}

GPSTIME=$(gpstime ${Y1} ${M1} ${D1} ${H1} ${N1} ${S1})
if (( $? != 0 )); then
    echo "${PROGRAM}: ${Y1} ${M1} ${D1} ${H1} ${N1} ${S1} failed!" 1>&2
    exit 1
fi

set -- ${GPSTIME}
SEC=${1}
WNO=${2}
TOW=${3}

echo "${PROGRAM}: ${Y0}-${M0}-${D0}T${H0}:${N0}:${S0} ${Y1}-${M1}-${D1}T${H1}:${N1}:${S1} ${SEC}sec ${WNO}wno ${TOW}tow" 1>&2

echo ${WNO} ${TOW}
