#!/bin/bash
# Copyright 2023 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# ABSTRACT
#
# Converts the start and duration numbers from a Thingstream (U-blox)
# PointPerfect SPARTNKEY to GPS Week Number (WNO) and Time Of Week
# (TOW) that can be used for a UBX-RXM-SPARTNKEY message. SPARTN
# (Safe Position Augmentation for Real-Time Navigation) provides
# DGNSS-like corrections over L-band via Inmarsat satellites.
#
# USAGE
#
# spartntime [ YEAR [ MONTH [ DAY [ HOUR [ MINUTE [ SECOND [ TIME:ZONE ] ] ] ] ] ] ]
#
# EXAMPLES
#
# spartnkey
#
# spartnkey 2023 08 12
#
# spartnkey 2023 08 12 23 59
#
# spartnkey 2023 08 12 23 59 59
#
# spartnkey 2023 08 12 23 59 59 -00:00
#

PROGRAM=$(basename $0)

if (( $# == 0 )); then
    set -- $(date -u +"%Y %m %d %H %M %S %:z")
fi

Y0=${1}
M0=${2}
D0=${3}
H0=${4:-"23"}
N0=${5:-"59"}
S0=${6:-"59"}
Z0=${7:-"-00:00"}

S0=${S0%%.*}

POSIXTIME=$(posixtime ${Y0} ${M0} ${D0} ${H0} ${N0} ${S0} ${Z0})
if (( $? != 0 )); then
    echo "${PROGRAM}: ${Y0} ${M0} ${D0} ${H0} ${N0} ${S0} ${Z0} failed!" 1>&2
    exit 1
fi

OFFSEC=$(( 4 * 7 * 24 * 60 * 60 ))
EXPSEC=$(( ${POSIXTIME} + 1 - ${OFFSEC} ))

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
OFF=${4}

EFF=$(( ${TOW} - ${OFF} ))

echo "${PROGRAM}: ${Y0}-${M0}-${D0}T${H0}:${N0}:${S0} ${Y1}-${M1}-${D1}T${H1}:${N1}:${S1} ${SEC}sec ${WNO}wno ${TOW}tow ${OFF}off ${EFF}eff" 1>&2

echo ${WNO} ${EFF}
