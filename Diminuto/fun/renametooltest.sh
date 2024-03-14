#!/bin/bash
# Copyright 2018-2020 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in LICENSE.txt
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto
#
# ABSTRACT
#
# Test the renametool.
#
# NOTE
#
# This doesn't test the atomic-ness of renametool.
#
# USAGE
#
# renametooltest
#

BASE=$(basename $0)
ROOT="/tmp"

SRCE=$(mktemp ${ROOT}/${BASE}-srce-XXXXXXXXXX)
DEST=$(mktemp -u ${ROOT}/${BASE}-dest-XXXXXXXXXX)
test -f ${SRCE} || exit 1
test -f ${DEST} && exit 2
renametool ${SRCE} ${DEST} || exit 3
test -f ${SRCE} && exit 4
test -f ${DEST} || exit 5
rm -f ${SRCE} ${DEST}
test -f ${SRCE} && exit 1
test -f ${DEST} && exit 2

SRCE=$(mktemp ${ROOT}/${BASE}-srce-XXXXXXXXXX)
DEST=$(mktemp ${ROOT}/${BASE}-dest-XXXXXXXXXX)
test -f ${SRCE} || exit 1
test -f ${DEST} || exit 2
renametool ${SRCE} ${DEST} && exit 3
test -f ${SRCE} || exit 4
test -f ${DEST} || exit 5
rm -f ${SRCE} ${DEST}
test -f ${SRCE} && exit 1
test -f ${DEST} && exit 2

SRCE=$(mktemp ${ROOT}/${BASE}-srce-XXXXXXXXXX)
DEST=$(mktemp ${ROOT}/${BASE}-dest-XXXXXXXXXX)
test -f ${SRCE} || exit 1
test -f ${DEST} || exit 2
echo "1" > ${SRCE}
echo "2" > ${DEST}
test "$(cat ${SRCE})" = "1" || exit 6
test "$(cat ${DEST})" = "2" || exit 7
renametool -x ${SRCE} ${DEST} || exit 3
test -f ${SRCE} || exit 4
test -f ${DEST} || exit 5
test "$(cat ${SRCE})" = "2" || exit 8
test "$(cat ${DEST})" = "1" || exit 9
rm -f ${SRCE} ${DEST}
test -f ${SRCE} && exit 1
test -f ${DEST} && exit 2

SRCE=$(mktemp -u ${ROOT}/${BASE}-srce-XXXXXXXXXX)
DEST=$(mktemp -u ${ROOT}/${BASE}-dest-XXXXXXXXXX)
test -f ${SRCE} && exit 1
test -f ${DEST} && exit 2
renametool -x ${SRCE} ${DEST} && exit 3
rm -f ${SRCE} ${DEST}
test -f ${SRCE} && exit 1
test -f ${DEST} && exit 2

SRCE=$(mktemp ${ROOT}/${BASE}-srce-XXXXXXXXXX)
DEST=$(mktemp -u ${ROOT}/${BASE}-dest-XXXXXXXXXX)
test -f ${SRCE} || exit 1
test -f ${DEST} && exit 2
renametool -x ${SRCE} ${DEST} && exit 3
rm -f ${SRCE} ${DEST}
test -f ${SRCE} && exit 1
test -f ${DEST} && exit 2

echo "SUCCESS." 1>&2
exit 0
