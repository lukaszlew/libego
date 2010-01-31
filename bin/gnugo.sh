#!/bin/sh
SCRIPT_DIR="$(readlink -f $(dirname "$0"))"

"${SCRIPT_DIR}"/gnugo-3.8 \
    --mode gtp \
    --chinese-rules \
    --capture-all-dead \
    --positional-superko \
    --level=0
