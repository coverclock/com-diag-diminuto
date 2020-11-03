#!/bin/bash
# Copyright 2018 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# ABSTRACT
#
# Continuously display a line on stdout containing the host name, the
# elapsed ticks, and a sequence number.
#
# USAGE
#
# timesource
#

NEHOST=$(hostname)
NEFREQ=$(frequency)
NELINE=0
while true; do
	echo ${NEHOST} $(elapsedtime ${NEFREQ}) ${NELINE}
	sleep 1
	NELINE=$((${NELINE} + 1))
done
