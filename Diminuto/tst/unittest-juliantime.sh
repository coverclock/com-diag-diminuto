#!/bin/bash
# Copyright 2023 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
# Test juliantime.

EXPECTED="2023 1 0"
ACTUAL=$(juliantime 2023)
if (( $? != 0 )); then
    exit 1
fi
if [[ "${ACTUAL}" != "${EXPECTED}" ]]; then
    echo ${ACTUAL} ${EXPECTED} 1>&2
    exit 2
fi

EXPECTED="2023 236 0"
ACTUAL=$(juliantime 2023 08 24)
if (( $? != 0 )); then
    exit 1
fi
if [[ "${ACTUAL}" != "${EXPECTED}" ]]; then
    echo ${ACTUAL} ${EXPECTED} 1>&2
    exit 2
fi

EXPECTED="2023 236 52220"
ACTUAL=$(juliantime 2023 08 24 14 30 20)
if (( $? != 0 )); then
    exit 1
fi
if [[ "${ACTUAL}" != "${EXPECTED}" ]]; then
    echo ${ACTUAL} ${EXPECTED} 1>&2
    exit 2
fi

EXPECTED="2023 236 52220.123456789"
ACTUAL=$(juliantime 2023 08 24 14 30 20.123456789)
if (( $? != 0 )); then
    exit 1
fi
if [[ "${ACTUAL}" != "${EXPECTED}" ]]; then
    echo ${ACTUAL} ${EXPECTED} 1>&2
    exit 2
fi

EXPECTED="2023 236 52220.123456789"
ACTUAL=$(juliantime 2023 08 24 14 30 20.123456789 -00:00)
if (( $? != 0 )); then
    exit 1
fi
if [[ "${ACTUAL}" != "${EXPECTED}" ]]; then
    echo ${ACTUAL} ${EXPECTED} 1>&2
    exit 2
fi

echo "$(basename $0): PASSED." 1>&2
exit 0

