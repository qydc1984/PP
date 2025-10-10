/**
 * common_tests.h - 通用测试功能
 *
 * 两个hypervisor驱动都可以使用的通用测试功能
 */

#pragma once

#include <windows.h>
#include <string>
#include <vector>

 // 通用测试结果枚举
enum CommonTestResult {
    COMMON_TEST_PASS = 0,
    COMMON_TEST_FAIL = 1,
    COMMON_TEST_SKIP = 2
};

// 通用测试信息结构
struct CommonTestInfo {
    std::string name;
    std::string description;
    CommonTestResult result;
    std::string error_message;
};

// 通用测试器类
class CommonTester {
private:
    std::vector<CommonTestInfo> test_results;

public:
    CommonTester();
    ~CommonTester();

    // 通用测试函数
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

    // 工具函数
    void PrintTestBanner(const std::string& test_name);
    void PrintTestStep(const std::string& step_name);
    void PrintTestSuccess(const std::string& message);
    void PrintTestFailure(const std::string& message);
    void PrintTestSkip(const std::string& message);

    // 内存测试函数
    CommonTestResult TestMemoryReadLargeBlocks();
    CommonTestResult TestMemoryWriteLargeBlocks();
    CommonTestResult TestMemoryCopyBetweenProcesses();
    CommonTestResult TestMemoryProtection();
    CommonTestResult TestMemoryAlignment();

    // 进程测试函数
    CommonTestResult TestProcessEnumeration();
    CommonTestResult TestProcessCreation();
    CommonTestResult TestProcessTermination();
    CommonTestResult TestProcessMemoryMapping();

    // 性能测试函数
    double MeasureReadPerformance();
    double MeasureWritePerformance();
    double MeasureCopyPerformance();
    void PrintPerformanceReport();

    // 压力测试函数
    CommonTestResult PerformStressTest(int iterations);
    CommonTestResult TestMultiThreadedAccess();
    CommonTestResult TestHighFrequencyOperations();

    // 错误处理测试
    CommonTestResult TestInvalidParameters();
    CommonTestResult TestNullPointerHandling();
    CommonTestResult TestBufferOverflows();
    CommonTestResult TestAccessViolations();

    // 边界条件测试
    CommonTestResult TestZeroSizeOperations();
    CommonTestResult TestMaximumSizeOperations();
    CommonTestResult TestAlignmentBoundaries();
    CommonTestResult TestAddressSpaceBoundaries();

    // 兼容性测试
    CommonTestResult TestWindowsVersionCompatibility();
    CommonTestResult TestArchitectureCompatibility();
    CommonTestResult TestDriverVersionCompatibility();

    // 安全性测试
    CommonTestResult TestMemoryProtectionMechanisms();
    CommonTestResult TestAccessControl();
    CommonTestResult TestPrivilegeEscalation();
    CommonTestResult TestAntiDebugging();

    // 资源管理测试
    CommonTestResult TestMemoryLeakDetection();
    CommonTestResult TestHandleLeakDetection();
    CommonTestResult TestResourceCleanup();
    CommonTestResult TestGarbageCollection();

    // 配置测试
    CommonTestResult TestConfigurationLoading();
    CommonTestResult TestConfigurationSaving();
    CommonTestResult TestConfigurationValidation();
    CommonTestResult TestConfigurationMigration();

    // 工具函数
    bool IsProcessRunning(const std::string& process_name);
    ULONG64 GetProcessBaseAddress(DWORD process_id);
    std::vector<DWORD> EnumerateProcessIds();
    bool ValidateMemoryAddress(ULONG64 address);
    bool CheckMemoryPermissions(ULONG64 address, SIZE_T size);

    // 测试报告函数
    void GenerateTestReport();
    void SaveTestReportToFile(const std::string& filename);
    void PrintDetailedTestResults();
    int GetFailedTestCount() const;

    // 性能监控
    struct PerformanceMetrics {
        double read_speed;      // MB/s
        double write_speed;     // MB/s
        double copy_speed;      // MB/s
        double latency;         // 微秒
        double throughput;      // 操作/秒
    };

    PerformanceMetrics GetPerformanceMetrics();

    // 资源使用统计
    struct ResourceUsage {
        SIZE_T memory_allocated;
        SIZE_T memory_peak;
        ULONG handle_count;
        ULONG thread_count;
    };

    ResourceUsage GetResourceUsage();
};

// 全局通用测试函数
CommonTestResult RunAllCommonTests();
CommonTestResult RunBasicCommonTests();
CommonTestResult RunAdvancedCommonTests();
CommonTestResult RunPerformanceTests();
CommonTestResult RunStressTests();
CommonTestResult RunSecurityTests();

// 快速测试函数
bool QuickSystemTest();
bool QuickMemoryTest();
bool QuickProcessTest();
bool QuickPerformanceTest();

// 专项测试函数
CommonTestResult TestMemoryIntegrity();
CommonTestResult TestProcessIntegrity();
CommonTestResult TestSystemStability();
CommonTestResult TestRecoveryMechanisms();

// 比较测试函数
CommonTestResult CompareHypervisorPerformance();
CommonTestResult CompareMemoryOperations();
CommonTestResult CompareProcessManagement();

// 验证函数
bool ValidateTestResults();
bool ValidatePerformanceMetrics();
bool ValidateResourceUsage();

// 辅助函数
void InitializeCommonTests();
void CleanupCommonTests();
void ResetTestEnvironment();
void SetupTestEnvironment();

// 配置函数
bool LoadTestConfiguration();
bool SaveTestConfiguration();
void ResetTestConfiguration();

// 日志函数
void LogTestStart(const std::string& test_name);
void LogTestEnd(const std::string& test_name, CommonTestResult result);
void LogTestStep(const std::string& step_name);
void LogTestError(const std::string& error_message);

// 报告函数
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

// 兼容性检查
bool CheckSystemCompatibility();
bool CheckDriverCompatibility();
bool CheckHardwareCompatibility();

// 环境检测
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