#!/bin/bash
# Copyright 2023 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
# Test IPv4 and IPv6 filtering regexes.

PGM=$(basename $0)
TMP=${TMPDIR:="/tmp"}
FIL=$(mktemp ${TMP}/${PGM}-XXXXXXXXXX)

trap "rm -f ${FIL}" 0 1 2 3 15

cat > ${FIL} << EOF
127.0.0.1
142.250.72.46
0.0.0.0
205.178.189.131
206.178.189.131
216.24.136.61
EOF

DAT=$(cat ${FIL} | egrep -v '[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*')

echo "${PGM}: IPv4=\"${DAT}\"" 1>&2

if [[ "${DAT}" != "0.0.0.0" ]]; then
	exit 1
fi

cat > ${FIL} << EOF
::1
2607:f8b0:400f:803::200e
::
2607:f8b0:400f:805::200e
::ffff:205.178.189.131
::ffff:209.17.116.163
::ffff:216.24.136.61
EOF

DAT=$(cat ${FIL} | egrep -v '([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*|::1)')

echo "${PGM}: IPv6=\"${DAT}\"" 1>&2

if [[ "${DAT}" != "::" ]]; then
	exit 1
fi

rm -f ${FIL}

echo "${PGM}: PASSED." 1>&2
exit 0
