/**
 * airhv_adapter.h - airhv 适配器接口
 *
 * 为 airhv 提供统一的接口适配层
 */

#pragma once
#include <ntddk.h>

#ifdef __cplusplus
extern "C" {
#endif

    // 前向声明避免类型冲突
    struct _EPROCESS;
    struct _ETHREAD;
    typedef struct _EPROCESS* PEPROCESS;
    typedef struct _ETHREAD* PETHREAD;
    typedef void* HANDLE;

    // 客户机寄存器结构体定义
    typedef struct _VMEXIT_GUEST_REGISTERS
    {
        unsigned __int64 rax;
        unsigned __int64 rbx;
        unsigned __int64 rcx;
        unsigned __int64 rdx;
        unsigned __int64 rsi;
        unsigned __int64 rdi;
        unsigned __int64 rbp;
        unsigned __int64 rsp;
        unsigned __int64 r8;
        unsigned __int64 r9;
        unsigned __int64 r10;
        unsigned __int64 r11;
        unsigned __int64 r12;
        unsigned __int64 r13;
        unsigned __int64 r14;
        unsigned __int64 r15;
        unsigned __int64 rip;
        unsigned __int64 rflags;
    } VMEXIT_GUEST_REGISTERS, * PVMEXIT_GUEST_REGISTERS;

    // 基本状态检查函数
    BOOLEAN airhv_adapter_is_running(VOID);
    BOOLEAN airhv_adapter_init(VOID);
    VOID airhv_adapter_cleanup(VOID);
    unsigned int airhv_adapter_get_proc_count(VOID);

    // 内存操作函数
    NTSTATUS AirHvReadProcessMemory(
        unsigned int processor_index,
        unsigned __int64 directory_base,
        unsigned __int64 address,
        void* buffer,
        unsigned __int64 size,
        unsigned __int64* bytes_read
    );

    NTSTATUS AirHvWriteProcessMemory(
        unsigned int processor_index,
        unsigned __int64 directory_base,
        unsigned __int64 address,
        void* buffer,
        unsigned __int64 size,
        unsigned __int64* bytes_written
    );

    NTSTATUS AirHvCopyProcessMemory(
        unsigned int processor_index,
        unsigned __int64 source_directory_base,
        unsigned __int64 source_address,
        unsigned __int64 destination_directory_base,
        unsigned __int64 destination_address,
        unsigned long size
    );

    // 进程管理函数
    void* AirHvGetProcessById(HANDLE process_id);
    unsigned __int64 AirHvGetDirectoryBase(PEPROCESS process);
    void* AirHvGetNextProcess(PEPROCESS process);
    BOOLEAN AirHvIsProcessValid(PEPROCESS process);
    unsigned __int64 AirHvGetProcessId(PEPROCESS process);
    void AirHvReferenceObject(void* object);
    void AirHvDereferenceObject(void* object);

    // 保护功能
    BOOLEAN AirHvProtectSelf(VOID);

    // MSR 和地址验证
    BOOLEAN AirHvIsValidMsr(unsigned int msr);
    BOOLEAN AirHvIsValidUserAddress(unsigned __int64 address);

    // 高级功能
    BOOLEAN airhv_adapter_set_msr_intercept(unsigned int msr_index, BOOLEAN read_intercept, BOOLEAN write_intercept);
    BOOLEAN airhv_adapter_set_io_intercept(unsigned short port_number, BOOLEAN intercept);
    BOOLEAN airhv_adapter_inject_interrupt(unsigned int processor_index, unsigned char vector,
        unsigned int interruption_type, unsigned int error_code,
        BOOLEAN error_code_valid);
    BOOLEAN airhv_adapter_hook_page(unsigned __int64 physical_address, void(*hook_handler)(void*));
    BOOLEAN airhv_adapter_unhook_page(unsigned __int64 physical_address);
    BOOLEAN airhv_adapter_get_guest_registers(unsigned int processor_index, void* registers);
    BOOLEAN airhv_adapter_set_guest_registers(unsigned int processor_index, void* registers);
    unsigned __int64 airhv_adapter_vmcall(unsigned int processor_index, unsigned __int64 hypercall_number, void* context);

    // 兼容性函数（为测试客户端提供统一接口）
    static inline BOOLEAN airhv_adapter_is_present(VOID)
    {
        return airhv_adapter_is_running();
    }

    static inline unsigned int airhv_adapter_get_processor_count(VOID)
    {
        return airhv_adapter_get_proc_count();
    }

    static inline NTSTATUS airhv_adapter_initialize(VOID)
    {
        return airhv_adapter_init() ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
    }

    static inline VOID airhv_adapter_cleanup_interface(VOID)
    {
        airhv_adapter_cleanup();
    }

    static inline NTSTATUS airhv_adapter_read_process_memory(
        unsigned __int64 directory_base,
        unsigned __int64 address,
        void* buffer,
        unsigned __int64 size,
        unsigned __int64* bytes_read)
    {
        return AirHvReadProcessMemory(0, directory_base, address, buffer, size, bytes_read);
    }

    static inline NTSTATUS airhv_adapter_write_process_memory(
        unsigned __int64 directory_base,
        unsigned __int64 address,
        void* buffer,
        unsigned __int64 size,
        unsigned __int64* bytes_written)
    {
        return AirHvWriteProcessMemory(0, directory_base, address, buffer, size, bytes_written);
    }

#ifdef __cplusplus
}
#endif

// C++ 封装类（可选）
#ifdef __cplusplus

class AirHvAdapter
{
public:
    static bool IsPresent()
    {
        return airhv_adapter_is_running() ? true : false;
    }

    static unsigned int GetProcessorCount()
    {
        return airhv_adapter_get_processor_count();
    }

    static bool Initialize()
    {
        return airhv_adapter_init() ? true : false;
    }

    static void Cleanup()
    {
        airhv_adapter_cleanup();
    }

    static NTSTATUS ReadProcessMemory(
        unsigned __int64 directoryBase,
        unsigned __int64 address,
        void* buffer,
        unsigned __int64 size,
        unsigned __int64* bytesRead = nullptr)
    {
        return AirHvReadProcessMemory(0, directoryBase, address, buffer, size, bytesRead);
    }

    static NTSTATUS WriteProcessMemory(
        unsigned __int64 directoryBase,
        unsigned __int64 address,
        const void* buffer,
        unsigned __int64 size,
        unsigned __int64* bytesWritten = nullptr)
    {
        return AirHvWriteProcessMemory(0, directoryBase, address, const_cast<void*>(buffer), size, bytesWritten);
    }
};

#endif // __cplusplus