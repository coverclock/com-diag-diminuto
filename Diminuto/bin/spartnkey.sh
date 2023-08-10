#!/bin/bash -x
# Copyright 2023 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
# Converts the start and duration numbers from a Thingstream
# PointPerfect SPARTNKEY to GPS Week Number (WNO) and Time Of Week
# (TOW).

PROGRAM=$(basename $0)
YY=${1}
MM=${2}
DD=${3}
HH=${4}
NN=${5}
SS="00"

DURSECONDS=2419200
GPSSECONDS=$(epochseconds 1980 01 06 00 00 00)
EXPSECONDS=$(epochseconds ${YY} ${MM} ${DD} ${HH} ${NN} ${SS})
BGNSECONDS=$((${EXPSECONDS} + 1 - ${DURSECONDS}))
WNOSECONDS=$((${BGNSECONDS} - ${GPSSECONDS}))
WEKSECONDS=$((60 * 60 * 24 * 7))
WNO=$((${WNOSECONDS} / ${WEKSECONDS}))
TOW=$((${WNOSECONDS} % ${WEKSECONDS}))

echo WNO ${WNO} TOW ${TOW}
