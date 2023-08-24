#!/bin/bash
# Copyright 2023 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
# Test spartntime.

EXPECTED="2271 0"
ACTUAL=$(spartntime 2023 08 12)
if (( $? != 0 )); then
    exit 1
fi
if [[ "${ACTUAL}" != "${EXPECTED}" ]]; then
    echo ${ACTUAL} ${EXPECTED} 1>&2
    exit 2
fi

EXPECTED="2271 0"
ACTUAL=$(spartntime 2023 08 12 23 59 59.999999999 +00:00)
if (( $? != 0 )); then
    exit 1
fi
if [[ "${ACTUAL}" != "${EXPECTED}" ]]; then
    echo ${ACTUAL} ${EXPECTED} 1>&2
    exit 2
fi

EXPECTED="2275 0"
ACTUAL=$(spartntime 2023 09 09)
if (( $? != 0 )); then
    exit 1
fi
if [[ "${ACTUAL}" != "${EXPECTED}" ]]; then
    echo ${ACTUAL} ${EXPECTED} 1>&2
    exit 2
fi

echo "$(basename $0): PASSED." 1>&2
exit 0
