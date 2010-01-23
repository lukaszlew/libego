#!/bin/sh
SCRIPT_DIR="$(readlink -f $(dirname "$0"))"

"${SCRIPT_DIR}"/engine \
    "Mcts.Params.Genmove playouts 10000" \
    "McMc.Params use 1" \
    "McMc.Params explore_coeff 10" \
    gtp
