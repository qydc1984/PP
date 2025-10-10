/**
 * memhv_adapter.h - memhv 适配器接口
 *
 * 为 memhv 提供统一的接口适配层
 */

#pragma once
#include <ntddk.h>

#ifdef __cplusplus
extern "C" {
#endif

    // 使用系统定义的类型
    typedef void* HANDLE;

    // 基本状态检查函数
    BOOLEAN MemHvIsPresent(VOID);
    BOOLEAN MemHvInitialize(VOID);
    VOID MemHvCleanup(VOID);

    // 内存操作函数
    NTSTATUS MemHvReadProcessMemory(
        unsigned __int64 directory_base,
        unsigned __int64 address,
        void* buffer,
        unsigned __int64 size,
        unsigned __int64* bytes_read
    );

    NTSTATUS MemHvWriteProcessMemory(
        unsigned __int64 directory_base,
        unsigned __int64 address,
        void* buffer,
        unsigned __int64 size,
        unsigned __int64* bytes_written
    );

    NTSTATUS MemHvReadProcessMemorySafe(
        unsigned __int64 directory_base,
        unsigned __int64 address,
        void* buffer,
        unsigned __int64 size,
        unsigned __int64* bytes_read
    );

    // 进程管理函数
    PEPROCESS MemHvGetProcessById(HANDLE process_id);
    unsigned __int64 MemHvGetProcessDirectoryBase(PEPROCESS process);
    PEPROCESS MemHvGetNextProcess(PEPROCESS process);
    BOOLEAN MemHvIsProcessValid(PEPROCESS process);
    HANDLE MemHvGetProcessId(PEPROCESS process);
    void MemHvReferenceObject(void* object);
    void MemHvDereferenceObject(void* object);

    // 保护功能
    NTSTATUS MemHvEnableProtection(VOID);
    NTSTATUS MemHvDisableProtection(VOID);

    // MSR 和地址验证
    BOOLEAN MemHvIsValidMsr(unsigned int msr);
    BOOLEAN MemHvIsValidUserAddress(unsigned __int64 address);

    // 高级功能
    unsigned __int64 MemHvVmCall(unsigned __int64 hypercall_number, void* context);

#ifdef __cplusplus
}
#endif