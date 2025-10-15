#pragma once
#include "hypervisor_base.h"

#ifdef __cplusplus
extern "C" {
#endif

	// ��������
	BOOLEAN HypervisorFactoryInitialize(HYPERVISOR_TYPE type);
	VOID HypervisorFactoryCleanup(VOID);
	BOOLEAN HypervisorFactoryIsSupported(HYPERVISOR_TYPE type);
	NTSTATUS HypervisorFactoryCreateInterface(HYPERVISOR_TYPE type);
	VOID HypervisorFactoryDestroyInterface(VOID);

	// ��ȡ�ض� hypervisor �Ľӿ�
	NTSTATUS HypervisorFactoryGetMemHvInterface(HYPERVISOR_INTERFACE* interface);
	NTSTATUS HypervisorFactoryGetAirHvInterface(HYPERVISOR_INTERFACE* interface);

#ifdef __cplusplus
}
#endif