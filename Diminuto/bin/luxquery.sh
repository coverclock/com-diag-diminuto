#!/bin/bash
# Copyright 2018 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto

# Simple script that uses the platform's I2C tools to play
# with the Avago APDS 9301 lux sensor chip that is part of
# my breadboarded HW test fixture.

PGM=$(basename $0 .sh)
BUS=${1:-1}
DEV=${2:-0x39}
PIN=${3:-26}

# Setup GPIO.

pintool -p ${PIN} -n 2> /dev/null
pintool -p ${PIN} -x -i -L -R
trap "pintool -p ${PIN} -n 2> /dev/null; echo ${PGM}: exit;  exit 0" 1 2 3 15

# Scan.

NUM=${DEV:2:2}
i2cdetect -y ${BUS}
i2cdetect -y ${BUS} | grep -q " ${NUM} " && MSG="OKAY" || MSG="FAIL"
echo ${PGM}: ${BUS} ${DEV} ${NUM} ${MSG}

# Power cycle and verify.

ADR=0x80
i2cset -y ${BUS} ${DEV} ${ADR}
DAT=0x00
i2cset -y ${BUS} ${DEV} ${DAT}
echo ${PGM}: ${BUS} ${DEV} ${ADR} ${DAT}

ADR=0x80
i2cset -y ${BUS} ${DEV} ${ADR}
DAT=0x03
i2cset -y ${BUS} ${DEV} ${DAT}
echo ${PGM}: ${BUS} ${DEV} ${ADR} ${DAT}
VAL=$(i2cget -y ${BUS} ${DEV})
[[ "${DAT}" = "${VAL}" ]] && MSG="OKAY" || MSG="FAIL"
echo ${PGM}: ${BUS} ${DEV} ${ADR} "...." ${VAL} ${MSG}

# Configure.

ADR=0x81
i2cset -y ${BUS} ${DEV} ${ADR}
DAT=0x02
i2cset -y ${BUS} ${DEV} ${DAT}
echo ${PGM}: ${BUS} ${DEV} ${ADR} ${DAT}

ADR=0x86
i2cset -y ${BUS} ${DEV} ${ADR}
DAT=0x10
i2cset -y ${BUS} ${DEV} ${DAT}
echo ${PGM}: ${BUS} ${DEV} ${ADR} ${DAT}

# Interrogate all control registers.

LST="0 1 2 3 4 5 6 8 A"
for REG in ${LST}; do
	ADR="0x8${REG}"
	i2cset -y ${BUS} ${DEV} ${ADR}
	VAL=$(i2cget -y ${BUS} ${DEV})
	echo ${PGM}: ${BUS} ${DEV} ${ADR} "...." ${VAL}
done

# Start.

TM0=$(usectime)
pintool -p ${PIN} -M | while read BIT; do
	if [[ ${BIT} -ne 0 ]]; then
		ADR=0xCC
		i2cset -y ${BUS} ${DEV} ${ADR}
		V0L=$(i2cget -y ${BUS} ${DEV})
		N0L=${V0L:2:2}
		ADR=0x8D
		i2cset -y ${BUS} ${DEV} ${ADR}
		V0H=$(i2cget -y ${BUS} ${DEV})
		N0H=${V0H:2:2}
		ADR=0x8E
		i2cset -y ${BUS} ${DEV} ${ADR}
		V1L=$(i2cget -y ${BUS} ${DEV})
		N1L=${V1L:2:2}
		ADR=0x8F
		i2cset -y ${BUS} ${DEV} ${ADR}
		V1H=$(i2cget -y ${BUS} ${DEV})
		N1H=${V1H:2:2}
		TM1=$(usectime)
		TMD=$((${TM1} - ${TM0}))
		TM0=${TM1}
		echo ${PGM}: ${PIN} ${BIT} ${BUS} ${DEV} ${ADR} .... 0x${N0H}${N0L} 0x${N1H}${N1L} ${TMD}us
	fi
done

# Error.

exit 1
