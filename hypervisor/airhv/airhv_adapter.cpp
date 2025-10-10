/**
 * airhv_adapter.cpp - airhv ������ʵ��
 *
 * ʵ�� airhv ��ͳһ�ӿ������
 */

#include "airhv_adapter.h"
#include "common.h"
#include <ntddk.h>
#include <intrin.h>

 // ǰ������
extern "C" __vmm_context* g_vmm_context;

// �ڲ�������������
static BOOLEAN IsAirHvRunning(VOID);
static unsigned int GetProcessorCount(VOID);

BOOLEAN airhv_adapter_is_running(VOID)
{
    return IsAirHvRunning();
}

BOOLEAN airhv_adapter_init(VOID)
{
    // ����Ƿ��Ѿ���ʼ��
    if (g_vmm_context != nullptr && g_vmm_context->hv_presence)
    {
        return TRUE;
    }

    // ��� VMX ֧��
    int cpuInfo[4];
    __cpuid(cpuInfo, 1);
    if (!(cpuInfo[2] & (1 << 5))) // ��� VMX ֧��λ
    {
        return FALSE;
    }

    return TRUE;
}

VOID airhv_adapter_cleanup(VOID)
{
    // ������Դ
    // ʵ��ʵ��Ӧ��ֹͣ���� VMM ����
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

    // ������
    if (!buffer || size == 0)
    {
        if (bytes_read) *bytes_read = 0;
        return STATUS_INVALID_PARAMETER;
    }

    __try
    {
        // ʹ���ں��ڴ濽��
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

    // ������
    if (!buffer || size == 0)
    {
        if (bytes_written) *bytes_written = 0;
        return STATUS_INVALID_PARAMETER;
    }

    __try
    {
        // ʹ���ں��ڴ濽��
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

    // ������
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

// ���̹�����ʵ��
void* AirHvGetProcessById(HANDLE process_id)
{
    // ע�⣺������Ҫʹ�� airhv �ṩ�� PsLookupProcessByProcessId
    // ���ڲ����������������Ǽ������Ѿ��������ط�����
    if ((ULONG_PTR)process_id == 4)
    {
        return PsInitialSystemProcess;
    }

    // ��� airhv �ṩ�����������ֱ��ʹ��
    // ���򷵻� NULL
    return nullptr;
}

unsigned __int64 AirHvGetDirectoryBase(PEPROCESS process)
{
    if (!process)
        return 0;

    // ��ȡ���̵�ҳĿ¼��ַ (CR3)
    return __readcr3();
}

void* AirHvGetNextProcess(PEPROCESS process)
{
    if (!process)
        return PsInitialSystemProcess;

    // ��ȡ��һ������
    // ע�⣺����Ҫ���� EPROCESS ��������򻯴���
    return nullptr;
}

BOOLEAN AirHvIsProcessValid(PEPROCESS process)
{
    if (!process)
        return FALSE;

    // �򵥵���Ч�Լ��
    return MmIsAddressValid(process);
}

unsigned __int64 AirHvGetProcessId(PEPROCESS process)
{
    if (!process)
        return 0;

    // ע�⣺������Ҫʹ�� airhv �ṩ�� PsGetProcessId
    // ���ڲ����������������Ǽ������Ѿ��������ط�����
    return (unsigned __int64)4; // �򻯴���
}

void AirHvReferenceObject(void* object)
{
    if (object && MmIsAddressValid(object))
    {
        // ע�⣺������Ҫʹ�� airhv �ṩ�� ObReferenceObject
        // ���ڲ����������������Ǽ������Ѿ��������ط�����
        // �򻯴���
    }
}

void AirHvDereferenceObject(void* object)
{
    if (object && MmIsAddressValid(object))
    {
        // ע�⣺������Ҫʹ�� airhv �ṩ�� ObDereferenceObject
        // ���ڲ����������������Ǽ������Ѿ��������ط�����
        // �򻯴���
    }
}

BOOLEAN AirHvProtectSelf(VOID)
{
    // �������ұ���
    // ����Ӧ������ VMM ������־
    return TRUE;
}

BOOLEAN AirHvIsValidMsr(unsigned int msr)
{
    // ��֤ MSR �Ƿ���Ч
    if (msr > 0xFFFFFFFF)
        return FALSE;

    // ��������Ч MSR ��Χ
    return TRUE;
}

BOOLEAN AirHvIsValidUserAddress(unsigned __int64 address)
{
    // ����Ƿ�Ϊ��Ч���û�̬��ַ
    return (address < 0x800000000000ULL) && (address >= 0x10000ULL);
}

// �߼�����ʵ��
BOOLEAN airhv_adapter_set_msr_intercept(unsigned int msr_index, BOOLEAN read_intercept, BOOLEAN write_intercept)
{
    // ���� MSR ����
    // ʵ��ʵ��Ӧ�õ��� airhv �ľ��庯��
    UNREFERENCED_PARAMETER(msr_index);
    UNREFERENCED_PARAMETER(read_intercept);
    UNREFERENCED_PARAMETER(write_intercept);
    return TRUE;
}

BOOLEAN airhv_adapter_set_io_intercept(unsigned short port_number, BOOLEAN intercept)
{
    // ���� IO �˿�����
    // ʵ��ʵ��Ӧ�õ��� airhv �ľ��庯��
    UNREFERENCED_PARAMETER(port_number);
    UNREFERENCED_PARAMETER(intercept);
    return TRUE;
}

BOOLEAN airhv_adapter_inject_interrupt(unsigned int processor_index, unsigned char vector,
    unsigned int interruption_type, unsigned int error_code,
    BOOLEAN error_code_valid)
{
    // ע���ж�
    // ʵ��ʵ��Ӧ�õ��� airhv �ľ��庯��
    UNREFERENCED_PARAMETER(processor_index);
    UNREFERENCED_PARAMETER(vector);
    UNREFERENCED_PARAMETER(interruption_type);
    UNREFERENCED_PARAMETER(error_code);
    UNREFERENCED_PARAMETER(error_code_valid);
    return TRUE;
}

BOOLEAN airhv_adapter_hook_page(unsigned __int64 physical_address, void(*hook_handler)(void*))
{
    // ����ҳ��
    // ʵ��ʵ��Ӧ�õ��� airhv �ľ��庯��
    UNREFERENCED_PARAMETER(physical_address);
    UNREFERENCED_PARAMETER(hook_handler);
    return TRUE;
}

BOOLEAN airhv_adapter_unhook_page(unsigned __int64 physical_address)
{
    // ȡ��ҳ�湳��
    // ʵ��ʵ��Ӧ�õ��� airhv �ľ��庯��
    UNREFERENCED_PARAMETER(physical_address);
    return TRUE;
}

BOOLEAN airhv_adapter_get_guest_registers(unsigned int processor_index, void* registers)
{
    UNREFERENCED_PARAMETER(processor_index);

    if (!registers || !MmIsAddressValid(registers))
        return FALSE;

    // ʵ��ʵ��Ӧ�ô� VMCS ��ȡ�Ĵ���
    // ����򻯴���
    UNREFERENCED_PARAMETER(registers);
    return TRUE;
}

BOOLEAN airhv_adapter_set_guest_registers(unsigned int processor_index, void* registers)
{
    UNREFERENCED_PARAMETER(processor_index);

    if (!registers || !MmIsAddressValid(registers))
        return FALSE;

    // ʵ��ʵ��Ӧ��д��Ĵ����� VMCS
    // ����򻯴���
    UNREFERENCED_PARAMETER(registers);
    return TRUE;
}

unsigned __int64 airhv_adapter_vmcall(unsigned int processor_index, unsigned __int64 hypercall_number, void* context)
{
    UNREFERENCED_PARAMETER(processor_index);

    // ִ�� VMCALL
    // ʵ��ʵ��Ӧ�õ��� airhv �ľ��庯��
    UNREFERENCED_PARAMETER(hypercall_number);
    UNREFERENCED_PARAMETER(context);
    return 0;
}

// �ڲ���������ʵ��
static BOOLEAN IsAirHvRunning(VOID)
{
    // ��� airhv �Ƿ�����
    if (g_vmm_context != nullptr)
    {
        return g_vmm_context->hv_presence == true;
    }

    // ��� CPUID �Ƿ�֧�� VMX
    int cpuInfo[4];
    __cpuid(cpuInfo, 1);
    return (cpuInfo[2] & (1 << 5)) != 0; // VMX ֧��λ
}

static unsigned int GetProcessorCount(VOID)
{
    if (g_vmm_context)
    {
        return g_vmm_context->processor_count;
    }

    // ����ϵͳ��������
    // ע�⣺����ʹ�� KeQueryActiveProcessorCount���������Ѿ��������ط�����
    return 1; // �򻯴���
}