#!/bin/bash
# Copyright 2018 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# ABSTRACT
#
# Continuously read a line from stdin and display it on stdout prefaced
# with the hostname, UTC time, and sequence number  as annotations.
#
# SEE ALSO
#
# datesource
#
# USAGE
#
# datesink
#

SN=0
while read LINE; do echo $(hostname) $(zulu) ${SN} ${LINE}; SN=$((${SN} + 1)); sleep 1; done
