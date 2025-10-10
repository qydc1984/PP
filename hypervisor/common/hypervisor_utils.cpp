#include "hypervisor_utils.h"
#include "hypervisor_base.h"
#include <ntddk.h>

VOID HypervisorUtilsDebugPrint(PCCHAR format, ...)
{
    UNREFERENCED_PARAMETER(format);
}

NTSTATUS HypervisorUtilsMemoryCopy(PVOID dest, const VOID* src, SIZE_T size)
{
    UNREFERENCED_PARAMETER(dest);
    UNREFERENCED_PARAMETER(src);
    UNREFERENCED_PARAMETER(size);
    return STATUS_SUCCESS;
}

VOID HypervisorUtilsMemoryZero(PVOID buffer, SIZE_T size)
{
    UNREFERENCED_PARAMETER(buffer);
    UNREFERENCED_PARAMETER(size);
}