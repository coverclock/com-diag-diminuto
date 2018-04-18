#!/bin/bash
# Copyright 2018 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in README.h
# Chip Overclock <coverclock@diag.com>
# https://github.com/coverclock/com-diag-diminuto

BASE=$(basename $0)
ROOT="/run/lock"
TEMP="/tmp"

SRCE=$(mktemp ${ROOT}/${BASE}-srce-XXXXXXXXXX)
DEST=$(mktemp -u ${ROOT}/${BASE}-dest-XXXXXXXXXX)
strace renametool ${SRCE} ${DEST} > ${BASE}-noreplace-good.log 2>&1
rm -f ${SRCE} ${DEST}

SRCE=$(mktemp ${ROOT}/${BASE}-srce-XXXXXXXXXX)
DEST=$(mktemp ${ROOT}/${BASE}-dest-XXXXXXXXXX)
strace renametool ${SRCE} ${DEST} > ${BASE}-noreplace-bad.log 2>&1 || true && false
rm -f ${SRCE} ${DEST}

SRCE=$(mktemp ${ROOT}/${BASE}-srce-XXXXXXXXXX)
DEST=$(mktemp -u ${TEMP}/${BASE}-dest-XXXXXXXXXX)
strace renametool ${SRCE} ${DEST} > ${BASE}-noreplace-nonatomic.log 2>&1 || true && false
rm -f ${SRCE} ${DEST}

SRCE=$(mktemp ${ROOT}/${BASE}-srce-XXXXXXXXXX)
DEST=$(mktemp ${ROOT}/${BASE}-dest-XXXXXXXXXX)
strace renametool -x ${SRCE} ${DEST} > ${BASE}-exchange-good.log 2>&1
rm -f ${SRCE} ${DEST}

SRCE=$(mktemp ${ROOT}/${BASE}-srce-XXXXXXXXXX)
DEST=$(mktemp -u ${ROOT}/${BASE}-dest-XXXXXXXXXX)
strace renametool -x ${SRCE} ${DEST} > ${BASE}-exchange-bad.log 2>&1 || true && false
rm -f ${SRCE} ${DEST}

SRCE=$(mktemp ${ROOT}/${BASE}-srce-XXXXXXXXXX)
DEST=$(mktemp ${TEMP}/${BASE}-dest-XXXXXXXXXX)
strace renametool -x ${SRCE} ${DEST} > ${BASE}-exchange-nonatomic.log 2>&1 || true && false
rm -f ${SRCE} ${DEST}
