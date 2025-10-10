/**
 * memhv_adapter.h - memhv �������ӿ�
 *
 * Ϊ memhv �ṩͳһ�Ľӿ������
 */

#pragma once
#include <ntddk.h>

#ifdef __cplusplus
extern "C" {
#endif

    // ʹ��ϵͳ���������
    typedef void* HANDLE;

    // ����״̬��麯��
    BOOLEAN MemHvIsPresent(VOID);
    BOOLEAN MemHvInitialize(VOID);
    VOID MemHvCleanup(VOID);

    // �ڴ��������
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

    // ���̹�����
    PEPROCESS MemHvGetProcessById(HANDLE process_id);
    unsigned __int64 MemHvGetProcessDirectoryBase(PEPROCESS process);
    PEPROCESS MemHvGetNextProcess(PEPROCESS process);
    BOOLEAN MemHvIsProcessValid(PEPROCESS process);
    HANDLE MemHvGetProcessId(PEPROCESS process);
    void MemHvReferenceObject(void* object);
    void MemHvDereferenceObject(void* object);

    // ��������
    NTSTATUS MemHvEnableProtection(VOID);
    NTSTATUS MemHvDisableProtection(VOID);

    // MSR �͵�ַ��֤
    BOOLEAN MemHvIsValidMsr(unsigned int msr);
    BOOLEAN MemHvIsValidUserAddress(unsigned __int64 address);

    // �߼�����
    unsigned __int64 MemHvVmCall(unsigned __int64 hypercall_number, void* context);

#ifdef __cplusplus
}
#endif