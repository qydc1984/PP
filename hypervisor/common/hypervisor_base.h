#pragma once

#include <ntdef.h>
#include <ntstatus.h>

#ifdef __cplusplus
extern "C" {
#endif

    // Hypervisor 类型枚举
    typedef enum _HYPERVISOR_TYPE
    {
        HYPERVISOR_TYPE_NONE = 0,
        HYPERVISOR_TYPE_MEMHV = 1,
        HYPERVISOR_TYPE_AIRHV = 2,
        HYPERVISOR_TYPE_MAX = 3
    } HYPERVISOR_TYPE;

    // Hypervisor 配置结构
#pragma pack(push, 8)
    typedef struct _HYPERVISOR_CONFIG
    {
        ULONG ProcessorCount;
        HYPERVISOR_TYPE HypervisorType;
        BOOLEAN IsInitialized;
        PVOID Reserved[8];
    } HYPERVISOR_CONFIG;
#pragma pack(pop)

    // 前向声明
    struct _EPROCESS;
    typedef struct _EPROCESS* PEPROCESS;
    typedef void* HANDLE;

    // Hypervisor 接口函数指针表
    typedef struct _HYPERVISOR_INTERFACE
    {
        // 基本操作
        BOOLEAN(*IsPresent)(VOID);
        BOOLEAN(*Initialize)(VOID);
        VOID(*Cleanup)(VOID);
        ULONG(*GetProcessorCount)(VOID);

        // 内存操作
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

        // 进程管理
        PEPROCESS(*GetProcessById)(HANDLE process_id);
        unsigned __int64 (*GetDirectoryBase)(PEPROCESS process);
        PEPROCESS(*GetNextProcess)(PEPROCESS process);
        BOOLEAN(*IsProcessValid)(PEPROCESS process);
        HANDLE(*GetProcessId)(PEPROCESS process);
        VOID(*ReferenceObject)(PVOID object);
        VOID(*DereferenceObject)(PVOID object);

        // 保护功能
        NTSTATUS(*EnableProtection)(VOID);
        NTSTATUS(*DisableProtection)(VOID);

        // 验证功能
        BOOLEAN(*IsValidMsr)(unsigned int msr);
        BOOLEAN(*IsValidUserAddress)(unsigned __int64 address);

        // 高级功能
        unsigned __int64 (*VmCall)(unsigned __int64 hypercall_number, void* context);

    } HYPERVISOR_INTERFACE;

    // 全局变量声明
    extern HYPERVISOR_CONFIG g_HypervisorConfig;
    extern HYPERVISOR_INTERFACE g_HypervisorInterface;

    // 基本函数声明
    BOOLEAN HypervisorIsPresent(VOID);
    BOOLEAN HypervisorInitialize(VOID);
    VOID HypervisorCleanup(VOID);
    ULONG HypervisorGetProcessorCount(VOID);

    // 内存操作函数
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

    // 进程管理函数
    PEPROCESS HypervisorGetProcessById(HANDLE process_id);
    unsigned __int64 HypervisorGetDirectoryBase(PEPROCESS process);
    PEPROCESS HypervisorGetNextProcess(PEPROCESS process);
    BOOLEAN HypervisorIsProcessValid(PEPROCESS process);
    HANDLE HypervisorGetProcessId(PEPROCESS process);
    VOID HypervisorReferenceObject(PVOID object);
    VOID HypervisorDereferenceObject(PVOID object);

    // 保护功能函数
    NTSTATUS HypervisorEnableProtection(VOID);
    NTSTATUS HypervisorDisableProtection(VOID);

    // 验证功能函数
    BOOLEAN HypervisorIsValidMsr(unsigned int msr);
    BOOLEAN HypervisorIsValidUserAddress(unsigned __int64 address);

    // 高级功能函数
    unsigned __int64 HypervisorVmCall(unsigned __int64 hypercall_number, void* context);

    // 工具函数
    HYPERVISOR_CONFIG* HypervisorGetGlobalConfig(VOID);
    HYPERVISOR_INTERFACE* HypervisorGetGlobalInterface(VOID);
    HYPERVISOR_TYPE HypervisorGetType(VOID);
    VOID HypervisorSetType(HYPERVISOR_TYPE type);

    // 宏定义
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