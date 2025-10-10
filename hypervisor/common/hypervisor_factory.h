#pragma once
#include "hypervisor_base.h"

#ifdef __cplusplus
extern "C" {
#endif

	// 工厂函数声明
	BOOLEAN HypervisorFactoryInitialize(HYPERVISOR_TYPE type);
	VOID HypervisorFactoryCleanup(VOID);
	BOOLEAN HypervisorFactoryIsSupported(HYPERVISOR_TYPE type);

#ifdef __cplusplus
}
#endif