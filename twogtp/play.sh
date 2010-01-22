#!/bin/sh

if test $# -ne 2; then
  echo "Usage: $0 GAME_NO PROC_NO"
  exit 1
fi

GAME_NO="$1"
PROC_NO="$2"


DIR=`date "+%F/%T"`

BIN="$(readlink -f ../bin)"

LIBEGO=" ${BIN}/engine \"Mcts.Params.Genmove playouts 10000\" gtp"
GNUGO="  ${BIN}/gnugo-3.8 --mode gtp --chinese-rules --capture-all-dead --positional-superko --level=0"

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
        -black "${LIBEGO}" \
        -white "${GNUGO}" \
        -sgffile "p${i}" \
        &
done

wait

cat p*.dat > "all.dat"
gogui-twogtp -analyze "all.dat"
