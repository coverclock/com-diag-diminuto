#!/bin/bash
# Copyright 2019-2020 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# ABSTRACT
#
# Sends a SIGHUP to a currently running observe script via its PID file.
#
# USAGE
#
# headsup [ OBSERVEPIDFILE ]
#

PIDFIL=${1:-"/tmp/observe.pid"}

kill -HUP $(cat ${PIDFIL})
