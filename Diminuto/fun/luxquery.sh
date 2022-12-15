#!/bin/bash
# Copyright 2018-2022 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# ABSTRACT
#
# Simple script that uses the platform's I2C tools to play
# with the Avago APDS 9301 lux sensor chip that is part of
# my breadboarded HW test fixture.
#
# luxquery [ I2CBUS [ I2CDEVICE [ GPIOPIN ] ] ]
#
# SEE ALSO
#
# hardware_test_fixture.sh
#
# REFERENCES
#
# Avago, "APDS-9301 Miniature Ambient Light Photo Sensor with Digital
# (I2C) Output", Avago Technologies, AV02-2315EN, 2010-01-07
#

. $(readlink -e $(dirname ${0})/../fun)/hardware_test_fixture

PGM=$(basename ${0} .sh)
BUS=${1:-${HARDWARE_TEST_FIXTURE_BUS_I2C}}
DEV=${2:-${HARDWARE_TEST_FIXTURE_DEV_I2C_LUX}}
PIN=${3:-${HARDWARE_TEST_FIXTURE_PIN_INT_LUX}}

TMP=${TMPDIR:="/tmp"}
FIL=$(mktemp ${TMP}/${PGM}-XXXXXXXXXX)

trap "pintool -p ${PIN} -n 2> /dev/null; rm -f ${FIL}; echo ${PGM}: exit; exit 0" 1 2 3 15

# Setup GPIO.

pintool -p ${PIN} -n 2> /dev/null
pintool -p ${PIN} -x -u 1000000 -i -L -R

# Scan.

NAM="i2c-${BUS}"
i2cdetect -l | tee ${FIL} | grep -q "^${NAM}[ 	]" && MSG="OKAY" || MSG="FAIL"
echo ${PGM}: ${BUS} ${NAM} ${MSG}
cat ${FIL}

NUM=${DEV:2:2}
i2cdetect -y ${BUS} | tee ${FIL} | grep -q " ${NUM} " && MSG="OKAY" || MSG="FAIL"
echo ${PGM}: ${BUS} ${DEV} ${NUM} ${MSG}
cat ${FIL}
echo

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
DAT=0x12
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

TM0=$(elapsedtime 1000)
pintool -p ${PIN} -M | while read BIT; do
	if [[ ${BIT} -ne 0 ]]; then
		TM1=$(elapsedtime 1000)
		TMD=$((${TM1} - ${TM0}))
		TM0=${TM1}
		# First read also clears interrupt.
		ADR=0xCC
		i2cset -y ${BUS} ${DEV} ${ADR}
		V0L=$(i2cget -y ${BUS} ${DEV})
		N0L=${V0L:2:2}
		ADR=0x8D
		i2cset -y ${BUS} ${DEV} ${ADR}
		# Visible + Infrared
		V0H=$(i2cget -y ${BUS} ${DEV})
		N0H=${V0H:2:2}
		N0V="0x${N0H}${N0L}"
		ADR=0x8E
		i2cset -y ${BUS} ${DEV} ${ADR}
		V1L=$(i2cget -y ${BUS} ${DEV})
		N1L=${V1L:2:2}
		ADR=0x8F
		i2cset -y ${BUS} ${DEV} ${ADR}
		# Infrared
		V1H=$(i2cget -y ${BUS} ${DEV})
		N1H=${V1H:2:2}
		N1V="0x${N1H}${N1L}"
		# Compute.
		LUX=$(luxcompute ${N0V} ${N1V})
		echo ${PGM}: ${PIN} ${BIT} ${BUS} ${DEV} ${V0L} ${V0H} ${V1L} ${V1H} .... ${N0V} ${N1V} ${TMD} ms ${LUX} lx

	fi
done

# Error.

exit 1
