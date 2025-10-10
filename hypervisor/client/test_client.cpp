/**
 * test_client.cpp - Hypervisor ���Կͻ���
 *
 * ���ڷֱ���� memhv �� airhv �����Ĺ���
 */

#include <iostream>
#include <windows.h>
#include <string>
#include <vector>

 // �������ǵ�ͳһ�ӿ�
#include "../common/hypervisor_interface.h"
#include "../common/hypervisor_factory.h"
#include "../common/hypervisor_utils.h"
#include "../common/hypervisor_config.h"

// �����������ӿ�
#include "../memhv/Source/memhv_adapter.h"
#include "../airhv/Source/airhv_adapter.h"

enum TestTarget {
    TARGET_AUTO = 0,
    TARGET_MEMHV = 1,
    TARGET_AIRHV = 2,
    TARGET_BOTH = 3
};

enum TestResult {
    TEST_PASS = 0,
    TEST_FAIL = 1,
    TEST_SKIP = 2
};

struct TestInfo {
    std::string name;
    std::string description;
    TestResult result;
    std::string error_message;
};

class HypervisorTester {
private:
    std::vector<TestInfo> test_results;
    HYPERVISOR_TYPE current_hypervisor;

public:
    HypervisorTester() : current_hypervisor(HYPERVISOR_TYPE_UNKNOWN) {}

    void PrintUsage() {
        std::cout << "Hypervisor ���Կͻ���\n";
        std::cout << "�÷�: test_client.exe [ѡ��]\n";
        std::cout << "ѡ��:\n";
        std::cout << "  --memhv     : ������memhv���� (AMD SVM)\n";
        std::cout << "  --airhv     : ������airhv���� (Intel VT-x)\n";
        std::cout << "  --both      : ���β�����������\n";
        std::cout << "  --auto      : �Զ���Ⲣ���� (Ĭ��)\n";
        std::cout << "  --help      : ��ʾ����\n";
    }

    bool IsAMDProcessor() {
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

    void PrintTestHeader(const std::string& driver_name) {
        std::cout << "\n" << std::string(50, '=') << "\n";
        std::cout << "���� " << driver_name << " ����\n";
        std::cout << std::string(50, '=') << "\n\n";
    }

    void PrintTestResult(const std::string& test_name, TestResult result, const std::string& message = "") {
        TestInfo info;
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

    TestResult TestDriverLoad(HYPERVISOR_TYPE type) {
        std::cout << "[*] ����1: ��������\n";

        NTSTATUS status = HypervisorInterfaceInitialize(type);
        if (NT_SUCCESS(status)) {
            PrintTestResult("��������", TEST_PASS);
            current_hypervisor = type;
            return TEST_PASS;
        }
        else {
            PrintTestResult("��������", TEST_FAIL, "��ʼ��ʧ�ܣ�״̬��: 0x" + std::to_string(status));
            return TEST_FAIL;
        }
    }

    TestResult TestHypervisorPresence() {
        std::cout << "[*] ����2: Hypervisor�����Լ��\n";

        if (HypervisorIsPresent()) {
            PrintTestResult("�����Լ��", TEST_PASS, "Hypervisor��������");
            return TEST_PASS;
        }
        else {
            PrintTestResult("�����Լ��", TEST_FAIL, "Hypervisorδ����");
            return TEST_FAIL;
        }
    }

    TestResult TestProcessorCount() {
        std::cout << "[*] ����3: �������������\n";

        ULONG count = HypervisorGetProcessorCount();
        if (count > 0) {
            PrintTestResult("���������", TEST_PASS, "��⵽ " + std::to_string(count) + " ��������");
            return TEST_PASS;
        }
        else {
            PrintTestResult("���������", TEST_FAIL, "�޷���ȡ����������");
            return TEST_FAIL;
        }
    }

    TestResult TestProcessEnumeration() {
        std::cout << "[*] ����4: ����ö��\n";

        // ���Ի�ȡSystem���� (PID 4)
        PEPROCESS systemProcess = HypervisorGetProcessById((HANDLE)4);
        if (systemProcess) {
            PrintTestResult("����ö��", TEST_PASS, "�ɹ���ȡSystem����");
            HypervisorDereferenceObject(systemProcess);
            return TEST_PASS;
        }
        else {
            PrintTestResult("����ö��", TEST_FAIL, "�޷���ȡSystem����");
            return TEST_FAIL;
        }
    }

    TestResult TestProcessDirectoryBase() {
        std::cout << "[*] ����5: ����Ŀ¼��ַ��ȡ\n";

        PEPROCESS systemProcess = HypervisorGetProcessById((HANDLE)4);
        if (systemProcess) {
            ULONG64 dirBase = HypervisorGetProcessDirectoryBase(systemProcess);
            if (dirBase != 0) {
                PrintTestResult("Ŀ¼��ַ��ȡ", TEST_PASS, "System����CR3: 0x" + std::to_string(dirBase));
                HypervisorDereferenceObject(systemProcess);
                return TEST_PASS;
            }
            else {
                PrintTestResult("Ŀ¼��ַ��ȡ", TEST_FAIL, "�޷���ȡĿ¼��ַ");
                HypervisorDereferenceObject(systemProcess);
                return TEST_FAIL;
            }
        }
        else {
            PrintTestResult("Ŀ¼��ַ��ȡ", TEST_SKIP, "System���̲�����");
            return TEST_SKIP;
        }
    }

    TestResult TestMemoryRead() {
        std::cout << "[*] ����6: �ڴ��ȡ\n";

        PEPROCESS systemProcess = HypervisorGetProcessById((HANDLE)4);
        if (systemProcess) {
            ULONG64 dirBase = HypervisorGetProcessDirectoryBase(systemProcess);
            if (dirBase != 0) {
                // ���Զ�ȡһ����֪���ں˵�ַ (ʾ��)
                UCHAR buffer[8] = { 0 };
                SIZE_T bytesRead = 0;
                NTSTATUS status = HypervisorReadProcessMemory(dirBase, 0xFFFFF80000000000, buffer, sizeof(buffer), &bytesRead);

                if (NT_SUCCESS(status)) {
                    PrintTestResult("�ڴ��ȡ", TEST_PASS, "�ɹ���ȡ " + std::to_string(bytesRead) + " �ֽ�");
                    HypervisorDereferenceObject(systemProcess);
                    return TEST_PASS;
                }
                else {
                    PrintTestResult("�ڴ��ȡ", TEST_FAIL, "��ȡʧ�ܣ�״̬��: 0x" + std::to_string(status));
                    HypervisorDereferenceObject(systemProcess);
                    return TEST_FAIL;
                }
            }
            else {
                PrintTestResult("�ڴ��ȡ", TEST_SKIP, "Ŀ¼��ַ������");
                HypervisorDereferenceObject(systemProcess);
                return TEST_SKIP;
            }
        }
        else {
            PrintTestResult("�ڴ��ȡ", TEST_SKIP, "System���̲�����");
            return TEST_SKIP;
        }
    }

    TestResult TestSelfProtection() {
        std::cout << "[*] ����7: ���ұ�������\n";

        NTSTATUS status = HypervisorProtectSelf();
        if (NT_SUCCESS(status)) {
            PrintTestResult("���ұ���", TEST_PASS, "�����������óɹ�");
            return TEST_PASS;
        }
        else {
            PrintTestResult("���ұ���", TEST_FAIL, "������������ʧ�ܣ�״̬��: 0x" + std::to_string(status));
            return TEST_FAIL;
        }
    }

    TestResult TestCleanup() {
        std::cout << "[*] ����8: ������Դ\n";

        try {
            HypervisorInterfaceCleanup();
            PrintTestResult("��Դ����", TEST_PASS, "��Դ�������");
            return TEST_PASS;
        }
        catch (...) {
            PrintTestResult("��Դ����", TEST_FAIL, "��������з����쳣");
            return TEST_FAIL;
        }
    }

    int RunTestsForHypervisor(HYPERVISOR_TYPE type) {
        std::string driver_name;
        switch (type) {
        case HYPERVISOR_TYPE_MEMHV:
            driver_name = "memhv (AMD SVM)";
            break;
        case HYPERVISOR_TYPE_AIRHV:
            driver_name = "airhv (Intel VT-x)";
            break;
        default:
            driver_name = "δ֪";
            break;
        }

        PrintTestHeader(driver_name);
        test_results.clear();

        // ִ�����в���
        int failed_tests = 0;

        if (TestDriverLoad(type) == TEST_FAIL) {
            std::cout << "[-] ��������ʧ�ܣ�������������\n";
            failed_tests++;
        }
        else {
            if (TestHypervisorPresence() == TEST_FAIL) failed_tests++;
            if (TestProcessorCount() == TEST_FAIL) failed_tests++;
            if (TestProcessEnumeration() == TEST_FAIL) failed_tests++;
            if (TestProcessDirectoryBase() == TEST_FAIL) failed_tests++;
            if (TestMemoryRead() == TEST_FAIL) failed_tests++;
            if (TestSelfProtection() == TEST_FAIL) failed_tests++;
            if (TestCleanup() == TEST_FAIL) failed_tests++;
        }

        // �������ժҪ
        std::cout << "\n" << std::string(30, '-') << "\n";
        std::cout << "����ժҪ: " << driver_name << "\n";
        std::cout << "�ܲ�����: " << test_results.size() << "\n";
        std::cout << "ͨ��: " << (test_results.size() - failed_tests) << "\n";
        std::cout << "ʧ��: " << failed_tests << "\n";
        std::cout << std::string(30, '-') << "\n";

        return failed_tests;
    }

    int TestMemhv() {
        return RunTestsForHypervisor(HYPERVISOR_TYPE_MEMHV);
    }

    int TestAirhv() {
        return RunTestsForHypervisor(HYPERVISOR_TYPE_AIRHV);
    }

    int TestAutoDetect() {
        if (IsAMDProcessor()) {
            std::cout << "[*] ��⵽AMD������\n";
            return TestMemhv();
        }
        else if (IsIntelProcessor()) {
            std::cout << "[*] ��⵽Intel������\n";
            return TestAirhv();
        }
        else {
            std::cout << "[-] δʶ��Ĵ���������\n";
            return -1;
        }
    }
};

int main(int argc, char* argv[]) {
    HypervisorTester tester;
    TestTarget target = TARGET_AUTO;

    // ���������в���
    if (argc > 1) {
        if (strcmp(argv[1], "--memhv") == 0) {
            target = TARGET_MEMHV;
        }
        else if (strcmp(argv[1], "--airhv") == 0) {
            target = TARGET_AIRHV;
        }
        else if (strcmp(argv[1], "--both") == 0) {
            target = TARGET_BOTH;
        }
        else if (strcmp(argv[1], "--auto") == 0) {
            target = TARGET_AUTO;
        }
        else if (strcmp(argv[1], "--help") == 0) {
            tester.PrintUsage();
            return 0;
        }
        else {
            tester.PrintUsage();
            return -1;
        }
    }

    std::cout << "[*] Hypervisor ���Կͻ���\n";

    int result = 0;
    switch (target) {
    case TARGET_MEMHV:
        result = tester.TestMemhv();
        break;

    case TARGET_AIRHV:
        result = tester.TestAirhv();
        break;

    case TARGET_BOTH:
    {
        int memhv_result = tester.TestMemhv();
        std::cout << "\n" << std::string(60, '=') << "\n\n";
        int airhv_result = tester.TestAirhv();
        result = (memhv_result == 0 && airhv_result == 0) ? 0 : -1;
    }
    break;

    case TARGET_AUTO:
    default:
        result = tester.TestAutoDetect();
        break;
    }

    std::cout << "\n[*] �������\n";
    return result;
}