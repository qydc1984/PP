#pragma once
#include <ntdef.h>

typedef enum _CPU_VENDOR
{
    CPU_UNKNOWN = 0,
    CPU_INTEL,
    CPU_AMD
} CPU_VENDOR;

// ͳһ��hypervisor���ܽӿ�
typedef struct _HYPERVISOR_INTERFACE
{
    NTSTATUS (*Initialize)(void);
    NTSTATUS (*Uninitialize)(void);
    NTSTATUS (*StartOnAllCpus)(void);
    NTSTATUS (*StopOnAllCpus)(void);
    NTSTATUS (*HandleIoctl)(ULONG IoControlCode, PVOID InputBuffer, ULONG InputBufferLength, 
                           PVOID OutputBuffer, ULONG OutputBufferLength, PULONG BytesReturned);
} HYPERVISOR_INTERFACE;

// CPU��⺯��
CPU_VENDOR DetectCpuVendor(void);
BOOLEAN IsIntelVmxSupported(void);
BOOLEAN IsAmdSvmSupported(void);

// ��ȡ��Ӧ��hypervisor�ӿ�
const HYPERVISOR_INTERFACE* GetHypervisorInterface(CPU_VENDOR Vendor);