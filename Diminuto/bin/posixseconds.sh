#!/bin/bash
# Copyright 2023 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# ABSTRACT
#
# Converts the date and time into the number of seconds since the POSIX
# (UNIX) epoch.
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
# posixseconds [ YEAR [ MONTH [ DAY [ HOUR [ MINUTE [ SECOND.[FRACTION] [ -TIME:ZONE ] ] ] ] ] ] ]
#
# EXAMPLES
#
# posixseconds
#
# posixseconds 1970
#
# posixseconds 1980 01 06
#
# posixseconds 2023 08 10 11 16 00.999 -07:00
#

if [[ $# == 0 ]]; then
    set -- $(date +"%Y %m %d %H %M %S.%N %:z")
fi

# It's impossible for all of the positional parameters to be unset
# (see above), but this reminds me what the POSIX epoch is.

YY=${1:-"1970"}
MM=${2:-"01"}
DD=${3:-"01"}
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
