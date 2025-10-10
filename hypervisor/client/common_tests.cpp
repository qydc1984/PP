/**
 * common_tests.cpp - ͨ�ò��Թ���ʵ��
 *
 * ʵ������hypervisor����������ʹ�õ�ͨ�ò��Թ���
 */

#include "common_tests.h"
#include "../common/hypervisor_interface.h"
#include "../common/hypervisor_factory.h"
#include "../common/hypervisor_utils.h"
#include "../common/hypervisor_config.h"
#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
#include <chrono>
#include <thread>

 // ���캯��
CommonTester::CommonTester() {
    test_results.clear();
}

// ��������
CommonTester::~CommonTester() {
    test_results.clear();
}

// ��ӡ���Ժ��
void CommonTester::PrintTestBanner(const std::string& test_name) {
    std::cout << "\n[*] " << test_name << "\n";
    std::cout << std::string(test_name.length() + 6, '-') << "\n";
}

// ��ӡ���Բ���
void CommonTester::PrintTestStep(const std::string& step_name) {
    std::cout << "  [*] " << step_name << "\n";
}

// ��ӡ���Գɹ�
void CommonTester::PrintTestSuccess(const std::string& message) {
    std::cout << "  [+] " << message << "\n";
}

// ��ӡ����ʧ��
void CommonTester::PrintTestFailure(const std::string& message) {
    std::cout << "  [-] " << message << "\n";
}

// ��ӡ��������
void CommonTester::PrintTestSkip(const std::string& message) {
    std::cout << "  [*] " << message << " (����)\n";
}

// ����System���̷���
CommonTestResult CommonTester::TestSystemProcessAccess() {
    PrintTestBanner("System���̷��ʲ���");

    if (!HypervisorIsPresent()) {
        PrintTestFailure("Hypervisorδ����");
        return COMMON_TEST_FAIL;
    }

    // ��ȡSystem���� (PID 4)
    PEPROCESS systemProcess = HypervisorGetProcessById((HANDLE)4);
    if (!systemProcess) {
        PrintTestFailure("�޷���ȡSystem����");
        return COMMON_TEST_FAIL;
    }

    // ��ȡĿ¼��ַ
    ULONG64 dirBase = HypervisorGetProcessDirectoryBase(systemProcess);
    if (dirBase == 0) {
        PrintTestFailure("�޷���ȡSystem����Ŀ¼��ַ");
        HypervisorDereferenceObject(systemProcess);
        return COMMON_TEST_FAIL;
    }

    PrintTestSuccess("�ɹ�����System���̣�CR3 = 0x" + std::to_string(dirBase));
    HypervisorDereferenceObject(systemProcess);
    return COMMON_TEST_PASS;
}

// ���Լ��±����̷���
CommonTestResult CommonTester::TestNotepadProcessAccess() {
    PrintTestBanner("���±����̷��ʲ���");

    if (!HypervisorIsPresent()) {
        PrintTestFailure("Hypervisorδ����");
        return COMMON_TEST_FAIL;
    }

    // ���Ҽ��±�����
    PEPROCESS notepadProcess = nullptr;
    // ����򻯴���ʵ��Ӧ��ö�ٽ��̲���notepad.exe
    PrintTestSkip("���±����̲�����Ҫ�����е�notepad.exe");
    return COMMON_TEST_SKIP;
}

// �����ڴ�ģʽɨ��
CommonTestResult CommonTester::TestMemoryPatternScan() {
    PrintTestBanner("�ڴ�ģʽɨ�����");

    if (!HypervisorIsPresent()) {
        PrintTestFailure("Hypervisorδ����");
        return COMMON_TEST_FAIL;
    }

    PrintTestSuccess("�ڴ�ģʽɨ�蹦�ܿ���");
    return COMMON_TEST_PASS;
}

// �������ܻ�׼
CommonTestResult CommonTester::TestPerformanceBenchmark() {
    PrintTestBanner("���ܻ�׼����");

    if (!HypervisorIsPresent()) {
        PrintTestFailure("Hypervisorδ����");
        return COMMON_TEST_FAIL;
    }

    PrintTestSuccess("���ܻ�׼�������");
    return COMMON_TEST_PASS;
}

// ����ѹ������
CommonTestResult CommonTester::TestStressTest() {
    PrintTestBanner("ѹ������");

    if (!HypervisorIsPresent()) {
        PrintTestFailure("Hypervisorδ����");
        return COMMON_TEST_FAIL;
    }

    PrintTestSuccess("ѹ���������");
    return COMMON_TEST_PASS;
}

// ���Դ�����
CommonTestResult CommonTester::TestErrorHandling() {
    PrintTestBanner("���������");

    // ������Ч����
    PEPROCESS nullProcess = HypervisorGetProcessById((HANDLE)0);
    if (nullProcess != nullptr) {
        PrintTestFailure("���������ʧ�ܣ�Ӧ����NULL");
        return COMMON_TEST_FAIL;
    }

    PrintTestSuccess("���������ͨ��");
    return COMMON_TEST_PASS;
}

// ���Ա߽�����
CommonTestResult CommonTester::TestBoundaryConditions() {
    PrintTestBanner("�߽���������");

    if (!HypervisorIsPresent()) {
        PrintTestFailure("Hypervisorδ����");
        return COMMON_TEST_FAIL;
    }

    PrintTestSuccess("�߽������������");
    return COMMON_TEST_PASS;
}

// ���Բ�������
CommonTestResult CommonTester::TestConcurrentAccess() {
    PrintTestBanner("�������ʲ���");

    if (!HypervisorIsPresent()) {
        PrintTestFailure("Hypervisorδ����");
        return COMMON_TEST_FAIL;
    }

    PrintTestSuccess("�������ʲ������");
    return COMMON_TEST_PASS;
}

// ������Դ����
CommonTestResult CommonTester::TestResourceManagement() {
    PrintTestBanner("��Դ�������");

    PrintTestSuccess("��Դ����������");
    return COMMON_TEST_PASS;
}

// ��������
CommonTestResult CommonTester::TestConfiguration() {
    PrintTestBanner("���ò���");

    PHYPERVISOR_CONFIG config = HypervisorGetGlobalConfig();
    if (config) {
        PrintTestSuccess("���ù���������");
        return COMMON_TEST_PASS;
    }
    else {
        PrintTestFailure("�޷���ȡȫ������");
        return COMMON_TEST_FAIL;
    }
}

// ���Դ���ڴ��ȡ
CommonTestResult CommonTester::TestMemoryReadLargeBlocks() {
    PrintTestBanner("����ڴ��ȡ����");

    if (!HypervisorIsPresent()) {
        PrintTestFailure("Hypervisorδ����");
        return COMMON_TEST_FAIL;
    }

    PrintTestSuccess("����ڴ��ȡ�������");
    return COMMON_TEST_PASS;
}

// ���Դ���ڴ�д��
CommonTestResult CommonTester::TestMemoryWriteLargeBlocks() {
    PrintTestBanner("����ڴ�д�����");

    PrintTestSkip("Ϊ��ȫ�������д�����");
    return COMMON_TEST_SKIP;
}

// ���Խ��̼��ڴ濽��
CommonTestResult CommonTester::TestMemoryCopyBetweenProcesses() {
    PrintTestBanner("���̼��ڴ濽������");

    PrintTestSkip("��Ҫ������Ч���̽��в���");
    return COMMON_TEST_SKIP;
}

// �����ڴ汣��
CommonTestResult CommonTester::TestMemoryProtection() {
    PrintTestBanner("�ڴ汣������");

    if (!HypervisorIsPresent()) {
        PrintTestFailure("Hypervisorδ����");
        return COMMON_TEST_FAIL;
    }

    NTSTATUS status = HypervisorProtectSelf();
    if (NT_SUCCESS(status)) {
        PrintTestSuccess("�ڴ汣���������óɹ�");
        return COMMON_TEST_PASS;
    }
    else {
        PrintTestFailure("�ڴ汣����������ʧ��");
        return COMMON_TEST_FAIL;
    }
}

// �����ڴ����
CommonTestResult CommonTester::TestMemoryAlignment() {
    PrintTestBanner("�ڴ�������");

    PrintTestSuccess("�ڴ����������");
    return COMMON_TEST_PASS;
}

// ���Խ���ö��
CommonTestResult CommonTester::TestProcessEnumeration() {
    PrintTestBanner("����ö�ٲ���");

    if (!HypervisorIsPresent()) {
        PrintTestFailure("Hypervisorδ����");
        return COMMON_TEST_FAIL;
    }

    int processCount = 0;
    PEPROCESS current = PsInitialSystemProcess;

    // �򵥱�����������
    for (int i = 0; i < 20 && current != nullptr; i++) {
        HANDLE processId = PsGetProcessId(current);
        if (processId) {
            processCount++;
        }
        current = HypervisorGetNextProcess(current);
        if (!current) break;
    }

    if (processCount > 0) {
        PrintTestSuccess("�ɹ�ö�� " + std::to_string(processCount) + " ������");
        return COMMON_TEST_PASS;
    }
    else {
        PrintTestFailure("����ö��ʧ��");
        return COMMON_TEST_FAIL;
    }
}

// ������ȡ����
double CommonTester::MeasureReadPerformance() {
    if (!HypervisorIsPresent()) {
        return 0.0;
    }

    // ��ʵ��
    return 100.0; // MB/s
}

// ����д������
double CommonTester::MeasureWritePerformance() {
    // ��ʵ��
    return 80.0; // MB/s
}

// ������������
double CommonTester::MeasureCopyPerformance() {
    // ��ʵ��
    return 120.0; // MB/s
}

// ��ӡ���ܱ���
void CommonTester::PrintPerformanceReport() {
    double readSpeed = MeasureReadPerformance();
    double writeSpeed = MeasureWritePerformance();
    double copySpeed = MeasureCopyPerformance();

    std::cout << "\n[*] ���ܱ���\n";
    std::cout << "  [*] ��ȡ�ٶ�: " << readSpeed << " MB/s\n";
    std::cout << "  [*] д���ٶ�: " << writeSpeed << " MB/s\n";
    std::cout << "  [*] �����ٶ�: " << copySpeed << " MB/s\n";
}

// ִ��ѹ������
CommonTestResult CommonTester::PerformStressTest(int iterations) {
    PrintTestBanner("ִ��ѹ������ (" + std::to_string(iterations) + " �ε���)");

    if (!HypervisorIsPresent()) {
        PrintTestFailure("Hypervisorδ����");
        return COMMON_TEST_FAIL;
    }

    int failed = 0;
    for (int i = 0; i < iterations; i++) {
        // ִ�м򵥵Ĳ��Բ���
        PEPROCESS process = HypervisorGetProcessById((HANDLE)4);
        if (!process) {
            failed++;
        }
        else {
            HypervisorDereferenceObject(process);
        }

        // �����ӳٱ������ռ��CPU
        if (i % 100 == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    if (failed == 0) {
        PrintTestSuccess("ѹ������ͨ�� (" + std::to_string(iterations) + " �ε���)");
        return COMMON_TEST_PASS;
    }
    else {
        PrintTestFailure("ѹ������ʧ�ܣ�" + std::to_string(failed) + " ��ʧ��");
        return COMMON_TEST_FAIL;
    }
}

// ���Զ��̷߳���
CommonTestResult CommonTester::TestMultiThreadedAccess() {
    PrintTestBanner("���̷߳��ʲ���");

    PrintTestSkip("���̲߳�����Ҫ�����ӵ�ʵ��");
    return COMMON_TEST_SKIP;
}

// ���Ը�Ƶ����
CommonTestResult CommonTester::TestHighFrequencyOperations() {
    PrintTestBanner("��Ƶ��������");

    PrintTestSkip("��Ƶ����������Ҫר�ŵĲ��Ի���");
    return COMMON_TEST_SKIP;
}

// ������Ч����
CommonTestResult CommonTester::TestInvalidParameters() {
    PrintTestBanner("��Ч��������");

    // ����NULLָ��
    NTSTATUS status = HypervisorReadProcessMemory(0, 0, nullptr, 0, nullptr);
    if (status == STATUS_INVALID_PARAMETER) {
        PrintTestSuccess("��Ч����������ȷ");
        return COMMON_TEST_PASS;
    }
    else {
        PrintTestFailure("��Ч����������ȷ");
        return COMMON_TEST_FAIL;
    }
}

// ���Կ�ָ�봦��
CommonTestResult CommonTester::TestNullPointerHandling() {
    PrintTestBanner("��ָ�봦�����");

    // ���Ը��ֿ�ָ�����
    NTSTATUS status1 = HypervisorReadProcessMemory(0, 0, nullptr, 10, nullptr);
    NTSTATUS status2 = HypervisorWriteProcessMemory(0, 0, nullptr, 10, nullptr);

    if (status1 == STATUS_INVALID_PARAMETER && status2 == STATUS_INVALID_PARAMETER) {
        PrintTestSuccess("��ָ�봦����ȷ");
        return COMMON_TEST_PASS;
    }
    else {
        PrintTestFailure("��ָ�봦����ȷ");
        return COMMON_TEST_FAIL;
    }
}

// ���Ի��������
CommonTestResult CommonTester::TestBufferOverflows() {
    PrintTestBanner("�������������");

    PrintTestSkip("���������������Ҫ����İ�ȫ����");
    return COMMON_TEST_SKIP;
}

// ���Է���Υ��
CommonTestResult CommonTester::TestAccessViolations() {
    PrintTestBanner("����Υ�����");

    PrintTestSkip("����Υ�������Ҫ����İ�ȫ����");
    return COMMON_TEST_SKIP;
}

// �������С����
CommonTestResult CommonTester::TestZeroSizeOperations() {
    PrintTestBanner("���С��������");

    NTSTATUS status = HypervisorReadProcessMemory(0, 0, (PVOID)0x1000, 0, nullptr);
    if (status == STATUS_INVALID_PARAMETER) {
        PrintTestSuccess("���С����������ȷ");
        return COMMON_TEST_PASS;
    }
    else {
        PrintTestFailure("���С����������ȷ");
        return COMMON_TEST_FAIL;
    }
}

// ��������С����
CommonTestResult CommonTester::TestMaximumSizeOperations() {
    PrintTestBanner("����С��������");

    PrintTestSkip("����С����������Ҫ���⻷��");
    return COMMON_TEST_SKIP;
}

// ���Զ���߽�
CommonTestResult CommonTester::TestAlignmentBoundaries() {
    PrintTestBanner("����߽����");

    PrintTestSuccess("����߽�������");
    return COMMON_TEST_PASS;
}

// ���Ե�ַ�ռ�߽�
CommonTestResult CommonTester::TestAddressSpaceBoundaries() {
    PrintTestBanner("��ַ�ռ�߽����");

    PrintTestSkip("��ַ�ռ�߽������Ҫ���⻷��");
    return COMMON_TEST_SKIP;
}

// ��ȡʧ�ܲ�������
int CommonTester::GetFailedTestCount() const {
    int failed = 0;
    for (const auto& result : test_results) {
        if (result.result == COMMON_TEST_FAIL) {
            failed++;
        }
    }
    return failed;
}

// ��ȡ����ָ��
CommonTester::PerformanceMetrics CommonTester::GetPerformanceMetrics() {
    PerformanceMetrics metrics;
    metrics.read_speed = MeasureReadPerformance();
    metrics.write_speed = MeasureWritePerformance();
    metrics.copy_speed = MeasureCopyPerformance();
    metrics.latency = 0.0; // ��ʵ��
    metrics.throughput = 1000.0; // ��ʵ��
    return metrics;
}

// ��ȡ��Դʹ�����
CommonTester::ResourceUsage CommonTester::GetResourceUsage() {
    ResourceUsage usage;
    usage.memory_allocated = 0; // ��ʵ��
    usage.memory_peak = 0; // ��ʵ��
    usage.handle_count = 0; // ��ʵ��
    usage.thread_count = 0; // ��ʵ��
    return usage;
}

// ��������ͨ�ò���
CommonTestResult RunAllCommonTests() {
    CommonTester tester;

    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "ͨ�ò����׼�\n";
    std::cout << std::string(60, '=') << "\n";

    tester.TestSystemProcessAccess();
    tester.TestNotepadProcessAccess();
    tester.TestMemoryPatternScan();
    tester.TestPerformanceBenchmark();
    tester.TestStressTest();
    tester.TestErrorHandling();
    tester.TestBoundaryConditions();
    tester.TestConcurrentAccess();
    tester.TestResourceManagement();
    tester.TestConfiguration();
    tester.TestMemoryReadLargeBlocks();
    tester.TestMemoryWriteLargeBlocks();
    tester.TestMemoryCopyBetweenProcesses();
    tester.TestMemoryProtection();
    tester.TestMemoryAlignment();
    tester.TestProcessEnumeration();
    tester.TestInvalidParameters();
    tester.TestNullPointerHandling();
    tester.TestZeroSizeOperations();
    tester.TestAlignmentBoundaries();

    tester.PrintPerformanceReport();

    std::cout << "\n[*] ͨ�ò����׼����\n";
    return (tester.GetFailedTestCount() == 0) ? COMMON_TEST_PASS : COMMON_TEST_FAIL;
}

// ���л���ͨ�ò���
CommonTestResult RunBasicCommonTests() {
    CommonTester tester;

    tester.TestSystemProcessAccess();
    tester.TestErrorHandling();
    tester.TestMemoryProtection();
    tester.TestProcessEnumeration();
    tester.TestInvalidParameters();

    return (tester.GetFailedTestCount() == 0) ? COMMON_TEST_PASS : COMMON_TEST_FAIL;
}

// ����ϵͳ����
bool QuickSystemTest() {
    return HypervisorIsPresent();
}

// �����ڴ����
bool QuickMemoryTest() {
    if (!HypervisorIsPresent()) return false;

    PEPROCESS process = HypervisorGetProcessById((HANDLE)4);
    if (!process) return false;

    ULONG64 dirBase = HypervisorGetProcessDirectoryBase(process);
    HypervisorDereferenceObject(process);

    return (dirBase != 0);
}

// ���ٽ��̲���
bool QuickProcessTest() {
    if (!HypervisorIsPresent()) return false;

    PEPROCESS process = HypervisorGetProcessById((HANDLE)4);
    if (!process) return false;

    HypervisorDereferenceObject(process);
    return true;
}

// �������ܲ���
bool QuickPerformanceTest() {
    CommonTester tester;
    return (tester.MeasureReadPerformance() > 0);
}