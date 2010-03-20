#!/bin/sh

# TODO jak ustawic sciezke przez ssh? / wykonac .bashrc

for i in \
    khaki02 khaki03 khaki04 khaki05 khaki06 khaki07 khaki08 \
    khaki09 khaki10 khaki11 khaki12 khaki13 khaki14 khaki15 khaki16;
do
    for j in 1 2 3 4; do
        echo -n " $j "
        ssh "$i" libego/twogtp/students/client.py &
        sleep 1
    done
done
