#include "hypervisor_interface.h"
#include "hypervisor_base.h"
#include <ntddk.h>

// 外部全局变量
extern HYPERVISOR_INTERFACE g_HypervisorInterface;

NTSTATUS HypervisorInterfaceInitialize(HYPERVISOR_TYPE type)
{
    UNREFERENCED_PARAMETER(type);
    return STATUS_SUCCESS;
}

VOID HypervisorInterfaceCleanup(VOID)
{
    RtlZeroMemory(&g_HypervisorInterface, sizeof(HYPERVISOR_INTERFACE));
}

BOOLEAN HypervisorInterfaceIsAvailable(VOID)
{
    return TRUE;
}