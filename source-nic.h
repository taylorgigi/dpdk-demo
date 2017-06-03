#ifndef SOURCE_NIC_H
#define SOURCE_NIC_H

#include <stdint.h>
#include <unistd.h>
#include <inttypes.h>
#include <rte_log.h>
#include <rte_lcore.h>
#include "nic-port.h"

#ifdef __cplusplus
extern "C" {
#endif

// burst number of packets
#define BURST_PKTS    512

// no. of prefetched cache line
#define PREFETCH_CACHE_LINE_NUM        (MBUF_DATA_ROOM_SIZE%CACHE_LINE_SIZE==0 ? MBUF_DATA_ROOM_SIZE%CACHE_LINE_SIZE : MBUF_DATA_ROOM_SIZE%CACHE_LINE_SIZE+1)

int DpdkNicReceiveLoop(void *arg);

#ifdef __cplusplus
}
#endif

#endif
