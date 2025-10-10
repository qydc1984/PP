/**
 * airhv_tester.h - airhv 驱动专用测试器
 *
 * 专门用于测试 airhv (Intel VT-x) 驱动的各项功能
 */

#pragma once

#include <windows.h>
#include <string>
#include <vector>

 // 包含 airhv 适配器接口
#include "../airhv/Source/airhv_adapter.h"

// 测试结果枚举
enum TestResult {
    TEST_PASS = 0,
    TEST_FAIL = 1,
    TEST_SKIP = 2
};

// 测试信息结构
struct AirhvTestInfo {
    std::string name;
    std::string description;
    TestResult result;
    std::string error_message;
};

// airhv 测试器类
class AirhvTester {
private:
    std::vector<AirhvTestInfo> test_results;

public:
    AirhvTester();
    ~AirhvTester();

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
    TestResult TestAdvancedFeatures();
    TestResult TestMsrInterception();
    TestResult TestIoInterception();
    TestResult TestInterruptInjection();
    TestResult TestEptHooks();
    TestResult TestGuestRegisters();
    TestResult TestVmCallInterface();
    TestResult TestCleanup();

    // 工具函数
    void PrintTestHeader();
    void PrintTestResult(const std::string& test_name, TestResult result, const std::string& message = "");
    void PrintTestSummary();

    // CPU 检测函数
    bool IsIntelProcessor();

    // 获取测试结果
    const std::vector<AirhvTestInfo>& GetTestResults() const { return test_results; }
    int GetFailedTestCount() const;
};

// 全局测试函数
int TestAirhvDriver();
bool TestAirhvLoad();
bool TestAirhvInitialization();
bool TestAirhvPresence();
bool TestAirhvProcessAccess();
bool TestAirhvMemoryRead();
bool TestAirhvMemoryWrite();
bool TestAirhvProtection();
bool TestAirhvCleanup();

// airhv 工具函数
bool IsIntelProcessor();
void PrintAirhvTestHeader();
void PrintAirhvTestResult(const char* test_name, bool result);

// 详细的 airhv 测试函数
TestResult PerformDetailedAirhvTests();

// 特定功能测试
TestResult TestAirhvVmCallInterface();
TestResult TestAirhvProcessChainTraversal();
TestResult TestAirhvLargeMemoryOperations();
TestResult TestAirhvConcurrentAccess();
TestResult TestAirhvErrorHandling();

// 高级功能测试
TestResult TestAirhvMsrInterception();
TestResult TestAirhvIoInterception();
TestResult TestAirhvInterruptInjection();
TestResult TestAirhvEptHooks();
TestResult TestAirhvGuestRegisters();

// 性能测试
TestResult TestAirhvPerformance();
double MeasureReadPerformance();
double MeasureWritePerformance();

// 兼容性测试
TestResult TestAirhvCompatibility();
bool CheckVmxSupport();
bool CheckEptSupport();

// 稳定性测试
TestResult TestAirhvStability();
bool PerformStressTest(int iteration_count);
bool TestResourceLeaks();

// 调试辅助函数
void EnableAirhvDebugOutput();
void DumpAirhvTestResults();
void SaveAirhvTestReport(const char* filename);

// 配置相关测试
TestResult TestAirhvConfiguration();
bool ValidateAirhvConfig();
bool TestConfigPersistence();

// 安全性测试
TestResult TestAirhvSecurity();
bool TestMemoryProtection();
bool TestAccessControl();

// 边界条件测试
TestResult TestAirhvBoundaryConditions();
bool TestNullPointers();
bool TestInvalidAddresses();
bool TestLargeOffsets();

// VMCALL 测试
TestResult TestAirhvVmCall(unsigned __int64 hypercall_number, void* context);

// 处理器相关测试
TestResult TestAirhvPerProcessorFeatures();
bool TestCurrentProcessorFeatures();

// 内存管理测试
TestResult TestAirhvMemoryManagement();
bool TestPageHooking();
bool TestPageUnhooking();