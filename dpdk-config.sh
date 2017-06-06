#!/bin/bash

# command define
RM=/bin/rm
MKDIR=/bin/mkdir
DPDK_DEVBIND=/home/ysm/dpdk-stable-16.11.1/tools/dpdk-devbind.py
INSMOD=/sbin/insmod
LSMOD=/sbin/lsmod
MODPROBE=/sbin/modprobe
MOUNT=/bin/mount
GREP=/bin/grep
ECHO=/bin/echo
IFCONFIG=/sbin/ifconfig

# hey, man, black hole file :)
BLACK_HOLE=/dev/null

HUGEPAGES=1024
HUGEPAGE_DIR=/mnt/huge
DEVICE=p2p1

# hugepage setting
${ECHO} ${HUGEPAGES} > /proc/sys/vm/nr_hugepages

# mount hugetlbfs
if [ ! -d ${HUGEPAGE_DIR} ];then
	if [ -f ${HUGEPAGE_DIR} ];then
		${RM} ${HUGEPAGE_DIR}
	fi
	${MKDIR} -p ${HUGEPAGE_DIR}
	${MOUNT} -t hugetlbfs ${HUGEPAGE_DIR}
fi

${MODPROBE} uio
# insert igb_uio mod
if [ -z "`${LSMOD}|${GREP} igb_uio`" ];then
	${INSMOD} ${RTE_SDK}/${RTE_TARGET}/kmod/igb_uio.ko
fi
# insert rte_kni mod
#if [ -z "`${LSMOD}|${GREP} rte_kni`" ];then
#	${INSMOD} ${RTE_SDK}/${RTE_TARGET}/kmod/rte_kni.ko
#fi

# if device not bind to igb_uio, do it
if [ -n "`${IFCONFIG} ${DEVICE} 2> ${BLACK_HOLE}`" ];then
	${IFCONFIG} ${DEVICE} down
	${DPDK_DEVBIND} --bind=igb_uio ${DEVICE}
fi

