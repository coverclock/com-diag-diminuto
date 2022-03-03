#!/bin/bash
# Copyright 2020 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# ABSTRACT
#
# Tests the header guards by generating a C file that includes all of them
# twice and then compiles it.
#
# USAGE
#
# unittest-guards
#

ARCDIR=$(readlink -e $(dirname ${0})/../arc)
INCDIR=$(readlink -e $(dirname ${0})/../../../inc)
SRCDIR=$(readlink -e $(dirname ${0})/../../../src)
GENDIR=$(readlink -e $(dirname ${0})/../inc)
SRCFIL=$(mktemp ${TMPDIR:-"/tmp"}/$(basename ${0} .sh)-XXXXXXXXXX.c)
OUTFIL=$(mktemp ${TMPDIR:-"/tmp"}/$(basename ${0} .sh)-XXXXXXXXXX.out)
trap "rm -f ${SRCFIL} ${OUTFIL}" 0 1 2 3 15
find ${INCDIR} ${SRCDIR} ${GENDIR} -type f -name '*.h' -print | awk '
	{ print "#include \""$1"\""; }
	{ print "#include \""$1"\""; }
	' > ${SRCFIL}
cat << EOF >> ${SRCFIL}
int main(void) { TEST(); STATUS(); EXIT(); }
EOF
cat -n ${SRCFIL}
gcc -I ${INCDIR} -I ${GENDIR} -D__USE_GNU -D_GNU_SOURCE -o ${OUTFIL} ${SRCFIL} ${ARCDIR}/libdiminuto.a -lpthread -lrt -ldl  || exit 1
${OUTFIL}
