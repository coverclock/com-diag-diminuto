#!/bin/bash
# Copyright 2022 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# USAGE
#
# internettooltests [ PORTNUMBER [ LOOPCOUNT [ SECONDS [ "root" ] ] ] ]
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
# Specifying "root" as the fourth argument causes the script to run the
# tool with the ICMP (ping) option. This must be run as root. If the
# idea of a shell script bring run as root doing kill -9 commands
# doesn't concern you, you aren't thinking clearly.
#
# EXAMPLES
#
# internettooltests
#
# internettooltests 5555 5 2
#
# sudo su
# . out/host/bin/setup
# internettooltests 5555 5 2 root
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

internettool -?

internettool -I

# PARSING TESTS

internettool -x -e 80                                       | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 :: 80"                      || exit 1
internettool -x -e 8888                                     | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 :: 8888"                    || exit 1
internettool -x -e 8888 -t                                  | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 :: 8888"                    || exit 1
internettool -x -e 8888 -u                                  | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 127.0.0.1 ::1 0 NE 0.0.0.0 :: 8888"                    || exit 1

internettool -x -e :80                                      | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 :: 80"                      || exit 1
internettool -x -e :8888                                    | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 :: 8888"                    || exit 1
internettool -x -e :8888 -t                                 | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 :: 8888"                    || exit 1
internettool -x -e :8888 -u                                 | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 127.0.0.1 ::1 0 NE 0.0.0.0 :: 8888"                    || exit 1

internettool -x -e :http                                    | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 :: 80"                      || exit 1
internettool -x -e :tftp                                    | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 127.0.0.1 ::1 0 NE 0.0.0.0 :: 69"                      || exit 1
internettool -x -e :time                                    | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 :: 37"                      || exit 1
internettool -x -e :time -t                                 | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 :: 37"                      || exit 1
internettool -x -e :time -u                                 | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 127.0.0.1 ::1 0 NE 0.0.0.0 :: 37"                      || exit 1

internettool -x -e localhost:80                             | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 127.0.0.1 ::1 80"                   || exit 1
internettool -x -e localhost:8888                           | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 127.0.0.1 ::1 8888"                 || exit 1
internettool -x -e localhost:8888 -t                        | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 127.0.0.1 ::1 8888"                 || exit 1
internettool -x -e localhost:8888 -u                        | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 127.0.0.1 ::1 0 NE 127.0.0.1 ::1 8888"                 || exit 1
internettool -x -e localhost:http                           | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 127.0.0.1 ::1 80"                   || exit 1
internettool -x -e localhost:tftp                           | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 127.0.0.1 ::1 0 NE 127.0.0.1 ::1 69"                   || exit 1
internettool -x -e localhost:time                           | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 127.0.0.1 ::1 37"                   || exit 1
internettool -x -e localhost:time -t                        | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 127.0.0.1 ::1 37"                   || exit 1
internettool -x -e localhost:time -u                        | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 127.0.0.1 ::1 0 NE 127.0.0.1 ::1 37"                   || exit 1

internettool -x -e google.com:80                            | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE .* .* 80"                           || exit 1
internettool -x -e google.com:8888                          | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE .* .* 8888"                         || exit 1
internettool -x -e google.com:8888 -t                       | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE .* .* 8888"                         || exit 1
internettool -x -e google.com:8888 -u                       | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 127.0.0.1 ::1 0 NE .* .* 8888"                         || exit 1
internettool -x -e google.com:http                          | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE .* .* 80"                           || exit 1
internettool -x -e google.com:tftp                          | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 127.0.0.1 ::1 0 NE .* .* 69"                           || exit 1
internettool -x -e google.com:time                          | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE .* .* 37"                           || exit 1
internettool -x -e google.com:time -t                       | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE .* .* 37"                           || exit 1
internettool -x -e google.com:time -u                       | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 127.0.0.1 ::1 0 NE .* .* 37"                           || exit 1

internettool -x -p 80                                       | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 :: 80"                      || exit 1
internettool -x -p 8888                                     | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 :: 8888"                    || exit 1
internettool -x -p 8888 -t                                  | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 :: 8888"                    || exit 1
internettool -x -p 8888 -u                                  | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 127.0.0.1 ::1 0 NE 0.0.0.0 :: 8888"                    || exit 1

internettool -x -p http                                     | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 :: 80"                      || exit 1
internettool -x -p http -t                                  | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 :: 80"                      || exit 1
internettool -x -p tftp -u                                  | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 127.0.0.1 ::1 0 NE 0.0.0.0 :: 69"                      || exit 1
internettool -x -p time                                     | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 :: 37"                      || exit 1
internettool -x -p time -t                                  | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 :: 37"                      || exit 1
internettool -x -p time -u                                  | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 127.0.0.1 ::1 0 NE 0.0.0.0 :: 37"                      || exit 1

internettool -x -a localhost -p 80                          | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 127.0.0.1 :: 80"                    || exit 1
internettool -x -a localhost -p 8888                        | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 127.0.0.1 :: 8888"                  || exit 1
internettool -x -a localhost -p 8888 -t                     | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 127.0.0.1 :: 8888"                  || exit 1
internettool -x -a localhost -p 8888 -u                     | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 127.0.0.1 ::1 0 NE 127.0.0.1 :: 8888"                  || exit 1

internettool -x -a localhost -p http                        | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 127.0.0.1 :: 80"                    || exit 1
internettool -x -a localhost -p http -t                     | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 127.0.0.1 :: 80"                    || exit 1
internettool -x -a localhost -p tftp -u                     | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 127.0.0.1 ::1 0 NE 127.0.0.1 :: 69"                    || exit 1
internettool -x -a localhost -p time                        | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 127.0.0.1 :: 37"                    || exit 1
internettool -x -a localhost -p time -t                     | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 127.0.0.1 :: 37"                    || exit 1
internettool -x -a localhost -p time -u                     | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 127.0.0.1 ::1 0 NE 127.0.0.1 :: 37"                    || exit 1

internettool -x -a localhost -4 -p 80                       | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 127.0.0.1 :: 80"                    || exit 1
internettool -x -a localhost -4 -p 8888                     | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 127.0.0.1 :: 8888"                  || exit 1
internettool -x -a localhost -4 -p 8888 -t                  | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 127.0.0.1 :: 8888"                  || exit 1
internettool -x -a localhost -4 -p 8888 -u                  | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 127.0.0.1 ::1 0 NE 127.0.0.1 :: 8888"                  || exit 1

internettool -x -a localhost -4 -p http                     | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 127.0.0.1 :: 80"                    || exit 1
internettool -x -a localhost -4 -p http -t                  | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 127.0.0.1 :: 80"                    || exit 1
internettool -x -a localhost -4 -p tftp -u                  | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 127.0.0.1 ::1 0 NE 127.0.0.1 :: 69"                    || exit 1
internettool -x -a localhost -4 -p time                     | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 127.0.0.1 :: 37"                    || exit 1
internettool -x -a localhost -4 -p time -t                  | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE 127.0.0.1 :: 37"                    || exit 1
internettool -x -a localhost -4 -p time -u                  | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 127.0.0.1 ::1 0 NE 127.0.0.1 :: 37"                    || exit 1

internettool -x -a localhost -6 -p 80                       | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 ::1 80"                     || exit 1
internettool -x -a localhost -6 -p 8888                     | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 ::1 8888"                   || exit 1
internettool -x -a localhost -6 -p 8888 -t                  | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 ::1 8888"                   || exit 1
internettool -x -a localhost -6 -p 8888 -u                  | tee /dev/stderr | grep -q "internettool L2 6 L3 u FE 127.0.0.1 ::1 0 NE 0.0.0.0 ::1 8888"                   || exit 1

internettool -x -a localhost -6 -p http                     | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 ::1 80"                     || exit 1
internettool -x -a localhost -6 -p http -t                  | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 ::1 80"                     || exit 1
internettool -x -a localhost -6 -p tftp -u                  | tee /dev/stderr | grep -q "internettool L2 6 L3 u FE 127.0.0.1 ::1 0 NE 0.0.0.0 ::1 69"                     || exit 1
internettool -x -a localhost -6 -p time                     | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 ::1 37"                     || exit 1
internettool -x -a localhost -6 -p time -t                  | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 ::1 37"                     || exit 1
internettool -x -a localhost -6 -p time -u                  | tee /dev/stderr | grep -q "internettool L2 6 L3 u FE 127.0.0.1 ::1 0 NE 0.0.0.0 ::1 37"                     || exit 1

internettool -x -a google.com -p 80                         | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE .* :: 80"                           || exit 1
internettool -x -a google.com -p 8888                       | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE .* :: 8888"                         || exit 1
internettool -x -a google.com -p 8888 -t                    | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE .* :: 8888"                         || exit 1
internettool -x -a google.com -p 8888 -u                    | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 127.0.0.1 ::1 0 NE .* :: 8888"                         || exit 1

internettool -x -a google.com -p http                       | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE .* :: 80"                           || exit 1
internettool -x -a google.com -p http -t                    | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE .* :: 80"                           || exit 1
internettool -x -a google.com -p tftp -u                    | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 127.0.0.1 ::1 0 NE .* :: 69"                           || exit 1
internettool -x -a google.com -p time                       | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE .* :: 37"                           || exit 1
internettool -x -a google.com -p time -t                    | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE .* :: 37"                           || exit 1
internettool -x -a google.com -p time -u                    | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 127.0.0.1 ::1 0 NE .* :: 37"                           || exit 1

internettool -x -a google.com -4 -p 80                      | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE .* :: 80"                           || exit 1
internettool -x -a google.com -4 -p 8888                    | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE .* :: 8888"                         || exit 1
internettool -x -a google.com -4 -p 8888 -t                 | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE .* :: 8888"                         || exit 1
internettool -x -a google.com -4 -p 8888 -u                 | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 127.0.0.1 ::1 0 NE .* :: 8888"                         || exit 1

internettool -x -a google.com -4 -p http                    | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE .* :: 80"                           || exit 1
internettool -x -a google.com -4 -p http -t                 | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE .* :: 80"                           || exit 1
internettool -x -a google.com -4 -p tftp -u                 | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 127.0.0.1 ::1 0 NE .* :: 69"                           || exit 1
internettool -x -a google.com -4 -p time                    | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE .* :: 37"                           || exit 1
internettool -x -a google.com -4 -p time -t                 | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 127.0.0.1 ::1 0 NE .* :: 37"                           || exit 1
internettool -x -a google.com -4 -p time -u                 | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 127.0.0.1 ::1 0 NE .* :: 37"                           || exit 1

internettool -x -a google.com -6 -p 80                      | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 .* 80"                      || exit 1
internettool -x -a google.com -6 -p 8888                    | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 .* 8888"                    || exit 1
internettool -x -a google.com -6 -p 8888 -t                 | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 .* 8888"                    || exit 1
internettool -x -a google.com -6 -p 8888 -u                 | tee /dev/stderr | grep -q "internettool L2 6 L3 u FE 127.0.0.1 ::1 0 NE 0.0.0.0 .* 8888"                    || exit 1

internettool -x -a google.com -6 -p http                    | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 .* 80"                      || exit 1
internettool -x -a google.com -6 -p http -t                 | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 .* 80"                      || exit 1
internettool -x -a google.com -6 -p tftp -u                 | tee /dev/stderr | grep -q "internettool L2 6 L3 u FE 127.0.0.1 ::1 0 NE 0.0.0.0 .* 69"                      || exit 1
internettool -x -a google.com -6 -p time                    | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 .* 37"                      || exit 1
internettool -x -a google.com -6 -p time -t                 | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 127.0.0.1 ::1 0 NE 0.0.0.0 .* 37"                      || exit 1
internettool -x -a google.com -6 -p time -u                 | tee /dev/stderr | grep -q "internettool L2 6 L3 u FE 127.0.0.1 ::1 0 NE 0.0.0.0 .* 37"                      || exit 1

internettool -x -E 206.178.189.131                          | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 206.178.189.131 :: 0 NE 0.0.0.0 :: 0"                  || exit 1

internettool -x -E 206.178.189.131:80                       | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 206.178.189.131 :: 80 NE 0.0.0.0 :: 0"                 || exit 1
internettool -x -E 206.178.189.131:8888                     | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 206.178.189.131 :: 8888 NE 0.0.0.0 :: 0"               || exit 1
internettool -x -E 206.178.189.131:8888 -t                  | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 206.178.189.131 :: 8888 NE 0.0.0.0 :: 0"               || exit 1
internettool -x -E 206.178.189.131:8888 -u                  | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 206.178.189.131 :: 8888 NE 0.0.0.0 :: 0"               || exit 1

internettool -x -E 206.178.189.131:http                     | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 206.178.189.131 :: 80 NE 0.0.0.0 :: 0"                 || exit 1
internettool -x -E 206.178.189.131:tftp                     | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 206.178.189.131 :: 69 NE 0.0.0.0 :: 0"                 || exit 1
internettool -x -E 206.178.189.131:time                     | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 206.178.189.131 :: 37 NE 0.0.0.0 :: 0"                 || exit 1
internettool -x -E 206.178.189.131:time -t                  | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 206.178.189.131 :: 37 NE 0.0.0.0 :: 0"                 || exit 1
internettool -x -E 206.178.189.131:time -u                  | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 206.178.189.131 :: 37 NE 0.0.0.0 :: 0"                 || exit 1

internettool -x -E '[2607:f8b0:400f:805::200e]'             | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 2607:f8b0:400f:805::200e 0 NE 0.0.0.0 :: 0"    || exit 1

internettool -x -E '[2607:f8b0:400f:805::200e]:80'          | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 2607:f8b0:400f:805::200e 80 NE 0.0.0.0 :: 0"   || exit 1
internettool -x -E '[2607:f8b0:400f:805::200e]:8888' -t     | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 2607:f8b0:400f:805::200e 8888 NE 0.0.0.0 :: 0" || exit 1
internettool -x -E '[2607:f8b0:400f:805::200e]:8888' -u     | tee /dev/stderr | grep -q "internettool L2 6 L3 u FE 0.0.0.0 2607:f8b0:400f:805::200e 8888 NE 0.0.0.0 :: 0" || exit 1
internettool -x -E '[2607:f8b0:400f:805::200e]:8888'        | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 2607:f8b0:400f:805::200e 8888 NE 0.0.0.0 :: 0" || exit 1

internettool -x -E '[2607:f8b0:400f:805::200e]:http'        | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 2607:f8b0:400f:805::200e 80 NE 0.0.0.0 :: 0"   || exit 1
internettool -x -E '[2607:f8b0:400f:805::200e]:tftp'        | tee /dev/stderr | grep -q "internettool L2 6 L3 u FE 0.0.0.0 2607:f8b0:400f:805::200e 69 NE 0.0.0.0 :: 0"   || exit 1
internettool -x -E '[2607:f8b0:400f:805::200e]:time'        | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 2607:f8b0:400f:805::200e 37 NE 0.0.0.0 :: 0"   || exit 1
internettool -x -E '[2607:f8b0:400f:805::200e]:time' -t     | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 2607:f8b0:400f:805::200e 37 NE 0.0.0.0 :: 0"   || exit 1
internettool -x -E '[2607:f8b0:400f:805::200e]:time' -u     | tee /dev/stderr | grep -q "internettool L2 6 L3 u FE 0.0.0.0 2607:f8b0:400f:805::200e 37 NE 0.0.0.0 :: 0"   || exit 1

internettool -x -E '[::ffff:209.17.116.163]'                | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 ::ffff:209.17.116.163 0 NE 0.0.0.0 :: 0"       || exit 1

internettool -x -E '[::ffff:209.17.116.163]:80'             | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 ::ffff:209.17.116.163 80 NE 0.0.0.0 :: 0"      || exit 1
internettool -x -E '[::ffff:209.17.116.163]:8888'           | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 ::ffff:209.17.116.163 8888 NE 0.0.0.0 :: 0"    || exit 1
internettool -x -E '[::ffff:209.17.116.163]:8888' -t        | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 ::ffff:209.17.116.163 8888 NE 0.0.0.0 :: 0"    || exit 1
internettool -x -E '[::ffff:209.17.116.163]:8888' -u        | tee /dev/stderr | grep -q "internettool L2 6 L3 u FE 0.0.0.0 ::ffff:209.17.116.163 8888 NE 0.0.0.0 :: 0"    || exit 1

internettool -x -E '[::ffff:209.17.116.163]:http'           | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 ::ffff:209.17.116.163 80 NE 0.0.0.0 :: 0"      || exit 1
internettool -x -E '[::ffff:209.17.116.163]:tftp'           | tee /dev/stderr | grep -q "internettool L2 6 L3 u FE 0.0.0.0 ::ffff:209.17.116.163 69 NE 0.0.0.0 :: 0"      || exit 1
internettool -x -E '[::ffff:209.17.116.163]:time'           | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 ::ffff:209.17.116.163 37 NE 0.0.0.0 :: 0"      || exit 1
internettool -x -E '[::ffff:209.17.116.163]:time' -t        | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 ::ffff:209.17.116.163 37 NE 0.0.0.0 :: 0"      || exit 1
internettool -x -E '[::ffff:209.17.116.163]:time' -u        | tee /dev/stderr | grep -q "internettool L2 6 L3 u FE 0.0.0.0 ::ffff:209.17.116.163 37 NE 0.0.0.0 :: 0"      || exit 1

internettool -x -E diag.com                                 | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* ::ffff:209.17.116.163 0 NE 0.0.0.0 :: 0"            || exit 1
internettool -x -E diag.com -4                              | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* ::ffff:209.17.116.163 0 NE 0.0.0.0 :: 0"            || exit 1
internettool -x -E diag.com -6                              | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE .* ::ffff:209.17.116.163 0 NE 0.0.0.0 :: 0"            || exit 1

internettool -x -E diag.com:80                              | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* ::ffff:209.17.116.163 80 NE 0.0.0.0 :: 0"           || exit 1
internettool -x -E diag.com:8888                            | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* ::ffff:209.17.116.163 8888 NE 0.0.0.0 :: 0"         || exit 1
internettool -x -E diag.com:8888 -t                         | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* ::ffff:209.17.116.163 8888 NE 0.0.0.0 :: 0"         || exit 1
internettool -x -E diag.com:8888 -u                         | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE .* ::ffff:209.17.116.163 8888 NE 0.0.0.0 :: 0"         || exit 1

internettool -x -E diag.com:http                            | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* ::ffff:209.17.116.163 80 NE 0.0.0.0 :: 0"           || exit 1
internettool -x -E diag.com:tftp                            | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE .* ::ffff:209.17.116.163 69 NE 0.0.0.0 :: 0"           || exit 1
internettool -x -E diag.com:time                            | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* ::ffff:209.17.116.163 37 NE 0.0.0.0 :: 0"           || exit 1
internettool -x -E diag.com:time -t                         | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* ::ffff:209.17.116.163 37 NE 0.0.0.0 :: 0"           || exit 1
internettool -x -E diag.com:time -u                         | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE .* ::ffff:209.17.116.163 37 NE 0.0.0.0 :: 0"           || exit 1

internettool -x -E google.com                               | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* .* 0 NE 0.0.0.0 :: 0"                               || exit 1
internettool -x -E google.com -4                            | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* .* 0 NE 0.0.0.0 :: 0"                               || exit 1
internettool -x -E google.com -6                            | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE .* .* 0 NE 0.0.0.0 :: 0"                               || exit 1

internettool -x -E google.com:80                            | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* .* 80 NE 0.0.0.0 :: 0"                              || exit 1
internettool -x -E google.com:8888                          | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* .* 8888 NE 0.0.0.0 :: 0"                            || exit 1
internettool -x -E google.com:8888 -t                       | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* .* 8888 NE 0.0.0.0 :: 0"                            || exit 1
internettool -x -E google.com:8888 -u                       | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE .* .* 8888 NE 0.0.0.0 :: 0"                            || exit 1

internettool -x -E google.com:http                          | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* .* 80 NE 0.0.0.0 :: 0"                              || exit 1
internettool -x -E google.com:tftp                          | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE .* .* 69 NE 0.0.0.0 :: 0"                              || exit 1
internettool -x -E google.com:time                          | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* .* 37 NE 0.0.0.0 :: 0"                              || exit 1
internettool -x -E google.com:time -t                       | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* .* 37 NE 0.0.0.0 :: 0"                              || exit 1
internettool -x -E google.com:time -u                       | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE .* .* 37 NE 0.0.0.0 :: 0"                              || exit 1

internettool -x -E prairiethorn.org                         | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* ::ffff:205.178.189.131 0 NE 0.0.0.0 :: 0"           || exit 1
internettool -x -E prairiethorn.org -4                      | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* ::ffff:205.178.189.131 0 NE 0.0.0.0 :: 0"           || exit 1
internettool -x -E prairiethorn.org -6                      | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE .* ::ffff:205.178.189.131 0 NE 0.0.0.0 :: 0"           || exit 1

internettool -x -E prairiethorn.org:80                      | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* ::ffff:205.178.189.131 80 NE 0.0.0.0 :: 0"          || exit 1
internettool -x -E prairiethorn.org:8888                    | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* ::ffff:205.178.189.131 8888 NE 0.0.0.0 :: 0"        || exit 1
internettool -x -E prairiethorn.org:8888 -t                 | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* ::ffff:205.178.189.131 8888 NE 0.0.0.0 :: 0"        || exit 1
internettool -x -E prairiethorn.org:8888 -u                 | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE .* ::ffff:205.178.189.131 8888 NE 0.0.0.0 :: 0"        || exit 1

internettool -x -E prairiethorn.org:http                    | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* ::ffff:205.178.189.131 80 NE 0.0.0.0 :: 0"          || exit 1
internettool -x -E prairiethorn.org:tftp                    | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE .* ::ffff:205.178.189.131 69 NE 0.0.0.0 :: 0"          || exit 1
internettool -x -E prairiethorn.org:time                    | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* ::ffff:205.178.189.131 37 NE 0.0.0.0 :: 0"          || exit 1
internettool -x -E prairiethorn.org:time -t                 | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* ::ffff:205.178.189.131 37 NE 0.0.0.0 :: 0"          || exit 1
internettool -x -E prairiethorn.org:time -u                 | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE .* ::ffff:205.178.189.131 37 NE 0.0.0.0 :: 0"          || exit 1

internettool -x -A 206.178.189.131                          | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 206.178.189.131 :: 0 NE 0.0.0.0 :: 0"                  || exit 1

internettool -x -A 206.178.189.131 -P 80                    | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 206.178.189.131 :: 80 NE 0.0.0.0 :: 0"                 || exit 1
internettool -x -A 206.178.189.131 -P 8888                  | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 206.178.189.131 :: 8888 NE 0.0.0.0 :: 0"               || exit 1
internettool -x -A 206.178.189.131 -P 8888 -t               | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 206.178.189.131 :: 8888 NE 0.0.0.0 :: 0"               || exit 1
internettool -x -A 206.178.189.131 -P 8888 -u               | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 206.178.189.131 :: 8888 NE 0.0.0.0 :: 0"               || exit 1
internettool -x -A 206.178.189.131 -P http                  | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 206.178.189.131 :: 80 NE 0.0.0.0 :: 0"                 || exit 1
internettool -x -A 206.178.189.131 -P http -t               | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 206.178.189.131 :: 80 NE 0.0.0.0 :: 0"                 || exit 1
internettool -x -A 206.178.189.131 -P tftp -u               | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 206.178.189.131 :: 0 NE 0.0.0.0 :: 0"                  || exit 1
internettool -x -A 206.178.189.131 -P time                  | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 206.178.189.131 :: 37 NE 0.0.0.0 :: 0"                 || exit 1
internettool -x -A 206.178.189.131 -P time -t               | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 206.178.189.131 :: 37 NE 0.0.0.0 :: 0"                 || exit 1
internettool -x -A 206.178.189.131 -P time -u               | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 206.178.189.131 :: 37 NE 0.0.0.0 :: 0"                 || exit 1

internettool -x -A 206.178.189.131 -4                       | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 206.178.189.131 :: 0 NE 0.0.0.0 :: 0"                  || exit 1

internettool -x -A 206.178.189.131 -4 -P 80                 | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 206.178.189.131 :: 80 NE 0.0.0.0 :: 0"                 || exit 1
internettool -x -A 206.178.189.131 -4 -P 8888               | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 206.178.189.131 :: 8888 NE 0.0.0.0 :: 0"               || exit 1
internettool -x -A 206.178.189.131 -4 -P 8888 -t            | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 206.178.189.131 :: 8888 NE 0.0.0.0 :: 0"               || exit 1
internettool -x -A 206.178.189.131 -4 -P 8888 -u            | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 206.178.189.131 :: 8888 NE 0.0.0.0 :: 0"               || exit 1

internettool -x -A 206.178.189.131 -4 -P http               | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 206.178.189.131 :: 80 NE 0.0.0.0 :: 0"                 || exit 1
internettool -x -A 206.178.189.131 -4 -P http -t            | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 206.178.189.131 :: 80 NE 0.0.0.0 :: 0"                 || exit 1
internettool -x -A 206.178.189.131 -4 -P tftp -u            | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 206.178.189.131 :: 0 NE 0.0.0.0 :: 0"                  || exit 1
internettool -x -A 206.178.189.131 -4 -P time               | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 206.178.189.131 :: 37 NE 0.0.0.0 :: 0"                 || exit 1
internettool -x -A 206.178.189.131 -4 -P time -t            | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE 206.178.189.131 :: 37 NE 0.0.0.0 :: 0"                 || exit 1
internettool -x -A 206.178.189.131 -4 -P time -u            | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE 206.178.189.131 :: 37 NE 0.0.0.0 :: 0"                 || exit 1

internettool -x -A 2607:f8b0:400f:805::200e -6              | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 2607:f8b0:400f:805::200e 0 NE 0.0.0.0 :: 0"    || exit 1

internettool -x -A 2607:f8b0:400f:805::200e -6 -P 80        | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 2607:f8b0:400f:805::200e 80 NE 0.0.0.0 :: 0"   || exit 1
internettool -x -A 2607:f8b0:400f:805::200e -6 -P 8888 -t   | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 2607:f8b0:400f:805::200e 8888 NE 0.0.0.0 :: 0" || exit 1
internettool -x -A 2607:f8b0:400f:805::200e -6 -P 8888 -u   | tee /dev/stderr | grep -q "internettool L2 6 L3 u FE 0.0.0.0 2607:f8b0:400f:805::200e 8888 NE 0.0.0.0 :: 0" || exit 1
internettool -x -A 2607:f8b0:400f:805::200e -6 -P 8888      | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 2607:f8b0:400f:805::200e 8888 NE 0.0.0.0 :: 0" || exit 1

internettool -x -A 2607:f8b0:400f:805::200e -6 -P http      | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 2607:f8b0:400f:805::200e 80 NE 0.0.0.0 :: 0"   || exit 1
internettool -x -A 2607:f8b0:400f:805::200e -6 -P http -t   | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 2607:f8b0:400f:805::200e 80 NE 0.0.0.0 :: 0"   || exit 1
internettool -x -A 2607:f8b0:400f:805::200e -6 -P tftp -u   | tee /dev/stderr | grep -q "internettool L2 6 L3 u FE 0.0.0.0 2607:f8b0:400f:805::200e 0 NE 0.0.0.0 :: 0"    || exit 1
internettool -x -A 2607:f8b0:400f:805::200e -6 -P time      | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 2607:f8b0:400f:805::200e 37 NE 0.0.0.0 :: 0"   || exit 1
internettool -x -A 2607:f8b0:400f:805::200e -6 -P time -t   | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 2607:f8b0:400f:805::200e 37 NE 0.0.0.0 :: 0"   || exit 1
internettool -x -A 2607:f8b0:400f:805::200e -6 -P time -u   | tee /dev/stderr | grep -q "internettool L2 6 L3 u FE 0.0.0.0 2607:f8b0:400f:805::200e 37 NE 0.0.0.0 :: 0"   || exit 1

internettool -x -A ::ffff:209.17.116.163 -6                 | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 ::ffff:209.17.116.163 0 NE 0.0.0.0 :: 0"       || exit 1

internettool -x -A ::ffff:209.17.116.163 -6 -P 80           | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 ::ffff:209.17.116.163 80 NE 0.0.0.0 :: 0"      || exit 1
internettool -x -A ::ffff:209.17.116.163 -6 -P 8888         | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 ::ffff:209.17.116.163 8888 NE 0.0.0.0 :: 0"    || exit 1
internettool -x -A ::ffff:209.17.116.163 -6 -P 8888 -t      | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 ::ffff:209.17.116.163 8888 NE 0.0.0.0 :: 0"    || exit 1
internettool -x -A ::ffff:209.17.116.163 -6 -P 8888 -u      | tee /dev/stderr | grep -q "internettool L2 6 L3 u FE 0.0.0.0 ::ffff:209.17.116.163 8888 NE 0.0.0.0 :: 0"    || exit 1

internettool -x -A ::ffff:209.17.116.163 -6 -P http         | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 ::ffff:209.17.116.163 80 NE 0.0.0.0 :: 0"      || exit 1
internettool -x -A ::ffff:209.17.116.163 -6 -P http -t      | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 ::ffff:209.17.116.163 80 NE 0.0.0.0 :: 0"      || exit 1
internettool -x -A ::ffff:209.17.116.163 -6 -P tftp -u      | tee /dev/stderr | grep -q "internettool L2 6 L3 u FE 0.0.0.0 ::ffff:209.17.116.163 0 NE 0.0.0.0 :: 0"       || exit 1
internettool -x -A ::ffff:209.17.116.163 -6 -P time         | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 ::ffff:209.17.116.163 37 NE 0.0.0.0 :: 0"      || exit 1
internettool -x -A ::ffff:209.17.116.163 -6 -P time -t      | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 ::ffff:209.17.116.163 37 NE 0.0.0.0 :: 0"      || exit 1
internettool -x -A ::ffff:209.17.116.163 -6 -P time -u      | tee /dev/stderr | grep -q "internettool L2 6 L3 u FE 0.0.0.0 ::ffff:209.17.116.163 37 NE 0.0.0.0 :: 0"      || exit 1

internettool -x -A diag.com                                 | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 0 NE 0.0.0.0 :: 0"                               || exit 1
internettool -x -A diag.com -4                              | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 0 NE 0.0.0.0 :: 0"                               || exit 1
internettool -x -A diag.com -6                              | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 ::ffff:.* 0 NE 0.0.0.0 :: 0"                   || exit 1

internettool -x -A diag.com -P 80                           | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 80 NE 0.0.0.0 :: 0"                              || exit 1
internettool -x -A diag.com -P 8888                         | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 8888 NE 0.0.0.0 :: 0"                            || exit 1
internettool -x -A diag.com -P 8888 -t                      | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 8888 NE 0.0.0.0 :: 0"                            || exit 1
internettool -x -A diag.com -P 8888 -u                      | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE .* :: 8888 NE 0.0.0.0 :: 0"                            || exit 1

internettool -x -A diag.com -P http                         | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 80 NE 0.0.0.0 :: 0"                              || exit 1
internettool -x -A diag.com -P http -t                      | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 80 NE 0.0.0.0 :: 0"                              || exit 1
internettool -x -A diag.com -P tftp -u                      | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE .* :: 0 NE 0.0.0.0 :: 0"                               || exit 1
internettool -x -A diag.com -P time                         | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 37 NE 0.0.0.0 :: 0"                              || exit 1
internettool -x -A diag.com -P time -t                      | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 37 NE 0.0.0.0 :: 0"                              || exit 1
internettool -x -A diag.com -P time -u                      | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE .* :: 37 NE 0.0.0.0 :: 0"                              || exit 1

internettool -x -A google.com                               | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 0 NE 0.0.0.0 :: 0"                               || exit 1
internettool -x -A google.com -4                            | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 0 NE 0.0.0.0 :: 0"                               || exit 1
internettool -x -A google.com -6                            | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 .* 0 NE 0.0.0.0 :: 0"                          || exit 1

internettool -x -A google.com -P 80                         | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 80 NE 0.0.0.0 :: 0"                              || exit 1
internettool -x -A google.com -P 8888                       | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 8888 NE 0.0.0.0 :: 0"                            || exit 1
internettool -x -A google.com -P 8888 -t                    | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 8888 NE 0.0.0.0 :: 0"                            || exit 1
internettool -x -A google.com -P 8888 -u                    | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE .* :: 8888 NE 0.0.0.0 :: 0"                            || exit 1
internettool -x -A google.com -P 80 -4                      | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 80 NE 0.0.0.0 :: 0"                              || exit 1
internettool -x -A google.com -P 8888 -4                    | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 8888 NE 0.0.0.0 :: 0"                            || exit 1
internettool -x -A google.com -P 8888 -4 -t                 | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 8888 NE 0.0.0.0 :: 0"                            || exit 1
internettool -x -A google.com -P 8888 -4 -u                 | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE .* :: 8888 NE 0.0.0.0 :: 0"                            || exit 1
internettool -x -A google.com -P 80 -6                      | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 .* 80 NE 0.0.0.0 :: 0"                         || exit 1
internettool -x -A google.com -P 8888 -6                    | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 .* 8888 NE 0.0.0.0 :: 0"                       || exit 1
internettool -x -A google.com -P 8888 -6 -t                 | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 .* 8888 NE 0.0.0.0 :: 0"                       || exit 1
internettool -x -A google.com -P 8888 -6 -u                 | tee /dev/stderr | grep -q "internettool L2 6 L3 u FE 0.0.0.0 .* 8888 NE 0.0.0.0 :: 0"                       || exit 1

internettool -x -A google.com -P http                       | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 80 NE 0.0.0.0 :: 0"                              || exit 1
internettool -x -A google.com -P http -t                    | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 80 NE 0.0.0.0 :: 0"                              || exit 1
internettool -x -A google.com -P tftp -u                    | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE .* :: 0 NE 0.0.0.0 :: 0"                               || exit 1
internettool -x -A google.com -P time                       | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 37 NE 0.0.0.0 :: 0"                              || exit 1
internettool -x -A google.com -P time -t                    | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 37 NE 0.0.0.0 :: 0"                              || exit 1
internettool -x -A google.com -P time -u                    | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE .* :: 37 NE 0.0.0.0 :: 0"                              || exit 1
internettool -x -A google.com -P http -4                    | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 80 NE 0.0.0.0 :: 0"                              || exit 1
internettool -x -A google.com -P http -4 -t                 | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 80 NE 0.0.0.0 :: 0"                              || exit 1
internettool -x -A google.com -P tftp -4 -u                 | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE .* :: 0 NE 0.0.0.0 :: 0"                               || exit 1
internettool -x -A google.com -P time -4                    | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 37 NE 0.0.0.0 :: 0"                              || exit 1
internettool -x -A google.com -P time -4 -t                 | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 37 NE 0.0.0.0 :: 0"                              || exit 1
internettool -x -A google.com -P time -4 -u                 | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE .* :: 37 NE 0.0.0.0 :: 0"                              || exit 1
internettool -x -A google.com -P http -6                    | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 .* 80 NE 0.0.0.0 :: 0"                         || exit 1
internettool -x -A google.com -P http -6 -t                 | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 .* 80 NE 0.0.0.0 :: 0"                         || exit 1
internettool -x -A google.com -P tftp -6 -u                 | tee /dev/stderr | grep -q "internettool L2 6 L3 u FE 0.0.0.0 .* 0 NE 0.0.0.0 :: 0"                          || exit 1
internettool -x -A google.com -P time -6                    | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 .* 37 NE 0.0.0.0 :: 0"                         || exit 1
internettool -x -A google.com -P time -6 -t                 | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 .* 37 NE 0.0.0.0 :: 0"                         || exit 1
internettool -x -A google.com -P time -6 -u                 | tee /dev/stderr | grep -q "internettool L2 6 L3 u FE 0.0.0.0 .* 37 NE 0.0.0.0 :: 0"                         || exit 1

internettool -x -A prairiethorn.org                         | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 0 NE 0.0.0.0 :: 0"                               || exit 1
internettool -x -A prairiethorn.org -4                      | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 0 NE 0.0.0.0 :: 0"                               || exit 1
internettool -x -A prairiethorn.org -6                      | tee /dev/stderr | grep -q "internettool L2 6 L3 t FE 0.0.0.0 ::ffff:.* 0 NE 0.0.0.0 :: 0"                   || exit 1

internettool -x -A prairiethorn.org -P 80                   | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 80 NE 0.0.0.0 :: 0"                              || exit 1
internettool -x -A prairiethorn.org -P 8888                 | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 8888 NE 0.0.0.0 :: 0"                            || exit 1
internettool -x -A prairiethorn.org -P 8888 -t              | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 8888 NE 0.0.0.0 :: 0"                            || exit 1
internettool -x -A prairiethorn.org -P 8888 -u              | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE .* :: 8888 NE 0.0.0.0 :: 0"                            || exit 1
internettool -x -A prairiethorn.org -P http                 | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 80 NE 0.0.0.0 :: 0"                              || exit 1
internettool -x -A prairiethorn.org -P http -t              | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 80 NE 0.0.0.0 :: 0"                              || exit 1
internettool -x -A prairiethorn.org -P tftp -u              | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE .* :: 0 NE 0.0.0.0 :: 0"                               || exit 1
internettool -x -A prairiethorn.org -P time                 | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 37 NE 0.0.0.0 :: 0"                              || exit 1
internettool -x -A prairiethorn.org -P time -t              | tee /dev/stderr | grep -q "internettool L2 4 L3 t FE .* :: 37 NE 0.0.0.0 :: 0"                              || exit 1
internettool -x -A prairiethorn.org -P time -u              | tee /dev/stderr | grep -q "internettool L2 4 L3 u FE .* :: 37 NE 0.0.0.0 :: 0"                              || exit 1

# CONNECTIVITY TESTS

internettool -4 -t -e :${PORT} & SERVERPID=$!
timesource | internettool -t -E localhost:${PORT} | timesink | head -${LOOP} || exit 2
kill -9 ${SERVERPID}
sleep ${WAIT}
PORT=$((${PORT} + 1))

internettool -4 -t -p ${PORT} & SERVERPID=$!
timesource | internettool -t -A localhost -P ${PORT} | timesink | head -${LOOP} || exit 2
kill -9 ${SERVERPID}
sleep ${WAIT}
PORT=$((${PORT} + 1))

internettool -4 -u -e :${PORT} & SERVERPID=$!
timesource | internettool -u -E localhost:${PORT} | timesink | head -${LOOP} || exit 2
kill -9 ${SERVERPID}
sleep ${WAIT}
PORT=$((${PORT} + 1))

internettool -4 -u -p ${PORT} & SERVERPID=$!
timesource | internettool -u -A localhost -P ${PORT} | timesink | head -${LOOP} || exit 2
kill -9 ${SERVERPID}
sleep ${WAIT}
PORT=$((${PORT} + 1))

internettool -6 -t -e :${PORT} & SERVERPID=$!
timesource | internettool -t -E ip6-localhost:${PORT} | timesink | head -${LOOP} || exit 2
kill -9 ${SERVERPID}
sleep ${WAIT}
PORT=$((${PORT} + 1))

internettool -6 -t -p ${PORT} & SERVERPID=$!
timesource | internettool -t -A ip6-localhost -P ${PORT} | timesink | head -${LOOP} || exit 2
kill -9 ${SERVERPID}
sleep ${WAIT}
PORT=$((${PORT} + 1))

internettool -6 -u -e :${PORT} & SERVERPID=$!
timesource | internettool -u -E ip6-localhost:${PORT} | timesink | head -${LOOP} || exit 2
kill -9 ${SERVERPID}
sleep ${WAIT}
PORT=$((${PORT} + 1))

internettool -6 -u -p ${PORT} & SERVERPID=$!
timesource | internettool -u -A ip6-localhost -P ${PORT} | timesink | head -${LOOP} || exit 2
kill -9 ${SERVERPID}
sleep ${WAIT}
PORT=$((${PORT} + 1))

if [[ "${ROOT}" == "root" ]]; then

	internettool -g -e localhost -i lo 2>&1 | tee /dev/stderr | grep " role=ping action=reply " | head -${LOOP} || exit 3
	sleep ${WAIT}

	internettool -g -e ip6-localhost -i lo 2>&1 | tee /dev/stderr | grep " role=ping action=reply " | head -${LOOP} || exit 3
	sleep ${WAIT}

fi

echo "Success."
exit 0
