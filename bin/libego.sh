#!/bin/sh
SCRIPT_DIR="$(readlink -f $(dirname "$0"))"

"${SCRIPT_DIR}"/engine \
    "param.other genmove_playouts 2000" \
    "LoadGammas ${SCRIPT_DIR}/3x3.gamma" \
    \
    "param.tree use 1" \
    "param.tree max_moves 400" \
    "param.tree progressive_bias 100" \
    \
    "param.other local_use 0" \
    \
    "param.other logger_is_active 0" \
    "param.other logger_directory logdir" \
    "$1" \
    \
    gtp
