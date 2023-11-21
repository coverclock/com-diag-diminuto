#!/bin/bash -x
# Copyright 2023 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# USAGE
#
# internettooltests [ PORTNUMBER [ LOOPCOUNT [ SECONDS ] ] ]
#
# ABSTRACT
#
# Run a bunch of examples of internettest, first just test the argument
# parsing capability without setting up a connection, then actually set
# up connections and pass data on the local host.
#
# The results of the parsing tests that use actual FQDNs are not completely
# checked since the resolved IPv4 and IPv6 addresses for those domains are
# not necessarily fixed.
#
# There are occasionally short delays in the DNS resolutions, so don't be
# surprised if the display pauses for a few seconds in the examples that
# have FQDNs.
#
# It can take a remarkable amount of time for the Linux kernel to recycle the
# port number so that it can be reused in a subsequent connection test.
#
# Because of the significant buffering in the multiple pipes, it may take
# a while for the head(1) command to terminate the pipeline.
#
# If the script is run under UID 0 ("root"), the ICMP ping tests, which
# require root privileges, are also run. (If the idea of a shell script
# being run as root doing kill -9 commands doesn't concern you, you aren't
# thinking clearly.)
#
# Whether "localhost" resolves to an IPv6 address of ::ffff:127.0.0.1 or ::1
# apparently depends on... well, I'm not presently sure what it depends on,
# but remarkably it does NOT depend on how you have have configued /etc/hosts.
#
# EXAMPLES
#
# internettooltests
#
# internettooltests 5555 5 2
#
# sudo su
# . out/host/bin/setup
# internettooltests 5555 5 2
#

. $(readlink -e $(dirname ${0})/../bin)/setup

PORT=${1:-5555}
LOOP=${2:-5}
WAIT=${3:-2}
ROOT=${4}

SERVERPID=0

export COM_DIAG_DIMINUTO_LOG_MASK=0xff

set -x

# PRELIMINARIES

internettool -? && exit 1

internettool -I || exit 1

# PARSING TESTS

internettool -x -e 80 | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 :: 80$' || exit 2
internettool -x -e 8888 | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 :: 8888$' || exit 2
internettool -x -e 8888 -t | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 :: 8888$' || exit 2
internettool -x -e 8888 -u | egrep -q '^internettool L2 4 L3 u FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 :: 8888$' || exit 2

internettool -x -e :80 | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 :: 80$' || exit 2
internettool -x -e :8888 | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 :: 8888$' || exit 2
internettool -x -e :8888 -t | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 :: 8888$' || exit 2
internettool -x -e :8888 -u | egrep -q '^internettool L2 4 L3 u FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 :: 8888$' || exit 2

internettool -x -e :http | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 :: 80$' || exit 2
internettool -x -e :tftp | egrep -q '^internettool L2 4 L3 u FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 :: 69$' || exit 2
internettool -x -e :time | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 :: 37$' || exit 2
internettool -x -e :time -t | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 :: 37$' || exit 2
internettool -x -e :time -u | egrep -q '^internettool L2 4 L3 u FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 :: 37$' || exit 2

internettool -x -e localhost:80 | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 127\.0\.0\.1 (::ffff:127\.0\.0\.1|::1) 80$' || exit 2
internettool -x -e localhost:8888 | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 127\.0\.0\.1 (::ffff:127\.0\.0\.1|::1) 8888$' || exit 2
internettool -x -e localhost:8888 -t | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 127\.0\.0\.1 (::ffff:127\.0\.0\.1|::1) 8888$' || exit 2
internettool -x -e localhost:8888 -u | egrep -q '^internettool L2 4 L3 u FE 127\.0\.0\.1 ::1 0 NE 127\.0\.0\.1 (::ffff:127\.0\.0\.1|::1) 8888$' || exit 2
internettool -x -e localhost:http | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 127\.0\.0\.1 (::ffff:127\.0\.0\.1|::1) 80$' || exit 2
internettool -x -e localhost:tftp | egrep -q '^internettool L2 4 L3 u FE 127\.0\.0\.1 ::1 0 NE 127\.0\.0\.1 (::ffff:127\.0\.0\.1|::1) 69$' || exit 2
internettool -x -e localhost:time | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 127\.0\.0\.1 (::ffff:127\.0\.0\.1|::1) 37$' || exit 2
internettool -x -e localhost:time -t | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 127\.0\.0\.1 (::ffff:127\.0\.0\.1|::1) 37$' || exit 2
internettool -x -e localhost:time -u | egrep -q '^internettool L2 4 L3 u FE 127\.0\.0\.1 ::1 0 NE 127\.0\.0\.1 (::ffff:127\.0\.0\.1|::1) 37$' || exit 2

internettool -x -e google.com:80 | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 80$' || exit 2
internettool -x -e google.com:8888 | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 8888$' || exit 2
internettool -x -e google.com:8888 -t | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 8888$' || exit 2
internettool -x -e google.com:8888 -u | egrep -q '^internettool L2 4 L3 u FE 127\.0\.0\.1 ::1 0 NE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 8888$' || exit 2
internettool -x -e google.com:http | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 80$' || exit 2
internettool -x -e google.com:tftp | egrep -q '^internettool L2 4 L3 u FE 127\.0\.0\.1 ::1 0 NE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 69$' || exit 2
internettool -x -e google.com:time | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 37$' || exit 2
internettool -x -e google.com:time -t | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 37$' || exit 2
internettool -x -e google.com:time -u | egrep -q '^internettool L2 4 L3 u FE 127\.0\.0\.1 ::1 0 NE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 37$' || exit 2

internettool -x -p 80 | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 :: 80$' || exit 2
internettool -x -p 8888 | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 :: 8888$' || exit 2
internettool -x -p 8888 -t | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 :: 8888$' || exit 2
internettool -x -p 8888 -u | egrep -q '^internettool L2 4 L3 u FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 :: 8888$' || exit 2

internettool -x -p http | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 :: 80$' || exit 2
internettool -x -p http -t | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 :: 80$' || exit 2
internettool -x -p tftp -u | egrep -q '^internettool L2 4 L3 u FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 :: 69$' || exit 2
internettool -x -p time | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 :: 37$' || exit 2
internettool -x -p time -t | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 :: 37$' || exit 2
internettool -x -p time -u | egrep -q '^internettool L2 4 L3 u FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 :: 37$' || exit 2

internettool -x -a localhost -p 80 | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 127\.0\.0\.1 :: 80$' || exit 2
internettool -x -a localhost -p 8888 | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 127\.0\.0\.1 :: 8888$' || exit 2
internettool -x -a localhost -p 8888 -t | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 127\.0\.0\.1 :: 8888$' || exit 2
internettool -x -a localhost -p 8888 -u | egrep -q '^internettool L2 4 L3 u FE 127\.0\.0\.1 ::1 0 NE 127\.0\.0\.1 :: 8888$' || exit 2

internettool -x -a localhost -p http | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 127\.0\.0\.1 :: 80$' || exit 2
internettool -x -a localhost -p http -t | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 127\.0\.0\.1 :: 80$' || exit 2
internettool -x -a localhost -p tftp -u | egrep -q '^internettool L2 4 L3 u FE 127\.0\.0\.1 ::1 0 NE 127\.0\.0\.1 :: 69$' || exit 2
internettool -x -a localhost -p time | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 127\.0\.0\.1 :: 37$' || exit 2
internettool -x -a localhost -p time -t | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 127\.0\.0\.1 :: 37$' || exit 2
internettool -x -a localhost -p time -u | egrep -q '^internettool L2 4 L3 u FE 127\.0\.0\.1 ::1 0 NE 127\.0\.0\.1 :: 37$' || exit 2

internettool -x -a localhost -4 -p 80 | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 127\.0\.0\.1 :: 80$' || exit 2
internettool -x -a localhost -4 -p 8888 | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 127\.0\.0\.1 :: 8888$' || exit 2
internettool -x -a localhost -4 -p 8888 -t | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 127\.0\.0\.1 :: 8888$' || exit 2
internettool -x -a localhost -4 -p 8888 -u | egrep -q '^internettool L2 4 L3 u FE 127\.0\.0\.1 ::1 0 NE 127\.0\.0\.1 :: 8888$' || exit 2

internettool -x -a localhost -4 -p http | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 127\.0\.0\.1 :: 80$' || exit 2
internettool -x -a localhost -4 -p http -t | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 127\.0\.0\.1 :: 80$' || exit 2
internettool -x -a localhost -4 -p tftp -u | egrep -q '^internettool L2 4 L3 u FE 127\.0\.0\.1 ::1 0 NE 127\.0\.0\.1 :: 69$' || exit 2
internettool -x -a localhost -4 -p time | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 127\.0\.0\.1 :: 37$' || exit 2
internettool -x -a localhost -4 -p time -t | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE 127\.0\.0\.1 :: 37$' || exit 2
internettool -x -a localhost -4 -p time -u | egrep -q '^internettool L2 4 L3 u FE 127\.0\.0\.1 ::1 0 NE 127\.0\.0\.1 :: 37$' || exit 2

internettool -x -a localhost -6 -p 80 | egrep -q '^internettool L2 6 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 (::ffff:127\.0\.0\.1|::1) 80$' || exit 2
internettool -x -a localhost -6 -p 8888 | egrep -q '^internettool L2 6 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 (::ffff:127\.0\.0\.1|::1) 8888$' || exit 2
internettool -x -a localhost -6 -p 8888 -t | egrep -q '^internettool L2 6 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 (::ffff:127\.0\.0\.1|::1) 8888$' || exit 2
internettool -x -a localhost -6 -p 8888 -u | egrep -q '^internettool L2 6 L3 u FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 (::ffff:127\.0\.0\.1|::1) 8888$' || exit 2

internettool -x -a localhost -6 -p http | egrep -q '^internettool L2 6 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 (::ffff:127\.0\.0\.1|::1) 80$' || exit 2
internettool -x -a localhost -6 -p http -t | egrep -q '^internettool L2 6 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 (::ffff:127\.0\.0\.1|::1) 80$' || exit 2
internettool -x -a localhost -6 -p tftp -u | egrep -q '^internettool L2 6 L3 u FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 (::ffff:127\.0\.0\.1|::1) 69$' || exit 2
internettool -x -a localhost -6 -p time | egrep -q '^internettool L2 6 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 (::ffff:127\.0\.0\.1|::1) 37$' || exit 2
internettool -x -a localhost -6 -p time -t | egrep -q '^internettool L2 6 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 (::ffff:127\.0\.0\.1|::1) 37$' || exit 2
internettool -x -a localhost -6 -p time -u | egrep -q '^internettool L2 6 L3 u FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 (::ffff:127\.0\.0\.1|::1) 37$' || exit 2

internettool -x -a google.com -p 80 | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* \:: 80$' || exit 2
internettool -x -a google.com -p 8888 | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* \:: 8888$' || exit 2
internettool -x -a google.com -p 8888 -t | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* \:: 8888$' || exit 2
internettool -x -a google.com -p 8888 -u | egrep -q '^internettool L2 4 L3 u FE 127\.0\.0\.1 ::1 0 NE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* \:: 8888$' || exit 2

internettool -x -a google.com -p http | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* \:: 80$' || exit 2
internettool -x -a google.com -p http -t | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* \:: 80$' || exit 2
internettool -x -a google.com -p tftp -u | egrep -q '^internettool L2 4 L3 u FE 127\.0\.0\.1 ::1 0 NE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* \:: 69$' || exit 2
internettool -x -a google.com -p time | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* \:: 37$' || exit 2
internettool -x -a google.com -p time -t | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* \:: 37$' || exit 2
internettool -x -a google.com -p time -u | egrep -q '^internettool L2 4 L3 u FE 127\.0\.0\.1 ::1 0 NE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* \:: 37$' || exit 2

internettool -x -a google.com -4 -p 80 | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* \:: 80$' || exit 2
internettool -x -a google.com -4 -p 8888 | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* \:: 8888$' || exit 2
internettool -x -a google.com -4 -p 8888 -t | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* \:: 8888$' || exit 2
internettool -x -a google.com -4 -p 8888 -u | egrep -q '^internettool L2 4 L3 u FE 127\.0\.0\.1 ::1 0 NE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* \:: 8888$' || exit 2

internettool -x -a google.com -4 -p http | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* \:: 80$' || exit 2
internettool -x -a google.com -4 -p http -t | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* \:: 80$' || exit 2
internettool -x -a google.com -4 -p tftp -u | egrep -q '^internettool L2 4 L3 u FE 127\.0\.0\.1 ::1 0 NE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* \:: 69$' || exit 2
internettool -x -a google.com -4 -p time | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* \:: 37$' || exit 2
internettool -x -a google.com -4 -p time -t | egrep -q '^internettool L2 4 L3 t FE 127\.0\.0\.1 ::1 0 NE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* \:: 37$' || exit 2
internettool -x -a google.com -4 -p time -u | egrep -q '^internettool L2 4 L3 u FE 127\.0\.0\.1 ::1 0 NE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* \:: 37$' || exit 2

internettool -x -a google.com -6 -p 80 | egrep -q '^internettool L2 6 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 80$' || exit 2
internettool -x -a google.com -6 -p 8888 | egrep -q '^internettool L2 6 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 8888$' || exit 2
internettool -x -a google.com -6 -p 8888 -t | egrep -q '^internettool L2 6 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 8888$' || exit 2
internettool -x -a google.com -6 -p 8888 -u | egrep -q '^internettool L2 6 L3 u FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 8888$' || exit 2

internettool -x -a google.com -6 -p http | egrep -q '^internettool L2 6 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 80$' || exit 2
internettool -x -a google.com -6 -p http -t | egrep -q '^internettool L2 6 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 80$' || exit 2
internettool -x -a google.com -6 -p tftp -u | egrep -q '^internettool L2 6 L3 u FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 69$' || exit 2
internettool -x -a google.com -6 -p time | egrep -q '^internettool L2 6 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 37$' || exit 2
internettool -x -a google.com -6 -p time -t | egrep -q '^internettool L2 6 L3 t FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 37$' || exit 2
internettool -x -a google.com -6 -p time -u | egrep -q '^internettool L2 6 L3 u FE 127\.0\.0\.1 ::1 0 NE 0\.0\.0\.0 ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 37$' || exit 2

internettool -x -E 206.178.189.131 | egrep -q '^internettool L2 4 L3 t FE 206\.178\.189\.131 :: 0 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -E 206.178.189.131:80 | egrep -q '^internettool L2 4 L3 t FE 206\.178\.189\.131 :: 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E 206.178.189.131:8888 | egrep -q '^internettool L2 4 L3 t FE 206\.178\.189\.131 :: 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E 206.178.189.131:8888 -t | egrep -q '^internettool L2 4 L3 t FE 206\.178\.189\.131 :: 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E 206.178.189.131:8888 -u | egrep -q '^internettool L2 4 L3 u FE 206\.178\.189\.131 :: 8888 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -E 206.178.189.131:http | egrep -q '^internettool L2 4 L3 t FE 206\.178\.189\.131 :: 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E 206.178.189.131:tftp | egrep -q '^internettool L2 4 L3 u FE 206\.178\.189\.131 :: 69 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E 206.178.189.131:time | egrep -q '^internettool L2 4 L3 t FE 206\.178\.189\.131 :: 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E 206.178.189.131:time -t | egrep -q '^internettool L2 4 L3 t FE 206\.178\.189\.131 :: 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E 206.178.189.131:time -u | egrep -q '^internettool L2 4 L3 u FE 206\.178\.189\.131 :: 37 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -E '[2607:f8b0:400f:805::200e]' | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 2607:f8b0:400f:805::200e 0 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -E '[2607:f8b0:400f:805::200e]:80' | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 2607:f8b0:400f:805::200e 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E '[2607:f8b0:400f:805::200e]:8888' -t | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 2607:f8b0:400f:805::200e 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E '[2607:f8b0:400f:805::200e]:8888' -u | egrep -q '^internettool L2 6 L3 u FE 0\.0\.0\.0 2607:f8b0:400f:805::200e 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E '[2607:f8b0:400f:805::200e]:8888' | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 2607:f8b0:400f:805::200e 8888 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -E '[2607:f8b0:400f:805::200e]:http' | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 2607:f8b0:400f:805::200e 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E '[2607:f8b0:400f:805::200e]:tftp' | egrep -q '^internettool L2 6 L3 u FE 0\.0\.0\.0 2607:f8b0:400f:805::200e 69 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E '[2607:f8b0:400f:805::200e]:time' | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 2607:f8b0:400f:805::200e 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E '[2607:f8b0:400f:805::200e]:time' -t | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 2607:f8b0:400f:805::200e 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E '[2607:f8b0:400f:805::200e]:time' -u | egrep -q '^internettool L2 6 L3 u FE 0\.0\.0\.0 2607:f8b0:400f:805::200e 37 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -E '[::ffff:209.17.116.163]' | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 ::ffff:209\.17\.116\.163 0 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -E '[::ffff:209.17.116.163]:80' | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 ::ffff:209\.17\.116\.163 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E '[::ffff:209.17.116.163]:8888' | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 ::ffff:209\.17\.116\.163 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E '[::ffff:209.17.116.163]:8888' -t | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 ::ffff:209\.17\.116\.163 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E '[::ffff:209.17.116.163]:8888' -u | egrep -q '^internettool L2 6 L3 u FE 0\.0\.0\.0 ::ffff:209\.17\.116\.163 8888 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -E '[::ffff:209.17.116.163]:http' | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 ::ffff:209\.17\.116\.163 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E '[::ffff:209.17.116.163]:tftp' | egrep -q '^internettool L2 6 L3 u FE 0\.0\.0\.0 ::ffff:209\.17\.116\.163 69 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E '[::ffff:209.17.116.163]:time' | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 ::ffff:209\.17\.116\.163 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E '[::ffff:209.17.116.163]:time' -t | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 ::ffff:209\.17\.116\.163 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E '[::ffff:209.17.116.163]:time' -u | egrep -q '^internettool L2 6 L3 u FE 0\.0\.0\.0 ::ffff:209\.17\.116\.163 37 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -E diag.com | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 0 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E diag.com -4 | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 0 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E diag.com -6 | egrep -q '^internettool L2 6 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 0 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -E diag.com:80 | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E diag.com:8888 | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E diag.com:8888 -t | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E diag.com:8888 -u | egrep -q '^internettool L2 4 L3 u FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 8888 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -E diag.com:http | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E diag.com:tftp | egrep -q '^internettool L2 4 L3 u FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 69 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E diag.com:time | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E diag.com:time -t | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E diag.com:time -u | egrep -q '^internettool L2 4 L3 u FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 37 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -E google.com | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 0 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E google.com -4 | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 0 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E google.com -6 | egrep -q '^internettool L2 6 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 0 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -E google.com:80 | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E google.com:8888 | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E google.com:8888 -t | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E google.com:8888 -u | egrep -q '^internettool L2 4 L3 u FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 8888 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -E google.com:http | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E google.com:tftp | egrep -q '^internettool L2 4 L3 u FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 69 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E google.com:time | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E google.com:time -t | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E google.com:time -u | egrep -q '^internettool L2 4 L3 u FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 37 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -E prairiethorn.org | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 0 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E prairiethorn.org -4 | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 0 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E prairiethorn.org -6 | egrep -q '^internettool L2 6 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 0 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -E prairiethorn.org:80 | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E prairiethorn.org:8888 | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E prairiethorn.org:8888 -t | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E prairiethorn.org:8888 -u | egrep -q '^internettool L2 4 L3 u FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 8888 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -E prairiethorn.org:http | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E prairiethorn.org:tftp | egrep -q '^internettool L2 4 L3 u FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 69 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E prairiethorn.org:time | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E prairiethorn.org:time -t | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -E prairiethorn.org:time -u | egrep -q '^internettool L2 4 L3 u FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 37 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -A 206.178.189.131 | egrep -q '^internettool L2 4 L3 t FE 206\.178\.189\.131 :: 0 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -A 206.178.189.131 -P 80 | egrep -q '^internettool L2 4 L3 t FE 206\.178\.189\.131 :: 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A 206.178.189.131 -P 8888 | egrep -q '^internettool L2 4 L3 t FE 206\.178\.189\.131 :: 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A 206.178.189.131 -P 8888 -t | egrep -q '^internettool L2 4 L3 t FE 206\.178\.189\.131 :: 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A 206.178.189.131 -P 8888 -u | egrep -q '^internettool L2 4 L3 u FE 206\.178\.189\.131 :: 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A 206.178.189.131 -P http | egrep -q '^internettool L2 4 L3 t FE 206\.178\.189\.131 :: 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A 206.178.189.131 -P http -t | egrep -q '^internettool L2 4 L3 t FE 206\.178\.189\.131 :: 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A 206.178.189.131 -P tftp -u | egrep -q '^internettool L2 4 L3 u FE 206\.178\.189\.131 :: 0 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A 206.178.189.131 -P time | egrep -q '^internettool L2 4 L3 t FE 206\.178\.189\.131 :: 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A 206.178.189.131 -P time -t | egrep -q '^internettool L2 4 L3 t FE 206\.178\.189\.131 :: 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A 206.178.189.131 -P time -u | egrep -q '^internettool L2 4 L3 u FE 206\.178\.189\.131 :: 37 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -A 206.178.189.131 -4 | egrep -q '^internettool L2 4 L3 t FE 206\.178\.189\.131 :: 0 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -A 206.178.189.131 -4 -P 80 | egrep -q '^internettool L2 4 L3 t FE 206\.178\.189\.131 :: 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A 206.178.189.131 -4 -P 8888 | egrep -q '^internettool L2 4 L3 t FE 206\.178\.189\.131 :: 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A 206.178.189.131 -4 -P 8888 -t | egrep -q '^internettool L2 4 L3 t FE 206\.178\.189\.131 :: 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A 206.178.189.131 -4 -P 8888 -u | egrep -q '^internettool L2 4 L3 u FE 206\.178\.189\.131 :: 8888 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -A 206.178.189.131 -4 -P http | egrep -q '^internettool L2 4 L3 t FE 206\.178\.189\.131 :: 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A 206.178.189.131 -4 -P http -t | egrep -q '^internettool L2 4 L3 t FE 206\.178\.189\.131 :: 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A 206.178.189.131 -4 -P tftp -u | egrep -q '^internettool L2 4 L3 u FE 206\.178\.189\.131 :: 0 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A 206.178.189.131 -4 -P time | egrep -q '^internettool L2 4 L3 t FE 206\.178\.189\.131 :: 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A 206.178.189.131 -4 -P time -t | egrep -q '^internettool L2 4 L3 t FE 206\.178\.189\.131 :: 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A 206.178.189.131 -4 -P time -u | egrep -q '^internettool L2 4 L3 u FE 206\.178\.189\.131 :: 37 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -A 2607:f8b0:400f:805::200e -6 | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 2607:f8b0:400f:805::200e 0 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -A 2607:f8b0:400f:805::200e -6 -P 80 | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 2607:f8b0:400f:805::200e 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A 2607:f8b0:400f:805::200e -6 -P 8888 -t | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 2607:f8b0:400f:805::200e 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A 2607:f8b0:400f:805::200e -6 -P 8888 -u | egrep -q '^internettool L2 6 L3 u FE 0\.0\.0\.0 2607:f8b0:400f:805::200e 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A 2607:f8b0:400f:805::200e -6 -P 8888 | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 2607:f8b0:400f:805::200e 8888 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -A 2607:f8b0:400f:805::200e -6 -P http | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 2607:f8b0:400f:805::200e 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A 2607:f8b0:400f:805::200e -6 -P http -t | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 2607:f8b0:400f:805::200e 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A 2607:f8b0:400f:805::200e -6 -P tftp -u | egrep -q '^internettool L2 6 L3 u FE 0\.0\.0\.0 2607:f8b0:400f:805::200e 0 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A 2607:f8b0:400f:805::200e -6 -P time | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 2607:f8b0:400f:805::200e 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A 2607:f8b0:400f:805::200e -6 -P time -t | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 2607:f8b0:400f:805::200e 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A 2607:f8b0:400f:805::200e -6 -P time -u | egrep -q '^internettool L2 6 L3 u FE 0\.0\.0\.0 2607:f8b0:400f:805::200e 37 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -A ::ffff:209.17.116.163 -6 | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 ::ffff:209\.17\.116\.163 0 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -A ::ffff:209.17.116.163 -6 -P 80 | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 ::ffff:209\.17\.116\.163 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A ::ffff:209.17.116.163 -6 -P 8888 | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 ::ffff:209\.17\.116\.163 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A ::ffff:209.17.116.163 -6 -P 8888 -t | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 ::ffff:209\.17\.116\.163 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A ::ffff:209.17.116.163 -6 -P 8888 -u | egrep -q '^internettool L2 6 L3 u FE 0\.0\.0\.0 ::ffff:209\.17\.116\.163 8888 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -A ::ffff:209.17.116.163 -6 -P http | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 ::ffff:209\.17\.116\.163 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A ::ffff:209.17.116.163 -6 -P http -t | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 ::ffff:209\.17\.116\.163 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A ::ffff:209.17.116.163 -6 -P tftp -u | egrep -q '^internettool L2 6 L3 u FE 0\.0\.0\.0 ::ffff:209\.17\.116\.163 0 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A ::ffff:209.17.116.163 -6 -P time | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 ::ffff:209\.17\.116\.163 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A ::ffff:209.17.116.163 -6 -P time -t | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 ::ffff:209\.17\.116\.163 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A ::ffff:209.17.116.163 -6 -P time -u | egrep -q '^internettool L2 6 L3 u FE 0\.0\.0\.0 ::ffff:209\.17\.116\.163 37 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -A diag.com | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 0 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A diag.com -4 | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 0 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A diag.com -6 | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 ::ffff:.* 0 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -A diag.com -P 80 | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A diag.com -P 8888 | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A diag.com -P 8888 -t | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A diag.com -P 8888 -u | egrep -q '^internettool L2 4 L3 u FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 8888 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -A diag.com -P http | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A diag.com -P http -t | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A diag.com -P tftp -u | egrep -q '^internettool L2 4 L3 u FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 0 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A diag.com -P time | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A diag.com -P time -t | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A diag.com -P time -u | egrep -q '^internettool L2 4 L3 u FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 37 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -A google.com | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 0 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -4 | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 0 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -6 | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 0 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -A google.com -P 80 | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -P 8888 | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -P 8888 -t | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -P 8888 -u | egrep -q '^internettool L2 4 L3 u FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -P 80 -4 | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -P 8888 -4 | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -P 8888 -4 -t | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -P 8888 -4 -u | egrep -q '^internettool L2 4 L3 u FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -P 80 -6 | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -P 8888 -6 | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -P 8888 -6 -t | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -P 8888 -6 -u | egrep -q '^internettool L2 6 L3 u FE 0\.0\.0\.0 ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 8888 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -A google.com -P http | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -P http -t | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -P tftp -u | egrep -q '^internettool L2 4 L3 u FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 0 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -P time | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -P time -t | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -P time -u | egrep -q '^internettool L2 4 L3 u FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -P http -4 | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -P http -4 -t | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -P tftp -4 -u | egrep -q '^internettool L2 4 L3 u FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 0 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -P time -4 | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -P time -4 -t | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -P time -4 -u | egrep -q '^internettool L2 4 L3 u FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -P http -6 | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -P http -6 -t | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -P tftp -6 -u | egrep -q '^internettool L2 6 L3 u FE 0\.0\.0\.0 ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 0 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -P time -6 | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -P time -6 -t | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A google.com -P time -6 -u | egrep -q '^internettool L2 6 L3 u FE 0\.0\.0\.0 ([0-9a-f][0-9a-f]*:[0-9a-f:]*:[0-9a-f][0-9a-f]*|::ffff:[1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]*) 37 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -A prairiethorn.org | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 0 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A prairiethorn.org -4 | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 0 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A prairiethorn.org -6 | egrep -q '^internettool L2 6 L3 t FE 0\.0\.0\.0 ::ffff:.* 0 NE 0\.0\.0\.0 :: 0$' || exit 2

internettool -x -A prairiethorn.org -P 80 | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A prairiethorn.org -P 8888 | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A prairiethorn.org -P 8888 -t | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A prairiethorn.org -P 8888 -u | egrep -q '^internettool L2 4 L3 u FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 8888 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A prairiethorn.org -P http | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A prairiethorn.org -P http -t | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 80 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A prairiethorn.org -P tftp -u | egrep -q '^internettool L2 4 L3 u FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 0 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A prairiethorn.org -P time | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A prairiethorn.org -P time -t | egrep -q '^internettool L2 4 L3 t FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 37 NE 0\.0\.0\.0 :: 0$' || exit 2
internettool -x -A prairiethorn.org -P time -u | egrep -q '^internettool L2 4 L3 u FE [1-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\.[1-9][0-9]* :: 37 NE 0\.0\.0\.0 :: 0$' || exit 2

# CONNECTIVITY TESTS

internettool -4 -t -e :${PORT} & SERVERPID=$!
sleep ${WAIT}
timesource | internettool -t -E localhost:${PORT} | timesink | head -${LOOP} || exit 3
kill -9 ${SERVERPID}
sleep ${WAIT}
PORT=$((${PORT} + 1))

internettool -4 -t -p ${PORT} & SERVERPID=$!
sleep ${WAIT}
timesource | internettool -t -A localhost -P ${PORT} | timesink | head -${LOOP} || exit 3
kill -9 ${SERVERPID}
sleep ${WAIT}
PORT=$((${PORT} + 1))

internettool -4 -u -e :${PORT} & SERVERPID=$!
sleep ${WAIT}
timesource | internettool -u -E localhost:${PORT} | timesink | head -${LOOP} || exit 3
kill -9 ${SERVERPID}
sleep ${WAIT}
PORT=$((${PORT} + 1))

internettool -4 -u -p ${PORT} & SERVERPID=$!
sleep ${WAIT}
timesource | internettool -u -A localhost -P ${PORT} | timesink | head -${LOOP} || exit 3
kill -9 ${SERVERPID}
sleep ${WAIT}
PORT=$((${PORT} + 1))

internettool -6 -t -e :${PORT} & SERVERPID=$!
sleep ${WAIT}
timesource | internettool -t -E ip6-localhost:${PORT} | timesink | head -${LOOP} || exit 3
kill -9 ${SERVERPID}
sleep ${WAIT}
PORT=$((${PORT} + 1))

internettool -6 -t -p ${PORT} & SERVERPID=$!
sleep ${WAIT}
timesource | internettool -t -A ip6-localhost -P ${PORT} | timesink | head -${LOOP} || exit 3
kill -9 ${SERVERPID}
sleep ${WAIT}
PORT=$((${PORT} + 1))

internettool -6 -u -e :${PORT} & SERVERPID=$!
sleep ${WAIT}
timesource | internettool -u -E ip6-localhost:${PORT} | timesink | head -${LOOP} || exit 3
kill -9 ${SERVERPID}
sleep ${WAIT}
PORT=$((${PORT} + 1))

internettool -6 -u -p ${PORT} & SERVERPID=$!
sleep ${WAIT}
timesource | internettool -u -A ip6-localhost -P ${PORT} | timesink | head -${LOOP} || exit 3
kill -9 ${SERVERPID}
sleep ${WAIT}
PORT=$((${PORT} + 1))

if [[ ${EUID} == 0 ]]; then

	internettool -g -e localhost -i lo 2>&1 | egrep ' role=ping action=reply ' | head -${LOOP} || exit 4
	sleep ${WAIT}

	internettool -g -e ip6-localhost -i lo 2>&1 | egrep ' role=ping action=reply ' | head -${LOOP} || exit 4
	sleep ${WAIT}

fi

echo "Success."
exit 0
