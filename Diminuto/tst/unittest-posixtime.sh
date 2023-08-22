#!/bin/bash
# Copyright 2023 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
# Test posixtime.

EXPECTED="0"
ACTUAL=$(posixtime 1970 01 01 00 00 00 +00:00)
if (( $? != 0 )); then
    exit 1
fi
if [[ "${ACTUAL}" != "${EXPECTED}" ]]; then
    echo ${ACTUAL} ${EXPECTED} 1>&2
    exit 2
fi

EXPECTED="0.000000000"
ACTUAL=$(posixtime 1970 01 01 00 00 00.000000000 +00:00)
if (( $? != 0 )); then
    exit 1
fi
if [[ "${ACTUAL}" != "${EXPECTED}" ]]; then
    echo ${ACTUAL} ${EXPECTED} 1>&2
    exit 2
fi

EXPECTED="0"
ACTUAL=$(posixtime 1970)
if (( $? != 0 )); then
    exit 1
fi
if [[ "${ACTUAL}" != "${EXPECTED}" ]]; then
    echo ${ACTUAL} ${EXPECTED} 1>&2
    exit 2
fi

EXPECTED="0.123456789"
ACTUAL=$(posixtime 1970 01 01 00 00 00.123456789 +00:00)
if (( $? != 0 )); then
    exit 1
fi
if [[ "${ACTUAL}" != "${EXPECTED}" ]]; then
    echo ${ACTUAL} ${EXPECTED} 1>&2
    exit 2
fi

EXPECTED="1692671712.551593964"
ACTUAL=$(posixtime 2023 08 22 08 35 12.551593964 +06:00)
if (( $? != 0 )); then
    exit 1
fi
if [[ "${ACTUAL}" != "${EXPECTED}" ]]; then
    echo ${ACTUAL} ${EXPECTED} 1>&2
    exit 2
fi

#
# This is what the date command returns, but I don't think this is correct.
# Shouldn't it be "-0.000000001"?
#
EXPECTED="-1.999999999"
ACTUAL=$(posixtime 1969 12 31 23 59 59.999999999 +00:00)
if (( $? != 0 )); then
    exit 1
fi
if [[ "${ACTUAL}" != "${EXPECTED}" ]]; then
    echo ${ACTUAL} ${EXPECTED} 1>&2
    exit 2
fi

echo "$(basename $0): PASSED." 1>&2
exit 0
