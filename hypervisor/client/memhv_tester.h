/**
 * memhv_tester.h - memhv 驱动专用测试器
 *
 * 专门用于测试 memhv (AMD SVM) 驱动的各项功能
 */

#pragma once

#include <windows.h>
#include <string>
#include <vector>

 // 包含 memhv 适配器接口
#include "../memhv/Source/memhv_adapter.h"

// 测试结果枚举
enum TestResult {
    TEST_PASS = 0,
    TEST_FAIL = 1,
    TEST_SKIP = 2
};

// 测试信息结构
struct MemhvTestInfo {
    std::string name;
    std::string description;
    TestResult result;
    std::string error_message;
};

// memhv 测试器类
class MemhvTester {
private:
    std::vector<MemhvTestInfo> test_results;

public:
    MemhvTester();
    ~MemhvTester();

    // 主测试函数
    int RunAllTests();

    // 单独测试函数
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

    // 工具函数
    void PrintTestHeader();
    void PrintTestResult(const std::string& test_name, TestResult result, const std::string& message = "");
    void PrintTestSummary();

    // CPU 检测函数
    bool IsAMDProcessor();

    // 获取测试结果
    const std::vector<MemhvTestInfo>& GetTestResults() const { return test_results; }
    int GetFailedTestCount() const;
};

// 全局测试函数
int TestMemhvDriver();
bool TestMemhvLoad();
bool TestMemhvInitialization();
bool TestMemhvPresence();
bool TestMemhvProcessAccess();
bool TestMemhvMemoryRead();
bool TestMemhvMemoryWrite();
bool TestMemhvProtection();
bool TestMemhvCleanup();

// memhv 工具函数
bool IsAMDProcessor();
void PrintMemhvTestHeader();
void PrintMemhvTestResult(const char* test_name, bool result);

// 详细的 memhv 测试函数
TestResult PerformDetailedMemhvTests();

// 特定功能测试
TestResult TestMemhvVmCallInterface();
TestResult TestMemhvProcessChainTraversal();
TestResult TestMemhvLargeMemoryOperations();
TestResult TestMemhvConcurrentAccess();
TestResult TestMemhvErrorHandling();

// 性能测试
TestResult TestMemhvPerformance();
double MeasureReadPerformance();
double MeasureWritePerformance();

// 兼容性测试
TestResult TestMemhvCompatibility();
bool CheckSvmSupport();
bool CheckNestedPagingSupport();

// 稳定性测试
TestResult TestMemhvStability();
bool PerformStressTest(int iteration_count);
bool TestResourceLeaks();

// 调试辅助函数
void EnableMemhvDebugOutput();
void DumpMemhvTestResults();
void SaveMemhvTestReport(const char* filename);

// 配置相关测试
TestResult TestMemhvConfiguration();
bool ValidateMemhvConfig();
bool TestConfigPersistence();

// 安全性测试
TestResult TestMemhvSecurity();
bool TestMemoryProtection();
bool TestAccessControl();

// 边界条件测试
TestResult TestMemhvBoundaryConditions();
bool TestNullPointers();
bool TestInvalidAddresses();
bool TestLargeOffsets();