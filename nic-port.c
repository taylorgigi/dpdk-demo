#include <rte_log.h>
#include "nic-port.h"

#ifdef __cplusplus
extern "C" {
#endif

static const struct rte_eth_conf default_conf = {
	.link_speeds = ETH_LINK_SPEED_AUTONEG,
	.rxmode = {
		.mq_mode = ETH_MQ_RX_RSS,
		.max_rx_pkt_len = ETHER_MAX_LEN,
	},
	.txmode = {
		.mq_mode = ETH_MQ_TX_NONE,
	},
	.rx_adv_conf = {
		.rss_conf = {
			.rss_hf = ETH_RSS_IP | ETH_RSS_TCP | ETH_RSS_UDP | ETH_RSS_SCTP,
		},
	},
};

static inline int DpdkNicRxQueueInit(DpdkNicRxQueue *rx_queue) {
	if(unlikely(rx_queue == NULL)) {
		return -1;
	}
	rx_queue->port_id = 0;
	rx_queue->lcore_id = 0;
	rx_queue->socket_id = 0;
	rx_queue->rx_queue_id = 0;
	// rx_queue-> rx_conf = {};
	rx_queue->pool = NULL;

	return 0;
}

static inline int DpdkNicTxQueueInit(DpdkNicTxQueue *tx_queue) {
	if(unlikely(tx_queue == NULL)) {
		return -1;
	}
	tx_queue->port_id = 0;
	tx_queue->lcore_id = 0;
	tx_queue->socket_id = 0;
	tx_queue->tx_queue_id = 0;
	// tx_queue-> tx_conf = {};
	tx_queue->pool = NULL;

	return 0;
}

inline int DpdkNicPortInit(uint16_t port_id, DpdkNicPort *nicport, uint16_t nb_rx_queues, uint16_t nb_tx_queues) {
	int ret = 0;
	int socketid = 0;
	uint16_t q = 0;
	// verify
	if(unlikely(nicport == NULL)) {
		return -1;
	}
	if(unlikely(nb_rx_queues == 0 || nb_tx_queues == 0)) {
		rte_log(RTE_LOG_WARNING, RTE_LOGTYPE_USER1, "Number of rx queues(%u) or tx queues(%u) is zero", nb_rx_queues, nb_tx_queues);
		return -1;
	}
	if(likely(nb_rx_queues > NIC_MAX_QUEUES || nb_tx_queues > NIC_MAX_QUEUES)) {
		rte_log(RTE_LOG_WARNING, RTE_LOGTYPE_USER1, "Number of rx queues(%u) or tx queues(%u) exceeded the max value(%u)", nb_rx_queues, nb_tx_queues, NIC_MAX_QUEUES);
		return -1;
	}
	nicport->nb_rx_queues = nb_rx_queues;
	nicport->nb_tx_queues = nb_tx_queues;
	nicport->port_id = port_id;
	socketid = rte_eth_dev_socket_id(port_id);
	if(socketid < 0) {
		socketid = 0;
		//rte_log(RTE_LOG_ERR, RTE_LOGTYPE_PORT, "Get socket id error of port %u, out of range", port_id);
		//return -1;
	}
	nicport->socket_id = socketid;
	// port configuration
	nicport->port_conf = default_conf;
	ret = rte_eth_dev_configure(port_id, nb_rx_queues, nb_tx_queues, &nicport->port_conf);
	if(ret < 0) {
		rte_log(RTE_LOG_ERR, RTE_LOGTYPE_PORT, "Failed to configure port %u", port_id);
		return -1;
	}
	// create mbuf mempool
	char namebuf[64] = {0};
	snprintf(namebuf, 64, "pool-socket%u-port%u", socketid, port_id);
	nicport->pool = rte_pktmbuf_pool_create(namebuf, MBUF_POOL_NUM, MBUF_POOL_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE, socketid);
	if(nicport->pool == NULL) {
		rte_log(RTE_LOG_ERR, RTE_LOGTYPE_MEMPOOL, "Error create mbuf mempool of port %u on socket %u", port_id, socketid);
		return -1;
	}
	// set up rx queue
	for(q=0; q<nb_rx_queues; ++q) {
		ret = rte_eth_rx_queue_setup(port_id, q, NB_RX_DESC, socketid, NULL, nicport->pool);
		if(ret < 0) {
			rte_log(RTE_LOG_ERR, RTE_LOGTYPE_PORT, "Error setup rx queue of port %u on socket %u", port_id, socketid);
			return -1;
		}
	}
	// set up tx queue
	for(q=0; q<nb_tx_queues; ++q) {
		ret = rte_eth_tx_queue_setup(port_id, q, NB_TX_DESC, socketid, NULL);
		if(ret < 0) {
			rte_log(RTE_LOG_ERR, RTE_LOGTYPE_PORT, "Error setup tx queue of port %u on socket %u", port_id, socketid);
			return -1;
		}
	}
	ret = rte_eth_dev_start(port_id);
	if(ret < 0) {
		rte_log(RTE_LOG_ERR, RTE_LOGTYPE_PORT, "Error start port %u on socket %u", port_id, socketid);
		return -1;
	}
	// print port info
	struct rte_eth_link link;
	link.link_status = ETH_LINK_DOWN;
	//rte_eth_link_get_nowait(port_id, &link);
	rte_eth_link_get(port_id, &link);
	if(link.link_status == ETH_LINK_UP) {
		struct ether_addr addr;
		rte_eth_macaddr_get(port_id, &addr);
		printf("Port %u is up MAC: %02"PRIx8" %02"PRIx8" %02"PRIx8
                        " %02"PRIx8" %02"PRIx8" %02"PRIx8"\n",
                        port_id,
                        addr.addr_bytes[0], addr.addr_bytes[1],
                        addr.addr_bytes[2], addr.addr_bytes[3],
                        addr.addr_bytes[4], addr.addr_bytes[5]);
	}
	// enable promisc mode
	rte_eth_promiscuous_enable(port_id);

	return 0;
}

#ifdef __cplusplus
}
#endif

