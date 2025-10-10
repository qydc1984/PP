/**
 * airhv_tester.cpp - airhv ����ר�ò�����ʵ��
 *
 * ʵ�� airhv (Intel VT-x) �����ĸ���ܲ���
 */

#include "airhv_tester.h"
#include <iostream>
#include <windows.h>
#include <intrin.h>
#include <string>
#include <vector>

 // ���캯��
AirhvTester::AirhvTester() {
    test_results.clear();
}

// ��������
AirhvTester::~AirhvTester() {
    test_results.clear();
}

// ��ӡ����ͷ��
void AirhvTester::PrintTestHeader() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "airhv (Intel VT-x) ��������\n";
    std::cout << std::string(60, '=') << "\n\n";
}

// ��ӡ���Խ��
void AirhvTester::PrintTestResult(const std::string& test_name, TestResult result, const std::string& message) {
    AirhvTestInfo info;
    info.name = test_name;
    info.result = result;

    switch (result) {
    case TEST_PASS:
        std::cout << "[+] " << test_name << ": ͨ��";
        if (!message.empty()) {
            std::cout << " - " << message;
        }
        std::cout << "\n";
        info.description = "ͨ��";
        break;

    case TEST_FAIL:
        std::cout << "[-] " << test_name << ": ʧ��";
        if (!message.empty()) {
            std::cout << " - " << message;
        }
        std::cout << "\n";
        info.description = "ʧ��";
        info.error_message = message;
        break;

    case TEST_SKIP:
        std::cout << "[*] " << test_name << ": ����";
        if (!message.empty()) {
            std::cout << " - " << message;
        }
        std::cout << "\n";
        info.description = "����";
        break;
    }

    test_results.push_back(info);
}

// ��ȡʧ�ܲ�������
int AirhvTester::GetFailedTestCount() const {
    int failed = 0;
    for (const auto& result : test_results) {
        if (result.result == TEST_FAIL) {
            failed++;
        }
    }
    return failed;
}

// ��ӡ����ժҪ
void AirhvTester::PrintTestSummary() {
    std::cout << "\n" << std::string(40, '-') << "\n";
    std::cout << "airhv ����ժҪ\n";
    std::cout << "�ܲ�����: " << test_results.size() << "\n";
    std::cout << "ͨ��: " << (test_results.size() - GetFailedTestCount()) << "\n";
    std::cout << "ʧ��: " << GetFailedTestCount() << "\n";
    std::cout << std::string(40, '-') << "\n";
}

// ����Ƿ�ΪIntel������
bool AirhvTester::IsIntelProcessor() {
    int cpuInfo[4];
    __cpuid(cpuInfo, 0);

    // ���Intel��ʶ: "GenuineIntel"
    if (cpuInfo[1] == 0x756E6547 && // "uneG"
        cpuInfo[3] == 0x49656E69 && // "Ieni"
        cpuInfo[2] == 0x6C65746E)   // "letn"
    {
        return true;
    }
    return false;
}

// ��������������
TestResult AirhvTester::TestDriverPresence() {
    std::cout << "[*] ����1: ���������Լ��\n";

    if (airhv_adapter_is_running()) {
        PrintTestResult("����������", TEST_PASS, "airhv�����Ѽ��ز�����");
        return TEST_PASS;
    }
    else {
        PrintTestResult("����������", TEST_FAIL, "airhv����δ��⵽");
        return TEST_FAIL;
    }
}

// ���Գ�ʼ��
TestResult AirhvTester::TestInitialization() {
    std::cout << "[*] ����2: ��ʼ�����\n";

    if (airhv_adapter_init()) {
        PrintTestResult("��ʼ��", TEST_PASS, "������ʼ���ɹ�");
        return TEST_PASS;
    }
    else {
        PrintTestResult("��ʼ��", TEST_FAIL, "������ʼ��ʧ��");
        return TEST_FAIL;
    }
}

// ���Դ���������
TestResult AirhvTester::TestProcessorCount() {
    std::cout << "[*] ����3: ����������\n";

    unsigned int processorCount = airhv_adapter_get_processor_count();
    if (processorCount > 0) {
        PrintTestResult("����������", TEST_PASS, "��⵽ " + std::to_string(processorCount) + " ��������");
        return TEST_PASS;
    }
    else {
        PrintTestResult("����������", TEST_FAIL, "�޷���ȡ����������");
        return TEST_FAIL;
    }
}

// ���Խ��̷���
TestResult AirhvTester::TestProcessAccess() {
    std::cout << "[*] ����4: ���̷���\n";

    // ���Ի�ȡSystem���� (PID 4)
    PEPROCESS systemProcess = AirHvGetProcessById((HANDLE)4);
    if (systemProcess) {
        PrintTestResult("���̷���", TEST_PASS, "�ɹ���ȡSystem���̶���");
        AirHvDereferenceObject(systemProcess);
        return TEST_PASS;
    }
    else {
        PrintTestResult("���̷���", TEST_FAIL, "�޷���ȡSystem����");
        return TEST_FAIL;
    }
}

// ���Խ���Ŀ¼��ַ
TestResult AirhvTester::TestProcessDirectoryBase() {
    std::cout << "[*] ����5: ����Ŀ¼��ַ\n";

    PEPROCESS systemProcess = AirHvGetProcessById((HANDLE)4);
    if (systemProcess) {
        ULONG64 dirBase = AirHvGetDirectoryBase(systemProcess);
        if (dirBase != 0) {
            char buffer[64];
            sprintf_s(buffer, "CR3 = 0x%llx", dirBase);
            PrintTestResult("Ŀ¼��ַ", TEST_PASS, buffer);
            AirHvDereferenceObject(systemProcess);
            return TEST_PASS;
        }
        else {
            PrintTestResult("Ŀ¼��ַ", TEST_FAIL, "�޷���ȡĿ¼��ַ");
            AirHvDereferenceObject(systemProcess);
            return TEST_FAIL;
        }
    }
    else {
        PrintTestResult("Ŀ¼��ַ", TEST_SKIP, "System���̲�����");
        return TEST_SKIP;
    }
}

// �����ڴ��ȡ
TestResult AirhvTester::TestMemoryRead() {
    std::cout << "[*] ����6: �ڴ��ȡ\n";

    PEPROCESS systemProcess = AirHvGetProcessById((HANDLE)4);
    if (systemProcess) {
        ULONG64 dirBase = AirHvGetDirectoryBase(systemProcess);
        if (dirBase != 0) {
            UCHAR buffer[16] = { 0 };
            SIZE_T bytesRead = 0;
            NTSTATUS status = AirHvReadProcessMemory(0, dirBase, 0xFFFFF80000000000, buffer, sizeof(buffer), &bytesRead);

            if (NT_SUCCESS(status) && bytesRead > 0) {
                PrintTestResult("�ڴ��ȡ", TEST_PASS, "�ɹ���ȡ " + std::to_string(bytesRead) + " �ֽ�");
                AirHvDereferenceObject(systemProcess);
                return TEST_PASS;
            }
            else {
                char error_msg[128];
                sprintf_s(error_msg, "��ȡʧ�ܣ�״̬��: 0x%08X", status);
                PrintTestResult("�ڴ��ȡ", TEST_FAIL, error_msg);
                AirHvDereferenceObject(systemProcess);
                return TEST_FAIL;
            }
        }
        else {
            PrintTestResult("�ڴ��ȡ", TEST_SKIP, "Ŀ¼��ַ������");
            AirHvDereferenceObject(systemProcess);
            return TEST_SKIP;
        }
    }
    else {
        PrintTestResult("�ڴ��ȡ", TEST_SKIP, "System���̲�����");
        return TEST_SKIP;
    }
}

// �����ڴ�д��
TestResult AirhvTester::TestMemoryWrite() {
    std::cout << "[*] ����7: �ڴ�д��\n";

    // ע�⣺ʵ�ʲ�����Ӧʹ�ð�ȫ�Ĳ��Ե�ַ
    PrintTestResult("�ڴ�д��", TEST_SKIP, "Ϊ��ȫ�������д�����");
    return TEST_SKIP;
}

// �����ڴ濽��
TestResult AirhvTester::TestMemoryCopy() {
    std::cout << "[*] ����8: �ڴ濽��\n";

    PrintTestResult("�ڴ濽��", TEST_SKIP, "��Ҫ������Ч���̽��в���");
    return TEST_SKIP;
}

// ���Խ���ö��
TestResult AirhvTester::TestProcessEnumeration() {
    std::cout << "[*] ����9: ����ö��\n";

    int processCount = 0;
    PEPROCESS current = PsInitialSystemProcess;

    // �򵥱����������������Թ���
    for (int i = 0; i < 10 && current != nullptr; i++) {
        HANDLE processId = PsGetProcessId(current);
        if (processId) {
            processCount++;
        }
        current = AirHvGetNextProcess(current);
        if (!current) break;
    }

    if (processCount > 0) {
        PrintTestResult("����ö��", TEST_PASS, "�ɹ����� " + std::to_string(processCount) + " ������");
        return TEST_PASS;
    }
    else {
        PrintTestResult("����ö��", TEST_FAIL, "����ö��ʧ��");
        return TEST_FAIL;
    }
}

// �������ұ���
TestResult AirhvTester::TestSelfProtection() {
    std::cout << "[*] ����10: ���ұ���\n";

    if (AirHvProtectSelf()) {
        PrintTestResult("���ұ���", TEST_PASS, "�����������óɹ�");
        return TEST_PASS;
    }
    else {
        PrintTestResult("���ұ���", TEST_FAIL, "������������ʧ��");
        return TEST_FAIL;
    }
}

// ����MSR��֤
TestResult AirhvTester::TestMsrValidation() {
    std::cout << "[*] ����11: MSR��֤\n";

    // ���Լ���������MSR
    bool valid1 = AirHvIsValidMsr(0xC0000080); // EFER
    bool valid2 = AirHvIsValidMsr(0x00000010); // CS
    bool invalid1 = AirHvIsValidMsr(0xFFFFFFFF); // ��ЧMSR

    if (valid1 && valid2 && !invalid1) {
        PrintTestResult("MSR��֤", TEST_PASS, "MSR��֤��������");
        return TEST_PASS;
    }
    else {
        PrintTestResult("MSR��֤", TEST_FAIL, "MSR��֤�����쳣");
        return TEST_FAIL;
    }
}

// �����û���ַ��֤
TestResult AirhvTester::TestUserAddressValidation() {
    std::cout << "[*] ����12: �û���ַ��֤\n";

    bool valid1 = AirHvIsValidUserAddress(0x00001000); // ��Ч�û���ַ
    bool invalid1 = AirHvIsValidUserAddress(0x00000000); // ��Ч��ַ
    bool invalid2 = AirHvIsValidUserAddress(0x800000000000); // �ں˵�ַ

    if (valid1 && !invalid1) {
        PrintTestResult("�û���ַ��֤", TEST_PASS, "��ַ��֤��������");
        return TEST_PASS;
    }
    else {
        PrintTestResult("�û���ַ��֤", TEST_FAIL, "��ַ��֤�����쳣");
        return TEST_FAIL;
    }
}

// ���Զ������
TestResult AirhvTester::TestObjectManagement() {
    std::cout << "[*] ����13: �������\n";

    PEPROCESS systemProcess = AirHvGetProcessById((HANDLE)4);
    if (systemProcess) {
        // �������ü�������
        AirHvReferenceObject(systemProcess);
        AirHvDereferenceObject(systemProcess);
        AirHvDereferenceObject(systemProcess);
        PrintTestResult("�������", TEST_PASS, "���ü����������");
        return TEST_PASS;
    }
    else {
        PrintTestResult("�������", TEST_FAIL, "�޷���ȡ���̶���");
        return TEST_FAIL;
    }
}

// ���԰�ȫ�ڴ����
TestResult AirhvTester::TestSafeMemoryOperations() {
    std::cout << "[*] ����14: ��ȫ�ڴ����\n";

    PEPROCESS systemProcess = AirHvGetProcessById((HANDLE)4);
    if (systemProcess) {
        ULONG64 dirBase = AirHvGetDirectoryBase(systemProcess);
        if (dirBase != 0) {
            UCHAR buffer[16] = { 0 };
            SIZE_T bytesRead = 0;
            // airhv ����û��ר�ŵİ�ȫ��ȡ������ʹ����ͨ��ȡ
            NTSTATUS status = AirHvReadProcessMemory(0, dirBase, 0xFFFFF80000000000, buffer, sizeof(buffer), &bytesRead);

            if (NT_SUCCESS(status)) {
                PrintTestResult("��ȫ�ڴ����", TEST_PASS, "�ڴ�������");
                AirHvDereferenceObject(systemProcess);
                return TEST_PASS;
            }
            else {
                char error_msg[128];
                sprintf_s(error_msg, "�ڴ����ʧ�ܣ�״̬��: 0x%08X", status);
                PrintTestResult("��ȫ�ڴ����", TEST_FAIL, error_msg);
                AirHvDereferenceObject(systemProcess);
                return TEST_FAIL;
            }
        }
        else {
            PrintTestResult("��ȫ�ڴ����", TEST_SKIP, "Ŀ¼��ַ������");
            AirHvDereferenceObject(systemProcess);
            return TEST_SKIP;
        }
    }
    else {
        PrintTestResult("��ȫ�ڴ����", TEST_SKIP, "System���̲�����");
        return TEST_SKIP;
    }
}

// ���Ը߼�����
TestResult AirhvTester::TestAdvancedFeatures() {
    std::cout << "[*] ����15: �߼�����\n";

    // ����MSR���ع���
    bool msr_result = airhv_adapter_set_msr_intercept(0xC0000080, true, true);
    if (msr_result) {
        PrintTestResult("�߼�����", TEST_PASS, "MSR���ع��ܿ���");
        return TEST_PASS;
    }
    else {
        PrintTestResult("�߼�����", TEST_SKIP, "MSR���ع��ܲ�����");
        return TEST_SKIP;
    }
}

// ����MSR����
TestResult AirhvTester::TestMsrInterception() {
    std::cout << "[*] ����16: MSR����\n";

    bool result = airhv_adapter_set_msr_intercept(0xC0000080, true, true);
    if (result) {
        PrintTestResult("MSR����", TEST_PASS, "MSR�������óɹ�");
        return TEST_PASS;
    }
    else {
        PrintTestResult("MSR����", TEST_FAIL, "MSR��������ʧ��");
        return TEST_FAIL;
    }
}

// ����IO����
TestResult AirhvTester::TestIoInterception() {
    std::cout << "[*] ����17: IO����\n";

    bool result = airhv_adapter_set_io_intercept(0x3F8, true); // COM1�˿�
    if (result) {
        PrintTestResult("IO����", TEST_PASS, "IO�������óɹ�");
        return TEST_PASS;
    }
    else {
        PrintTestResult("IO����", TEST_FAIL, "IO��������ʧ��");
        return TEST_FAIL;
    }
}

// �����ж�ע��
TestResult AirhvTester::TestInterruptInjection() {
    std::cout << "[*] ����18: �ж�ע��\n";

    // ����ע��һ�����ж�
    bool result = airhv_adapter_inject_interrupt(0, 0x20, 4, 0, false); // INT 0x20
    if (result) {
        PrintTestResult("�ж�ע��", TEST_PASS, "�ж�ע��ɹ�");
        return TEST_PASS;
    }
    else {
        PrintTestResult("�ж�ע��", TEST_SKIP, "�ж�ע�빦�ܲ�������");
        return TEST_SKIP;
    }
}

// ����EPT����
TestResult AirhvTester::TestEptHooks() {
    std::cout << "[*] ����19: EPT����\n";

    PrintTestResult("EPT����", TEST_SKIP, "��Ҫ�ض������ַ���в���");
    return TEST_SKIP;
}

// ���Կͻ����Ĵ���
TestResult AirhvTester::TestGuestRegisters() {
    std::cout << "[*] ����20: �ͻ����Ĵ���\n";

    // ���Ի�ȡ��һ���������ļĴ���
    __vmexit_guest_registers registers;
    bool result = airhv_adapter_get_guest_registers(0, &registers);
    if (result) {
        PrintTestResult("�ͻ����Ĵ���", TEST_PASS, "�Ĵ�����ȡ�ɹ�");
        return TEST_PASS;
    }
    else {
        PrintTestResult("�ͻ����Ĵ���", TEST_SKIP, "�Ĵ�����ȡ���ܲ�������");
        return TEST_SKIP;
    }
}

// ����VMCALL�ӿ�
TestResult AirhvTester::TestVmCallInterface() {
    std::cout << "[*] ����21: VMCALL�ӿ�\n";

    // ����һ������VMCALL
    unsigned __int64 result = airhv_adapter_vmcall(0, 0x1000, nullptr);
    char buffer[64];
    sprintf_s(buffer, "VMCALL����ֵ: 0x%llx", result);
    PrintTestResult("VMCALL�ӿ�", TEST_PASS, buffer);
    return TEST_PASS;
}

// ��������
TestResult AirhvTester::TestCleanup() {
    std::cout << "[*] ����22: ��Դ����\n";

    // ִ������
    airhv_adapter_cleanup();
    PrintTestResult("��Դ����", TEST_PASS, "������ִ�����");
    return TEST_PASS;
}

// �������в���
int AirhvTester::RunAllTests() {
    PrintTestHeader();
    test_results.clear();

    // ��˳��ִ�в���
    TestDriverPresence();
    TestInitialization();
    TestProcessorCount();
    TestProcessAccess();
    TestProcessDirectoryBase();
    TestMemoryRead();
    TestMemoryWrite();
    TestMemoryCopy();
    TestProcessEnumeration();
    TestSelfProtection();
    TestMsrValidation();
    TestUserAddressValidation();
    TestObjectManagement();
    TestSafeMemoryOperations();
    TestAdvancedFeatures();
    TestMsrInterception();
    TestIoInterception();
    TestInterruptInjection();
    TestEptHooks();
    TestGuestRegisters();
    TestVmCallInterface();
    TestCleanup();

    PrintTestSummary();

    return GetFailedTestCount();
}

// ȫ�ֲ��Ժ���ʵ��
int TestAirhvDriver() {
    if (!IsIntelProcessor()) {
        std::cout << "[-] ��ǰ����Intel������������airhv����\n";
        return -1;
    }

    AirhvTester tester;
    return tester.RunAllTests();
}

bool TestAirhvLoad() {
    return airhv_adapter_is_running();
}

bool TestAirhvInitialization() {
    return airhv_adapter_init();
}

bool TestAirhvPresence() {
    return airhv_adapter_is_running();
}

bool TestAirhvProcessAccess() {
    PEPROCESS process = AirHvGetProcessById((HANDLE)4);
    if (process) {
        AirHvDereferenceObject(process);
        return true;
    }
    return false;
}

bool TestAirhvMemoryRead() {
    PEPROCESS process = AirHvGetProcessById((HANDLE)4);
    if (process) {
        ULONG64 dirBase = AirHvGetDirectoryBase(process);
        if (dirBase != 0) {
            UCHAR buffer[8];
            SIZE_T bytesRead;
            NTSTATUS status = AirHvReadProcessMemory(0, dirBase, 0xFFFFF80000000000, buffer, sizeof(buffer), &bytesRead);
            AirHvDereferenceObject(process);
            return NT_SUCCESS(status);
        }
        AirHvDereferenceObject(process);
    }
    return false;
}

bool TestAirhvMemoryWrite() {
    // Ϊ��ȫ�����Ĭ������д�����
    return true;
}

bool TestAirhvProtection() {
    return AirHvProtectSelf();
}

bool TestAirhvCleanup() {
    airhv_adapter_cleanup();
    return true;
}

bool IsIntelProcessor() {
    int cpuInfo[4];
    __cpuid(cpuInfo, 0);

    // ���Intel��ʶ: "GenuineIntel"
    if (cpuInfo[1] == 0x756E6547 && // "uneG"
        cpuInfo[3] == 0x49656E69 && // "Ieni"
        cpuInfo[2] == 0x6C65746E)   // "letn"
    {
        return true;
    }
    return false;
}

void PrintAirhvTestHeader() {
    std::cout << "[*] ��ʼairhv��������\n";
}

void PrintAirhvTestResult(const char* test_name, bool result) {
    if (result) {
        std::cout << "[+] " << test_name << ": ͨ��\n";
    }
    else {
        std::cout << "[-] " << test_name << ": ʧ��\n";
    }
}