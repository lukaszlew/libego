#!/bin/sh

SCRIPT_DIR="$(readlink -f $(dirname "$0"))"

if test $# -ne 2; then
  echo "Usage: $0 GAME_NO PROC_NO"
  exit 1
fi

BLACK="${SCRIPT_DIR}/../bin/libego.sh"
WHITE="${SCRIPT_DIR}/../bin/gnugo.sh"
GAME_NO="$1"
PROC_NO="$2"

DIR="${SCRIPT_DIR}/$(date +%F/%T)"

mkdir -p "${DIR}"
cd "${DIR}"

echo "Results will be in $DIR"
echo -n "Starting processes: "

for i in `seq ${PROC_NO}`; do
    echo -n "$i "
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
        -verbose \
        2> "p${i}.log" &
done

echo
echo "Waiting to finish ..."
wait

cat p*.dat > "all.dat"
gogui-twogtp -analyze "all.dat"

echo "Starting browser ..."

firefox "all.html" &
