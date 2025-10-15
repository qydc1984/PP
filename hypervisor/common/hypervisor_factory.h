#pragma once
#include "hypervisor_base.h"

#ifdef __cplusplus
extern "C" {
#endif

	// 工厂函数
	BOOLEAN HypervisorFactoryInitialize(HYPERVISOR_TYPE type);
	VOID HypervisorFactoryCleanup(VOID);
	BOOLEAN HypervisorFactoryIsSupported(HYPERVISOR_TYPE type);
	NTSTATUS HypervisorFactoryCreateInterface(HYPERVISOR_TYPE type);
	VOID HypervisorFactoryDestroyInterface(VOID);

	// 获取特定 hypervisor 的接口
	NTSTATUS HypervisorFactoryGetMemHvInterface(HYPERVISOR_INTERFACE* interface);
	NTSTATUS HypervisorFactoryGetAirHvInterface(HYPERVISOR_INTERFACE* interface);

#ifdef __cplusplus
}
#endif