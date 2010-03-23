#!/bin/sh

colors="red pink orange brown yellow khaki cyan green violet blue crimson"

for c in $colors; do
    for i in `seq -w 0 16`; do
        echo $c$i `ssh "$c$i" $@` &
        sleep 0.1
    done
done
