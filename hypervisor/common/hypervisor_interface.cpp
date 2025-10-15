#include "hypervisor_interface.h"
#include "hypervisor_base.h"
#include "hypervisor_factory.h"
#include <ntddk.h>
#include <ntstrsafe.h>

// 外部全局变量引用
extern HYPERVISOR_CONFIG g_HypervisorConfig;
extern HYPERVISOR_INTERFACE g_HypervisorInterface;

// 性能统计结构
static HYPERVISOR_PERFORMANCE_STATS g_PerformanceStats = { 0 };
static HYPERVISOR_ERROR_CODE g_LastError = HYPERVISOR_ERROR_SUCCESS;
static KSPIN_LOCK g_InterfaceLock = { 0 };
static BOOLEAN g_InterfaceInitialized = FALSE;

// 错误消息字符串（中文）
static const CHAR* g_ErrorStrings[] = {
    "成功",
    "未初始化",
    "无效参数",
    "不支持",
    "访问被拒绝",
    "内存不足",
    "超时",
    "内部错误"
};

// 内部辅助函数声明
static VOID HypervisorInterfaceSetError(HYPERVISOR_ERROR_CODE error);

NTSTATUS HypervisorInterfaceInitialize(HYPERVISOR_TYPE type)
{
    NTSTATUS status = STATUS_SUCCESS;
    KIRQL oldIrql = 0;

    // 检查是否已经初始化
    if (g_InterfaceInitialized) {
        return STATUS_SUCCESS;
    }

    // 初始化自旋锁
    KeInitializeSpinLock(&g_InterfaceLock);

    // 获取锁
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // 通过工厂初始化指定类型的 hypervisor
        if (!HypervisorFactoryInitialize(type)) {
            status = STATUS_UNSUCCESSFUL;
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_INTERNAL_ERROR);
            __leave;
        }

        // 验证接口
        status = HypervisorInterfaceValidate();
        if (!NT_SUCCESS(status)) {
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_INTERNAL_ERROR);
            __leave;
        }

        g_InterfaceInitialized = TRUE;
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_SUCCESS);
    }
    __finally {
        // 释放锁
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return status;
}

VOID HypervisorInterfaceCleanup(VOID)
{
    KIRQL oldIrql = 0;

    // 获取锁
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // 清理工厂
        HypervisorFactoryCleanup();

        // 重置性能统计
        RtlZeroMemory(&g_PerformanceStats, sizeof(g_PerformanceStats));

        // 重置状态
        g_InterfaceInitialized = FALSE;
        g_LastError = HYPERVISOR_ERROR_SUCCESS;
    }
    __finally {
        // 释放锁
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }
}

BOOLEAN HypervisorInterfaceIsAvailable(VOID)
{
    BOOLEAN result = FALSE;
    KIRQL oldIrql = 0;

    // 获取锁
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        if (g_InterfaceInitialized && g_HypervisorInterface.IsPresent) {
            result = g_HypervisorInterface.IsPresent();
        }
    }
    __finally {
        // 释放锁
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return result;
}

NTSTATUS HypervisorInterfaceValidate(VOID)
{
    // 检查基本接口函数指针
    if (!g_HypervisorInterface.IsPresent ||
        !g_HypervisorInterface.Initialize ||
        !g_HypervisorInterface.Cleanup) {
        return STATUS_INVALID_PARAMETER;
    }

    return STATUS_SUCCESS;
}

// 内存操作接口实现
NTSTATUS HypervisorInterfaceReadProcessMemory(
    unsigned __int64 directory_base,
    unsigned __int64 address,
    void* buffer,
    unsigned __int64 size,
    unsigned __int64* bytes_read)
{
    NTSTATUS status = STATUS_SUCCESS;
    KIRQL oldIrql = 0;

    // 参数验证
    if (!buffer || (size && !bytes_read)) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_INVALID_PARAMETER);
        return STATUS_INVALID_PARAMETER;
    }

    // 检查初始化状态
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return STATUS_UNSUCCESSFUL;
    }

    // 获取锁
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // 调用底层实现
        if (g_HypervisorInterface.ReadProcessMemory) {
            status = g_HypervisorInterface.ReadProcessMemory(
                directory_base, address, buffer, size, bytes_read);

            // 更新性能统计
            g_PerformanceStats.ReadMemoryCalls++;
            KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
        }
        else {
            status = STATUS_NOT_SUPPORTED;
            if (bytes_read) {
                *bytes_read = 0;
            }
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_SUPPORTED);
        }
    }
    __finally {
        // 释放锁
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return status;
}

NTSTATUS HypervisorInterfaceWriteProcessMemory(
    unsigned __int64 directory_base,
    unsigned __int64 address,
    void* buffer,
    unsigned __int64 size,
    unsigned __int64* bytes_written)
{
    NTSTATUS status = STATUS_SUCCESS;
    KIRQL oldIrql = 0;

    // 参数验证
    if (!buffer || (size && !bytes_written)) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_INVALID_PARAMETER);
        return STATUS_INVALID_PARAMETER;
    }

    // 检查初始化状态
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return STATUS_UNSUCCESSFUL;
    }

    // 获取锁
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // 调用底层实现
        if (g_HypervisorInterface.WriteProcessMemory) {
            status = g_HypervisorInterface.WriteProcessMemory(
                directory_base, address, buffer, size, bytes_written);

            // 更新性能统计
            g_PerformanceStats.WriteMemoryCalls++;
            KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
        }
        else {
            status = STATUS_NOT_SUPPORTED;
            if (bytes_written) {
                *bytes_written = 0;
            }
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_SUPPORTED);
        }
    }
    __finally {
        // 释放锁
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return status;
}

NTSTATUS HypervisorInterfaceCopyProcessMemory(
    unsigned __int64 source_directory_base,
    unsigned __int64 source_address,
    unsigned __int64 destination_directory_base,
    unsigned __int64 destination_address,
    unsigned long size)
{
    NTSTATUS status = STATUS_SUCCESS;
    KIRQL oldIrql = 0;

    // 检查初始化状态
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return STATUS_UNSUCCESSFUL;
    }

    // 获取锁
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // 调用底层实现
        if (g_HypervisorInterface.CopyProcessMemory) {
            status = g_HypervisorInterface.CopyProcessMemory(
                source_directory_base, source_address,
                destination_directory_base, destination_address,
                size);

            // 更新性能统计
            g_PerformanceStats.CopyMemoryCalls++;
            KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
        }
        else {
            status = STATUS_NOT_SUPPORTED;
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_SUPPORTED);
        }
    }
    __finally {
        // 释放锁
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return status;
}

// 进程管理接口实现
PEPROCESS HypervisorInterfaceGetProcessById(HANDLE process_id)
{
    PEPROCESS process = NULL;
    KIRQL oldIrql = 0;

    // 检查初始化状态
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return NULL;
    }

    // 获取锁
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // 调用底层实现
        if (g_HypervisorInterface.GetProcessById) {
            process = g_HypervisorInterface.GetProcessById(process_id);

            // 更新性能统计
            g_PerformanceStats.ProcessOperations++;
            KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
        }
        else {
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_SUPPORTED);
        }
    }
    __finally {
        // 释放锁
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return process;
}

unsigned __int64 HypervisorInterfaceGetDirectoryBase(PEPROCESS process)
{
    unsigned __int64 directory_base = 0;
    KIRQL oldIrql = 0;

    // 检查初始化状态
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return 0;
    }

    // 获取锁
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // 调用底层实现
        if (g_HypervisorInterface.GetDirectoryBase) {
            directory_base = g_HypervisorInterface.GetDirectoryBase(process);

            // 更新性能统计
            g_PerformanceStats.ProcessOperations++;
            KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
        }
        else {
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_SUPPORTED);
        }
    }
    __finally {
        // 释放锁
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return directory_base;
}

PEPROCESS HypervisorInterfaceGetNextProcess(PEPROCESS process)
{
    PEPROCESS next_process = NULL;
    KIRQL oldIrql = 0;

    // 检查初始化状态
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return NULL;
    }

    // 获取锁
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // 调用底层实现
        if (g_HypervisorInterface.GetNextProcess) {
            next_process = g_HypervisorInterface.GetNextProcess(process);

            // 更新性能统计
            g_PerformanceStats.ProcessOperations++;
            KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
        }
        else {
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_SUPPORTED);
        }
    }
    __finally {
        // 释放锁
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return next_process;
}

BOOLEAN HypervisorInterfaceIsProcessValid(PEPROCESS process)
{
    BOOLEAN result = FALSE;
    KIRQL oldIrql = 0;

    // 检查初始化状态
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return FALSE;
    }

    // 获取锁
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // 调用底层实现
        if (g_HypervisorInterface.IsProcessValid) {
            result = g_HypervisorInterface.IsProcessValid(process);

            // 更新性能统计
            g_PerformanceStats.ProcessOperations++;
            KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
        }
        else {
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_SUPPORTED);
        }
    }
    __finally {
        // 释放锁
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return result;
}

HANDLE HypervisorInterfaceGetProcessId(PEPROCESS process)
{
    HANDLE process_id = NULL;
    KIRQL oldIrql = 0;

    // 检查初始化状态
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return NULL;
    }

    // 获取锁
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // 调用底层实现
        if (g_HypervisorInterface.GetProcessId) {
            process_id = g_HypervisorInterface.GetProcessId(process);

            // 更新性能统计
            g_PerformanceStats.ProcessOperations++;
            KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
        }
        else {
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_SUPPORTED);
        }
    }
    __finally {
        // 释放锁
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return process_id;
}

VOID HypervisorInterfaceReferenceObject(PVOID object)
{
    KIRQL oldIrql = 0;

    // 检查初始化状态
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return;
    }

    // 检查对象
    if (!object) {
        return;
    }

    // 获取锁
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // 调用底层实现
        if (g_HypervisorInterface.ReferenceObject) {
            g_HypervisorInterface.ReferenceObject(object);

            // 更新性能统计
            g_PerformanceStats.ProcessOperations++;
            KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
        }
        else {
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_SUPPORTED);
        }
    }
    __finally {
        // 释放锁
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }
}

VOID HypervisorInterfaceDereferenceObject(PVOID object)
{
    KIRQL oldIrql = 0;

    // 检查初始化状态
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return;
    }

    // 检查对象
    if (!object) {
        return;
    }

    // 获取锁
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // 调用底层实现
        if (g_HypervisorInterface.DereferenceObject) {
            g_HypervisorInterface.DereferenceObject(object);

            // 更新性能统计
            g_PerformanceStats.ProcessOperations++;
            KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
        }
        else {
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_SUPPORTED);
        }
    }
    __finally {
        // 释放锁
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }
}

// 保护功能接口实现
NTSTATUS HypervisorInterfaceEnableProtection(VOID)
{
    NTSTATUS status = STATUS_SUCCESS;
    KIRQL oldIrql = 0;

    // 检查初始化状态
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return STATUS_UNSUCCESSFUL;
    }

    // 获取锁
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // 调用底层实现
        if (g_HypervisorInterface.EnableProtection) {
            status = g_HypervisorInterface.EnableProtection();

            // 更新性能统计
            KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
        }
        else {
            status = STATUS_NOT_SUPPORTED;
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_SUPPORTED);
        }
    }
    __finally {
        // 释放锁
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return status;
}

NTSTATUS HypervisorInterfaceDisableProtection(VOID)
{
    NTSTATUS status = STATUS_SUCCESS;
    KIRQL oldIrql = 0;

    // 检查初始化状态
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return STATUS_UNSUCCESSFUL;
    }

    // 获取锁
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // 调用底层实现
        if (g_HypervisorInterface.DisableProtection) {
            status = g_HypervisorInterface.DisableProtection();

            // 更新性能统计
            KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
        }
        else {
            status = STATUS_NOT_SUPPORTED;
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_SUPPORTED);
        }
    }
    __finally {
        // 释放锁
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return status;
}

// 验证功能接口实现
BOOLEAN HypervisorInterfaceIsValidMsr(unsigned int msr)
{
    BOOLEAN result = FALSE;
    KIRQL oldIrql = 0;

    // 检查初始化状态
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return FALSE;
    }

    // 获取锁
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // 调用底层实现
        if (g_HypervisorInterface.IsValidMsr) {
            result = g_HypervisorInterface.IsValidMsr(msr);

            // 更新性能统计
            KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
        }
        else {
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_SUPPORTED);
        }
    }
    __finally {
        // 释放锁
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return result;
}

BOOLEAN HypervisorInterfaceIsValidUserAddress(unsigned __int64 address)
{
    BOOLEAN result = FALSE;
    KIRQL oldIrql = 0;

    // 检查初始化状态
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return FALSE;
    }

    // 获取锁
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // 调用底层实现
        if (g_HypervisorInterface.IsValidUserAddress) {
            result = g_HypervisorInterface.IsValidUserAddress(address);

            // 更新性能统计
            KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
        }
        else {
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_SUPPORTED);
        }
    }
    __finally {
        // 释放锁
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return result;
}

// 高级功能接口实现
unsigned __int64 HypervisorInterfaceVmCall(unsigned __int64 hypercall_number, void* context)
{
    unsigned __int64 result = 0;
    KIRQL oldIrql = 0;

    // 检查初始化状态
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return 0;
    }

    // 获取锁
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // 调用底层实现
        if (g_HypervisorInterface.VmCall) {
            result = g_HypervisorInterface.VmCall(hypercall_number, context);

            // 更新性能统计
            g_PerformanceStats.VmCallOperations++;
            KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
        }
        else {
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_SUPPORTED);
        }
    }
    __finally {
        // 释放锁
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return result;
}

// 性能统计接口实现
NTSTATUS HypervisorInterfaceGetPerformanceStats(HYPERVISOR_PERFORMANCE_STATS* stats)
{
    KIRQL oldIrql = 0;

    if (!stats) {
        return STATUS_INVALID_PARAMETER;
    }

    // 获取锁
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // 复制性能统计数据
        RtlCopyMemory(stats, &g_PerformanceStats, sizeof(HYPERVISOR_PERFORMANCE_STATS));
    }
    __finally {
        // 释放锁
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return STATUS_SUCCESS;
}

VOID HypervisorInterfaceResetPerformanceStats(VOID)
{
    KIRQL oldIrql = 0;

    // 获取锁
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // 重置性能统计数据
        RtlZeroMemory(&g_PerformanceStats, sizeof(g_PerformanceStats));
        KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
    }
    __finally {
        // 释放锁
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }
}

// 错误处理接口实现
HYPERVISOR_ERROR_CODE HypervisorInterfaceGetLastError(VOID)
{
    return g_LastError;
}

PCSTR HypervisorInterfaceGetLastErrorString(VOID)
{
    ULONG errorIndex = (ULONG)g_LastError;

    if (errorIndex >= sizeof(g_ErrorStrings) / sizeof(g_ErrorStrings[0])) {
        errorIndex = 0; // 默认为成功
    }

    return g_ErrorStrings[errorIndex];
}

VOID HypervisorInterfaceClearLastError(VOID)
{
    g_LastError = HYPERVISOR_ERROR_SUCCESS;
}

// 内部辅助函数实现
static VOID HypervisorInterfaceSetError(HYPERVISOR_ERROR_CODE error)
{
    g_LastError = error;
    g_PerformanceStats.LastError = (ULONGLONG)error;
}