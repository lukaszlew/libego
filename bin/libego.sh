#!/bin/sh
SCRIPT_DIR="$(readlink -f $(dirname "$0"))"

"${SCRIPT_DIR}"/engine \
    "param.other genmove_playouts 10000" \
    "param.other local_use 0" \
    \
    "param.tree use 1" \
    \
    "param.mcmc update 0" \
    "param.mcmc use    0" \
    "param.mcmc max_depth 20" \
    "param.mcmc stat_bias 0" \
    "param.mcmc rave_bias 0.01" \
    "param.mcmc explore_coeff 1" \
    gtp
