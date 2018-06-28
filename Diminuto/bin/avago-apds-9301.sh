#!/bin/bash
# Copyright 2018 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto

PGM=$(basename $0 .sh)
BUS=1
DEV=0x39
LST="0 1 2 3 4 5 6 8 A C D E F"

for REG in ${LST}; do
	ADR="0x8${REG}"
	i2cset -y ${BUS} ${DEV} ${ADR}
	VAL=$(i2cget -y 1 ${DEV})
	echo ${PGM}: ${BUS} ${DEV} ${ADR} ${VAL}
done

exit 0
