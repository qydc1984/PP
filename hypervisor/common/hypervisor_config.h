#pragma once
#include "hypervisor_base.h"

#ifdef __cplusplus
extern "C" {
#endif

	// ע���·��
#define HYPERVISOR_REGISTRY_PATH L"\\Registry\\Machine\\Software\\Hypervisor"
#define HYPERVISOR_PARAMETERS_KEY L"Parameters"

// ���ù�����
	NTSTATUS HypervisorConfigInitialize(VOID);
	VOID HypervisorConfigCleanup(VOID);
	NTSTATUS HypervisorConfigLoad(VOID);
	NTSTATUS HypervisorConfigSave(VOID);

	// ���÷��ʺ���
	ULONG HypervisorConfigGetProcessorCount(VOID);
	VOID HypervisorConfigSetProcessorCount(ULONG count);
	HYPERVISOR_TYPE HypervisorConfigGetType(VOID);
	VOID HypervisorConfigSetType(HYPERVISOR_TYPE type);
	BOOLEAN HypervisorConfigIsInitialized(VOID);
	VOID HypervisorConfigSetInitialized(BOOLEAN initialized);

	// ע����������
	NTSTATUS HypervisorConfigReadFromRegistry(VOID);
	NTSTATUS HypervisorConfigWriteToRegistry(VOID);

	// ��������
	NTSTATUS HypervisorConfigReadDwordFromRegistry(PCWSTR ValueName, PULONG Value);
	NTSTATUS HypervisorConfigWriteDwordToRegistry(PCWSTR ValueName, ULONG Value);
	NTSTATUS HypervisorConfigReadStringFromRegistry(PCWSTR ValueName, PWSTR Buffer, ULONG BufferSize);
	NTSTATUS HypervisorConfigWriteStringToRegistry(PCWSTR ValueName, PCWSTR Value);

#ifdef __cplusplus
}
#endif