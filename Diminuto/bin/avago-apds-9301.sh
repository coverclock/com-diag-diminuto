#!/bin/bash
# Copyright 2018 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto

# Simple script that uses the platform's I2C tools to play
# with the Avago APDS 9301 lux sensor chip that is part of
# my breadboarded HW test fixture.

PGM=$(basename $0 .sh)
BUS=1
DEV=0x39
PIN=26

# Setup GPIO.

pintool -p ${PIN} -n
pintool -p ${PIN} -x -i -L

# Power down.

ADR=0x80
i2cset -y ${BUS} ${DEV} ${ADR}
DAT=0x00
i2cset -y ${BUS} ${DEV} ${DAT}
echo ${PGM}: ${BUS} ${DEV} ${ADR} ${DAT}

# Power up and verify.

ADR=0x80
i2cset -y ${BUS} ${DEV} ${ADR}
DAT=0x03
i2cset -y ${BUS} ${DEV} ${DAT}
VAL=$(i2cget -y ${BUS} ${DEV})
echo ${PGM}: ${BUS} ${DEV} ${ADR} ${DAT} ${VAL}

# Interrogate all registers.

LST="0 1 2 3 4 5 6 8 A C D E F"
for REG in ${LST}; do
	ADR="0x8${REG}"
	i2cset -y ${BUS} ${DEV} ${ADR}
	VAL=$(i2cget -y ${BUS} ${DEV})
	echo ${PGM}: ${BUS} ${DEV} ${ADR} .... ${VAL}
done

# Configure.

ADR=0x81
i2cset -y ${BUS} ${DEV} ${ADR}
DAT=0x02
i2cset -y ${BUS} ${DEV} ${DAT}

ADR=0x86
i2cset -y ${BUS} ${DEV} ${ADR}
DAT=0x10
i2cset -y ${BUS} ${DEV} ${DAT}

# Wait.

sleep 1

VAL=$(pintool -p 26 -r)
echo ${PGM}: ${PIN} ${VAL}

# Sense.

ADR=0x8C
i2cset -y ${BUS} ${DEV} ${ADR}
V0L=$(i2cget -y ${BUS} ${DEV})
ADR=0x8D
i2cset -y ${BUS} ${DEV} ${ADR}
V0H=$(i2cget -y ${BUS} ${DEV})
echo ${PGM}: ${BUS} ${DEV} ${ADR} .... ${V0H},${V0L}

ADR=0x8E
i2cset -y ${BUS} ${DEV} ${ADR}
V1L=$(i2cget -y ${BUS} ${DEV})
ADR=0x8F
i2cset -y ${BUS} ${DEV} ${ADR}
V1H=$(i2cget -y ${BUS} ${DEV})
echo ${PGM}: ${BUS} ${DEV} ${ADR} .... ${V1H},${V1L}

exit 0
