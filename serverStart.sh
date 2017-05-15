#!/bin/bash

find /usr/etc/netopeer -name 'datastore*' -exec grep -l dns {} \;
sleep 3
/usr/sbin/useradd orca
echo orca:password | /usr/sbin/chpasswd
echo "***adding ksi common yin"
netopeer-manager add --name ksi-common --model /usr/src/netopeer/ksiModels/ksi-common\@2017-02-17.yin
echo "***adding ksi aggregator yin"
netopeer-manager add --name ksi-aggregator --model /usr/src/netopeer/ksiModels/ksi-aggregator\@2017-02-17.yin
echo "***adding ksi extender yin"
netopeer-manager add --name ksi-extender --model /usr/src/netopeer/ksiModels/ksi-extender\@2017-02-17.yin

echo "***starting netconf server"
/usr/bin/netopeer-server -v 3
