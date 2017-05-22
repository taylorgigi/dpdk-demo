#ifndef NIC_PORT_
#define NIC_PORT_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_log.h>
#include <rte_branch_prediction.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NIC_MAX_PORT		8

// mbuf mempool
//#define MBUF_POOL_NUM		(1024*1024-1)
#define MBUF_POOL_NUM		128
#define MBUF_POOL_CACHE_SIZE	64

#define NB_RX_DESC		32
#define NB_TX_DESC		32

#define NIC_MAX_QUEUES		16
#define NIC_MAX_RX_QUEUES	NIC_MAX_QUEUES
#define NIC_MAX_TX_QUEUES	NIC_MAX_QUEUES

typedef struct DpdkNicRxQueue_ {
	uint16_t port_id;
	uint16_t lcore_id;
	uint16_t socket_id;
	uint16_t rx_queue_id;
	struct rte_eth_txconf tx_conf;
	struct rte_mempool *pool;
}__rte_cache_aligned DpdkNicRxQueue;

typedef struct DpdkNicTxQueue_ {
	uint16_t port_id;
	uint16_t lcore_id;
	uint16_t socket_id;
	uint16_t tx_queue_id;
	struct rte_eth_rxconf rx_conf;
	struct rte_mempool *pool;
}__rte_cache_aligned DpdkNicTxQueue;

typedef struct DpdkNicPort_ {
	uint16_t port_id;
	uint16_t socket_id;
	uint16_t nb_rx_queues;
	uint16_t nb_tx_queues;
	struct rte_eth_conf port_conf;
	struct rte_mempool *pool;
	struct rte_lpm *lpm;
	struct rte_lpm6 *lpm6;
	struct rte_ip_frag_tbl *frag_tbl;
	DpdkNicRxQueue rx_queues[NIC_MAX_RX_QUEUES];
	DpdkNicTxQueue tx_queues[NIC_MAX_TX_QUEUES];
}__rte_cache_aligned DpdkNicPort;

inline int DpdkNicPortInit(uint16_t port_id, DpdkNicPort *nicport, uint16_t nb_rx_queues, uint16_t nb_tx_queues);

#ifdef __cplusplus
}
#endif

#endif

