#!/bin/sh
SCRIPT_DIR="$(readlink -f $(dirname "$0"))"

"${SCRIPT_DIR}"/engine \
    "Params playouts 10000" \
    "Params use_mcmc 1" \
    "Params use_tree 0" \
    "Params.McMc explore_coeff 4" \
    gtp
