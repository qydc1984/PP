#include "hypervisor_base.h"
#include <ntddk.h>

HYPERVISOR_CONFIG g_HypervisorConfig = {0};
HYPERVISOR_INTERFACE g_HypervisorInterface = {0};

BOOLEAN HypervisorIsPresent(VOID)
{
    if (g_HypervisorInterface.IsPresent) {
        return g_HypervisorInterface.IsPresent();
    }
    return FALSE;
}

BOOLEAN HypervisorInitialize(VOID)
{
    if (g_HypervisorInterface.Initialize) {
        return g_HypervisorInterface.Initialize();
    }
    return FALSE;
}

VOID HypervisorCleanup(VOID)
{
    if (g_HypervisorInterface.Cleanup) {
        g_HypervisorInterface.Cleanup();
    }
}

ULONG HypervisorGetProcessorCount(VOID)
{
    if (g_HypervisorInterface.GetProcessorCount) {
        return g_HypervisorInterface.GetProcessorCount();
    }
    return 0;
}

NTSTATUS HypervisorReadProcessMemory(
    unsigned __int64 directory_base,
    unsigned __int64 address,
    void* buffer,
    unsigned __int64 size,
    unsigned __int64* bytes_read)
{
    UNREFERENCED_PARAMETER(directory_base);

    if (!buffer || (size && !bytes_read))
        return STATUS_INVALID_PARAMETER;

    if (g_HypervisorInterface.ReadProcessMemory) {
        return g_HypervisorInterface.ReadProcessMemory(
            directory_base, address, buffer, size, bytes_read);
    }

    if (bytes_read)
        *bytes_read = 0;

    return STATUS_NOT_SUPPORTED;
}

NTSTATUS HypervisorWriteProcessMemory(
    unsigned __int64 directory_base,
    unsigned __int64 address,
    void* buffer,
    unsigned __int64 size,
    unsigned __int64* bytes_written)
{
    UNREFERENCED_PARAMETER(directory_base);

    if (!buffer || (size && !bytes_written))
        return STATUS_INVALID_PARAMETER;

    if (g_HypervisorInterface.WriteProcessMemory) {
        return g_HypervisorInterface.WriteProcessMemory(
            directory_base, address, buffer, size, bytes_written);
    }

    if (bytes_written)
        *bytes_written = 0;

    return STATUS_NOT_SUPPORTED;
}

NTSTATUS HypervisorCopyProcessMemory(
    unsigned __int64 source_directory_base,
    unsigned __int64 source_address,
    unsigned __int64 destination_directory_base,
    unsigned __int64 destination_address,
    unsigned long size)
{
    UNREFERENCED_PARAMETER(source_directory_base);
    UNREFERENCED_PARAMETER(destination_directory_base);

    if (g_HypervisorInterface.CopyProcessMemory) {
        return g_HypervisorInterface.CopyProcessMemory(
            source_directory_base, source_address,
            destination_directory_base, destination_address,
            size);
    }

    return STATUS_NOT_SUPPORTED;
}

PEPROCESS HypervisorGetProcessById(HANDLE process_id)
{
    if (g_HypervisorInterface.GetProcessById) {
        return g_HypervisorInterface.GetProcessById(process_id);
    }
    return NULL;
}

unsigned __int64 HypervisorGetDirectoryBase(PEPROCESS process)
{
    if (g_HypervisorInterface.GetDirectoryBase) {
        return g_HypervisorInterface.GetDirectoryBase(process);
    }
    return 0;
}

PEPROCESS HypervisorGetNextProcess(PEPROCESS process)
{
    if (g_HypervisorInterface.GetNextProcess) {
        return g_HypervisorInterface.GetNextProcess(process);
    }
    return NULL;
}

BOOLEAN HypervisorIsProcessValid(PEPROCESS process)
{
    if (g_HypervisorInterface.IsProcessValid) {
        return g_HypervisorInterface.IsProcessValid(process);
    }
    return FALSE;
}

HANDLE HypervisorGetProcessId(PEPROCESS process)
{
    if (g_HypervisorInterface.GetProcessId) {
        return g_HypervisorInterface.GetProcessId(process);
    }
    return NULL;
}

VOID HypervisorReferenceObject(PVOID object)
{
    if (g_HypervisorInterface.ReferenceObject && object) {
        g_HypervisorInterface.ReferenceObject(object);
    }
}

VOID HypervisorDereferenceObject(PVOID object)
{
    if (g_HypervisorInterface.DereferenceObject && object) {
        g_HypervisorInterface.DereferenceObject(object);
    }
}

NTSTATUS HypervisorEnableProtection(VOID)
{
    if (g_HypervisorInterface.EnableProtection) {
        return g_HypervisorInterface.EnableProtection();
    }
    return STATUS_NOT_SUPPORTED;
}

NTSTATUS HypervisorDisableProtection(VOID)
{
    if (g_HypervisorInterface.DisableProtection) {
        return g_HypervisorInterface.DisableProtection();
    }
    return STATUS_NOT_SUPPORTED;
}

BOOLEAN HypervisorIsValidMsr(unsigned int msr)
{
    if (g_HypervisorInterface.IsValidMsr) {
        return g_HypervisorInterface.IsValidMsr(msr);
    }
    return FALSE;
}

BOOLEAN HypervisorIsValidUserAddress(unsigned __int64 address)
{
    if (g_HypervisorInterface.IsValidUserAddress) {
        return g_HypervisorInterface.IsValidUserAddress(address);
    }
    return FALSE;
}

unsigned __int64 HypervisorVmCall(unsigned __int64 hypercall_number, void* context)
{
    if (g_HypervisorInterface.VmCall) {
        return g_HypervisorInterface.VmCall(hypercall_number, context);
    }
    return 0;
}

HYPERVISOR_CONFIG* HypervisorGetGlobalConfig(VOID)
{
    return &g_HypervisorConfig;
}

HYPERVISOR_INTERFACE* HypervisorGetGlobalInterface(VOID)
{
    return &g_HypervisorInterface;
}

HYPERVISOR_TYPE HypervisorGetType(VOID)
{
    return g_HypervisorConfig.HypervisorType;
}

VOID HypervisorSetType(HYPERVISOR_TYPE type)
{
    g_HypervisorConfig.HypervisorType = type;
}
