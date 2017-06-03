#ifdef __cplusplus
extern "C" {
#endif

#include "source-nic.h"

int DpdkNicReceiveLoop(void *arg)
{
  uint32_t lcore_id;
  uint32_t nb_rx_queues = 0;
  uint32_t queue_id = 0;
  DpdkNicPort *nic_port = NULL;

  lcore_id = rte_lcore_id();
  nic_port = (DpdkNicPort *)arg;

  nb_rx_queues = nic_port->nb_rx_queues;

  if(lcore_id != rte_get_master_lcore()) {
    return 0;
  }
  rte_log(RTE_LOG_INFO, RTE_LOGTYPE_EAL, "DpdkNicReceiveLoop on lcore %u startup\n", lcore_id);
  while(1) {
    //rte_log(RTE_LOG_INFO, RTE_LOGTYPE_EAL, "Receive packet loop on lcore %"PRIu32"\n", lcore_id);
    //sleep(1);
    for(queue_id = 0; queue_id < nb_rx_queues; ++queue_id) {
    }
  }
  return 0;
}

#ifdef __cplusplus
}
#endif

