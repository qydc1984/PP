#include "hypervisor_config.h"
#include "hypervisor_base.h"
#include <ntddk.h>

// 外部全局变量（从 hypervisor_base.h 继承）
extern HYPERVISOR_CONFIG g_HypervisorConfig;
extern HYPERVISOR_INTERFACE g_HypervisorInterface;

NTSTATUS HypervisorConfigInitialize(VOID)
{
    g_HypervisorConfig.ProcessorCount = 0;
    g_HypervisorConfig.HypervisorType = HYPERVISOR_TYPE_NONE;
    g_HypervisorConfig.IsInitialized = FALSE;
    return STATUS_SUCCESS;
}

VOID HypervisorConfigCleanup(VOID)
{
    RtlZeroMemory(&g_HypervisorConfig, sizeof(HYPERVISOR_CONFIG));
}

NTSTATUS HypervisorConfigLoad(VOID)
{
    return STATUS_SUCCESS;
}

NTSTATUS HypervisorConfigSave(VOID)
{
    return STATUS_SUCCESS;
}

ULONG HypervisorConfigGetProcessorCount(VOID)
{
    return g_HypervisorConfig.ProcessorCount;
}

VOID HypervisorConfigSetProcessorCount(ULONG count)
{
    g_HypervisorConfig.ProcessorCount = count;
}

HYPERVISOR_TYPE HypervisorConfigGetType(VOID)
{
    return g_HypervisorConfig.HypervisorType;
}

VOID HypervisorConfigSetType(HYPERVISOR_TYPE type)
{
    g_HypervisorConfig.HypervisorType = type;
}

BOOLEAN HypervisorConfigIsInitialized(VOID)
{
    return g_HypervisorConfig.IsInitialized;
}

VOID HypervisorConfigSetInitialized(BOOLEAN initialized)
{
    g_HypervisorConfig.IsInitialized = initialized;
}