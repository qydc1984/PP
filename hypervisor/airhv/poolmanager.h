#pragma once
#include <ntddk.h>
#include "spinlock.h"  // �����һ��������spinlock��Ķ���

// ǰ������ pool_manager �еĽṹ��
namespace pool_manager {
    struct __pool_manager;
}

// ���� VMM_CONTEXT �ṹ��
struct VMM_CONTEXT
{
    pool_manager::__pool_manager* pool_manager;
    unsigned int processor_count; // ���processor_count�ֶ�
    // ����������������Ա����
    // ...
};

namespace pool_manager
{
    enum allocation_intention
    {
        INTENTION_NONE,
        INTENTION_TRACK_HOOKED_PAGES,
        INTENTION_EXEC_TRAMPOLINE,
        INTENTION_SPLIT_PML2,
        INTENTION_TRACK_HOOKED_FUNCTIONS,
        INTENTION_FAKE_PAGE_CONTENTS,
    };

    struct __request_new_allocation
    {
        unsigned __int64 size[10];
        unsigned __int32 count[10];
        allocation_intention intention[10];
    };

    struct __pool_manager
    {
        __request_new_allocation* allocation_requests;
        PLIST_ENTRY list_of_allocated_pools;
        volatile long lock_for_request_allocation;  // �Ļ�volatile long������ƥ��spinlock��
        volatile long lock_for_reading_pool;        // �Ļ�volatile long������ƥ��spinlock��
        bool is_request_for_allocation_recived;
    };

    struct __pool_table
    {
        void* address;
        unsigned __int64  size;
        allocation_intention intention;
        LIST_ENTRY pool_list;
        bool is_busy;
        bool recycled;
    };

    /// <summary>
    /// Writes all information about allocated pools
    /// </summary>
    void dump_pools_info();

    /// <summary>
    /// Request allocation
    /// </summary>
    /// <param name="size">Size of pool</param>
    /// <param name="count">Number of pools to allocate</param>
    /// <param name="intention"></param>
    /// <returns></returns>
    bool request_allocation(unsigned __int64 size, unsigned __int32 count, allocation_intention intention);

    /// <summary>
    /// Initalize pool manager struct and preallocate pools
    /// </summary>
    /// <returns> status </returns>
    bool initialize();

    /// <summary>
    /// Free all allocted pools
    /// </summary>
    void uninitialize();

    /// <summary>
    /// Set information that pool is no longer used by anyone and mark as recycled
    /// </summary>
    /// <param name="address"></param>
    void release_pool(void* address);

    /// <summary>
    /// Allocate all requested pools
    /// </summary>
    /// <returns></returns>
    bool perform_allocation();

    /// <summary>
    /// Returns pre allocated pool and request new one for allocation
    /// </summary>
    /// <param name="vmm_context">VMM������ָ��</param>
    /// <param name="intention">Indicates what will be pool used for</param>
    /// <param name="new_pool">If set new pool will (with same properties) be requested to allocate</param>
    /// <param name="size">Only if new_pool is true. Size of new pool</param>
    /// <returns></returns>
    template <typename T>
    T request_pool(VMM_CONTEXT* vmm_context, allocation_intention intention, bool new_pool, unsigned __int64 size)
    {
        PLIST_ENTRY current = nullptr;
        void* address = nullptr;
        bool is_recycled = false;
        __pool_table* pool_table = nullptr;

        // ��鴫��������ĺͳع������Ƿ����
        if (vmm_context && vmm_context->pool_manager) {
            // ʹ���Զ���spinlock��
            spinlock pool_lock(&vmm_context->pool_manager->lock_for_reading_pool);

            // ��ȫ�ػ�ȡ����ͷ
            current = vmm_context->pool_manager->list_of_allocated_pools;

            // ��������Ƿ���Ч�Ҳ�Ϊ��
            if (current && current->Flink) {
                PLIST_ENTRY list_head = current;
                current = current->Flink;

                // ��ȷ�����������ʽ
                while (current != list_head) {
                    // ��ȡ�ر���
                    pool_table = CONTAINING_RECORD(current, __pool_table, pool_list);

                    if (pool_table->intention == intention && pool_table->is_busy == false) {
                        pool_table->is_busy = true;
                        is_recycled = pool_table->recycled;
                        address = pool_table->address;
                        break;
                    }

                    // �ƶ�����һ���ڵ�
                    current = current->Flink;

                    // ��ֹ����ѭ���İ�ȫ���
                    if (!current) {
                        break;
                    }
                }
            }
        }

        //
        // If pool which we got is recycled then we don't allocate
        // a new one because we don't want to overload memory, If there wasn't any preallocated pool
        // this function will send a request
        //
        if (new_pool == true && is_recycled == false)
            request_allocation(size, 1, intention);

        return (T)address;
    }
}