#!/bin/bash

find /usr/etc/netopeer -name 'datastore*' -exec grep -l dns {} \;
sleep 3
/usr/bin/netopeer-server -v 3
