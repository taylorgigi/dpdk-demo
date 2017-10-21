#!/bin/bash

# command define
RM=/bin/rm
MKDIR=/bin/mkdir
#DPDK_DEVBIND=${RTE_SDK}/usertools/dpdk-devbind.py
DPDK_DEVBIND=${RTE_SDK}/tools/dpdk-devbind.py
INSMOD=/sbin/insmod
LSMOD=/sbin/lsmod
MODPROBE=/sbin/modprobe
DEPMOD=/sbin/depmod
MOUNT=/bin/mount
GREP=/bin/grep
ECHO=/bin/echo
IFCONFIG=/sbin/ifconfig
UNAME=/bin/uname

declare -a device_list
declare -i device_num
declare -i itr

device_list=($*)
device_num=$#
itr=0

pktgen_debug_print() {
	${ECHO} "Device num: ${device_num}"
	for ((itr=0; itr<$device_num; itr++ ))
	do
		${ECHO} ${device_list[$itr]}
	done
}

PKT_COUNT=10000000
CLONE_SKB=1000
PKT_SIZE=64
DST_IP="192.168.1.102"
DST_MAC="e0:db:55:cc:7d:43"

# function define
pktgen_device_setup() {
	dev=$1
	id=$2
	echo add_device ${dev}         > /proc/net/pktgen/kpktgend_${id}
	echo count      ${PKT_COUNT}   > /proc/net/pktgen/${dev}
	echo clone_skb  ${CLONE_SKB}   > /proc/net/pktgen/${dev}
	echo pkt_size   ${PKT_SIZE}    > /proc/net/pktgen/${dev}
	echo dst        ${DST_IP}      > /proc/net/pktgen/${dev}
	echo dst_mac    ${DST_MAC}     > /proc/net/pktgen/${dev}
}

pktgen_insert_mod() {
	# insert pktgen mod
	if [ -z "`${LSMOD}|${GREP} pktgen`" ];then
		${INSMOD} /lib/modules/`${UNAME} -r`/kernel/net/core/pktgen.ko
	fi
}

pktgen_setup() {
	# device setup
	for ((itr=0; itr<$device_num; itr++ ))
	do
		device=${device_list[$itr]}
		pktgen_device_setup $device $itr
	done
}

pktgen_launch() {
	# start sending pkts
	${ECHO} start                  > /proc/net/pktgen/pgctrl
}

pktgen_check_args() {
	if [ $device_num -eq 0 ];then
		${ECHO} "$0 <interface-name1> [<interface-name2>] ... [<interface-nameN>]"
		${ECHO} "Example: " $0 p2p1 p2p3
		exit 0
	fi
}

# now, start working
pktgen_check_args
#pktgen_debug_print
pktgen_insert_mod
pktgen_setup
pktgen_launch

