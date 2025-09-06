#include "cpu_detection.h"
#include "../airhv/Source/main.h"  // airhv���
#include "../memhv-driver/amd_vmm.h"  // AMD֧��

static const HYPERVISOR_INTERFACE* g_CurrentHypervisor = nullptr;
static CPU_VENDOR g_DetectedCpuVendor = CPU_UNKNOWN;

// ͳһ���������
extern "C" NTSTATUS UnifiedDriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(RegistryPath);
    
    Log("[<] Universal Hypervisor Driver Entry\n");
    
    // ���CPU����
    g_DetectedCpuVendor = DetectCpuVendor();
    Log("[*] Detected CPU Vendor: %s\n", 
        g_DetectedCpuVendor == CPU_INTEL ? "Intel" : 
        g_DetectedCpuVendor == CPU_AMD ? "AMD" : "Unknown");
    
    // ����CPU����ѡ���Ӧ��hypervisorʵ��
    g_CurrentHypervisor = GetHypervisorInterface(g_DetectedCpuVendor);
    if (!g_CurrentHypervisor)
    {
        Log("[!] No hypervisor support for this CPU\n");
        return STATUS_HV_FEATURE_UNAVAILABLE;
    }
    
    // ��ʼ����Ӧ��hypervisor
    NTSTATUS status = g_CurrentHypervisor->Initialize();
    if (!NT_SUCCESS(status))
    {
        Log("[!] Failed to initialize hypervisor\n");
        return status;
    }
    
    // �������⻯
    status = g_CurrentHypervisor->StartOnAllCpus();
    if (!NT_SUCCESS(status))
    {
        Log("[!] Failed to start virtualization\n");
        g_CurrentHypervisor->Uninitialize();
        return status;
    }
    
    // ע��ͳһ��IO���ƴ���
    DriverObject->DriverUnload = UnifiedDriverUnload;
    DriverObject->MajorFunction[IRP_MJ_CREATE] = UnifiedCreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = UnifiedCreateClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = UnifiedIoctlDispatcher;
    
    Log("[+] Universal Hypervisor started successfully\n");
    return STATUS_SUCCESS;
}

// ͳһ��IO���Ʒַ���
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

// ��ȡ��ӦCPU��hypervisor�ӿ�
const HYPERVISOR_INTERFACE* GetHypervisorInterface(CPU_VENDOR Vendor)
{
    switch (Vendor)
    {
        case CPU_INTEL:
            return &g_AirHvInterface;  // ʹ��airhv��Intelʵ��
        case CPU_AMD:
            return &g_AmdVmmInterface; // ʹ��memhv����AMDʵ��
        default:
            return nullptr;
    }
}

// ͳһ��ж������
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