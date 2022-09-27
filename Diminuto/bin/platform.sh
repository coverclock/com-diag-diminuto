#!/bin/bash
# Copyright 2022 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# When run on the target, this script generates the platform descriptions I
# use in the project's README. Note that in the case of cross-compilation, there
# will effectively be three platforms: the one on which the project is built,
# the one defined by the cross-compilation toolchain on the platform on which
# the project is built, and the on which the project is actually run. See the
# platforms listed in the Diminuto README for additional examples.
#
# Long long ago in a galaxy far far away, former colleagues may remember
# with some amusement a script like this being somewhat controversially
# named "wtf".
#

PROCESSORS=1
if [[ -r /proc/cpuinfo ]]; then
	MODELNAME=$(grep '^model name[	]*: ' /proc/cpuinfo | head -1 | sed 's/^model name[	]*: //')
	MODEL=$(grep '^Model[	]*: ' /proc/cpuinfo | head -1 | sed 's/^Model[	]*: //')
	HARDWARE=$(grep '^Hardware[	]*: ' /proc/cpuinfo | head -1 | sed 's/^Hardware[	]*: //')
	REVISION=$(grep '^Revision[	]*: ' /proc/cpuinfo | head -1 | sed 's/^Revision[	]*: //')
	PROCESSORS=$(grep '^processor[	]*: ' /proc/cpuinfo | wc -l)
	if [[ -z "${MODEL}" ]]; then
		MODEL=$(grep '^uarch[	]*: ' /proc/cpuinfo | head -1 | sed 's/^uarch[	]*: //')
	fi
	if [[ -z "${HARDWARE}" ]]; then
		HARDWARE="$(grep '^isa[	]*: ' /proc/cpuinfo | head -1 | sed 's/^isa[	]*: //') $(grep '^mmu[	]*: ' /proc/cpuinfo | head -1 | sed 's/^mmu[	]*: //')"
	fi
fi

if [[ -r /etc/os-release ]]; then
	. /etc/os-release
	OPERATINGSYSTEM="${NAME} ${VERSION}"
fi

PROCESSORTYPE=$(uname -m)
KERNELNAME=$(uname -s)
KERNELRELEASE=$(uname -r)

GCCVERSION=$(gcc --version | head -1)
LIBCVERSION=$(ldd --version | head -1)
BINUTILSVERSION=$(ld --version 2>&1 | head -1)
MAKEVERSION=$(make --version | head -1)

TARGET="${MODEL}"
if [[ -n "${MODELNAME}" ]]; then
	TARGET="${TARGET} ${MODELNAME}"
fi
if [[ -n "${HARDWARE}" ]]; then
	TARGET="${TARGET} ${HARDWARE}"
fi
if [[ -n "${REVISION}" ]]; then
	TARGET="${TARGET} ${REVISION}"
fi

ABI=$(basename $(readlink -e $(which gcc)))
TRIPLET=$(gcc -dumpmachine)

BIGLITTLE=$(endianess)
ENDIANESS="${BIGLITTLE}-endian"

echo ${TARGET} "    "
echo ${PROCESSORTYPE} x${PROCESSORS} "    "
echo ${OPERATINGSYSTEM} "    "
echo ${KERNELNAME} ${KERNELRELEASE} "    "
echo ${GCCVERSION} "    "
echo ${LIBCVERSION} "    "
echo ${BINUTILSVERSION} "    "
echo ${MAKEVERSION} "    "
echo ${ABI} "    "
echo ${TRIPLET} "    "
echo ${ENDIANESS} "    "

exit 0
