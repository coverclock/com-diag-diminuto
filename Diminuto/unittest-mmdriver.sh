#!/bin/sh
# Copyright 2010 Digital Aggregates Corporation, Colorado, USA<BR>
# Licensed under the terms in README.h<BR>
# Chip Overclock (coverclock@diag.com)<BR>
# http://www.diag.com/navigation/downloads/Diminuto.html<BR>

PER=0x0			# Program Enable Register
OER=0x10		# Output Enable Register
SODR=0x30		# Set Output Data Register
CODR=0x34		# Clear Output Data Register

GREEN=0x1		# PIOB0: disk activity
YELLOW=0x2		# PIOB1: cpu idle
RED=0x4			# PIOB2: ours

USEC=1000000		# 1 second

TRIES=30		# 30 tries (about a minute)

mmdrivertool \
	-4 ${PER} -s ${RED} \
	-4 ${OER} -s ${RED}

while [ ${TRIES} -gt 0 ]; do
	mmdrivertool \
		-4 ${CODR} -s ${RED} -u ${USEC} \
		-4 ${SODR} -s ${RED} -u ${USEC}
	TRIES=`expr ${TRIES} - 1`
done
