#pragma once

#include <windows.h>
#include <string>
#include <vector>

// 测试结果枚举
typedef enum _TestResult {
    TEST_PASS,
    TEST_FAIL,
    TEST_SKIP
} TestResult;

// 测试信息结构
typedef struct _AirhvTestInfo {
    std::string name;
    TestResult result;
    std::string description;
    std::string error_message;
} AirhvTestInfo;

// 客户机寄存器结构体定义
typedef struct _vmexit_guest_registers {
    unsigned __int64 rax;
    unsigned __int64 rbx;
    unsigned __int64 rcx;
    unsigned __int64 rdx;
    unsigned __int64 rsi;
    unsigned __int64 rdi;
    unsigned __int64 r8;
    unsigned __int64 r9;
    unsigned __int64 r10;
    unsigned __int64 r11;
    unsigned __int64 r12;
    unsigned __int64 r13;
    unsigned __int64 r14;
    unsigned __int64 r15;
    unsigned __int64 rip;
    unsigned __int64 rflags;
} __vmexit_guest_registers;

// airhv适配器函数声明
extern "C" {
    BOOLEAN airhv_adapter_is_running(void);
    BOOLEAN airhv_adapter_init(void);
    unsigned int airhv_adapter_get_processor_count(void);
    BOOLEAN airhv_adapter_set_msr_intercept(unsigned int msr, BOOLEAN read_intercept, BOOLEAN write_intercept);
    BOOLEAN airhv_adapter_set_io_intercept(unsigned short port, BOOLEAN intercept);
    BOOLEAN airhv_adapter_inject_interrupt(unsigned char processor, unsigned char vector, unsigned char type, unsigned int error_code, BOOLEAN has_error_code);
    BOOLEAN airhv_adapter_get_guest_registers(unsigned char processor, __vmexit_guest_registers* registers);
    unsigned __int64 airhv_adapter_vmcall(unsigned char processor, unsigned __int64 hypercall_number, void* context);
    void airhv_adapter_cleanup(void);
}

// 测试器类
class AirhvTester {
private:
    std::vector<AirhvTestInfo> test_results;

public:
    AirhvTester();
    ~AirhvTester();

    // 测试管理函数
    void PrintTestHeader();
    void PrintTestResult(const std::string& test_name, TestResult result, const std::string& message = "");
    int GetFailedTestCount() const;
    void PrintTestSummary();
    bool IsIntelProcessor();

    // 各项测试函数
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

    // 运行所有测试
    int RunAllTests();
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
bool IsIntelProcessor();
void PrintAirhvTestHeader();
void PrintAirhvTestResult(const char* test_name, bool result);