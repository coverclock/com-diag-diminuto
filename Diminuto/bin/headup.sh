#!/bin/bash
# Copyright 2019 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# Sends a SIGHUP to a currently running headless script via its PID file.

PIDFIL=${1:-"./headless.pid"}

sudo kill -HUP $(cat ${PIDFIL})
