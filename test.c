#include "nic-port.h"
#include "source-nic.h"

DpdkNicPort nic_ports[NIC_MAX_PORT];

int main(int argc, char *argv[])
{
	int ret = 0;
	uint32_t lcore_id = 0;
	uint16_t nb_ports = 0;
	uint16_t port_id = 0;
	uint16_t nb_rx_queues = 1;
	uint16_t nb_tx_queues = 1;

	ret = rte_eal_init(argc, argv);
	if(ret < 0) {
		rte_exit(EXIT_FAILURE, "Error witch EAL initialization\n");
	}
	nb_ports = rte_eth_dev_count();
	for(port_id = 0; port_id < nb_ports; ++port_id) {
		ret = DpdkNicPortInit(port_id, &nic_ports[port_id], nb_rx_queues, nb_tx_queues);
		if(ret < 0) {
			rte_exit(EXIT_FAILURE, "Failed to initialize port %u\n", port_id);
		}
	}
	if(rte_eal_mp_remote_launch(DpdkNicReceiveLoop, NULL, CALL_MASTER) < 0) { 
		rte_exit(EXIT_FAILURE, "Failed to launch nic-receive-loop thread\n");
	}
	rte_log(RTE_LOG_INFO, RTE_LOGTYPE_EAL, "Remote launch ok!\n");
	RTE_LCORE_FOREACH_SLAVE(lcore_id) {
		if(rte_eal_wait_lcore(lcore_id) < 0) {
			rte_exit(EXIT_FAILURE, "Failed to wait lcore\n");
		}
	}

	return 0;
}

