#ifdef __cplusplus
extern "C" {
#endif

#include <rte_lcore.h>
#include "source-nic.h"

int DpdkNicReceiveLoop(void *arg)
{
  uint32_t lcore_id;
  lcore_id = rte_lcore_id();
  if(lcore_id != rte_get_master_lcore()) {
    return 0;
  }
  rte_log(RTE_LOG_INFO, RTE_LOGTYPE_EAL, "DpdkNicReceiveLoop on lcore %u startup\n", lcore_id);
  while(1) {
    rte_log(RTE_LOG_INFO, RTE_LOGTYPE_EAL, "Receive packet loop\n");
    sleep(1);
  }
  return 0;
}

#ifdef __cplusplus
}
#endif

