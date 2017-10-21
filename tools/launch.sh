#!/bin/bash

PGREP=/bin/pgrep
PKILL=/usr/bin/pkill
ECHO=/usr/bin/echo

PROGRAM_NAME=test
#PROGRAM_PATH=/home/ysm/dpdk-demo/build/${PROGRAM_NAME}
PROGRAM_PATH=../build/${PROGRAM_NAME}

CORE_MASK=f
MASTER_LCORE_ID=1

# check program file
if [ ! -x ${PROGRAM_PATH} ]; then
    ${ECHO} "Program ${PROGRAM_PATH} not exist or have not executable permition, check it!!!"
    exit 1
fi

# if program is already running, kill it
pid=`${PGREP} ${PROGRAM_NAME}`
if [ -n "${pid}" ]; then
    ${ECHO} "${PROGRAM_NAME} is already running, will be killed."
    ${PKILL} -9 ${PROGRAM_NAME}
fi

# launch program
${PROGRAM_PATH} -c ${CORE_MASK} --master-lcore ${MASTER_LCORE_ID}

${ECHO} "Launch ${PROGRAM_PATH} ok."

