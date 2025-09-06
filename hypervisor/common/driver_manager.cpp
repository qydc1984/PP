#include "cpu_detection.h"
#include "../airhv/Source/main.h"  // airhv入口
#include "../memhv-driver/amd_vmm.h"  // AMD支持

static const HYPERVISOR_INTERFACE* g_CurrentHypervisor = nullptr;
static CPU_VENDOR g_DetectedCpuVendor = CPU_UNKNOWN;

// 统一的驱动入口
extern "C" NTSTATUS UnifiedDriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(RegistryPath);
    
    Log("[<] Universal Hypervisor Driver Entry\n");
    
    // 检测CPU类型
    g_DetectedCpuVendor = DetectCpuVendor();
    Log("[*] Detected CPU Vendor: %s\n", 
        g_DetectedCpuVendor == CPU_INTEL ? "Intel" : 
        g_DetectedCpuVendor == CPU_AMD ? "AMD" : "Unknown");
    
    // 根据CPU类型选择对应的hypervisor实现
    g_CurrentHypervisor = GetHypervisorInterface(g_DetectedCpuVendor);
    if (!g_CurrentHypervisor)
    {
        Log("[!] No hypervisor support for this CPU\n");
        return STATUS_HV_FEATURE_UNAVAILABLE;
    }
    
    // 初始化对应的hypervisor
    NTSTATUS status = g_CurrentHypervisor->Initialize();
    if (!NT_SUCCESS(status))
    {
        Log("[!] Failed to initialize hypervisor\n");
        return status;
    }
    
    // 启动虚拟化
    status = g_CurrentHypervisor->StartOnAllCpus();
    if (!NT_SUCCESS(status))
    {
        Log("[!] Failed to start virtualization\n");
        g_CurrentHypervisor->Uninitialize();
        return status;
    }
    
    // 注册统一的IO控制处理
    DriverObject->DriverUnload = UnifiedDriverUnload;
    DriverObject->MajorFunction[IRP_MJ_CREATE] = UnifiedCreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = UnifiedCreateClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = UnifiedIoctlDispatcher;
    
    Log("[+] Universal Hypervisor started successfully\n");
    return STATUS_SUCCESS;
}

// 统一的IO控制分发器
NTSTATUS UnifiedIoctlDispatcher(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    ULONG ioControlCode = irpStack->Parameters.DeviceIoControl.IoControlCode;
    PVOID inputBuffer = Irp->AssociatedIrp.SystemBuffer;
    ULONG inputBufferLength = irpStack->Parameters.DeviceIoControl.InputBufferLength;
    PVOID outputBuffer = Irp->AssociatedIrp.SystemBuffer;
    ULONG outputBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;
    
    ULONG bytesReturned = 0;
    NTSTATUS status = STATUS_SUCCESS;
    
    if (g_CurrentHypervisor)
    {
        status = g_CurrentHypervisor->HandleIoctl(ioControlCode, inputBuffer, inputBufferLength,
                                                 outputBuffer, outputBufferLength, &bytesReturned);
    }
    else
    {
        status = STATUS_INVALID_DEVICE_REQUEST;
    }
    
    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = bytesReturned;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}

// 获取对应CPU的hypervisor接口
const HYPERVISOR_INTERFACE* GetHypervisorInterface(CPU_VENDOR Vendor)
{
    switch (Vendor)
    {
        case CPU_INTEL:
            return &g_AirHvInterface;  // 使用airhv的Intel实现
        case CPU_AMD:
            return &g_AmdVmmInterface; // 使用memhv风格的AMD实现
        default:
            return nullptr;
    }
}

// 统一的卸载例程
VOID UnifiedDriverUnload(_In_ PDRIVER_OBJECT DriverObject)
{
    UNREFERENCED_PARAMETER(DriverObject);
    
    Log("[<] Universal Hypervisor Driver Unload\n");
    
    if (g_CurrentHypervisor)
    {
        g_CurrentHypervisor->StopOnAllCpus();
        g_CurrentHypervisor->Uninitialize();
    }
    
    Log("[+] Driver unloaded successfully\n");
}