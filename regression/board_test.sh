#!/bin/sh
ROOT="$(readlink -f $(dirname "$0"))/.."

if   test $# -eq 0; then
  "${ROOT}/bin/engine" "board_test"
elif test $# -eq 1; then
  "${ROOT}/bin/engine" "board_test 1" 2> "${ROOT}/regression/board_test.txt"
else
  echo "fail"
  exit 1
fi
