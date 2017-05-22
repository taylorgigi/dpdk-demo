#ifndef SOURCE_NIC_H
#define SOURCE_NIC_H

#include <stdint.h>
#include <unistd.h>
#include <rte_log.h>

#ifdef __cplusplus
extern "C" {
#endif

int DpdkNicReceiveLoop(void *arg);

#ifdef __cplusplus
}
#endif

#endif
