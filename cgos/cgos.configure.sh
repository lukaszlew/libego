#!/bin/sh

THIS_DIR="$(readlink -f $(dirname "${0}"))"

LOGIN="${1}"          # you can hardcode login
#LOGIN="MyBot-${1}"   # or at least add some prefix
PASSWORD="${2}"        # yep, hardcode password as well
GTP_CONFIG_FILE="${3}"

usage () {
  echo "Usage: ${0} LOGIN PASSWORD GTP_CONFIG_FILE"
  exit 1
}


if test -z "${LOGIN}"; then
  usage
fi

if test -z "${PASSWORD}"; then
  usage
fi

if test -z "${GTP_CONFIG_FILE}"; then
  usage
fi

# Login's subdirectory and files

LOGIN_DIR="${THIS_DIR}/${LOGIN}"

CGOS_CONFIG_FILE="${LOGIN_DIR}/cgos.cfg"
PLEASE_WAIT_FILE="${LOGIN_DIR}/please_wait_for_game_end"
STARTSCRIPT_FILE="${LOGIN_DIR}/cgos.start.sh"
STOPSCRIPT_FILE="${LOGIN_DIR}/cgos.stop.sh"
KILLSCRIPT_FILE="${LOGIN_DIR}/cgos.kill.sh"
CONFIGGTP_FILE="${LOGIN_DIR}/config.gtp"

DATE=`date "+%FT%T"`
LOG_FILE="${LOGIN_DIR}/${DATE}.log"

# CGOS configuration setup

CGOS_CONFIG="
%section server
    server cgos.boardspace.net
    port 6867

%section player
     name      ${LOGIN}
     password  ${PASSWORD}
     invoke    ${THIS_DIR}/../bin/engine \"gtpfile ${CONFIGGTP_FILE}\" gtp
     priority  10
"

# Magic scripts

STARTSCRIPT="
${THIS_DIR}/client/cgosGtp.tcl \\
      -c \"${CGOS_CONFIG_FILE}\" \\
      -k \"${PLEASE_WAIT_FILE}\" \\
   | tee \"${LOG_FILE}\" &&
rm -f    \"${PLEASE_WAIT_FILE}\" &&
echo \"CGOS session on login ${LOGIN} finished properly.\"
"

STOPSCRIPT="#!/bin/sh
touch \"${PLEASE_WAIT_FILE}\"
"

KILLSCRIPT="#!/bin/sh
pkill -f \"cgosGtp.tcl.*${LOGIN_DIR}\"
rm -f \"${PLEASE_WAIT_FILE}\"
"
# Do the job

mkdir -p "${LOGIN_DIR}" &&
cp "${GTP_CONFIG_FILE}" "${CONFIGGTP_FILE}" &&
echo "${CGOS_CONFIG}" > "${CGOS_CONFIG_FILE}" &&
echo "${STARTSCRIPT}" > "${STARTSCRIPT_FILE}" &&
echo "${STOPSCRIPT}"  > "${STOPSCRIPT_FILE}" &&
echo "${KILLSCRIPT}"  > "${KILLSCRIPT_FILE}" &&
chmod +x "${STARTSCRIPT_FILE}" &&
chmod +x "${STOPSCRIPT_FILE}" &&
chmod +x "${KILLSCRIPT_FILE}"


