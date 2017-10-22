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

# hey, man, black hole file :)
BLACK_HOLE=/dev/null

HUGEPAGES=1024
HUGEPAGE_DIR=/mnt/huge
declare -a device_list
declare -i device_num
declare -i itr

program=$0
device_list=($*)
device_num=$#
itr=0

# functions define
dpdk_hugetable_set() {
	# hugepage setting
	#${ECHO} ${HUGEPAGES} > /proc/sys/vm/nr_hugepages
	${ECHO} ${HUGEPAGES} > /sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages

	# mount hugetlbfs
	if [ ! -d ${HUGEPAGE_DIR} ];then
		if [ -f ${HUGEPAGE_DIR} ];then
			${RM} ${HUGEPAGE_DIR}
		fi
		${MKDIR} -p ${HUGEPAGE_DIR}
		${MOUNT} -t hugetlbfs ${HUGEPAGE_DIR}
	fi
}

dpdk_insert_igb_uio() {
	${DEPMOD} /usr/lib/modules/`${UNAME} -r`/kernel/drivers/uio/uio.ko
	${MODPROBE} uio
	# insert igb_uio mod
	if [ -z "`${LSMOD}|${GREP} igb_uio`" ];then
		${INSMOD} ${RTE_SDK}/${RTE_TARGET}/kmod/igb_uio.ko
	fi
}

dpdk_insert_kni() {
	# insert rte_kni mod
	if [ -z "`${LSMOD}|${GREP} rte_kni`" ];then
		${INSMOD} ${RTE_SDK}/${RTE_TARGET}/kmod/rte_kni.ko kthread_mode=multiple
	fi
}

dpdk_device_bind() {
	# if device not bind to igb_uio, do it
	for ((itr=0; itr<$device_num; itr++ ))
	do
		device=${device_list[$itr]}
		if [ -n "`${IFCONFIG} ${device} 2> ${BLACK_HOLE}`" ];then
			${IFCONFIG} ${device} down
			${DPDK_DEVBIND} --bind=igb_uio ${device}
		fi
	done
}

dpdk_check_args() {
	if [ ${device_num} -eq 0 ];then
		${ECHO} "${program} <interface1> [<interface2>] ... [<interfaceN>]"
		${ECHO} "Example: ${program} p2p1 p2p3"
		exit 0
	fi
}

# now, start working
dpdk_check_args
dpdk_hugetable_set
dpdk_insert_igb_uio
dpdk_insert_kni
dpdk_device_bind

