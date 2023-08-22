#!/bin/bash
# Copyright 2023 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
# Test gpstime.

EXPECTED="0 0 0"
ACTUAL=$(gpstime 1980 01 06 00 00 00 +00:00)
if (( $? != 0 )); then
    exit 1
fi
if [[ "${ACTUAL}" != "${EXPECTED}" ]]; then
    echo ${ACTUAL} ${EXPECTED} 1>&2
    exit 2
fi

EXPECTED="0.000000000 0 0.000000000"
ACTUAL=$(gpstime 1980 01 06 00 00 00.000000000 +00:00)
if (( $? != 0 )); then
    exit 1
fi
if [[ "${ACTUAL}" != "${EXPECTED}" ]]; then
    echo ${ACTUAL} ${EXPECTED} 1>&2
    exit 2
fi

EXPECTED="0 0 0"
ACTUAL=$(gpstime 1980 01 06)
if (( $? != 0 )); then
    exit 1
fi
if [[ "${ACTUAL}" != "${EXPECTED}" ]]; then
    echo ${ACTUAL} ${EXPECTED} 1>&2
    exit 2
fi

EXPECTED="0.123456789 0 0.123456789"
ACTUAL=$(gpstime 1980 01 06 00 00 00.123456789 +00:00)
if (( $? != 0 )); then
    exit 1
fi
if [[ "${ACTUAL}" != "${EXPECTED}" ]]; then
    echo ${ACTUAL} ${EXPECTED} 1>&2
    exit 2
fi

EXPECTED="1376706930.551593964 2276 182130.551593964"
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

echo "$(basename $0): PASSED." 1>&2
exit 0
