#pragma once
#include "hypervisor_base.h"

#ifdef __cplusplus
extern "C" {
#endif

	// 注册表路径
#define HYPERVISOR_REGISTRY_PATH L"\\Registry\\Machine\\Software\\Hypervisor"
#define HYPERVISOR_PARAMETERS_KEY L"Parameters"

// 配置管理函数
	NTSTATUS HypervisorConfigInitialize(VOID);
	VOID HypervisorConfigCleanup(VOID);
	NTSTATUS HypervisorConfigLoad(VOID);
	NTSTATUS HypervisorConfigSave(VOID);

	// 配置访问函数
	ULONG HypervisorConfigGetProcessorCount(VOID);
	VOID HypervisorConfigSetProcessorCount(ULONG count);
	HYPERVISOR_TYPE HypervisorConfigGetType(VOID);
	VOID HypervisorConfigSetType(HYPERVISOR_TYPE type);
	BOOLEAN HypervisorConfigIsInitialized(VOID);
	VOID HypervisorConfigSetInitialized(BOOLEAN initialized);

	// 注册表操作函数
	NTSTATUS HypervisorConfigReadFromRegistry(VOID);
	NTSTATUS HypervisorConfigWriteToRegistry(VOID);

	// 辅助函数
	NTSTATUS HypervisorConfigReadDwordFromRegistry(PCWSTR ValueName, PULONG Value);
	NTSTATUS HypervisorConfigWriteDwordToRegistry(PCWSTR ValueName, ULONG Value);
	NTSTATUS HypervisorConfigReadStringFromRegistry(PCWSTR ValueName, PWSTR Buffer, ULONG BufferSize);
	NTSTATUS HypervisorConfigWriteStringToRegistry(PCWSTR ValueName, PCWSTR Value);

#ifdef __cplusplus
}
#endif