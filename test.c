#include <inttypes.h>
#include "source-nic.h"

DpdkNicPort nic_ports[NIC_MAX_PORT];

int main(int argc, char *argv[])
{
	int ret = 0;
	uint32_t slave_lcore = 0;
	uint32_t lcore = 0;
	uint16_t nb_ports = 0;
	uint16_t port_id = 0;
	uint16_t nb_rx_queues = 1;
	uint16_t nb_tx_queues = 1;

  // init dpdk eal
	ret = rte_eal_init(argc, argv);
	if(ret < 0) {
		rte_exit(EXIT_FAILURE, "Error witch EAL initialization\n");
	}
  // get no. of nic ports
	nb_ports = rte_eth_dev_count();
  // initialize all ports
	for(port_id = 0; port_id < nb_ports; ++port_id) {
    ret = DpdkNicPortInit(port_id, &nic_ports[port_id], nb_rx_queues, nb_tx_queues);
    if(ret < 0) {
			rte_exit(EXIT_FAILURE, "Failed to initialize port %u\n", port_id);
		}
	}
  // to-do: initialize other resources

  // to-do: launch other thread on slave lcore

  // launch nic-capture thread on master lcore. inside the callback func, it make a judgement if lcore is the master, if not so, return back.
  RTE_LCORE_FOREACH(lcore) { 
    if(!rte_lcore_is_enabled(lcore))
      continue;
    if(rte_eal_mp_remote_launch(DpdkNicReceiveLoop, &nic_ports[0], CALL_MASTER) < 0) { 
      rte_exit(EXIT_FAILURE, "Failed to launch nic-receive-loop thread on lcore %"PRIu32"\n", lcore);
    }
  }

  // call nic-capture function, it will not return util error.
  //DpdkNicReceiveLoop(&nic_port[0]);

  // wait all slave lcores
	RTE_LCORE_FOREACH_SLAVE(slave_lcore) {
		if(rte_eal_wait_lcore(slave_lcore) < 0) {
			rte_exit(EXIT_FAILURE, "Failed to wait slave lcore %"PRIu32"\n", slave_lcore);
		}
	}

	return 0;
}

