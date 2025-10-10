/**
 * airhv_tester.h - airhv ����ר�ò�����
 *
 * ר�����ڲ��� airhv (Intel VT-x) �����ĸ����
 */

#pragma once

#include <windows.h>
#include <string>
#include <vector>

 // ���� airhv �������ӿ�
#include "../airhv/Source/airhv_adapter.h"

// ���Խ��ö��
enum TestResult {
    TEST_PASS = 0,
    TEST_FAIL = 1,
    TEST_SKIP = 2
};

// ������Ϣ�ṹ
struct AirhvTestInfo {
    std::string name;
    std::string description;
    TestResult result;
    std::string error_message;
};

// airhv ��������
class AirhvTester {
private:
    std::vector<AirhvTestInfo> test_results;

public:
    AirhvTester();
    ~AirhvTester();

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
    TestResult TestAdvancedFeatures();
    TestResult TestMsrInterception();
    TestResult TestIoInterception();
    TestResult TestInterruptInjection();
    TestResult TestEptHooks();
    TestResult TestGuestRegisters();
    TestResult TestVmCallInterface();
    TestResult TestCleanup();

    // ���ߺ���
    void PrintTestHeader();
    void PrintTestResult(const std::string& test_name, TestResult result, const std::string& message = "");
    void PrintTestSummary();

    // CPU ��⺯��
    bool IsIntelProcessor();

    // ��ȡ���Խ��
    const std::vector<AirhvTestInfo>& GetTestResults() const { return test_results; }
    int GetFailedTestCount() const;
};

// ȫ�ֲ��Ժ���
int TestAirhvDriver();
bool TestAirhvLoad();
bool TestAirhvInitialization();
bool TestAirhvPresence();
bool TestAirhvProcessAccess();
bool TestAirhvMemoryRead();
bool TestAirhvMemoryWrite();
bool TestAirhvProtection();
bool TestAirhvCleanup();

// airhv ���ߺ���
bool IsIntelProcessor();
void PrintAirhvTestHeader();
void PrintAirhvTestResult(const char* test_name, bool result);

// ��ϸ�� airhv ���Ժ���
TestResult PerformDetailedAirhvTests();

// �ض����ܲ���
TestResult TestAirhvVmCallInterface();
TestResult TestAirhvProcessChainTraversal();
TestResult TestAirhvLargeMemoryOperations();
TestResult TestAirhvConcurrentAccess();
TestResult TestAirhvErrorHandling();

// �߼����ܲ���
TestResult TestAirhvMsrInterception();
TestResult TestAirhvIoInterception();
TestResult TestAirhvInterruptInjection();
TestResult TestAirhvEptHooks();
TestResult TestAirhvGuestRegisters();

// ���ܲ���
TestResult TestAirhvPerformance();
double MeasureReadPerformance();
double MeasureWritePerformance();

// �����Բ���
TestResult TestAirhvCompatibility();
bool CheckVmxSupport();
bool CheckEptSupport();

// �ȶ��Բ���
TestResult TestAirhvStability();
bool PerformStressTest(int iteration_count);
bool TestResourceLeaks();

// ���Ը�������
void EnableAirhvDebugOutput();
void DumpAirhvTestResults();
void SaveAirhvTestReport(const char* filename);

// ������ز���
TestResult TestAirhvConfiguration();
bool ValidateAirhvConfig();
bool TestConfigPersistence();

// ��ȫ�Բ���
TestResult TestAirhvSecurity();
bool TestMemoryProtection();
bool TestAccessControl();

// �߽���������
TestResult TestAirhvBoundaryConditions();
bool TestNullPointers();
bool TestInvalidAddresses();
bool TestLargeOffsets();

// VMCALL ����
TestResult TestAirhvVmCall(unsigned __int64 hypercall_number, void* context);

// ��������ز���
TestResult TestAirhvPerProcessorFeatures();
bool TestCurrentProcessorFeatures();

// �ڴ�������
TestResult TestAirhvMemoryManagement();
bool TestPageHooking();
bool TestPageUnhooking();