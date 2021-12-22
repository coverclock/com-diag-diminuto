# vi: set ts=4 shiftwidth=4:
# Copyright 2021 Digital Aggregates Corporation
# Licensed under the terms in LICENSE.txt
# author:Chip Overclock
# mailto:coverclock@diag.com
# http://www.diag.com/navigation/downloads/Diminuto.html
# "Chip Overclock" is a registered trademark.
# "Digital Aggregates Corporation" is a registered trademark.

# I have yet to find a reliable way to determine under what
# ARCH for which the host Linux kernel was built. You would
# think uname would do this for you. The ones this make script
# understands are just the ones I have used myself.
#
# Below is a list of directories under the arch directory of
# the 5.15.11 Linux kernel, illustrating some of the possiblities.
#
#   alpha
#   arc
#   arm
#   arm64
#   csky
#   h8300
#   hexagon
#   ia64
#   m68k
#   microblaze
#   mips
#   nds32
#   nios2
#   openrisc
#   parisc
#   powerpc
#   riscv
#   s390
#   sh
#   sparc
#   um
#   x86
#   xtensa
#

ifeq ($(MACHINE),x86_64)
ARCH				:=	x86
else
ifeq ($(MACHINE),armv7l)
ARCH				:=	arm
else
ifeq ($(MACHINE),aarch64)
ARCH				:=	arm64
else
ARCH				:=	other
$(error MACHINE not recognized for ARCH)
endif
endif
endif
