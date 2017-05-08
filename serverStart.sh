#!/bin/bash

find /usr/etc/netopeer -name 'datastore*' -exec grep -l dns {} \;
sleep 3
/usr/sbin/useradd netconf
echo netconf:password | /usr/sbin/chpasswd

/usr/bin/netopeer-server -v 3
