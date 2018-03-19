#!/bin/bash
# Copyright 2018 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in README.h
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto

SN=0
while true; do echo $(hostname) $(zulu) ${SN}; SN=$((${SN} + 1)); sleep 1; done
