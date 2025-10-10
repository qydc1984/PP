#pragma once
#include "hypervisor_base.h"

#ifdef __cplusplus
extern "C" {
#endif

	// 工具函数声明
	VOID HypervisorUtilsDebugPrint(PCCHAR format, ...);
	NTSTATUS HypervisorUtilsMemoryCopy(PVOID dest, const VOID* src, SIZE_T size);
	VOID HypervisorUtilsMemoryZero(PVOID buffer, SIZE_T size);

#ifdef __cplusplus
}
#endif