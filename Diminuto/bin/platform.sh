#!/bin/bash
# Copyright 2022 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# When run on the target, this script generates the platform descriptions I
# use in the project's README. Note that in the case of cross-compilation, there
# will be two platforms: the one on which the project is built, and the on
# which the project is run. See the platforms listed in the Diminuto README
# for examples.
#

. /etc/os-release

MODELNAME=$(grep '^model name[	]*: ' /proc/cpuinfo | head -1 | sed 's/^model name[	]*: //')

MODEL=$(grep '^Model[	]*: ' /proc/cpuinfo | head -1 | sed 's/^Model[	]*: //')

HARDWARE=$(grep '^Hardware[	]*: ' /proc/cpuinfo | head -1 | sed 's/^Hardware[	]*: //')

REVISION=$(grep '^Revision[	]*: ' /proc/cpuinfo | head -1 | sed 's/^Revision[	]*: //')

PROCESSORS=$(grep '^processor[	]*: ' /proc/cpuinfo | wc -l)

PROCESSORTYPE=$(uname -m)

OPERATINGSYSTEM="${NAME} ${VERSION}"

KERNELNAME=$(uname -s)

KERNELRELEASE=$(uname -r)

GCCVERSION=$(gcc --version | head -1)

LIBCVERSION=$(ldd --version | head -1)

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

echo ${TARGET}
echo ${PROCESSORTYPE} x${PROCESSORS}
echo ${OPERATINGSYSTEM}
echo ${KERNELNAME} ${KERNELRELEASE}
echo ${GCCVERSION}
echo ${LIBCVERSION}
echo ${MAKEVERSION}

exit 0
