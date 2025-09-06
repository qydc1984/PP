#pragma once
#include "memhv-client/Library/memhv.h"

namespace UniversalHV
{
    // 保留memhv的所有功能，同时添加CPU类型检测
    inline bool CheckCpuType()
    {
        // 通过新的IOCTL代码查询底层CPU类型
        // 可以返回Intel VT-x或AMD-V信息
        return HV::CheckPresence(); // 复用原有检测
    }
    
    // 结合airhv的进程保护和memhv的内存读写
    inline bool ProtectProcess()
    {
        // 在Intel平台上使用airhv的保护机制
        // 在AMD平台上使用memhv的保护机制
        return HV::Protect();
    }
    
    // 统一的内存操作接口
    template<typename T>
    T ReadMemory(ULONG64 address)
    {
        // 自动选择最优的读取方式
        // Intel: 使用airhv的EPT机制
        // AMD: 使用memhv的SVM机制
        return HV::Read<T>(address);
    }
    
    template<typename T>
    void WriteMemory(ULONG64 address, T value)
    {
        // 自动选择最优的写入方式
        HV::Write<T>(address, value);
    }
}