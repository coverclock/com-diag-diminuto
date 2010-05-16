#!/bin/sh

PER=0x0
OER=0x10
SODR=0x30
CODR=0x34

GREEN=0x1
YELLOW=0x2
RED=0x4

USEC=1000000

mmdrivertool -d \
	-4 ${PER} -s ${RED} \
	-4 ${OER} -s ${RED} \
	-4 ${CODR} -s ${RED} -u ${USEC} \
	-4 ${SODR} -s ${RED} -u ${USEC} \
	-4 ${CODR} -s ${RED} -u ${USEC} \
	-4 ${SODR} -s ${RED} -u ${USEC} \
	-4 ${CODR} -s ${RED} -u ${USEC} \
	-4 ${SODR} -s ${RED} -u ${USEC} \
	-4 ${CODR} -s ${RED} -u ${USEC} \
	-4 ${SODR} -s ${RED} -u ${USEC} \
	-4 ${CODR} -s ${RED} -u ${USEC} \
	-4 ${SODR} -s ${RED} -u ${USEC} \
	-4 ${CODR} -s ${RED} -u ${USEC} \
	-4 ${SODR} -s ${RED} -u ${USEC} \
	-4 ${CODR} -s ${RED} -u ${USEC} \
	-4 ${SODR} -s ${RED} -u ${USEC} \
	-4 ${CODR} -s ${RED} -u ${USEC} \
	-4 ${SODR} -s ${RED} -u ${USEC}
