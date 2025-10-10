#pragma once

#include <ntdef.h>
#include <ntstatus.h>

#ifdef __cplusplus
extern "C" {
#endif

    // Hypervisor ����ö��
    typedef enum _HYPERVISOR_TYPE
    {
        HYPERVISOR_TYPE_NONE = 0,
        HYPERVISOR_TYPE_MEMHV = 1,
        HYPERVISOR_TYPE_AIRHV = 2,
        HYPERVISOR_TYPE_MAX = 3
    } HYPERVISOR_TYPE;

    // Hypervisor ���ýṹ
#pragma pack(push, 8)
    typedef struct _HYPERVISOR_CONFIG
    {
        ULONG ProcessorCount;
        HYPERVISOR_TYPE HypervisorType;
        BOOLEAN IsInitialized;
        PVOID Reserved[8];
    } HYPERVISOR_CONFIG;
#pragma pack(pop)

    // ǰ������
    struct _EPROCESS;
    typedef struct _EPROCESS* PEPROCESS;
    typedef void* HANDLE;

    // Hypervisor �ӿں���ָ���
    typedef struct _HYPERVISOR_INTERFACE
    {
        // ��������
        BOOLEAN(*IsPresent)(VOID);
        BOOLEAN(*Initialize)(VOID);
        VOID(*Cleanup)(VOID);
        ULONG(*GetProcessorCount)(VOID);

        // �ڴ����
        NTSTATUS(*ReadProcessMemory)(
            unsigned __int64 directory_base,
            unsigned __int64 address,
            void* buffer,
            unsigned __int64 size,
            unsigned __int64* bytes_read
            );

        NTSTATUS(*WriteProcessMemory)(
            unsigned __int64 directory_base,
            unsigned __int64 address,
            void* buffer,
            unsigned __int64 size,
            unsigned __int64* bytes_written
            );

        NTSTATUS(*CopyProcessMemory)(
            unsigned __int64 source_directory_base,
            unsigned __int64 source_address,
            unsigned __int64 destination_directory_base,
            unsigned __int64 destination_address,
            unsigned long size
            );

        // ���̹���
        PEPROCESS(*GetProcessById)(HANDLE process_id);
        unsigned __int64 (*GetDirectoryBase)(PEPROCESS process);
        PEPROCESS(*GetNextProcess)(PEPROCESS process);
        BOOLEAN(*IsProcessValid)(PEPROCESS process);
        HANDLE(*GetProcessId)(PEPROCESS process);
        VOID(*ReferenceObject)(PVOID object);
        VOID(*DereferenceObject)(PVOID object);

        // ��������
        NTSTATUS(*EnableProtection)(VOID);
        NTSTATUS(*DisableProtection)(VOID);

        // ��֤����
        BOOLEAN(*IsValidMsr)(unsigned int msr);
        BOOLEAN(*IsValidUserAddress)(unsigned __int64 address);

        // �߼�����
        unsigned __int64 (*VmCall)(unsigned __int64 hypercall_number, void* context);

    } HYPERVISOR_INTERFACE;

    // ȫ�ֱ�������
    extern HYPERVISOR_CONFIG g_HypervisorConfig;
    extern HYPERVISOR_INTERFACE g_HypervisorInterface;

    // ������������
    BOOLEAN HypervisorIsPresent(VOID);
    BOOLEAN HypervisorInitialize(VOID);
    VOID HypervisorCleanup(VOID);
    ULONG HypervisorGetProcessorCount(VOID);

    // �ڴ��������
    NTSTATUS HypervisorReadProcessMemory(
        unsigned __int64 directory_base,
        unsigned __int64 address,
        void* buffer,
        unsigned __int64 size,
        unsigned __int64* bytes_read
    );

    NTSTATUS HypervisorWriteProcessMemory(
        unsigned __int64 directory_base,
        unsigned __int64 address,
        void* buffer,
        unsigned __int64 size,
        unsigned __int64* bytes_written
    );

    NTSTATUS HypervisorCopyProcessMemory(
        unsigned __int64 source_directory_base,
        unsigned __int64 source_address,
        unsigned __int64 destination_directory_base,
        unsigned __int64 destination_address,
        unsigned long size
    );

    // ���̹�����
    PEPROCESS HypervisorGetProcessById(HANDLE process_id);
    unsigned __int64 HypervisorGetDirectoryBase(PEPROCESS process);
    PEPROCESS HypervisorGetNextProcess(PEPROCESS process);
    BOOLEAN HypervisorIsProcessValid(PEPROCESS process);
    HANDLE HypervisorGetProcessId(PEPROCESS process);
    VOID HypervisorReferenceObject(PVOID object);
    VOID HypervisorDereferenceObject(PVOID object);

    // �������ܺ���
    NTSTATUS HypervisorEnableProtection(VOID);
    NTSTATUS HypervisorDisableProtection(VOID);

    // ��֤���ܺ���
    BOOLEAN HypervisorIsValidMsr(unsigned int msr);
    BOOLEAN HypervisorIsValidUserAddress(unsigned __int64 address);

    // �߼����ܺ���
    unsigned __int64 HypervisorVmCall(unsigned __int64 hypercall_number, void* context);

    // ���ߺ���
    HYPERVISOR_CONFIG* HypervisorGetGlobalConfig(VOID);
    HYPERVISOR_INTERFACE* HypervisorGetGlobalInterface(VOID);
    HYPERVISOR_TYPE HypervisorGetType(VOID);
    VOID HypervisorSetType(HYPERVISOR_TYPE type);

    // �궨��
#define HYPERVISOR_IS_PRESENT() \
    (g_HypervisorInterface.IsPresent ? g_HypervisorInterface.IsPresent() : FALSE)

#define HYPERVISOR_INITIALIZE() \
    (g_HypervisorInterface.Initialize ? g_HypervisorInterface.Initialize() : FALSE)

#define HYPERVISOR_CLEANUP() \
    do { if (g_HypervisorInterface.Cleanup) g_HypervisorInterface.Cleanup(); } while(0)

#define HYPERVISOR_GET_PROCESSOR_COUNT() \
    (g_HypervisorInterface.GetProcessorCount ? g_HypervisorInterface.GetProcessorCount() : 0)

#ifdef __cplusplus
}
#endif