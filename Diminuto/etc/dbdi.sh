#! /bin/sh
# Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>

. `which diminuto`

BASENAME="`basename $0`"
GDBRC="${TMPDIR}/${BASENAME}.$$.gdbrc"
trap "rm -f ${GDBRC}" 1 2 3 15
echo "target remote ${BDIADDRESS}:${BDIPORT}" > ${GDBRC}

${CROSS_COMPILE}gdb -x ${GDBRC} ${KERNEL}/vmlinux
RC=$?

rm -f ${GDBRC}

exit $RC
