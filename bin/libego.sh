#!/bin/sh
SCRIPT_DIR="$(readlink -f $(dirname "$0"))"

"${SCRIPT_DIR}"/engine \
    "param.other seed 123" \
    "param.other genmove_playouts 10000" \
    \
    "param.tree use 1" \
    "param.tree max_moves 400" \
    \
    "param.mcmc update 0" \
    "param.mcmc use    0" \
    "param.mcmc max_moves 12" \
    "param.mcmc stat_bias 0.0" \
    "param.mcmc rave_bias 0.001" \
    "param.mcmc explore_coeff 0" \
    \
    "model.param boltzmann_constant 1.0" \
    "model.param update 0" \
    \
    "param.other local_use 0" \
    \
    "param.other logger_is_active 0" \
    "param.other logger_directory logdir" \
    \
    gtp
