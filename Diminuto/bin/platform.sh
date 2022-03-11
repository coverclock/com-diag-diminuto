#!/bin/bash
# Copyright 2022 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# This script generates the platform descriptions I use in the README when
# it is run on the target. Note that in the case of cross-compilation, there
# will be two platforms: the one on which the project is built, and the on
# which the project is run.
#
# EXAMPLES
#
# Intel(R) Core(TM) i7-7567U CPU @ 3.50GHz x86_64
# Ubuntu 20.04.3 LTS (Focal Fossa)
# Linux 5.13.0-28-generic
# gcc (Ubuntu 9.4.0-1ubuntu1~20.04) 9.4.0
# ldd (Ubuntu GLIBC 2.31-0ubuntu9.7) 2.31
# GNU Make 4.2.1
#
# Intel(R) Core(TM) i7-5557U CPU @ 3.10GHz x86_64
# Ubuntu 20.04.3 LTS (Focal Fossa)
# Linux 5.4.0-97-generic
# gcc (Ubuntu 9.4.0-1ubuntu1~20.04) 9.4.0
# ldd (Ubuntu GLIBC 2.31-0ubuntu9.7) 2.31
# GNU Make 4.2.1
#
# aarch64
# Ubuntu 21.10 (Impish Indri)
# Linux 5.13.0-1017-raspi
# gcc (Ubuntu 11.2.0-7ubuntu2) 11.2.0
# ldd (Ubuntu GLIBC 2.34-0ubuntu3.2) 2.34
# GNU Make 4.3
#
# ARMv7 Processor rev 3 (v7l) armv7l
# Raspbian GNU/Linux 10 (buster)
# Linux 5.4.51-v7l+
# gcc (Raspbian 8.3.0-6+rpi1) 8.3.0
# ldd (Debian GLIBC 2.28-10+rpi1) 2.28
# GNU Make 4.2.1

. /etc/os-release

MODELNAME=$(grep '^model name	: ' /proc/cpuinfo | head -1 | sed 's/^model name	: //')
PROCESSORTYPE=$(uname -m)
OPERATINGSYSTEM="${NAME} ${VERSION}"
KERNELNAME=$(uname -s)
KERNELRELEASE=$(uname -r)
GCCVERSION=$(gcc --version | head -1)
LIBCVERSION=$(ldd --version | head -1)
MAKEVERSION=$(make --version | head -1)

echo ${MODELNAME} ${PROCESSORTYPE}
echo ${OPERATINGSYSTEM}
echo ${KERNELNAME} ${KERNELRELEASE}
echo ${GCCVERSION}
echo ${LIBCVERSION}
echo ${MAKEVERSION}

exit 0
