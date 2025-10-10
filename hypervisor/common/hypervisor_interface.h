#pragma once
#include "hypervisor_base.h"

#ifdef __cplusplus
extern "C" {
#endif

	// 接口管理函数
	NTSTATUS HypervisorInterfaceInitialize(HYPERVISOR_TYPE type);
	VOID HypervisorInterfaceCleanup(VOID);
	BOOLEAN HypervisorInterfaceIsAvailable(VOID);

#ifdef __cplusplus
}
#endif