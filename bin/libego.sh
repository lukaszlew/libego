#!/bin/bash
SCRIPT_DIR="$(readlink -f $(dirname "$0"))"

"${SCRIPT_DIR}"/engine \
    "LoadGammas ${SCRIPT_DIR}/3x3.gamma" \
    "$1" \
    \
    gtp
