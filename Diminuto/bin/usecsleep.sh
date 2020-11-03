#!/bin/bash
# Copyright 2018 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# ABSTRACT
#
# Sleep for the specified number of microseconds.
#
# USAGE
#
# usecsleep [ MICROSECONDS ]
#
# EXAMPLE
#
# usecsleep 1000000
#

elapsedsleep ${1:-"0"} 1000000
