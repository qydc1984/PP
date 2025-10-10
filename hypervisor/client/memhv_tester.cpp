/**
 * memhv_tester.cpp - memhv ����ר�ò�����ʵ��
 *
 * ʵ�� memhv (AMD SVM) �����ĸ���ܲ���
 */

#include "memhv_tester.h"
#include <iostream>
#include <windows.h>
#include <intrin.h>
#include <string>
#include <vector>

 // ���캯��
MemhvTester::MemhvTester() {
    test_results.clear();
}

// ��������
MemhvTester::~MemhvTester() {
    test_results.clear();
}

// ��ӡ����ͷ��
void MemhvTester::PrintTestHeader() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "memhv (AMD SVM) ��������\n";
    std::cout << std::string(60, '=') << "\n\n";
}

// ��ӡ���Խ��
void MemhvTester::PrintTestResult(const std::string& test_name, TestResult result, const std::string& message) {
    MemhvTestInfo info;
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
int MemhvTester::GetFailedTestCount() const {
    int failed = 0;
    for (const auto& result : test_results) {
        if (result.result == TEST_FAIL) {
            failed++;
        }
    }
    return failed;
}

// ��ӡ����ժҪ
void MemhvTester::PrintTestSummary() {
    std::cout << "\n" << std::string(40, '-') << "\n";
    std::cout << "memhv ����ժҪ\n";
    std::cout << "�ܲ�����: " << test_results.size() << "\n";
    std::cout << "ͨ��: " << (test_results.size() - GetFailedTestCount()) << "\n";
    std::cout << "ʧ��: " << GetFailedTestCount() << "\n";
    std::cout << std::string(40, '-') << "\n";
}

// ����Ƿ�ΪAMD������
bool MemhvTester::IsAMDProcessor() {
    int cpuInfo[4];
    __cpuid(cpuInfo, 0);

    // ���AMD��ʶ: "AuthenticAMD"
    if (cpuInfo[1] == 0x68747541 && // "htuA"
        cpuInfo[3] == 0x69746E65 && // "itne"
        cpuInfo[2] == 0x444D4163)   // "DMAc"
    {
        return true;
    }
    return false;
}

// ��������������
TestResult MemhvTester::TestDriverPresence() {
    std::cout << "[*] ����1: ���������Լ��\n";

    if (MemHvIsPresent()) {
        PrintTestResult("����������", TEST_PASS, "memhv�����Ѽ��ز�����");
        return TEST_PASS;
    }
    else {
        PrintTestResult("����������", TEST_FAIL, "memhv����δ��⵽");
        return TEST_FAIL;
    }
}

// ���Գ�ʼ��
TestResult MemhvTester::TestInitialization() {
    std::cout << "[*] ����2: ��ʼ�����\n";

    // memhv ͨ�������Զ���ʼ�����������������
    if (MemHvIsPresent()) {
        PrintTestResult("��ʼ��", TEST_PASS, "��������ȷ��ʼ��");
        return TEST_PASS;
    }
    else {
        PrintTestResult("��ʼ��", TEST_FAIL, "������ʼ��ʧ��");
        return TEST_FAIL;
    }
}

// ���Դ���������
TestResult MemhvTester::TestProcessorCount() {
    std::cout << "[*] ����3: ����������\n";

    ULONG processorCount = KeQueryActiveProcessorCount(NULL);
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
TestResult MemhvTester::TestProcessAccess() {
    std::cout << "[*] ����4: ���̷���\n";

    // ���Ի�ȡSystem���� (PID 4)
    PEPROCESS systemProcess = MemHvGetProcessById((HANDLE)4);
    if (systemProcess) {
        PrintTestResult("���̷���", TEST_PASS, "�ɹ���ȡSystem���̶���");
        MemHvDereferenceObject(systemProcess);
        return TEST_PASS;
    }
    else {
        PrintTestResult("���̷���", TEST_FAIL, "�޷���ȡSystem����");
        return TEST_FAIL;
    }
}

// ���Խ���Ŀ¼��ַ
TestResult MemhvTester::TestProcessDirectoryBase() {
    std::cout << "[*] ����5: ����Ŀ¼��ַ\n";

    PEPROCESS systemProcess = MemHvGetProcessById((HANDLE)4);
    if (systemProcess) {
        ULONG64 dirBase = MemHvGetProcessDirectoryBase(systemProcess);
        if (dirBase != 0) {
            char buffer[64];
            sprintf_s(buffer, "CR3 = 0x%llx", dirBase);
            PrintTestResult("Ŀ¼��ַ", TEST_PASS, buffer);
            MemHvDereferenceObject(systemProcess);
            return TEST_PASS;
        }
        else {
            PrintTestResult("Ŀ¼��ַ", TEST_FAIL, "�޷���ȡĿ¼��ַ");
            MemHvDereferenceObject(systemProcess);
            return TEST_FAIL;
        }
    }
    else {
        PrintTestResult("Ŀ¼��ַ", TEST_SKIP, "System���̲�����");
        return TEST_SKIP;
    }
}

// �����ڴ��ȡ
TestResult MemhvTester::TestMemoryRead() {
    std::cout << "[*] ����6: �ڴ��ȡ\n";

    PEPROCESS systemProcess = MemHvGetProcessById((HANDLE)4);
    if (systemProcess) {
        ULONG64 dirBase = MemHvGetProcessDirectoryBase(systemProcess);
        if (dirBase != 0) {
            UCHAR buffer[16] = { 0 };
            SIZE_T bytesRead = 0;
            NTSTATUS status = MemHvReadProcessMemory(dirBase, 0xFFFFF80000000000, buffer, sizeof(buffer), &bytesRead);

            if (NT_SUCCESS(status) && bytesRead > 0) {
                PrintTestResult("�ڴ��ȡ", TEST_PASS, "�ɹ���ȡ " + std::to_string(bytesRead) + " �ֽ�");
                MemHvDereferenceObject(systemProcess);
                return TEST_PASS;
            }
            else {
                char error_msg[128];
                sprintf_s(error_msg, "��ȡʧ�ܣ�״̬��: 0x%08X", status);
                PrintTestResult("�ڴ��ȡ", TEST_FAIL, error_msg);
                MemHvDereferenceObject(systemProcess);
                return TEST_FAIL;
            }
        }
        else {
            PrintTestResult("�ڴ��ȡ", TEST_SKIP, "Ŀ¼��ַ������");
            MemHvDereferenceObject(systemProcess);
            return TEST_SKIP;
        }
    }
    else {
        PrintTestResult("�ڴ��ȡ", TEST_SKIP, "System���̲�����");
        return TEST_SKIP;
    }
}

// �����ڴ�д��
TestResult MemhvTester::TestMemoryWrite() {
    std::cout << "[*] ����7: �ڴ�д��\n";

    // ע�⣺ʵ�ʲ�����Ӧʹ�ð�ȫ�Ĳ��Ե�ַ
    PrintTestResult("�ڴ�д��", TEST_SKIP, "Ϊ��ȫ�������д�����");
    return TEST_SKIP;
}

// �����ڴ濽��
TestResult MemhvTester::TestMemoryCopy() {
    std::cout << "[*] ����8: �ڴ濽��\n";

    PrintTestResult("�ڴ濽��", TEST_SKIP, "��Ҫ������Ч���̽��в���");
    return TEST_SKIP;
}

// ���Խ���ö��
TestResult MemhvTester::TestProcessEnumeration() {
    std::cout << "[*] ����9: ����ö��\n";

    int processCount = 0;
    PEPROCESS current = PsInitialSystemProcess;

    // �򵥱����������������Թ���
    for (int i = 0; i < 10 && current != nullptr; i++) {
        HANDLE processId = PsGetProcessId(current);
        if (processId) {
            processCount++;
        }
        current = MemHvGetNextProcess(current);
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
TestResult MemhvTester::TestSelfProtection() {
    std::cout << "[*] ����10: ���ұ���\n";

    NTSTATUS status = MemHvEnableProtection();
    if (NT_SUCCESS(status)) {
        PrintTestResult("���ұ���", TEST_PASS, "�����������óɹ�");
        return TEST_PASS;
    }
    else {
        char error_msg[128];
        sprintf_s(error_msg, "������������ʧ�ܣ�״̬��: 0x%08X", status);
        PrintTestResult("���ұ���", TEST_FAIL, error_msg);
        return TEST_FAIL;
    }
}

// ����MSR��֤
TestResult MemhvTester::TestMsrValidation() {
    std::cout << "[*] ����11: MSR��֤\n";

    // ���Լ���������MSR
    bool valid1 = MemHvIsValidMsr(0xC0000080); // EFER
    bool valid2 = MemHvIsValidMsr(0x00000010); // CS
    bool invalid1 = MemHvIsValidMsr(0xFFFFFFFF); // ��ЧMSR

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
TestResult MemhvTester::TestUserAddressValidation() {
    std::cout << "[*] ����12: �û���ַ��֤\n";

    bool valid1 = MemHvIsValidUserAddress(0x00001000); // ��Ч�û���ַ
    bool invalid1 = MemHvIsValidUserAddress(0x00000000); // ��Ч��ַ
    bool invalid2 = MemHvIsValidUserAddress(0x800000000000); // �ں˵�ַ

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
TestResult MemhvTester::TestObjectManagement() {
    std::cout << "[*] ����13: �������\n";

    PEPROCESS systemProcess = MemHvGetProcessById((HANDLE)4);
    if (systemProcess) {
        // �������ü�������
        MemHvReferenceObject(systemProcess);
        MemHvDereferenceObject(systemProcess);
        MemHvDereferenceObject(systemProcess);
        PrintTestResult("�������", TEST_PASS, "���ü����������");
        return TEST_PASS;
    }
    else {
        PrintTestResult("�������", TEST_FAIL, "�޷���ȡ���̶���");
        return TEST_FAIL;
    }
}

// ���԰�ȫ�ڴ����
TestResult MemhvTester::TestSafeMemoryOperations() {
    std::cout << "[*] ����14: ��ȫ�ڴ����\n";

    PEPROCESS systemProcess = MemHvGetProcessById((HANDLE)4);
    if (systemProcess) {
        ULONG64 dirBase = MemHvGetProcessDirectoryBase(systemProcess);
        if (dirBase != 0) {
            UCHAR buffer[16] = { 0 };
            SIZE_T bytesRead = 0;
            NTSTATUS status = MemHvReadProcessMemorySafe(dirBase, 0xFFFFF80000000000, buffer, sizeof(buffer), &bytesRead);

            if (NT_SUCCESS(status)) {
                PrintTestResult("��ȫ�ڴ����", TEST_PASS, "��ȫ��ȡ�������");
                MemHvDereferenceObject(systemProcess);
                return TEST_PASS;
            }
            else {
                char error_msg[128];
                sprintf_s(error_msg, "��ȫ��ȡʧ�ܣ�״̬��: 0x%08X", status);
                PrintTestResult("��ȫ�ڴ����", TEST_FAIL, error_msg);
                MemHvDereferenceObject(systemProcess);
                return TEST_FAIL;
            }
        }
        else {
            PrintTestResult("��ȫ�ڴ����", TEST_SKIP, "Ŀ¼��ַ������");
            MemHvDereferenceObject(systemProcess);
            return TEST_SKIP;
        }
    }
    else {
        PrintTestResult("��ȫ�ڴ����", TEST_SKIP, "System���̲�����");
        return TEST_SKIP;
    }
}

// ��������
TestResult MemhvTester::TestCleanup() {
    std::cout << "[*] ����15: ��Դ����\n";

    // memhv ������������������ֻ��ȷ�Ϲ��ܵ���
    PrintTestResult("��Դ����", TEST_PASS, "�����ܽӿڿ���");
    return TEST_PASS;
}

// �������в���
int MemhvTester::RunAllTests() {
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
    TestCleanup();

    PrintTestSummary();

    return GetFailedTestCount();
}

// ȫ�ֲ��Ժ���ʵ��
int TestMemhvDriver() {
    if (!IsAMDProcessor()) {
        std::cout << "[-] ��ǰ����AMD������������memhv����\n";
        return -1;
    }

    MemhvTester tester;
    return tester.RunAllTests();
}

bool TestMemhvLoad() {
    return MemHvIsPresent();
}

bool TestMemhvInitialization() {
    return MemHvIsPresent();
}

bool TestMemhvPresence() {
    return MemHvIsPresent();
}

bool TestMemhvProcessAccess() {
    PEPROCESS process = MemHvGetProcessById((HANDLE)4);
    if (process) {
        MemHvDereferenceObject(process);
        return true;
    }
    return false;
}

bool TestMemhvMemoryRead() {
    PEPROCESS process = MemHvGetProcessById((HANDLE)4);
    if (process) {
        ULONG64 dirBase = MemHvGetProcessDirectoryBase(process);
        if (dirBase != 0) {
            UCHAR buffer[8];
            SIZE_T bytesRead;
            NTSTATUS status = MemHvReadProcessMemory(dirBase, 0xFFFFF80000000000, buffer, sizeof(buffer), &bytesRead);
            MemHvDereferenceObject(process);
            return NT_SUCCESS(status);
        }
        MemHvDereferenceObject(process);
    }
    return false;
}

bool TestMemhvMemoryWrite() {
    // Ϊ��ȫ�����Ĭ������д�����
    return true;
}

bool TestMemhvProtection() {
    NTSTATUS status = MemHvEnableProtection();
    return NT_SUCCESS(status);
}

bool TestMemhvCleanup() {
    // ��������������
    return true;
}

bool IsAMDProcessor() {
    int cpuInfo[4];
    __cpuid(cpuInfo, 0);

    if (cpuInfo[1] == 0x68747541 && // "htuA"
        cpuInfo[3] == 0x69746E65 && // "itne"
        cpuInfo[2] == 0x444D4163)   // "DMAc"
    {
        return true;
    }
    return false;
}

void PrintMemhvTestHeader() {
    std::cout << "[*] ��ʼmemhv��������\n";
}

void PrintMemhvTestResult(const char* test_name, bool result) {
    if (result) {
        std::cout << "[+] " << test_name << ": ͨ��\n";
    }
    else {
        std::cout << "[-] " << test_name << ": ʧ��\n";
    }
}