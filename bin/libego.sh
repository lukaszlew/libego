#!/bin/sh
SCRIPT_DIR="$(readlink -f $(dirname "$0"))"

"${SCRIPT_DIR}"/engine \
    "param.other genmove_playouts 10000" \
    \
    "param.tree use 1" \
    "param.tree max_moves 400" \
    \
    "param.mcmc update 1" \
    "param.mcmc use    0" \
    "param.mcmc max_moves 4" \
    "param.mcmc stat_bias 0" \
    "param.mcmc rave_bias 0.01" \
    "param.mcmc explore_coeff 10" \
    \
    "param.other local_use 0" \
    \
    "param.other logger_is_active 1" \
    "param.other logger_directory logdir" \
    \
    gtp
