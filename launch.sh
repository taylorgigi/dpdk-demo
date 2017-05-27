#!/bin/bash

PROGRAM_NAME=test
PROGRAM_PATH=/home/ysm/dpdk-demo/build/${PROGRAM_NAME}

CORE_MASK=ff
MASTER_LCORE_ID=1

# check program file
if [ ! -x ${PROGRAM_PATH} ]; then
    echo "Program file not exist or have not executable permition!!!"
    echo "Check it!!!"
    exit 1
fi

# if program is already running, kill it
pid=`${PGREP} ${PROGRAM_NAME}`
if [ -n "${pid}" ]; then
    pkill -9 ${PROGRAM_NAME}
fi

# launch program
${PROGRAM_PATH} -c ${CORE_MASK} --master-lcore ${MASTER_LCORE_ID}

echo "Launch ${PROGRAM_PATH} ok."

