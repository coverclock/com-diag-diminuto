#!/bin/bash -x
# Copyright 2023 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
# Converts the start and duration numbers from a Thingstream
# PointPerfect SPARTNKEY to GPS Week Number (WNO) and Time Of Week
# (TOW) that can be used for a UBX-RXM-SPARTNKEY message.

PROGRAM=$(basename $0)
YY=${1}
MM=${2}
DD=${3}
HH=${4}
NN=${5}
SS="00"

DURSECONDS=$((60 * 60 * 24 * 28))
WEKSECONDS=$((60 * 60 * 24 * 7))
GPSSECONDS=$(epochseconds 1980 01 06)
EXPSECONDS=$(epochseconds ${YY} ${MM} ${DD} ${HH} ${NN} ${SS})
BGNSECONDS=$((${EXPSECONDS} - ${DURSECONDS} + 1))
WNOSECONDS=$((${BGNSECONDS} - ${GPSSECONDS}))

WNO=$((${WNOSECONDS} / ${WEKSECONDS}))
TOW=$((${WNOSECONDS} % ${WEKSECONDS}))

echo ${PROGRAM}:  WNO ${WNO} TOW ${TOW}
