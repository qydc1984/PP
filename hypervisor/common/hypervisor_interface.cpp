#include "hypervisor_interface.h"
#include "hypervisor_base.h"
#include "hypervisor_factory.h"
#include <ntddk.h>
#include <ntstrsafe.h>

// �ⲿȫ�ֱ�������
extern HYPERVISOR_CONFIG g_HypervisorConfig;
extern HYPERVISOR_INTERFACE g_HypervisorInterface;

// ����ͳ�ƽṹ
static HYPERVISOR_PERFORMANCE_STATS g_PerformanceStats = { 0 };
static HYPERVISOR_ERROR_CODE g_LastError = HYPERVISOR_ERROR_SUCCESS;
static KSPIN_LOCK g_InterfaceLock = { 0 };
static BOOLEAN g_InterfaceInitialized = FALSE;

// ������Ϣ�ַ��������ģ�
static const CHAR* g_ErrorStrings[] = {
    "�ɹ�",
    "δ��ʼ��",
    "��Ч����",
    "��֧��",
    "���ʱ��ܾ�",
    "�ڴ治��",
    "��ʱ",
    "�ڲ�����"
};

// �ڲ�������������
static VOID HypervisorInterfaceSetError(HYPERVISOR_ERROR_CODE error);

NTSTATUS HypervisorInterfaceInitialize(HYPERVISOR_TYPE type)
{
    NTSTATUS status = STATUS_SUCCESS;
    KIRQL oldIrql = 0;

    // ����Ƿ��Ѿ���ʼ��
    if (g_InterfaceInitialized) {
        return STATUS_SUCCESS;
    }

    // ��ʼ��������
    KeInitializeSpinLock(&g_InterfaceLock);

    // ��ȡ��
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // ͨ��������ʼ��ָ�����͵� hypervisor
        if (!HypervisorFactoryInitialize(type)) {
            status = STATUS_UNSUCCESSFUL;
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_INTERNAL_ERROR);
            __leave;
        }

        // ��֤�ӿ�
        status = HypervisorInterfaceValidate();
        if (!NT_SUCCESS(status)) {
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_INTERNAL_ERROR);
            __leave;
        }

        g_InterfaceInitialized = TRUE;
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_SUCCESS);
    }
    __finally {
        // �ͷ���
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return status;
}

VOID HypervisorInterfaceCleanup(VOID)
{
    KIRQL oldIrql = 0;

    // ��ȡ��
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // ������
        HypervisorFactoryCleanup();

        // ��������ͳ��
        RtlZeroMemory(&g_PerformanceStats, sizeof(g_PerformanceStats));

        // ����״̬
        g_InterfaceInitialized = FALSE;
        g_LastError = HYPERVISOR_ERROR_SUCCESS;
    }
    __finally {
        // �ͷ���
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }
}

BOOLEAN HypervisorInterfaceIsAvailable(VOID)
{
    BOOLEAN result = FALSE;
    KIRQL oldIrql = 0;

    // ��ȡ��
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        if (g_InterfaceInitialized && g_HypervisorInterface.IsPresent) {
            result = g_HypervisorInterface.IsPresent();
        }
    }
    __finally {
        // �ͷ���
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return result;
}

NTSTATUS HypervisorInterfaceValidate(VOID)
{
    // �������ӿں���ָ��
    if (!g_HypervisorInterface.IsPresent ||
        !g_HypervisorInterface.Initialize ||
        !g_HypervisorInterface.Cleanup) {
        return STATUS_INVALID_PARAMETER;
    }

    return STATUS_SUCCESS;
}

// �ڴ�����ӿ�ʵ��
NTSTATUS HypervisorInterfaceReadProcessMemory(
    unsigned __int64 directory_base,
    unsigned __int64 address,
    void* buffer,
    unsigned __int64 size,
    unsigned __int64* bytes_read)
{
    NTSTATUS status = STATUS_SUCCESS;
    KIRQL oldIrql = 0;

    // ������֤
    if (!buffer || (size && !bytes_read)) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_INVALID_PARAMETER);
        return STATUS_INVALID_PARAMETER;
    }

    // ����ʼ��״̬
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return STATUS_UNSUCCESSFUL;
    }

    // ��ȡ��
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // ���õײ�ʵ��
        if (g_HypervisorInterface.ReadProcessMemory) {
            status = g_HypervisorInterface.ReadProcessMemory(
                directory_base, address, buffer, size, bytes_read);

            // ��������ͳ��
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
        // �ͷ���
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

    // ������֤
    if (!buffer || (size && !bytes_written)) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_INVALID_PARAMETER);
        return STATUS_INVALID_PARAMETER;
    }

    // ����ʼ��״̬
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return STATUS_UNSUCCESSFUL;
    }

    // ��ȡ��
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // ���õײ�ʵ��
        if (g_HypervisorInterface.WriteProcessMemory) {
            status = g_HypervisorInterface.WriteProcessMemory(
                directory_base, address, buffer, size, bytes_written);

            // ��������ͳ��
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
        // �ͷ���
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

    // ����ʼ��״̬
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return STATUS_UNSUCCESSFUL;
    }

    // ��ȡ��
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // ���õײ�ʵ��
        if (g_HypervisorInterface.CopyProcessMemory) {
            status = g_HypervisorInterface.CopyProcessMemory(
                source_directory_base, source_address,
                destination_directory_base, destination_address,
                size);

            // ��������ͳ��
            g_PerformanceStats.CopyMemoryCalls++;
            KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
        }
        else {
            status = STATUS_NOT_SUPPORTED;
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_SUPPORTED);
        }
    }
    __finally {
        // �ͷ���
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return status;
}

// ���̹���ӿ�ʵ��
PEPROCESS HypervisorInterfaceGetProcessById(HANDLE process_id)
{
    PEPROCESS process = NULL;
    KIRQL oldIrql = 0;

    // ����ʼ��״̬
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return NULL;
    }

    // ��ȡ��
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // ���õײ�ʵ��
        if (g_HypervisorInterface.GetProcessById) {
            process = g_HypervisorInterface.GetProcessById(process_id);

            // ��������ͳ��
            g_PerformanceStats.ProcessOperations++;
            KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
        }
        else {
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_SUPPORTED);
        }
    }
    __finally {
        // �ͷ���
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return process;
}

unsigned __int64 HypervisorInterfaceGetDirectoryBase(PEPROCESS process)
{
    unsigned __int64 directory_base = 0;
    KIRQL oldIrql = 0;

    // ����ʼ��״̬
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return 0;
    }

    // ��ȡ��
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // ���õײ�ʵ��
        if (g_HypervisorInterface.GetDirectoryBase) {
            directory_base = g_HypervisorInterface.GetDirectoryBase(process);

            // ��������ͳ��
            g_PerformanceStats.ProcessOperations++;
            KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
        }
        else {
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_SUPPORTED);
        }
    }
    __finally {
        // �ͷ���
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return directory_base;
}

PEPROCESS HypervisorInterfaceGetNextProcess(PEPROCESS process)
{
    PEPROCESS next_process = NULL;
    KIRQL oldIrql = 0;

    // ����ʼ��״̬
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return NULL;
    }

    // ��ȡ��
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // ���õײ�ʵ��
        if (g_HypervisorInterface.GetNextProcess) {
            next_process = g_HypervisorInterface.GetNextProcess(process);

            // ��������ͳ��
            g_PerformanceStats.ProcessOperations++;
            KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
        }
        else {
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_SUPPORTED);
        }
    }
    __finally {
        // �ͷ���
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return next_process;
}

BOOLEAN HypervisorInterfaceIsProcessValid(PEPROCESS process)
{
    BOOLEAN result = FALSE;
    KIRQL oldIrql = 0;

    // ����ʼ��״̬
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return FALSE;
    }

    // ��ȡ��
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // ���õײ�ʵ��
        if (g_HypervisorInterface.IsProcessValid) {
            result = g_HypervisorInterface.IsProcessValid(process);

            // ��������ͳ��
            g_PerformanceStats.ProcessOperations++;
            KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
        }
        else {
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_SUPPORTED);
        }
    }
    __finally {
        // �ͷ���
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return result;
}

HANDLE HypervisorInterfaceGetProcessId(PEPROCESS process)
{
    HANDLE process_id = NULL;
    KIRQL oldIrql = 0;

    // ����ʼ��״̬
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return NULL;
    }

    // ��ȡ��
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // ���õײ�ʵ��
        if (g_HypervisorInterface.GetProcessId) {
            process_id = g_HypervisorInterface.GetProcessId(process);

            // ��������ͳ��
            g_PerformanceStats.ProcessOperations++;
            KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
        }
        else {
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_SUPPORTED);
        }
    }
    __finally {
        // �ͷ���
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return process_id;
}

VOID HypervisorInterfaceReferenceObject(PVOID object)
{
    KIRQL oldIrql = 0;

    // ����ʼ��״̬
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return;
    }

    // ������
    if (!object) {
        return;
    }

    // ��ȡ��
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // ���õײ�ʵ��
        if (g_HypervisorInterface.ReferenceObject) {
            g_HypervisorInterface.ReferenceObject(object);

            // ��������ͳ��
            g_PerformanceStats.ProcessOperations++;
            KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
        }
        else {
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_SUPPORTED);
        }
    }
    __finally {
        // �ͷ���
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }
}

VOID HypervisorInterfaceDereferenceObject(PVOID object)
{
    KIRQL oldIrql = 0;

    // ����ʼ��״̬
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return;
    }

    // ������
    if (!object) {
        return;
    }

    // ��ȡ��
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // ���õײ�ʵ��
        if (g_HypervisorInterface.DereferenceObject) {
            g_HypervisorInterface.DereferenceObject(object);

            // ��������ͳ��
            g_PerformanceStats.ProcessOperations++;
            KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
        }
        else {
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_SUPPORTED);
        }
    }
    __finally {
        // �ͷ���
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }
}

// �������ܽӿ�ʵ��
NTSTATUS HypervisorInterfaceEnableProtection(VOID)
{
    NTSTATUS status = STATUS_SUCCESS;
    KIRQL oldIrql = 0;

    // ����ʼ��״̬
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return STATUS_UNSUCCESSFUL;
    }

    // ��ȡ��
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // ���õײ�ʵ��
        if (g_HypervisorInterface.EnableProtection) {
            status = g_HypervisorInterface.EnableProtection();

            // ��������ͳ��
            KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
        }
        else {
            status = STATUS_NOT_SUPPORTED;
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_SUPPORTED);
        }
    }
    __finally {
        // �ͷ���
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return status;
}

NTSTATUS HypervisorInterfaceDisableProtection(VOID)
{
    NTSTATUS status = STATUS_SUCCESS;
    KIRQL oldIrql = 0;

    // ����ʼ��״̬
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return STATUS_UNSUCCESSFUL;
    }

    // ��ȡ��
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // ���õײ�ʵ��
        if (g_HypervisorInterface.DisableProtection) {
            status = g_HypervisorInterface.DisableProtection();

            // ��������ͳ��
            KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
        }
        else {
            status = STATUS_NOT_SUPPORTED;
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_SUPPORTED);
        }
    }
    __finally {
        // �ͷ���
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return status;
}

// ��֤���ܽӿ�ʵ��
BOOLEAN HypervisorInterfaceIsValidMsr(unsigned int msr)
{
    BOOLEAN result = FALSE;
    KIRQL oldIrql = 0;

    // ����ʼ��״̬
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return FALSE;
    }

    // ��ȡ��
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // ���õײ�ʵ��
        if (g_HypervisorInterface.IsValidMsr) {
            result = g_HypervisorInterface.IsValidMsr(msr);

            // ��������ͳ��
            KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
        }
        else {
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_SUPPORTED);
        }
    }
    __finally {
        // �ͷ���
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return result;
}

BOOLEAN HypervisorInterfaceIsValidUserAddress(unsigned __int64 address)
{
    BOOLEAN result = FALSE;
    KIRQL oldIrql = 0;

    // ����ʼ��״̬
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return FALSE;
    }

    // ��ȡ��
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // ���õײ�ʵ��
        if (g_HypervisorInterface.IsValidUserAddress) {
            result = g_HypervisorInterface.IsValidUserAddress(address);

            // ��������ͳ��
            KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
        }
        else {
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_SUPPORTED);
        }
    }
    __finally {
        // �ͷ���
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return result;
}

// �߼����ܽӿ�ʵ��
unsigned __int64 HypervisorInterfaceVmCall(unsigned __int64 hypercall_number, void* context)
{
    unsigned __int64 result = 0;
    KIRQL oldIrql = 0;

    // ����ʼ��״̬
    if (!g_InterfaceInitialized) {
        HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_INITIALIZED);
        return 0;
    }

    // ��ȡ��
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // ���õײ�ʵ��
        if (g_HypervisorInterface.VmCall) {
            result = g_HypervisorInterface.VmCall(hypercall_number, context);

            // ��������ͳ��
            g_PerformanceStats.VmCallOperations++;
            KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
        }
        else {
            HypervisorInterfaceSetError(HYPERVISOR_ERROR_NOT_SUPPORTED);
        }
    }
    __finally {
        // �ͷ���
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return result;
}

// ����ͳ�ƽӿ�ʵ��
NTSTATUS HypervisorInterfaceGetPerformanceStats(HYPERVISOR_PERFORMANCE_STATS* stats)
{
    KIRQL oldIrql = 0;

    if (!stats) {
        return STATUS_INVALID_PARAMETER;
    }

    // ��ȡ��
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // ��������ͳ������
        RtlCopyMemory(stats, &g_PerformanceStats, sizeof(HYPERVISOR_PERFORMANCE_STATS));
    }
    __finally {
        // �ͷ���
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }

    return STATUS_SUCCESS;
}

VOID HypervisorInterfaceResetPerformanceStats(VOID)
{
    KIRQL oldIrql = 0;

    // ��ȡ��
    KeAcquireSpinLock(&g_InterfaceLock, &oldIrql);

    __try {
        // ��������ͳ������
        RtlZeroMemory(&g_PerformanceStats, sizeof(g_PerformanceStats));
        KeQuerySystemTimePrecise(&g_PerformanceStats.LastOperationTime);
    }
    __finally {
        // �ͷ���
        KeReleaseSpinLock(&g_InterfaceLock, oldIrql);
    }
}

// ������ӿ�ʵ��
HYPERVISOR_ERROR_CODE HypervisorInterfaceGetLastError(VOID)
{
    return g_LastError;
}

PCSTR HypervisorInterfaceGetLastErrorString(VOID)
{
    ULONG errorIndex = (ULONG)g_LastError;

    if (errorIndex >= sizeof(g_ErrorStrings) / sizeof(g_ErrorStrings[0])) {
        errorIndex = 0; // Ĭ��Ϊ�ɹ�
    }

    return g_ErrorStrings[errorIndex];
}

VOID HypervisorInterfaceClearLastError(VOID)
{
    g_LastError = HYPERVISOR_ERROR_SUCCESS;
}

// �ڲ���������ʵ��
static VOID HypervisorInterfaceSetError(HYPERVISOR_ERROR_CODE error)
{
    g_LastError = error;
    g_PerformanceStats.LastError = (ULONGLONG)error;
}