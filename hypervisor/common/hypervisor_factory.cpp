#include "hypervisor_factory.h"
#include "hypervisor_base.h"
#include <ntddk.h>

// 外部全局变量引用
extern HYPERVISOR_CONFIG g_HypervisorConfig;
extern HYPERVISOR_INTERFACE g_HypervisorInterface;

// MemHv 函数声明 (来自 memhv_adapter.h)
BOOLEAN MemHvIsPresent(VOID);
BOOLEAN MemHvInitialize(VOID);
VOID MemHvCleanup(VOID);
ULONG MemHvGetProcessorCount(VOID);
NTSTATUS MemHvReadProcessMemory(unsigned __int64 directory_base, unsigned __int64 address, void* buffer, unsigned __int64 size, unsigned __int64* bytes_read);
NTSTATUS MemHvWriteProcessMemory(unsigned __int64 directory_base, unsigned __int64 address, void* buffer, unsigned __int64 size, unsigned __int64* bytes_written);
NTSTATUS MemHvCopyProcessMemory(unsigned __int64 source_directory_base, unsigned __int64 source_address, unsigned __int64 destination_directory_base, unsigned __int64 destination_address, unsigned long size);
PEPROCESS MemHvGetProcessById(HANDLE process_id);
unsigned __int64 MemHvGetDirectoryBase(PEPROCESS process);
PEPROCESS MemHvGetNextProcess(PEPROCESS process);
BOOLEAN MemHvIsProcessValid(PEPROCESS process);
HANDLE MemHvGetProcessId(PEPROCESS process);
VOID MemHvReferenceObject(PVOID object);
VOID MemHvDereferenceObject(PVOID object);
NTSTATUS MemHvEnableProtection(VOID);
NTSTATUS MemHvDisableProtection(VOID);
BOOLEAN MemHvIsValidMsr(unsigned int msr);
BOOLEAN MemHvIsValidUserAddress(unsigned __int64 address);
unsigned __int64 MemHvVmCall(unsigned __int64 hypercall_number, void* context);

// AirHv 函数声明 (来自 airhv_adapter.h)
BOOLEAN AirHvIsPresent(VOID);
BOOLEAN AirHvInitialize(VOID);
VOID AirHvCleanup(VOID);
ULONG AirHvGetProcessorCount(VOID);
NTSTATUS AirHvReadProcessMemory(unsigned __int64 directory_base, unsigned __int64 address, void* buffer, unsigned __int64 size, unsigned __int64* bytes_read);
NTSTATUS AirHvWriteProcessMemory(unsigned __int64 directory_base, unsigned __int64 address, void* buffer, unsigned __int64 size, unsigned __int64* bytes_written);
NTSTATUS AirHvCopyProcessMemory(unsigned __int64 source_directory_base, unsigned __int64 source_address, unsigned __int64 destination_directory_base, unsigned __int64 destination_address, unsigned long size);
PEPROCESS AirHvGetProcessById(HANDLE process_id);
unsigned __int64 AirHvGetDirectoryBase(PEPROCESS process);
PEPROCESS AirHvGetNextProcess(PEPROCESS process);
BOOLEAN AirHvIsProcessValid(PEPROCESS process);
HANDLE AirHvGetProcessId(PEPROCESS process);
VOID AirHvReferenceObject(PVOID object);
VOID AirHvDereferenceObject(PVOID object);
NTSTATUS AirHvEnableProtection(VOID);
NTSTATUS AirHvDisableProtection(VOID);
BOOLEAN AirHvIsValidMsr(unsigned int msr);
BOOLEAN AirHvIsValidUserAddress(unsigned __int64 address);
unsigned __int64 AirHvVmCall(unsigned __int64 hypercall_number, void* context);

BOOLEAN HypervisorFactoryInitialize(HYPERVISOR_TYPE type)
{
    NTSTATUS status = STATUS_SUCCESS;

    // 检查是否已经初始化
    if (g_HypervisorConfig.IsInitialized) {
        return TRUE;
    }

    // 检查 hypervisor 类型是否支持
    if (!HypervisorFactoryIsSupported(type)) {
        return FALSE;
    }

    // 创建对应的接口
    status = HypervisorFactoryCreateInterface(type);
    if (!NT_SUCCESS(status)) {
        return FALSE;
    }

    // 设置配置
    g_HypervisorConfig.HypervisorType = type;
    g_HypervisorConfig.IsInitialized = TRUE;

    return TRUE;
}

VOID HypervisorFactoryCleanup(VOID)
{
    // 调用当前 hypervisor 的清理函数
    if (g_HypervisorInterface.Cleanup) {
        g_HypervisorInterface.Cleanup();
    }

    // 销毁当前接口
    HypervisorFactoryDestroyInterface();

    // 重置配置
    g_HypervisorConfig.HypervisorType = HYPERVISOR_TYPE_NONE;
    g_HypervisorConfig.IsInitialized = FALSE;
}

BOOLEAN HypervisorFactoryIsSupported(HYPERVISOR_TYPE type)
{
    switch (type) {
    case HYPERVISOR_TYPE_MEMHV:
        return MemHvIsPresent();
    case HYPERVISOR_TYPE_AIRHV:
        return AirHvIsPresent();
    case HYPERVISOR_TYPE_NONE:
    default:
        return FALSE;
    }
}

NTSTATUS HypervisorFactoryCreateInterface(HYPERVISOR_TYPE type)
{
    NTSTATUS status = STATUS_SUCCESS;

    // 根据类型创建对应的接口
    switch (type) {
    case HYPERVISOR_TYPE_MEMHV:
        status = HypervisorFactoryGetMemHvInterface(&g_HypervisorInterface);
        break;

    case HYPERVISOR_TYPE_AIRHV:
        status = HypervisorFactoryGetAirHvInterface(&g_HypervisorInterface);
        break;

    case HYPERVISOR_TYPE_NONE:
    default:
        status = STATUS_NOT_SUPPORTED;
        break;
    }

    return status;
}

VOID HypervisorFactoryDestroyInterface(VOID)
{
    // 清理接口结构
    RtlZeroMemory(&g_HypervisorInterface, sizeof(HYPERVISOR_INTERFACE));
}

// MemHv 接口实现
NTSTATUS HypervisorFactoryGetMemHvInterface(HYPERVISOR_INTERFACE* interface)
{
    if (!interface) {
        return STATUS_INVALID_PARAMETER;
    }

    // 初始化 MemHv 接口函数指针
    RtlZeroMemory(interface, sizeof(HYPERVISOR_INTERFACE));

    // 设置 MemHv 的具体实现
    interface->IsPresent = MemHvIsPresent;
    interface->Initialize = MemHvInitialize;
    interface->Cleanup = MemHvCleanup;
    interface->GetProcessorCount = MemHvGetProcessorCount;
    interface->ReadProcessMemory = MemHvReadProcessMemory;
    interface->WriteProcessMemory = MemHvWriteProcessMemory;
    interface->CopyProcessMemory = MemHvCopyProcessMemory;
    interface->GetProcessById = MemHvGetProcessById;
    interface->GetDirectoryBase = MemHvGetDirectoryBase;
    interface->GetNextProcess = MemHvGetNextProcess;
    interface->IsProcessValid = MemHvIsProcessValid;
    interface->GetProcessId = MemHvGetProcessId;
    interface->ReferenceObject = MemHvReferenceObject;
    interface->DereferenceObject = MemHvDereferenceObject;
    interface->EnableProtection = MemHvEnableProtection;
    interface->DisableProtection = MemHvDisableProtection;
    interface->IsValidMsr = MemHvIsValidMsr;
    interface->IsValidUserAddress = MemHvIsValidUserAddress;
    interface->VmCall = MemHvVmCall;

    return STATUS_SUCCESS;
}

// AirHv 接口实现
NTSTATUS HypervisorFactoryGetAirHvInterface(HYPERVISOR_INTERFACE* interface)
{
    if (!interface) {
        return STATUS_INVALID_PARAMETER;
    }

    // 初始化 AirHv 接口函数指针
    RtlZeroMemory(interface, sizeof(HYPERVISOR_INTERFACE));

    // 设置 AirHv 的具体实现
    interface->IsPresent = AirHvIsPresent;
    interface->Initialize = AirHvInitialize;
    interface->Cleanup = AirHvCleanup;
    interface->GetProcessorCount = AirHvGetProcessorCount;
    interface->ReadProcessMemory = AirHvReadProcessMemory;
    interface->WriteProcessMemory = AirHvWriteProcessMemory;
    interface->CopyProcessMemory = AirHvCopyProcessMemory;
    interface->GetProcessById = AirHvGetProcessById;
    interface->GetDirectoryBase = AirHvGetDirectoryBase;
    interface->GetNextProcess = AirHvGetNextProcess;
    interface->IsProcessValid = AirHvIsProcessValid;
    interface->GetProcessId = AirHvGetProcessId;
    interface->ReferenceObject = AirHvReferenceObject;
    interface->DereferenceObject = AirHvDereferenceObject;
    interface->EnableProtection = AirHvEnableProtection;
    interface->DisableProtection = AirHvDisableProtection;
    interface->IsValidMsr = AirHvIsValidMsr;
    interface->IsValidUserAddress = AirHvIsValidUserAddress;
    interface->VmCall = AirHvVmCall;

    return STATUS_SUCCESS;
}