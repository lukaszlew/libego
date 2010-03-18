#!/bin/sh

for i in \
    khaki01 khaki02 khaki03 khaki04 khaki05 khaki06 khaki07 khaki08 \
    khaki09 khaki10 khaki11 khaki12 khaki13 khaki14 khaki15 khaki16;
do
    echo "$i"
    ssh "$i" ./client.py "$i" &
    sleep 1
done
