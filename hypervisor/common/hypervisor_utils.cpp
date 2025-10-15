#include "hypervisor_utils.h"
#include "hypervisor_base.h"
#include <ntddk.h>
#include <ntstrsafe.h>
#include <stdarg.h>

// CRC32���ұ�
static ULONG g_Crc32Table[256] = { 0 };

// ��ʼ��CRC32��
static VOID HypervisorUtilsInitializeCrc32Table(VOID)
{
    ULONG polynomial = 0xEDB88320;
    for (ULONG i = 0; i < 256; i++)
    {
        ULONG crc = i;
        for (ULONG j = 0; j < 8; j++)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ polynomial;
            else
                crc >>= 1;
        }
        g_Crc32Table[i] = crc;
    }
}

// �ַ���������ʵ��
NTSTATUS HypervisorUtilsStringCopy(PCHAR Destination, SIZE_T DestinationSize, PCCHAR Source)
{
    if (!Destination || !Source || DestinationSize == 0)
    {
        return STATUS_INVALID_PARAMETER;
    }

    return RtlStringCchCopyA(Destination, DestinationSize, Source);
}

NTSTATUS HypervisorUtilsStringConcat(PCHAR Destination, SIZE_T DestinationSize, PCCHAR Source)
{
    if (!Destination || !Source || DestinationSize == 0)
    {
        return STATUS_INVALID_PARAMETER;
    }

    return RtlStringCchCatA(Destination, DestinationSize, Source);
}

SIZE_T HypervisorUtilsStringLength(PCCHAR String)
{
    SIZE_T length = 0;

    if (String)
    {
        RtlStringCchLengthA(String, NTSTRSAFE_MAX_CCH, &length);
    }

    return length;
}

BOOLEAN HypervisorUtilsStringCompare(PCCHAR String1, PCCHAR String2)
{
    if (!String1 || !String2)
        return FALSE;

    return (RtlCompareString((PSTRING)String1, (PSTRING)String2, FALSE) == 0);
}

BOOLEAN HypervisorUtilsStringCompareNoCase(PCCHAR String1, PCCHAR String2)
{
    if (!String1 || !String2)
        return FALSE;

    ANSI_STRING ansi1, ansi2;
    RtlInitAnsiString(&ansi1, String1);
    RtlInitAnsiString(&ansi2, String2);

    return (RtlCompareString(&ansi1, &ansi2, TRUE) == 0);
}

NTSTATUS HypervisorUtilsStringToInteger(PCCHAR String, ULONG Base, PULONG Result)
{
    if (!String || !Result)
    {
        return STATUS_INVALID_PARAMETER;
    }

    return RtlCharToInteger(String, Base, Result);
}

// ��ʵ�ֵ�����ת�ַ�������
NTSTATUS HypervisorUtilsIntegerToString(ULONG Value, ULONG Base, PCHAR Buffer, SIZE_T BufferSize)
{
    if (!Buffer || BufferSize == 0)
    {
        return STATUS_INVALID_PARAMETER;
    }

    if (Base < 2 || Base > 36)
    {
        return STATUS_INVALID_PARAMETER;
    }

    // �����������
    if (Value == 0)
    {
        if (BufferSize < 2)
            return STATUS_BUFFER_TOO_SMALL;

        Buffer[0] = '0';
        Buffer[1] = '\0';
        return STATUS_SUCCESS;
    }

    // ת������
    CHAR temp[33]; // �㹻����32λ����
    ULONG index = 0;

    // ������ת��Ϊ�ַ�������
    ULONG tempValue = Value;
    while (tempValue > 0)
    {
        ULONG remainder = tempValue % Base;
        if (remainder < 10)
            temp[index] = (CHAR)('0' + remainder);
        else
            temp[index] = (CHAR)('A' + remainder - 10);

        tempValue /= Base;
        index++;
    }

    // ��黺������С
    if (index >= BufferSize)
        return STATUS_BUFFER_TOO_SMALL;

    // ��ת�ַ���
    for (ULONG i = 0; i < index; i++)
    {
        Buffer[i] = temp[index - 1 - i];
    }
    Buffer[index] = '\0';

    return STATUS_SUCCESS;
}

// �ڴ��������ʵ��
NTSTATUS HypervisorUtilsMemoryCopy(PVOID Destination, SIZE_T DestinationSize, const VOID* Source, SIZE_T SourceSize)
{
    if (!Destination || !Source || DestinationSize < SourceSize)
    {
        return STATUS_INVALID_PARAMETER;
    }

    RtlCopyMemory(Destination, Source, SourceSize);
    return STATUS_SUCCESS;
}

NTSTATUS HypervisorUtilsMemoryZero(PVOID Buffer, SIZE_T Size)
{
    if (!Buffer || Size == 0)
    {
        return STATUS_INVALID_PARAMETER;
    }

    RtlZeroMemory(Buffer, Size);
    return STATUS_SUCCESS;
}

BOOLEAN HypervisorUtilsMemoryCompare(const VOID* Buffer1, const VOID* Buffer2, SIZE_T Size)
{
    if (!Buffer1 || !Buffer2 || Size == 0)
    {
        return FALSE;
    }

    return (RtlCompareMemory(Buffer1, Buffer2, Size) == Size);
}

NTSTATUS HypervisorUtilsMemoryAllocate(SIZE_T Size, PVOID* Buffer)
{
    PVOID memory = NULL;

    if (!Buffer || Size == 0)
    {
        return STATUS_INVALID_PARAMETER;
    }

    memory = ExAllocatePoolWithTag(NonPagedPool, Size, 'UH');
    if (!memory)
    {
        *Buffer = NULL;
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(memory, Size);
    *Buffer = memory;
    return STATUS_SUCCESS;
}

VOID HypervisorUtilsMemoryFree(PVOID Buffer)
{
    if (Buffer)
    {
        ExFreePoolWithTag(Buffer, 'UH');
    }
}

// �����������ʵ��
VOID HypervisorUtilsDebugPrint(PCCHAR Format, ...)
{
    va_list args;
    CHAR buffer[512] = { 0 };

    if (!Format)
        return;

    va_start(args, Format);
    RtlStringCchVPrintfA(buffer, sizeof(buffer), Format, args);
    va_end(args);

    DbgPrint("%s", buffer);
}

VOID HypervisorUtilsDebugPrintEx(ULONG Level, PCCHAR Format, ...)
{
    UNREFERENCED_PARAMETER(Level);

    va_list args;
    CHAR buffer[512] = { 0 };

    if (!Format)
        return;

    va_start(args, Format);
    RtlStringCchVPrintfA(buffer, sizeof(buffer), Format, args);
    va_end(args);

    DbgPrint("%s", buffer);
}

// λ��������ʵ��
BOOLEAN HypervisorUtilsIsPowerOfTwo(ULONG Value)
{
    return (Value != 0) && ((Value & (Value - 1)) == 0);
}

ULONG HypervisorUtilsRoundUpToPowerOfTwo(ULONG Value)
{
    if (Value == 0)
        return 1;

    Value--;
    Value |= Value >> 1;
    Value |= Value >> 2;
    Value |= Value >> 4;
    Value |= Value >> 8;
    Value |= Value >> 16;
    Value++;

    return Value;
}

ULONG HypervisorUtilsCountSetBits(ULONG Value)
{
    ULONG count = 0;

    while (Value)
    {
        count += Value & 1;
        Value >>= 1;
    }

    return count;
}

ULONG HypervisorUtilsGetHighestSetBit(ULONG Value)
{
    if (Value == 0)
        return 0;

    ULONG bitPosition = 0;
    while (Value >>= 1)
    {
        bitPosition++;
    }

    return bitPosition;
}

ULONG HypervisorUtilsGetLowestSetBit(ULONG Value)
{
    if (Value == 0)
        return 0;

    return (Value & -Value);
}

// ʱ����غ���ʵ��
ULONG64 HypervisorUtilsGetTickCount(VOID)
{
    LARGE_INTEGER frequency, counter;

    frequency = KeQueryPerformanceCounter(&counter);
    return (counter.QuadPart * 1000) / frequency.QuadPart;
}

VOID HypervisorUtilsSleep(ULONG Milliseconds)
{
    LARGE_INTEGER interval;

    interval.QuadPart = -((LONGLONG)Milliseconds * 10000); // ת��Ϊ100���뵥λ
    KeDelayExecutionThread(KernelMode, FALSE, &interval);
}

BOOLEAN HypervisorUtilsIsTimeout(ULONGLONG StartTime, ULONG TimeoutMs)
{
    ULONGLONG currentTime = HypervisorUtilsGetTickCount();
    return (currentTime - StartTime) >= TimeoutMs;
}

// ��ѧ����ʵ��
ULONG HypervisorUtilsAlignUp(ULONG Value, ULONG Alignment)
{
    if (Alignment == 0)
        return Value;

    return (Value + Alignment - 1) & ~(Alignment - 1);
}

ULONG HypervisorUtilsAlignDown(ULONG Value, ULONG Alignment)
{
    if (Alignment == 0)
        return Value;

    return Value & ~(Alignment - 1);
}

BOOLEAN HypervisorUtilsIsAligned(ULONG Value, ULONG Alignment)
{
    if (Alignment == 0)
        return (Value == 0);

    return (Value & (Alignment - 1)) == 0;
}

// ������������ʵ��
NTSTATUS HypervisorUtilsBufferInitialize(PHYPERVISOR_BUFFER Buffer, SIZE_T InitialSize)
{
    if (!Buffer)
    {
        return STATUS_INVALID_PARAMETER;
    }

    RtlZeroMemory(Buffer, sizeof(HYPERVISOR_BUFFER));

    if (InitialSize > 0)
    {
        Buffer->Buffer = ExAllocatePoolWithTag(NonPagedPool, InitialSize, 'fB');
        if (!Buffer->Buffer)
        {
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        Buffer->Size = InitialSize;
        Buffer->Used = 0;
    }

    return STATUS_SUCCESS;
}

VOID HypervisorUtilsBufferCleanup(PHYPERVISOR_BUFFER Buffer)
{
    if (!Buffer)
        return;

    if (Buffer->Buffer)
    {
        ExFreePoolWithTag(Buffer->Buffer, 'fB');
        Buffer->Buffer = NULL;
    }

    Buffer->Size = 0;
    Buffer->Used = 0;
}

NTSTATUS HypervisorUtilsBufferResize(PHYPERVISOR_BUFFER Buffer, SIZE_T NewSize)
{
    PVOID newBuffer = NULL;

    if (!Buffer)
    {
        return STATUS_INVALID_PARAMETER;
    }

    if (NewSize == 0)
    {
        HypervisorUtilsBufferCleanup(Buffer);
        return STATUS_SUCCESS;
    }

    newBuffer = ExAllocatePoolWithTag(NonPagedPool, NewSize, 'fB');
    if (!newBuffer)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    if (Buffer->Buffer && Buffer->Used > 0)
    {
        SIZE_T copySize = Buffer->Used < NewSize ? Buffer->Used : NewSize;
        RtlCopyMemory(newBuffer, Buffer->Buffer, copySize);
    }

    if (Buffer->Buffer)
    {
        ExFreePoolWithTag(Buffer->Buffer, 'fB');
    }

    Buffer->Buffer = newBuffer;
    Buffer->Size = NewSize;

    if (Buffer->Used > NewSize)
    {
        Buffer->Used = NewSize;
    }

    return STATUS_SUCCESS;
}

NTSTATUS HypervisorUtilsBufferAppend(PHYPERVISOR_BUFFER Buffer, const VOID* Data, SIZE_T DataSize)
{
    NTSTATUS status = STATUS_SUCCESS;
    SIZE_T requiredSize = 0;

    if (!Buffer || !Data || DataSize == 0)
    {
        return STATUS_INVALID_PARAMETER;
    }

    requiredSize = Buffer->Used + DataSize;
    if (requiredSize > Buffer->Size)
    {
        SIZE_T newSize = HypervisorUtilsRoundUpToPowerOfTwo(requiredSize);
        status = HypervisorUtilsBufferResize(Buffer, newSize);
        if (!NT_SUCCESS(status))
        {
            return status;
        }
    }

    RtlCopyMemory((PCHAR)Buffer->Buffer + Buffer->Used, Data, DataSize);
    Buffer->Used += DataSize;

    return STATUS_SUCCESS;
}

NTSTATUS HypervisorUtilsBufferAppendString(PHYPERVISOR_BUFFER Buffer, PCCHAR String)
{
    if (!String)
    {
        return STATUS_INVALID_PARAMETER;
    }

    SIZE_T length = strlen(String);
    return HypervisorUtilsBufferAppend(Buffer, String, length);
}

// CRCУ�麯��ʵ��
ULONG HypervisorUtilsCalculateCrc32(const VOID* Data, SIZE_T Size)
{
    static BOOLEAN tableInitialized = FALSE;
    const UCHAR* bytes = (const UCHAR*)Data;
    ULONG crc = 0xFFFFFFFF;

    // ��ʼ��CRC32��ֻ��ʼ��һ�Σ�
    if (!tableInitialized)
    {
        HypervisorUtilsInitializeCrc32Table();
        tableInitialized = TRUE;
    }

    for (SIZE_T i = 0; i < Size; i++)
    {
        UCHAR index = (crc ^ bytes[i]) & 0xFF;
        crc = (crc >> 8) ^ g_Crc32Table[index];
    }

    return crc ^ 0xFFFFFFFF;
}

USHORT HypervisorUtilsCalculateCrc16(const VOID* Data, SIZE_T Size)
{
    const UCHAR* bytes = (const UCHAR*)Data;
    USHORT crc = 0xFFFF;

    for (SIZE_T i = 0; i < Size; i++)
    {
        crc ^= bytes[i];

        for (int j = 0; j < 8; j++)
        {
            if (crc & 0x0001)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }

    return crc;
}

// GUID��غ���ʵ��
BOOLEAN HypervisorUtilsIsEqualGuid(const HYPERVISOR_GUID* Guid1, const HYPERVISOR_GUID* Guid2)
{
    if (!Guid1 || !Guid2)
        return FALSE;

    return (Guid1->Data1 == Guid2->Data1 &&
        Guid1->Data2 == Guid2->Data2 &&
        Guid1->Data3 == Guid2->Data3 &&
        RtlCompareMemory(Guid1->Data4, Guid2->Data4, 8) == 8);
}

VOID HypervisorUtilsGenerateRandomGuid(HYPERVISOR_GUID* Guid)
{
    LARGE_INTEGER time;

    if (!Guid)
        return;

    // ��ȡ��ǰʱ����Ϊ�������
    time = KeQueryPerformanceCounter(NULL);

    // �������GUID
    Guid->Data1 = (ULONG)time.QuadPart;
    Guid->Data2 = (USHORT)(time.QuadPart >> 32);
    Guid->Data3 = (USHORT)(time.QuadPart >> 48);

    // ���Data4����
    for (int i = 0; i < 8; i++)
    {
        Guid->Data4[i] = (UCHAR)((time.QuadPart >> (i * 8)) & 0xFF);
    }
}

// �б������ʵ��
VOID HypervisorUtilsInitializeListHead(PHYPERVISOR_LIST_ENTRY ListHead)
{
    if (ListHead)
    {
        ListHead->Flink = ListHead->Blink = ListHead;
    }
}

BOOLEAN HypervisorUtilsIsListEmpty(const HYPERVISOR_LIST_ENTRY* ListHead)
{
    if (!ListHead)
        return TRUE;

    return (ListHead->Flink == ListHead);
}

BOOLEAN HypervisorUtilsRemoveEntryList(PHYPERVISOR_LIST_ENTRY Entry)
{
    PHYPERVISOR_LIST_ENTRY Blink;
    PHYPERVISOR_LIST_ENTRY Flink;

    if (!Entry)
        return FALSE;

    Flink = Entry->Flink;
    Blink = Entry->Blink;
    Blink->Flink = Flink;
    Flink->Blink = Blink;

    return (Flink == Blink);
}

PHYPERVISOR_LIST_ENTRY HypervisorUtilsRemoveHeadList(PHYPERVISOR_LIST_ENTRY ListHead)
{
    PHYPERVISOR_LIST_ENTRY Flink;
    PHYPERVISOR_LIST_ENTRY Entry;

    if (!ListHead)
        return NULL;

    Entry = ListHead->Flink;
    Flink = Entry->Flink;
    ListHead->Flink = Flink;
    Flink->Blink = ListHead;

    return Entry;
}

PHYPERVISOR_LIST_ENTRY HypervisorUtilsRemoveTailList(PHYPERVISOR_LIST_ENTRY ListHead)
{
    PHYPERVISOR_LIST_ENTRY Blink;
    PHYPERVISOR_LIST_ENTRY Entry;

    if (!ListHead)
        return NULL;

    Entry = ListHead->Blink;
    Blink = Entry->Blink;
    ListHead->Blink = Blink;
    Blink->Flink = ListHead;

    return Entry;
}

VOID HypervisorUtilsInsertTailList(PHYPERVISOR_LIST_ENTRY ListHead, PHYPERVISOR_LIST_ENTRY Entry)
{
    PHYPERVISOR_LIST_ENTRY Blink;

    if (!ListHead || !Entry)
        return;

    Blink = ListHead->Blink;
    Entry->Flink = ListHead;
    Entry->Blink = Blink;
    Blink->Flink = Entry;
    ListHead->Blink = Entry;
}

VOID HypervisorUtilsInsertHeadList(PHYPERVISOR_LIST_ENTRY ListHead, PHYPERVISOR_LIST_ENTRY Entry)
{
    PHYPERVISOR_LIST_ENTRY Flink;

    if (!ListHead || !Entry)
        return;

    Flink = ListHead->Flink;
    Entry->Flink = Flink;
    Entry->Blink = ListHead;
    Flink->Blink = Entry;
    ListHead->Flink = Entry;
}