#pragma once
#include "hypervisor_base.h"

#ifdef __cplusplus
extern "C" {
#endif

    // �ӿڹ�����
    NTSTATUS HypervisorInterfaceInitialize(HYPERVISOR_TYPE type);
    VOID HypervisorInterfaceCleanup(VOID);
    BOOLEAN HypervisorInterfaceIsAvailable(VOID);
    NTSTATUS HypervisorInterfaceValidate(VOID);

    // �ڴ�����ӿ�
    NTSTATUS HypervisorInterfaceReadProcessMemory(
        unsigned __int64 directory_base,
        unsigned __int64 address,
        void* buffer,
        unsigned __int64 size,
        unsigned __int64* bytes_read
    );

    NTSTATUS HypervisorInterfaceWriteProcessMemory(
        unsigned __int64 directory_base,
        unsigned __int64 address,
        void* buffer,
        unsigned __int64 size,
        unsigned __int64* bytes_written
    );

    NTSTATUS HypervisorInterfaceCopyProcessMemory(
        unsigned __int64 source_directory_base,
        unsigned __int64 source_address,
        unsigned __int64 destination_directory_base,
        unsigned __int64 destination_address,
        unsigned long size
    );

    // ���̹���ӿ�
    PEPROCESS HypervisorInterfaceGetProcessById(HANDLE process_id);
    unsigned __int64 HypervisorInterfaceGetDirectoryBase(PEPROCESS process);
    PEPROCESS HypervisorInterfaceGetNextProcess(PEPROCESS process);
    BOOLEAN HypervisorInterfaceIsProcessValid(PEPROCESS process);
    HANDLE HypervisorInterfaceGetProcessId(PEPROCESS process);
    VOID HypervisorInterfaceReferenceObject(PVOID object);
    VOID HypervisorInterfaceDereferenceObject(PVOID object);

    // �������ܽӿ�
    NTSTATUS HypervisorInterfaceEnableProtection(VOID);
    NTSTATUS HypervisorInterfaceDisableProtection(VOID);

    // ��֤���ܽӿ�
    BOOLEAN HypervisorInterfaceIsValidMsr(unsigned int msr);
    BOOLEAN HypervisorInterfaceIsValidUserAddress(unsigned __int64 address);

    // �߼����ܽӿ�
    unsigned __int64 HypervisorInterfaceVmCall(unsigned __int64 hypercall_number, void* context);

    // ����ͳ�ƽӿ�
    typedef struct _HYPERVISOR_PERFORMANCE_STATS
    {
        ULONGLONG ReadMemoryCalls;
        ULONGLONG WriteMemoryCalls;
        ULONGLONG CopyMemoryCalls;
        ULONGLONG ProcessOperations;
        ULONGLONG VmCallOperations;
        ULONGLONG LastError;
        LARGE_INTEGER LastOperationTime;
    } HYPERVISOR_PERFORMANCE_STATS, * PHYPERVISOR_PERFORMANCE_STATS;

    NTSTATUS HypervisorInterfaceGetPerformanceStats(HYPERVISOR_PERFORMANCE_STATS* stats);
    VOID HypervisorInterfaceResetPerformanceStats(VOID);

    // ������ӿ�
    typedef enum _HYPERVISOR_ERROR_CODE
    {
        HYPERVISOR_ERROR_SUCCESS = 0,
        HYPERVISOR_ERROR_NOT_INITIALIZED,
        HYPERVISOR_ERROR_INVALID_PARAMETER,
        HYPERVISOR_ERROR_NOT_SUPPORTED,
        HYPERVISOR_ERROR_ACCESS_DENIED,
        HYPERVISOR_ERROR_OUT_OF_MEMORY,
        HYPERVISOR_ERROR_TIMEOUT,
        HYPERVISOR_ERROR_INTERNAL_ERROR
    } HYPERVISOR_ERROR_CODE;

    HYPERVISOR_ERROR_CODE HypervisorInterfaceGetLastError(VOID);
    PCSTR HypervisorInterfaceGetLastErrorString(VOID);
    VOID HypervisorInterfaceClearLastError(VOID);

#ifdef __cplusplus
}
#endif