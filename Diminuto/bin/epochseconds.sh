#!/bin/bash
# Copyright 2023 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# ABSTRACT
#
# Converts the date and time into the number of seconds since the UNIX epoch.
#
# NOTE
#
# Does not handle DST. This can be adjusted for by the user by adjusting the
# time zone offset.
#
# SEE ALSO
#
# iso8601
#
# USAGE
#
# epochseconds [ YEAR [ MONTH [ DAY [ HOUR [ MINUTE [ SECOND.[FRACTION] [ TZHOURS [ TZMINUTES ] ] ] ] ] ] ] ]
#
# EXAMPLES
#
# epochseconds
#
# epochseconds 1980 01 06
#
# epochseconds 2023 08 10 11 16 00.999 07
#

YY=${1:-"1970"}
MM=${2:-"01"}
DD=${3:-"01"}
HH=${4:-"00"}
NN=${5:-"00"}
SS=${6:-"00"}
TH=${7:-"00"}
TM=${8:-"00"}

if [[ "${SS%%\.*}" == "${SS}" ]]; then
    FORMAT="%s"
else
    FORMAT="%s.%N"
fi

# date -u -d "1980-01-06 00:00:00.1234 -00:00" +"%s.%N"

exec date -d "${YY}-${MM}-${DD} ${HH}:${NN}:${SS} -${TH}:${TM}" +"${FORMAT}"
