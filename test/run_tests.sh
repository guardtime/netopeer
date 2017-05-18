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
MSGS="messages"
REPORT="test_report"
RESULTS="results"

if [ "$CLI" = "" ]; then
  echo "Cannot find netopeer-cli executable"
  exit 1
fi

#
# Iterate through the test message files and send them to the server.
#
NUM=0
if [ ! -d "$MSGS" ]; then
  echo -e "\nERROR: Directory $MSGS not found\n"
  exit 1
fi

for f in $( ls $MSGS/*.xml ); do
  ((NUM++))
  msg="$f"
  basemsg=$( basename "$msg" )
  echo -e "$NUM: Sending message $msg"
  netopeer-cli &> $RESULTS/$basemsg.out <<ORCATEST
connect --login $NC_USER $NC_HOST
user-rpc --file $msg
disconnect
ORCATEST
echo "" >> $RESULTS/$basemsg.out
done

echo -e "\n*** TEST RESULTS ***\n"
#
# Generate results report.
#
PASSNUM=0
if [ ! -d "$RESULTS" ]; then
  echo -e "\nERROR: Directory $RESULTS not found\n"
  exit 1
fi

:>$RESULTS/$REPORT
for v in $( ls $RESULTS/*.out); do
  grep -i error $v > /dev/null
  status=$?
  if [[ $status == 0 ]]; then
    echo -e "$v:\tERROR" | tee -a $RESULTS/$REPORT
  else
    echo -e "$v:\tOK" | tee -a $RESULTS/$REPORT
    ((PASSNUM++))
  fi
done

echo -e "\nPASS: $PASSNUM/$NUM" | tee -a $RESULTS/$REPORT

exit 0

