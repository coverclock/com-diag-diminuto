#!/bin/bash
# Copyright 2018 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in README.h
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto

NEHOST=$(hostname)
NEFREQ=$(frequency)
NELINE=0
while true; do
	echo ${NEHOST} $(elapsedtime ${NEFREQ}) ${NELINE}
	sleep 1
	NELINE=$((${NELINE} + 1))
done
