#!/bin/bash
# Copyright 2018 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# ABSTRACT
#
# Continuously generate a line of output to stdout consisting of the
# hostname, the UTC time, and a sequence number.
#
# SEE ALSO
#
# datesink
#
# USAGE
#
# datesource
#

SN=0
while true; do echo $(hostname) $(zulu) ${SN}; SN=$((${SN} + 1)); sleep 1; done
