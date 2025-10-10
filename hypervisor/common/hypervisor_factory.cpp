#include "hypervisor_factory.h"
#include "hypervisor_base.h"
#include <ntddk.h>

// 外部全局变量
extern HYPERVISOR_CONFIG g_HypervisorConfig;
extern HYPERVISOR_INTERFACE g_HypervisorInterface;

BOOLEAN HypervisorFactoryInitialize(HYPERVISOR_TYPE type)
{
    UNREFERENCED_PARAMETER(type);
    g_HypervisorConfig.HypervisorType = type;
    return TRUE;
}

VOID HypervisorFactoryCleanup(VOID)
{
    // 清理工厂
}

BOOLEAN HypervisorFactoryIsSupported(HYPERVISOR_TYPE type)
{
    UNREFERENCED_PARAMETER(type);
    return TRUE;
}