#pragma once
#include "hypervisor_base.h"

#ifdef __cplusplus
extern "C" {
#endif

    // 字符串处理函数
    NTSTATUS HypervisorUtilsStringCopy(PCHAR Destination, SIZE_T DestinationSize, PCCHAR Source);
    NTSTATUS HypervisorUtilsStringConcat(PCHAR Destination, SIZE_T DestinationSize, PCCHAR Source);
    SIZE_T HypervisorUtilsStringLength(PCCHAR String);
    BOOLEAN HypervisorUtilsStringCompare(PCCHAR String1, PCCHAR String2);
    BOOLEAN HypervisorUtilsStringCompareNoCase(PCCHAR String1, PCCHAR String2);
    NTSTATUS HypervisorUtilsStringToInteger(PCCHAR String, ULONG Base, PULONG Result);
    NTSTATUS HypervisorUtilsIntegerToString(ULONG Value, ULONG Base, PCHAR Buffer, SIZE_T BufferSize);

    // 内存操作函数
    NTSTATUS HypervisorUtilsMemoryCopy(PVOID Destination, SIZE_T DestinationSize, const VOID* Source, SIZE_T SourceSize);
    NTSTATUS HypervisorUtilsMemoryZero(PVOID Buffer, SIZE_T Size);
    BOOLEAN HypervisorUtilsMemoryCompare(const VOID* Buffer1, const VOID* Buffer2, SIZE_T Size);
    NTSTATUS HypervisorUtilsMemoryAllocate(SIZE_T Size, PVOID* Buffer);
    VOID HypervisorUtilsMemoryFree(PVOID Buffer);

    // 调试输出函数
    VOID HypervisorUtilsDebugPrint(PCCHAR Format, ...);
    VOID HypervisorUtilsDebugPrintEx(ULONG Level, PCCHAR Format, ...);

    // 位操作函数
    BOOLEAN HypervisorUtilsIsPowerOfTwo(ULONG Value);
    ULONG HypervisorUtilsRoundUpToPowerOfTwo(ULONG Value);
    ULONG HypervisorUtilsCountSetBits(ULONG Value);
    ULONG HypervisorUtilsGetHighestSetBit(ULONG Value);
    ULONG HypervisorUtilsGetLowestSetBit(ULONG Value);

    // 时间相关函数
    ULONG64 HypervisorUtilsGetTickCount(VOID);
    VOID HypervisorUtilsSleep(ULONG Milliseconds);
    BOOLEAN HypervisorUtilsIsTimeout(ULONGLONG StartTime, ULONG TimeoutMs);

    // 数学函数
    ULONG HypervisorUtilsAlignUp(ULONG Value, ULONG Alignment);
    ULONG HypervisorUtilsAlignDown(ULONG Value, ULONG Alignment);
    BOOLEAN HypervisorUtilsIsAligned(ULONG Value, ULONG Alignment);

    // 缓冲区管理函数
    typedef struct _HYPERVISOR_BUFFER
    {
        PVOID Buffer;
        SIZE_T Size;
        SIZE_T Used;
    } HYPERVISOR_BUFFER, * PHYPERVISOR_BUFFER;

    NTSTATUS HypervisorUtilsBufferInitialize(PHYPERVISOR_BUFFER Buffer, SIZE_T InitialSize);
    VOID HypervisorUtilsBufferCleanup(PHYPERVISOR_BUFFER Buffer);
    NTSTATUS HypervisorUtilsBufferResize(PHYPERVISOR_BUFFER Buffer, SIZE_T NewSize);
    NTSTATUS HypervisorUtilsBufferAppend(PHYPERVISOR_BUFFER Buffer, const VOID* Data, SIZE_T DataSize);
    NTSTATUS HypervisorUtilsBufferAppendString(PHYPERVISOR_BUFFER Buffer, PCCHAR String);

    // CRC校验函数
    ULONG HypervisorUtilsCalculateCrc32(const VOID* Data, SIZE_T Size);
    USHORT HypervisorUtilsCalculateCrc16(const VOID* Data, SIZE_T Size);

    // GUID相关函数
    typedef struct _HYPERVISOR_GUID
    {
        ULONG Data1;
        USHORT Data2;
        USHORT Data3;
        UCHAR Data4[8];
    } HYPERVISOR_GUID, * PHYPERVISOR_GUID;

    BOOLEAN HypervisorUtilsIsEqualGuid(const HYPERVISOR_GUID* Guid1, const HYPERVISOR_GUID* Guid2);
    VOID HypervisorUtilsGenerateRandomGuid(HYPERVISOR_GUID* Guid);

    // 列表管理函数
    typedef struct _HYPERVISOR_LIST_ENTRY
    {
        struct _HYPERVISOR_LIST_ENTRY* Flink;
        struct _HYPERVISOR_LIST_ENTRY* Blink;
    } HYPERVISOR_LIST_ENTRY, * PHYPERVISOR_LIST_ENTRY;

    VOID HypervisorUtilsInitializeListHead(PHYPERVISOR_LIST_ENTRY ListHead);
    BOOLEAN HypervisorUtilsIsListEmpty(const HYPERVISOR_LIST_ENTRY* ListHead);
    BOOLEAN HypervisorUtilsRemoveEntryList(PHYPERVISOR_LIST_ENTRY Entry);
    PHYPERVISOR_LIST_ENTRY HypervisorUtilsRemoveHeadList(PHYPERVISOR_LIST_ENTRY ListHead);
    PHYPERVISOR_LIST_ENTRY HypervisorUtilsRemoveTailList(PHYPERVISOR_LIST_ENTRY ListHead);
    VOID HypervisorUtilsInsertTailList(PHYPERVISOR_LIST_ENTRY ListHead, PHYPERVISOR_LIST_ENTRY Entry);
    VOID HypervisorUtilsInsertHeadList(PHYPERVISOR_LIST_ENTRY ListHead, PHYPERVISOR_LIST_ENTRY Entry);

#ifdef __cplusplus
}
#endif