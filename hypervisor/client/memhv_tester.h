/**
 * memhv_tester.h - memhv ����ר�ò�����
 *
 * ר�����ڲ��� memhv (AMD SVM) �����ĸ����
 */

#pragma once

#include <windows.h>
#include <string>
#include <vector>

 // ���� memhv �������ӿ�
#include "../memhv/Source/memhv_adapter.h"

// ���Խ��ö��
enum TestResult {
    TEST_PASS = 0,
    TEST_FAIL = 1,
    TEST_SKIP = 2
};

// ������Ϣ�ṹ
struct MemhvTestInfo {
    std::string name;
    std::string description;
    TestResult result;
    std::string error_message;
};

// memhv ��������
class MemhvTester {
private:
    std::vector<MemhvTestInfo> test_results;

public:
    MemhvTester();
    ~MemhvTester();

    // �����Ժ���
    int RunAllTests();

    // �������Ժ���
    TestResult TestDriverPresence();
    TestResult TestInitialization();
    TestResult TestProcessorCount();
    TestResult TestProcessAccess();
    TestResult TestProcessDirectoryBase();
    TestResult TestMemoryRead();
    TestResult TestMemoryWrite();
    TestResult TestMemoryCopy();
    TestResult TestProcessEnumeration();
    TestResult TestSelfProtection();
    TestResult TestMsrValidation();
    TestResult TestUserAddressValidation();
    TestResult TestObjectManagement();
    TestResult TestSafeMemoryOperations();
    TestResult TestCleanup();

    // ���ߺ���
    void PrintTestHeader();
    void PrintTestResult(const std::string& test_name, TestResult result, const std::string& message = "");
    void PrintTestSummary();

    // CPU ��⺯��
    bool IsAMDProcessor();

    // ��ȡ���Խ��
    const std::vector<MemhvTestInfo>& GetTestResults() const { return test_results; }
    int GetFailedTestCount() const;
};

// ȫ�ֲ��Ժ���
int TestMemhvDriver();
bool TestMemhvLoad();
bool TestMemhvInitialization();
bool TestMemhvPresence();
bool TestMemhvProcessAccess();
bool TestMemhvMemoryRead();
bool TestMemhvMemoryWrite();
bool TestMemhvProtection();
bool TestMemhvCleanup();

// memhv ���ߺ���
bool IsAMDProcessor();
void PrintMemhvTestHeader();
void PrintMemhvTestResult(const char* test_name, bool result);

// ��ϸ�� memhv ���Ժ���
TestResult PerformDetailedMemhvTests();

// �ض����ܲ���
TestResult TestMemhvVmCallInterface();
TestResult TestMemhvProcessChainTraversal();
TestResult TestMemhvLargeMemoryOperations();
TestResult TestMemhvConcurrentAccess();
TestResult TestMemhvErrorHandling();

// ���ܲ���
TestResult TestMemhvPerformance();
double MeasureReadPerformance();
double MeasureWritePerformance();

// �����Բ���
TestResult TestMemhvCompatibility();
bool CheckSvmSupport();
bool CheckNestedPagingSupport();

// �ȶ��Բ���
TestResult TestMemhvStability();
bool PerformStressTest(int iteration_count);
bool TestResourceLeaks();

// ���Ը�������
void EnableMemhvDebugOutput();
void DumpMemhvTestResults();
void SaveMemhvTestReport(const char* filename);

// ������ز���
TestResult TestMemhvConfiguration();
bool ValidateMemhvConfig();
bool TestConfigPersistence();

// ��ȫ�Բ���
TestResult TestMemhvSecurity();
bool TestMemoryProtection();
bool TestAccessControl();

// �߽���������
TestResult TestMemhvBoundaryConditions();
bool TestNullPointers();
bool TestInvalidAddresses();
bool TestLargeOffsets();