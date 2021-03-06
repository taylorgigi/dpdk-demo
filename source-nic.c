#ifdef __cplusplus
extern "C" {
#endif

#include <sys/time.h>
#include "source-nic.h"

static void GiveAllMbufBackToPool(struct rte_mbuf **mbufs, uint32_t num)
{
  uint32_t i;
  for(i=0; i<num; ++i) {
    rte_pktmbuf_free(mbufs[i]);
  }
}

int DpdkNicReceiveLoop(void *arg)
{
  uint16_t offset = 0;
  uint16_t num;
  uint32_t lcore_id;
  uint32_t nb_rx_queues = 0;
  uint32_t queue_id = 0;
  uint32_t port_id;
  uint32_t total_pkts = 0;
  uint32_t sum = 0;
  DpdkNicPort *nic_port = NULL;
  struct timeval told;
  struct timeval tnow;
  uint32_t delay = 0;

  gettimeofday(&told, NULL);

  struct rte_mbuf *burst_pkts[BURST_PKTS];

  lcore_id = rte_lcore_id();
  nic_port = (DpdkNicPort *)arg;
  port_id = nic_port->port_id;

  nb_rx_queues = nic_port->nb_rx_queues;

  if(lcore_id != rte_get_master_lcore()) {
    return 0;
  }
  rte_log(RTE_LOG_INFO, RTE_LOGTYPE_EAL, "DpdkNicReceiveLoop on lcore %u startup\n", lcore_id);
  while(1) {
    for(queue_id = 0; queue_id < nb_rx_queues; ++queue_id) {
      num = rte_eth_rx_burst(port_id, queue_id, burst_pkts+offset, BURST_PKTS-offset);
      if(num <= 0)
        continue;
      gettimeofday(&tnow, NULL);
      delay = tnow.tv_sec - told.tv_sec;
      total_pkts += num;
      sum += num;
      if(delay >= 60) {
        rte_log(RTE_LOG_INFO, RTE_LOGTYPE_EAL, "Receive %u packets from port:%u queue:%u speed: %.2fpkt/s\n", total_pkts, port_id, queue_id, sum*1.0/delay);
	sum = 0;
        told.tv_sec = tnow.tv_sec;
        told.tv_usec = tnow.tv_usec;
      }
      offset += num;
      if(offset == BURST_PKTS) {
        // to-do: process burst_pkts
        GiveAllMbufBackToPool(burst_pkts, BURST_PKTS);
        // reset offset
        offset = 0;
      }
    }
  }
  return 0;
}

#ifdef __cplusplus
}
#endif

