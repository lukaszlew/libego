#!/bin/sh
SCRIPT_DIR="$(readlink -f $(dirname "$0"))"

"${SCRIPT_DIR}"/engine \
    "param.other genmove_playouts 10000" \
    "param.other local_use 1" \
    \
    "param.tree use 1" \
    \
    "param.mcmc update 0" \
    "param.mcmc use    0" \
    gtp
