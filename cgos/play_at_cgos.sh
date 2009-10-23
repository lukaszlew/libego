#!/bin/sh

THIS_DIR="$(readlink -f $(dirname "${0}"))"

LOGIN="${1}"          # you can hardcode login
#LOGIN="MyBot-${1}"   # or at least add some prefix
PASSWORD="${2}"        # yep, hardcode password as well

usage () {
  echo "Usage: ${0} LOGIN PASSWORD"
  exit 1
}


if test -z "${LOGIN}"; then
  usage
fi

if test -z "${PASSWORD}"; then
  usage
fi


CGOS_TCL="/usr/bin/env tclsh ${THIS_DIR}/client/cgosGtp.tcl"

# Login's subdirectory

LOGIN_DIR="${THIS_DIR}/${LOGIN}/"

# Redirected stdout (log) file setup

DATE=`date "+%FT%T"`
LOG_FILE="${LOGIN_DIR}/${DATE}.log"

# CGOS configuration setup

ENGINE_BIN="${THIS_DIR}/../bin/engine"
CONFIG_1="${THIS_DIR}/../scripts/config.gtp"
CONFIG_2="${THIS_DIR}/../scripts/config_cgos.gtp"
CGOS_CONFIG_FILE="${LOGIN_DIR}/cgos.cfg"
CGOS_CONFIG="
%section server
    server cgos.boardspace.net
    port 6867

%section player
     name      ${LOGIN}
     password  ${PASSWORD}
     invoke    ${ENGINE_BIN} ${CONFIG_1} ${CONFIG_2} -
     priority  10
"

PLEASE_WAIT_FILE="${LOGIN_DIR}/please_wait_for_game_end"


# Magic stopping scripts

STOPSCRIPT_FILE="${LOGIN_DIR}/stop_cgos.sh"
STOPSCRIPT="#!/bin/sh
touch \"${PLEASE_WAIT_FILE}\"
rm \"${STOPSCRIPT_FILE}\"
"

KILLSCRIPT_FILE="${LOGIN_DIR}/kill_cgos.sh"
KILLSCRIPT="#!/bin/sh
pkill -f \"cgosGtp.tcl*${LOGIN_DIR}\"
rm -f \"${CGOS_CONFIG_FILE}\" \"${PLEASE_WAIT_FILE}\" \"${STOPSCRIPT_FILE}\" \"${KILLSCRIPT_FILE}\"
"

# Do the job

(
  mkdir -p "${LOGIN_DIR}" &&
  echo "${CGOS_CONFIG}" > "${CGOS_CONFIG_FILE}" &&
  echo "${STOPSCRIPT}"  > "${STOPSCRIPT_FILE}" &&
  echo "${KILLSCRIPT}"  > "${KILLSCRIPT_FILE}" &&
  chmod +x "${STOPSCRIPT_FILE}" &&
  chmod +x "${KILLSCRIPT_FILE}" &&
  ${CGOS_TCL} -c "${CGOS_CONFIG_FILE}" -k "${PLEASE_WAIT_FILE}" > "${LOG_FILE}" &&
  rm -f "${CGOS_CONFIG_FILE}" "${PLEASE_WAIT_FILE}" "${STOPSCRIPT_FILE}" "${KILLSCRIPT_FILE}" &&
  echo "CGOS session on login \"${LOGIN}\" finished properly."
) &
