#!/bin/bash
#
# File: run_test.h
# Author: Kostas Peletidis
# Date: 15-May-2017
#
# Description: Script to run the test cases for Orca.
#

CLI=`which netopeer-cli`
NC_USER=$USER
NC_HOST="localhost"

if [ "$CLI" = "" ]; then
  echo "Cannot find netopeer-cli executable"
  exit 1
fi

#
# Iterate through the test message files and send them to the server.
#
NUM=0
for f in $( ls *.xml ); do
  ((NUM++))
  msg="$( dirname $f)/$f"
  echo -e "$NUM: Sending message $msg"
  netopeer-cli &> $msg.out <<ORCATEST
connect --login $NC_USER $NC_HOST
user-rpc --file $msg
disconnect
ORCATEST

done

echo -e "\n*** TEST RESULTS ***\n"
#
# Count how many test messages got error responses.
#
PASSNUM=0
for v in $( ls *.out); do
  grep -i error $v > /dev/null
  status=$?
#  echo "STATUS($v): $status"
  if [[ $status == 0 ]]; then
    echo -e "$v:\tERROR"
  else
    echo -e "$v:\tOK"
    ((PASSNUM++))
  fi
done

echo -e "\nPASS: $PASSNUM/$NUM\n"

exit 0

