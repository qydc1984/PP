/**
 * common_tests.h - ͨ�ò��Թ���
 *
 * ����hypervisor����������ʹ�õ�ͨ�ò��Թ���
 */

#pragma once

#include <windows.h>
#include <string>
#include <vector>

 // ͨ�ò��Խ��ö��
enum CommonTestResult {
    COMMON_TEST_PASS = 0,
    COMMON_TEST_FAIL = 1,
    COMMON_TEST_SKIP = 2
};

// ͨ�ò�����Ϣ�ṹ
struct CommonTestInfo {
    std::string name;
    std::string description;
    CommonTestResult result;
    std::string error_message;
};

// ͨ�ò�������
class CommonTester {
private:
    std::vector<CommonTestInfo> test_results;

public:
    CommonTester();
    ~CommonTester();

    // ͨ�ò��Ժ���
    CommonTestResult TestSystemProcessAccess();
    CommonTestResult TestNotepadProcessAccess();
    CommonTestResult TestMemoryPatternScan();
    CommonTestResult TestPerformanceBenchmark();
    CommonTestResult TestStressTest();
    CommonTestResult TestErrorHandling();
    CommonTestResult TestBoundaryConditions();
    CommonTestResult TestConcurrentAccess();
    CommonTestResult TestResourceManagement();
    CommonTestResult TestConfiguration();

    // ���ߺ���
    void PrintTestBanner(const std::string& test_name);
    void PrintTestStep(const std::string& step_name);
    void PrintTestSuccess(const std::string& message);
    void PrintTestFailure(const std::string& message);
    void PrintTestSkip(const std::string& message);

    // �ڴ���Ժ���
    CommonTestResult TestMemoryReadLargeBlocks();
    CommonTestResult TestMemoryWriteLargeBlocks();
    CommonTestResult TestMemoryCopyBetweenProcesses();
    CommonTestResult TestMemoryProtection();
    CommonTestResult TestMemoryAlignment();

    // ���̲��Ժ���
    CommonTestResult TestProcessEnumeration();
    CommonTestResult TestProcessCreation();
    CommonTestResult TestProcessTermination();
    CommonTestResult TestProcessMemoryMapping();

    // ���ܲ��Ժ���
    double MeasureReadPerformance();
    double MeasureWritePerformance();
    double MeasureCopyPerformance();
    void PrintPerformanceReport();

    // ѹ�����Ժ���
    CommonTestResult PerformStressTest(int iterations);
    CommonTestResult TestMultiThreadedAccess();
    CommonTestResult TestHighFrequencyOperations();

    // ���������
    CommonTestResult TestInvalidParameters();
    CommonTestResult TestNullPointerHandling();
    CommonTestResult TestBufferOverflows();
    CommonTestResult TestAccessViolations();

    // �߽���������
    CommonTestResult TestZeroSizeOperations();
    CommonTestResult TestMaximumSizeOperations();
    CommonTestResult TestAlignmentBoundaries();
    CommonTestResult TestAddressSpaceBoundaries();

    // �����Բ���
    CommonTestResult TestWindowsVersionCompatibility();
    CommonTestResult TestArchitectureCompatibility();
    CommonTestResult TestDriverVersionCompatibility();

    // ��ȫ�Բ���
    CommonTestResult TestMemoryProtectionMechanisms();
    CommonTestResult TestAccessControl();
    CommonTestResult TestPrivilegeEscalation();
    CommonTestResult TestAntiDebugging();

    // ��Դ�������
    CommonTestResult TestMemoryLeakDetection();
    CommonTestResult TestHandleLeakDetection();
    CommonTestResult TestResourceCleanup();
    CommonTestResult TestGarbageCollection();

    // ���ò���
    CommonTestResult TestConfigurationLoading();
    CommonTestResult TestConfigurationSaving();
    CommonTestResult TestConfigurationValidation();
    CommonTestResult TestConfigurationMigration();

    // ���ߺ���
    bool IsProcessRunning(const std::string& process_name);
    ULONG64 GetProcessBaseAddress(DWORD process_id);
    std::vector<DWORD> EnumerateProcessIds();
    bool ValidateMemoryAddress(ULONG64 address);
    bool CheckMemoryPermissions(ULONG64 address, SIZE_T size);

    // ���Ա��溯��
    void GenerateTestReport();
    void SaveTestReportToFile(const std::string& filename);
    void PrintDetailedTestResults();
    int GetFailedTestCount() const;

    // ���ܼ��
    struct PerformanceMetrics {
        double read_speed;      // MB/s
        double write_speed;     // MB/s
        double copy_speed;      // MB/s
        double latency;         // ΢��
        double throughput;      // ����/��
    };

    PerformanceMetrics GetPerformanceMetrics();

    // ��Դʹ��ͳ��
    struct ResourceUsage {
        SIZE_T memory_allocated;
        SIZE_T memory_peak;
        ULONG handle_count;
        ULONG thread_count;
    };

    ResourceUsage GetResourceUsage();
};

// ȫ��ͨ�ò��Ժ���
CommonTestResult RunAllCommonTests();
CommonTestResult RunBasicCommonTests();
CommonTestResult RunAdvancedCommonTests();
CommonTestResult RunPerformanceTests();
CommonTestResult RunStressTests();
CommonTestResult RunSecurityTests();

// ���ٲ��Ժ���
bool QuickSystemTest();
bool QuickMemoryTest();
bool QuickProcessTest();
bool QuickPerformanceTest();

// ר����Ժ���
CommonTestResult TestMemoryIntegrity();
CommonTestResult TestProcessIntegrity();
CommonTestResult TestSystemStability();
CommonTestResult TestRecoveryMechanisms();

// �Ƚϲ��Ժ���
CommonTestResult CompareHypervisorPerformance();
CommonTestResult CompareMemoryOperations();
CommonTestResult CompareProcessManagement();

// ��֤����
bool ValidateTestResults();
bool ValidatePerformanceMetrics();
bool ValidateResourceUsage();

// ��������
void InitializeCommonTests();
void CleanupCommonTests();
void ResetTestEnvironment();
void SetupTestEnvironment();

// ���ú���
bool LoadTestConfiguration();
bool SaveTestConfiguration();
void ResetTestConfiguration();

// ��־����
void LogTestStart(const std::string& test_name);
void LogTestEnd(const std::string& test_name, CommonTestResult result);
void LogTestStep(const std::string& step_name);
void LogTestError(const std::string& error_message);

// ���溯��
struct TestReport {
    std::string timestamp;
    std::string system_info;
    int total_tests;
    int passed_tests;
    int failed_tests;
    int skipped_tests;
    double execution_time;
    std::vector<CommonTestInfo> detailed_results;
    CommonTester::PerformanceMetrics performance;
    CommonTester::ResourceUsage resources;
};

TestReport GenerateDetailedReport();
void SaveReportToHtml(const TestReport& report, const std::string& filename);
void SaveReportToJson(const TestReport& report, const std::string& filename);
void PrintSummaryReport(const TestReport& report);

// �����Լ��
bool CheckSystemCompatibility();
bool CheckDriverCompatibility();
bool CheckHardwareCompatibility();

// �������
struct TestEnvironment {
    bool is_debug_build;
    bool is_administrator;
    bool has_sufficient_privileges;
    SIZE_T available_memory;
    ULONG processor_count;
    std::string windows_version;
};

TestEnvironment GetTestEnvironment();
bool ValidateTestEnvironment(const TestEnvironment& env);