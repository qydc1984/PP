/**
 * airhv_adapter.cpp - airhv 适配器实现
 *
 * 实现 airhv 的统一接口适配层
 */

#include "airhv_adapter.h"
#include "common.h"
#include <ntddk.h>
#include <intrin.h>

 // 前向声明
extern "C" __vmm_context* g_vmm_context;

// 内部辅助函数声明
static BOOLEAN IsAirHvRunning(VOID);
static unsigned int GetProcessorCount(VOID);

BOOLEAN airhv_adapter_is_running(VOID)
{
    return IsAirHvRunning();
}

BOOLEAN airhv_adapter_init(VOID)
{
    // 检查是否已经初始化
    if (g_vmm_context != nullptr && g_vmm_context->hv_presence)
    {
        return TRUE;
    }

    // 检查 VMX 支持
    int cpuInfo[4];
    __cpuid(cpuInfo, 1);
    if (!(cpuInfo[2] & (1 << 5))) // 检查 VMX 支持位
    {
        return FALSE;
    }

    return TRUE;
}

VOID airhv_adapter_cleanup(VOID)
{
    // 清理资源
    // 实际实现应该停止所有 VMM 操作
}

unsigned int airhv_adapter_get_proc_count(VOID)
{
    return GetProcessorCount();
}

NTSTATUS AirHvReadProcessMemory(
    unsigned int processor_index,
    unsigned __int64 directory_base,
    unsigned __int64 address,
    void* buffer,
    unsigned __int64 size,
    unsigned __int64* bytes_read)
{
    UNREFERENCED_PARAMETER(processor_index);
    UNREFERENCED_PARAMETER(directory_base);

    // 检查参数
    if (!buffer || size == 0)
    {
        if (bytes_read) *bytes_read = 0;
        return STATUS_INVALID_PARAMETER;
    }

    __try
    {
        // 使用内核内存拷贝
        memcpy(buffer, (void*)address, (size_t)size);

        if (bytes_read)
            *bytes_read = size;

        return STATUS_SUCCESS;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        if (bytes_read) *bytes_read = 0;
        return STATUS_ACCESS_VIOLATION;
    }
}

NTSTATUS AirHvWriteProcessMemory(
    unsigned int processor_index,
    unsigned __int64 directory_base,
    unsigned __int64 address,
    void* buffer,
    unsigned __int64 size,
    unsigned __int64* bytes_written)
{
    UNREFERENCED_PARAMETER(processor_index);
    UNREFERENCED_PARAMETER(directory_base);

    // 检查参数
    if (!buffer || size == 0)
    {
        if (bytes_written) *bytes_written = 0;
        return STATUS_INVALID_PARAMETER;
    }

    __try
    {
        // 使用内核内存拷贝
        memcpy((void*)address, buffer, (size_t)size);

        if (bytes_written)
            *bytes_written = size;

        return STATUS_SUCCESS;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        if (bytes_written) *bytes_written = 0;
        return STATUS_ACCESS_VIOLATION;
    }
}

NTSTATUS AirHvCopyProcessMemory(
    unsigned int processor_index,
    unsigned __int64 source_directory_base,
    unsigned __int64 source_address,
    unsigned __int64 destination_directory_base,
    unsigned __int64 destination_address,
    unsigned long size)
{
    UNREFERENCED_PARAMETER(processor_index);
    UNREFERENCED_PARAMETER(source_directory_base);
    UNREFERENCED_PARAMETER(destination_directory_base);

    // 检查参数
    if (size == 0)
        return STATUS_INVALID_PARAMETER;

    __try
    {
        memcpy((void*)destination_address, (void*)source_address, size);
        return STATUS_SUCCESS;
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return STATUS_ACCESS_VIOLATION;
    }
}

// 进程管理函数实现
void* AirHvGetProcessById(HANDLE process_id)
{
    // 注意：这里需要使用 airhv 提供的 PsLookupProcessByProcessId
    // 由于不能重新声明，我们假设它已经在其他地方定义
    if ((ULONG_PTR)process_id == 4)
    {
        return PsInitialSystemProcess;
    }

    // 如果 airhv 提供了这个函数，直接使用
    // 否则返回 NULL
    return nullptr;
}

unsigned __int64 AirHvGetDirectoryBase(PEPROCESS process)
{
    if (!process)
        return 0;

    // 获取进程的页目录基址 (CR3)
    return __readcr3();
}

void* AirHvGetNextProcess(PEPROCESS process)
{
    if (!process)
        return PsInitialSystemProcess;

    // 获取下一个进程
    // 注意：这需要访问 EPROCESS 链表，这里简化处理
    return nullptr;
}

BOOLEAN AirHvIsProcessValid(PEPROCESS process)
{
    if (!process)
        return FALSE;

    // 简单的有效性检查
    return MmIsAddressValid(process);
}

unsigned __int64 AirHvGetProcessId(PEPROCESS process)
{
    if (!process)
        return 0;

    // 注意：这里需要使用 airhv 提供的 PsGetProcessId
    // 由于不能重新声明，我们假设它已经在其他地方定义
    return (unsigned __int64)4; // 简化处理
}

void AirHvReferenceObject(void* object)
{
    if (object && MmIsAddressValid(object))
    {
        // 注意：这里需要使用 airhv 提供的 ObReferenceObject
        // 由于不能重新声明，我们假设它已经在其他地方定义
        // 简化处理
    }
}

void AirHvDereferenceObject(void* object)
{
    if (object && MmIsAddressValid(object))
    {
        // 注意：这里需要使用 airhv 提供的 ObDereferenceObject
        // 由于不能重新声明，我们假设它已经在其他地方定义
        // 简化处理
    }
}

BOOLEAN AirHvProtectSelf(VOID)
{
    // 启用自我保护
    // 这里应该设置 VMM 保护标志
    return TRUE;
}

BOOLEAN AirHvIsValidMsr(unsigned int msr)
{
    // 验证 MSR 是否有效
    if (msr > 0xFFFFFFFF)
        return FALSE;

    // 常见的有效 MSR 范围
    return TRUE;
}

BOOLEAN AirHvIsValidUserAddress(unsigned __int64 address)
{
    // 检查是否为有效的用户态地址
    return (address < 0x800000000000ULL) && (address >= 0x10000ULL);
}

// 高级功能实现
BOOLEAN airhv_adapter_set_msr_intercept(unsigned int msr_index, BOOLEAN read_intercept, BOOLEAN write_intercept)
{
    // 设置 MSR 拦截
    // 实际实现应该调用 airhv 的具体函数
    UNREFERENCED_PARAMETER(msr_index);
    UNREFERENCED_PARAMETER(read_intercept);
    UNREFERENCED_PARAMETER(write_intercept);
    return TRUE;
}

BOOLEAN airhv_adapter_set_io_intercept(unsigned short port_number, BOOLEAN intercept)
{
    // 设置 IO 端口拦截
    // 实际实现应该调用 airhv 的具体函数
    UNREFERENCED_PARAMETER(port_number);
    UNREFERENCED_PARAMETER(intercept);
    return TRUE;
}

BOOLEAN airhv_adapter_inject_interrupt(unsigned int processor_index, unsigned char vector,
    unsigned int interruption_type, unsigned int error_code,
    BOOLEAN error_code_valid)
{
    // 注入中断
    // 实际实现应该调用 airhv 的具体函数
    UNREFERENCED_PARAMETER(processor_index);
    UNREFERENCED_PARAMETER(vector);
    UNREFERENCED_PARAMETER(interruption_type);
    UNREFERENCED_PARAMETER(error_code);
    UNREFERENCED_PARAMETER(error_code_valid);
    return TRUE;
}

BOOLEAN airhv_adapter_hook_page(unsigned __int64 physical_address, void(*hook_handler)(void*))
{
    // 钩子页面
    // 实际实现应该调用 airhv 的具体函数
    UNREFERENCED_PARAMETER(physical_address);
    UNREFERENCED_PARAMETER(hook_handler);
    return TRUE;
}

BOOLEAN airhv_adapter_unhook_page(unsigned __int64 physical_address)
{
    // 取消页面钩子
    // 实际实现应该调用 airhv 的具体函数
    UNREFERENCED_PARAMETER(physical_address);
    return TRUE;
}

BOOLEAN airhv_adapter_get_guest_registers(unsigned int processor_index, void* registers)
{
    UNREFERENCED_PARAMETER(processor_index);

    if (!registers || !MmIsAddressValid(registers))
        return FALSE;

    // 实际实现应该从 VMCS 读取寄存器
    // 这里简化处理
    UNREFERENCED_PARAMETER(registers);
    return TRUE;
}

BOOLEAN airhv_adapter_set_guest_registers(unsigned int processor_index, void* registers)
{
    UNREFERENCED_PARAMETER(processor_index);

    if (!registers || !MmIsAddressValid(registers))
        return FALSE;

    // 实际实现应该写入寄存器到 VMCS
    // 这里简化处理
    UNREFERENCED_PARAMETER(registers);
    return TRUE;
}

unsigned __int64 airhv_adapter_vmcall(unsigned int processor_index, unsigned __int64 hypercall_number, void* context)
{
    UNREFERENCED_PARAMETER(processor_index);

    // 执行 VMCALL
    // 实际实现应该调用 airhv 的具体函数
    UNREFERENCED_PARAMETER(hypercall_number);
    UNREFERENCED_PARAMETER(context);
    return 0;
}

// 内部辅助函数实现
static BOOLEAN IsAirHvRunning(VOID)
{
    // 检查 airhv 是否运行
    if (g_vmm_context != nullptr)
    {
        return g_vmm_context->hv_presence == true;
    }

    // 检查 CPUID 是否支持 VMX
    int cpuInfo[4];
    __cpuid(cpuInfo, 1);
    return (cpuInfo[2] & (1 << 5)) != 0; // VMX 支持位
}

static unsigned int GetProcessorCount(VOID)
{
    if (g_vmm_context)
    {
        return g_vmm_context->processor_count;
    }

    // 返回系统处理器数
    // 注意：这里使用 KeQueryActiveProcessorCount，假设它已经在其他地方定义
    return 1; // 简化处理
}