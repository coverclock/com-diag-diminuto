#! /bin/sh
# Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA
# Licensed under the GPL V2
# Chip Overclock <coverclock@diag.com>

. `which diminuto`

BASENAME="`basename $0`"
GDBRC="${TMPDIR}/${BASENAME}.$$.gdbrc"
trap "rm -f ${GDBRC}" 1 2 3 15

rm -f ${GDBRC}

for SO in `find ${TOOLCHAIN} -type f -name 'lib*.so*' -print`; do
	echo "sharedlibrary ${SO}" >> ${GDBRC}
done
echo "target remote ${TGTADDRESS}:${BDIPORT}" >> ${GDBRC}

${CROSS_COMPILE}gdb -x ${GDBRC} $*
RC=$?

rm -f ${GDBRC}

exit $RC
