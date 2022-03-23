#!/bin/bash
# Copyright 2022 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# ABSTRACT
#
# Run a bunch of examples of internettest without setting up a
# connection and see if any of them take an assert failure.
# This is just a sanity test.
#

. $(readlink -e $(dirname ${0})/../bin)/setup

set -x

internettool -?

internettool -x -E 80 || exit 1
internettool -x -E 8888 || exit 1
internettool -x -E 8888 -t || exit 1
internettool -x -E 8888 -u || exit 1
internettool -x -E :80 || exit 1
internettool -x -E :8888 || exit 1
internettool -x -E :8888 -t || exit 1
internettool -x -E :8888 -u || exit 1
internettool -x -E :http || exit 1
internettool -x -E :http -t || exit 1
internettool -x -E :http -u || exit 1
internettool -x -E :tftp || exit 1
internettool -x -E :time || exit 1
internettool -x -E :time -t || exit 1
internettool -x -E :time -u || exit 1

internettool -x -p 80 || exit 1
internettool -x -p 8888 || exit 1
internettool -x -p 8888 -t || exit 1
internettool -x -p 8888 -u || exit 1

internettool -x -p http || exit 1
internettool -x -p http -t || exit 1
internettool -x -p http -u || exit 1
internettool -x -p tftp || exit 1
internettool -x -p time || exit 1
internettool -x -p time -t || exit 1
internettool -x -p time -u || exit 1

internettool -x -E 206.178.189.131 || exit 1
internettool -x -E 206.178.189.131:80 || exit 1
internettool -x -E 206.178.189.131:8888 || exit 1
internettool -x -E 206.178.189.131:8888 -t || exit 1
internettool -x -E 206.178.189.131:8888 -u || exit 1
internettool -x -E 206.178.189.131:http || exit 1
internettool -x -E 206.178.189.131:http -t || exit 1
internettool -x -E 206.178.189.131:http -u || exit 1
internettool -x -E 206.178.189.131:tftp || exit 1
internettool -x -E 206.178.189.131:time || exit 1
internettool -x -E 206.178.189.131:time -t || exit 1
internettool -x -E 206.178.189.131:time -u || exit 1

internettool -x -E "[2607:f8b0:400f:805::200e]" || exit 1
internettool -x -E "[2607:f8b0:400f:805::200e]:80" || exit 1
internettool -x -E "[2607:f8b0:400f:805::200e]:8888" -t || exit 1
internettool -x -E "[2607:f8b0:400f:805::200e]:8888" -u || exit 1
internettool -x -E "[2607:f8b0:400f:805::200e]:8888" || exit 1
internettool -x -E "[2607:f8b0:400f:805::200e]:http" || exit 1
internettool -x -E "[2607:f8b0:400f:805::200e]:http" -t || exit 1
internettool -x -E "[2607:f8b0:400f:805::200e]:http" -u || exit 1
internettool -x -E "[2607:f8b0:400f:805::200e]:tftp" || exit 1
internettool -x -E "[2607:f8b0:400f:805::200e]:time" || exit 1
internettool -x -E "[2607:f8b0:400f:805::200e]:time" -t || exit 1
internettool -x -E "[2607:f8b0:400f:805::200e]:time" -u || exit 1

internettool -x -E "[::ffff:209.17.116.163]" || exit 1
internettool -x -E "[::ffff:209.17.116.163]:80" || exit 1
internettool -x -E "[::ffff:209.17.116.163]:8888" || exit 1
internettool -x -E "[::ffff:209.17.116.163]:8888" -t || exit 1
internettool -x -E "[::ffff:209.17.116.163]:8888" -u || exit 1
internettool -x -E "[::ffff:209.17.116.163]:http" || exit 1
internettool -x -E "[::ffff:209.17.116.163]:http" -t || exit 1
internettool -x -E "[::ffff:209.17.116.163]:http" -u || exit 1
internettool -x -E "[::ffff:209.17.116.163]:tftp" || exit 1
internettool -x -E "[::ffff:209.17.116.163]:time" || exit 1
internettool -x -E "[::ffff:209.17.116.163]:time" -t || exit 1
internettool -x -E "[::ffff:209.17.116.163]:time" -u || exit 1

internettool -x -E diag.com || exit 1
internettool -x -E diag.com -4 || exit 1
internettool -x -E diag.com -6 || exit 1
internettool -x -E diag.com:80 || exit 1
internettool -x -E diag.com:8888 || exit 1
internettool -x -E diag.com:8888 -t || exit 1
internettool -x -E diag.com:8888 -u || exit 1
internettool -x -E diag.com:http || exit 1
internettool -x -E diag.com:http -t || exit 1
internettool -x -E diag.com:http -u || exit 1
internettool -x -E diag.com:tftp || exit 1
internettool -x -E diag.com:time || exit 1
internettool -x -E diag.com:time -t || exit 1
internettool -x -E diag.com:time -u || exit 1

internettool -x -E google.com || exit 1
internettool -x -E google.com -4 || exit 1
internettool -x -E google.com -6 || exit 1
internettool -x -E google.com:80  || exit 1
internettool -x -E google.com:8888 || exit 1
internettool -x -E google.com:8888 -t || exit 1
internettool -x -E google.com:8888 -u || exit 1
internettool -x -E google.com:http || exit 1
internettool -x -E google.com:http -t || exit 1
internettool -x -E google.com:http -u || exit 1
internettool -x -E google.com:tftp || exit 1
internettool -x -E google.com:time || exit 1
internettool -x -E google.com:time -t || exit 1
internettool -x -E google.com:time -u || exit 1

internettool -x -E prairiethorn.org || exit 1
internettool -x -E prairiethorn.org -4 || exit 1
internettool -x -E prairiethorn.org -6 || exit 1
internettool -x -E prairiethorn.org:80 || exit 1
internettool -x -E prairiethorn.org:8888 || exit 1
internettool -x -E prairiethorn.org:8888 -t || exit 1
internettool -x -E prairiethorn.org:8888 -u || exit 1
internettool -x -E prairiethorn.org:http || exit 1
internettool -x -E prairiethorn.org:http -t || exit 1
internettool -x -E prairiethorn.org:http -u || exit 1
internettool -x -E prairiethorn.org:tftp || exit 1
internettool -x -E prairiethorn.org:time || exit 1
internettool -x -E prairiethorn.org:time -t || exit 1
internettool -x -E prairiethorn.org:time -u || exit 1

internettool -x -A 206.178.189.131 || exit 1
internettool -x -A 206.178.189.131 -P 80 || exit 1
internettool -x -A 206.178.189.131 -P 8888 || exit 1
internettool -x -A 206.178.189.131 -P 8888 -t || exit 1
internettool -x -A 206.178.189.131 -P 8888 -u || exit 1
internettool -x -A 206.178.189.131 -P http || exit 1
internettool -x -A 206.178.189.131 -P http -t || exit 1
internettool -x -A 206.178.189.131 -P http -u || exit 1
internettool -x -A 206.178.189.131 -P tftp || exit 1
internettool -x -A 206.178.189.131 -P time || exit 1
internettool -x -A 206.178.189.131 -P time -t || exit 1
internettool -x -A 206.178.189.131 -P time -u || exit 1

internettool -x -A "[2607:f8b0:400f:805::200e]" || exit 1
internettool -x -A "[2607:f8b0:400f:805::200e]" -P 80 || exit 1
internettool -x -A "[2607:f8b0:400f:805::200e]" -P 8888 -t || exit 1
internettool -x -A "[2607:f8b0:400f:805::200e]" -P 8888 -u || exit 1
internettool -x -A "[2607:f8b0:400f:805::200e]" -P 8888 || exit 1
internettool -x -A "[2607:f8b0:400f:805::200e]" -P http || exit 1
internettool -x -A "[2607:f8b0:400f:805::200e]" -P http -t || exit 1
internettool -x -A "[2607:f8b0:400f:805::200e]" -P http -u || exit 1
internettool -x -A "[2607:f8b0:400f:805::200e]" -P tftp || exit 1
internettool -x -A "[2607:f8b0:400f:805::200e]" -P time || exit 1
internettool -x -A "[2607:f8b0:400f:805::200e]" -P time -t || exit 1
internettool -x -A "[2607:f8b0:400f:805::200e]" -P time -u || exit 1

internettool -x -A "[::ffff:209.17.116.163]" || exit 1
internettool -x -A "[::ffff:209.17.116.163]" -P 80 || exit 1
internettool -x -A "[::ffff:209.17.116.163]" -P 8888 || exit 1
internettool -x -A "[::ffff:209.17.116.163]" -P 8888 -t || exit 1
internettool -x -A "[::ffff:209.17.116.163]" -P 8888 -u || exit 1
internettool -x -A "[::ffff:209.17.116.163]" -P http || exit 1
internettool -x -A "[::ffff:209.17.116.163]" -P http -t || exit 1
internettool -x -A "[::ffff:209.17.116.163]" -P http -u || exit 1
internettool -x -A "[::ffff:209.17.116.163]" -P tftp || exit 1
internettool -x -A "[::ffff:209.17.116.163]" -P time || exit 1
internettool -x -A "[::ffff:209.17.116.163]" -P time -t || exit 1
internettool -x -A "[::ffff:209.17.116.163]" -P time -u || exit 1

internettool -x -A diag.com || exit 1
internettool -x -A diag.com -4 || exit 1
internettool -x -A diag.com -6 || exit 1
internettool -x -A diag.com -P 80 || exit 1
internettool -x -A diag.com -P 8888 || exit 1
internettool -x -A diag.com -P 8888 -t || exit 1
internettool -x -A diag.com -P 8888 -u || exit 1
internettool -x -A diag.com -P http || exit 1
internettool -x -A diag.com -P http -t || exit 1
internettool -x -A diag.com -P http -u || exit 1
internettool -x -A diag.com -P tftp || exit 1
internettool -x -A diag.com -P time || exit 1
internettool -x -A diag.com -P time -t || exit 1
internettool -x -A diag.com -P time -u || exit 1

internettool -x -A google.com || exit 1
internettool -x -A google.com -4 || exit 1
internettool -x -A google.com -6 || exit 1
internettool -x -A google.com -P 80  || exit 1
internettool -x -A google.com -P 8888 || exit 1
internettool -x -A google.com -P 8888 -t || exit 1
internettool -x -A google.com -P 8888 -u || exit 1
internettool -x -A google.com -P http || exit 1
internettool -x -A google.com -P http -t || exit 1
internettool -x -A google.com -P http -u || exit 1
internettool -x -A google.com -P tftp || exit 1
internettool -x -A google.com -P time || exit 1
internettool -x -A google.com -P time -t || exit 1
internettool -x -A google.com -P time -u || exit 1

internettool -x -A prairiethorn.org || exit 1
internettool -x -A prairiethorn.org -4 || exit 1
internettool -x -A prairiethorn.org -6 || exit 1
internettool -x -A prairiethorn.org -P 80 || exit 1
internettool -x -A prairiethorn.org -P 8888 || exit 1
internettool -x -A prairiethorn.org -P 8888 -t || exit 1
internettool -x -A prairiethorn.org -P 8888 -u || exit 1
internettool -x -A prairiethorn.org -P http || exit 1
internettool -x -A prairiethorn.org -P http -t || exit 1
internettool -x -A prairiethorn.org -P http -u || exit 1
internettool -x -A prairiethorn.org -P tftp || exit 1
internettool -x -A prairiethorn.org -P time || exit 1
internettool -x -A prairiethorn.org -P time -t || exit 1
internettool -x -A prairiethorn.org -P time -u || exit 1

echo "Success."
exit 0
