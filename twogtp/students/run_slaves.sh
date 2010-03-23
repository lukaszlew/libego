#!/bin/sh

colors="red pink orange brown yellow khaki cyan green violet blue crimson"

for c in $colors; do
    for i in `seq -w 0 16`; do
        for k in `seq 4`; do
            echo "$c$i $k"
            ssh "$c$i" screen -dm python libego/twogtp/students/client.py
            sleep 0.2
        done
    done
done
