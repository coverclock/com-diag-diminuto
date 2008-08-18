#! /bin/sh
# Copyright 2008 Digital Aggregates Corporation, Arvada CO 80001-0587 USA
# Licensed under the GPL V2
# Chip Overclock <coverclock@diag.com>

export PATH=$PATH:/usr/local/bin

export IPADDR=`getubenv ipaddr`
export NETMASK=`getubenv netmask`
test -z "$NETMASK" && export `ipcalc -m $IPADDR`
export BROADCAST=`getubenv broadcastip`
test -z "$BROADCAST" && export `ipcalc -b $IPADDR`
export GATEWAY=`getubenv gatewayip`

cat > /etc/network/interfaces << EOF
auto lo
iface lo inet loopback
auto eth0
iface eth0 inet static
address $IPADDR
netmask $NETMASK
broadcast $BROADCAST
gateway $GATEWAY
EOF

exit 0
