#!/bin/sh

if test $# -ne 4; then
  echo "Usage: $0 ENGINE_BLACK ENGINE_WHITE GAME_NO PROC_NO"
  exit 1
fi

BLACK="$(readlink -f "$1")"
WHITE="$(readlink -f "$2")"
GAME_NO="$3"
PROC_NO="$4"

DIR=`date "+%F/%T"`


mkdir -p "${DIR}"
cd "${DIR}"

for i in `seq ${PROC_NO}`; do
    gogui-twogtp \
        -auto \
        -size 9 \
        -komi 6.5 \
        -alternate \
        -referee "${GNUGO}" \
        -games "${GAME_NO}" \
        -black "${BLACK}" \
        -white "${WHITE}" \
        -sgffile "p${i}" \
        &
done

wait

cat p*.dat > "all.dat"
gogui-twogtp -analyze "all.dat"
