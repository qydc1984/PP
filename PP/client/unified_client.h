#pragma once
#include "memhv-client/Library/memhv.h"

namespace UniversalHV
{
    // ����memhv�����й��ܣ�ͬʱ���CPU���ͼ��
    inline bool CheckCpuType()
    {
        // ͨ���µ�IOCTL�����ѯ�ײ�CPU����
        // ���Է���Intel VT-x��AMD-V��Ϣ
        return HV::CheckPresence(); // ����ԭ�м��
    }
    
    // ���airhv�Ľ��̱�����memhv���ڴ��д
    inline bool ProtectProcess()
    {
        // ��Intelƽ̨��ʹ��airhv�ı�������
        // ��AMDƽ̨��ʹ��memhv�ı�������
        return HV::Protect();
    }
    
    // ͳһ���ڴ�����ӿ�
    template<typename T>
    T ReadMemory(ULONG64 address)
    {
        // �Զ�ѡ�����ŵĶ�ȡ��ʽ
        // Intel: ʹ��airhv��EPT����
        // AMD: ʹ��memhv��SVM����
        return HV::Read<T>(address);
    }
    
    template<typename T>
    void WriteMemory(ULONG64 address, T value)
    {
        // �Զ�ѡ�����ŵ�д�뷽ʽ
        HV::Write<T>(address, value);
    }
}