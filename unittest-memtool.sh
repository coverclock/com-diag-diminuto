#!/bin/sh
# Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
# Licensed under the terms in README.h<BR>
# Chip Overclock (coverclock@diag.com)<BR>
# http://www.diag.com/navigation/downloads/Diminuto.html<BR>

PIOB=0xfffff600		# PIOB
SIZE=0x200		# PIOC - PIOB
PER=0xfffff600		# Program Enable Register
OER=0xfffff610		# Output Enable Register
SODR=0xfffff630		# Set Output Data Register
CODR=0xfffff634		# Clear Output Data Register

GREEN=0x1		# PIOB0: disk activity
YELLOW=0x2		# PIOB1: cpu idle
RED=0x4			# PIOB2: ours

USEC=1000000		# 1 second

memtool -a ${PIOB} -l ${SIZE} \
	-4 ${PER} -s ${RED} \
	-4 ${OER} -s ${RED}

while true; do
	memtool -a ${PIOB} -l ${SIZE} \
		-4 ${CODR} -s ${RED} -u ${USEC} \
		-4 ${SODR} -s ${RED} -u ${USEC}
done
