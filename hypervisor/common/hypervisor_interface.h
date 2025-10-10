#pragma once
#include "hypervisor_base.h"

#ifdef __cplusplus
extern "C" {
#endif

	// �ӿڹ�����
	NTSTATUS HypervisorInterfaceInitialize(HYPERVISOR_TYPE type);
	VOID HypervisorInterfaceCleanup(VOID);
	BOOLEAN HypervisorInterfaceIsAvailable(VOID);

#ifdef __cplusplus
}
#endif