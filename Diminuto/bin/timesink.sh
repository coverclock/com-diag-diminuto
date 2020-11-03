#!/bin/bash
# Copyright 2018 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# ABSTRACT
#
# Continuously read a line containing a host name, an elapsed ticks, and a
# sequence number, from stdin and display on stdout a resulting line expressing
# the difference between the far end time and the near end time in ticks.
#
# USAGE
#
# timesink
#

NEHOST=$(hostname)
NEFREQ=$(frequency)
NELINE=0
while read FEHOST FETIME FELINE; do
	NETIME=$(elapsedtime ${NEFREQ})
	DTIME=$((${NETIME} - ${FETIME}))
	DLINE=$((${NELINE} - ${FELINE}))
	echo ${NEHOST} ${NETIME} ${NELINE} ":" ${FEHOST} ${FETIME} ${FELINE} ":" ${DTIME} ${DLINE}
	sleep 1
	NELINE=$((${NELINE} + 1))
done
