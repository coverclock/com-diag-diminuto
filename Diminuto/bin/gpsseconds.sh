#!/bin/bash
# Copyright 2023 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
# WORK IN PROGRESS

PGMNAM=$(basename $0)
LSLDIR=${TMPDIR:-"/tmp"}
LSLFIL="${LSLDIR}/${PGMNAM}.txt"
LSLURL="https://www.ietf.org/timezones/data/leap-seconds.list"

wget -O - ${LSLURL} | sed -e '/^#/d' -e 's/[ ]*#.*$//' > ${LSLFIL}

exit 0

if [[ $# == 0 ]]; then
    set -- $(date +"%Y %m %d %H %M %S.%N %:z")
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

if [[ "${SS%%\.*}" == "${SS}" ]]; then
    FORMAT="%s"
else
    FORMAT="%s.%N"
fi

exec date -d "${YY}-${MM}-${DD} ${HH}:${NN}:${SS} ${TZ}" +"${FORMAT}"
