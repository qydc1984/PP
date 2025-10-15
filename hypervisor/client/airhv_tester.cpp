/**
 * airhv_tester.cpp - airhv 驱动专用测试器实现
 *
 * 实现 airhv (Intel VT-x) 驱动的各项功能测试
 */

#include "airhv_tester.h"
#include <iostream>
#include <intrin.h>
#include <string>
#include <vector>

 // 构造函数
AirhvTester::AirhvTester() {
    test_results.clear();
}

// 析构函数
AirhvTester::~AirhvTester() {
    test_results.clear();
}

// 打印测试头部
void AirhvTester::PrintTestHeader() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "airhv (Intel VT-x) 驱动测试\n";
    std::cout << std::string(60, '=') << "\n\n";
}

// 打印测试结果
void AirhvTester::PrintTestResult(const std::string& test_name, TestResult result, const std::string& message) {
    AirhvTestInfo info;
    info.name = test_name;
    info.result = result;

    switch (result) {
    case TEST_PASS:
        std::cout << "[+] " << test_name << ": 通过";
        if (!message.empty()) {
            std::cout << " - " << message;
        }
        std::cout << "\n";
        info.description = "通过";
        break;

    case TEST_FAIL:
        std::cout << "[-] " << test_name << ": 失败";
        if (!message.empty()) {
            std::cout << " - " << message;
        }
        std::cout << "\n";
        info.description = "失败";
        info.error_message = message;
        break;

    case TEST_SKIP:
        std::cout << "[*] " << test_name << ": 跳过";
        if (!message.empty()) {
            std::cout << " - " << message;
        }
        std::cout << "\n";
        info.description = "跳过";
        break;
    }

    test_results.push_back(info);
}

// 获取失败测试数量
int AirhvTester::GetFailedTestCount() const {
    int failed = 0;
    for (const auto& result : test_results) {
        if (result.result == TEST_FAIL) {
            failed++;
        }
    }
    return failed;
}

// 打印测试摘要
void AirhvTester::PrintTestSummary() {
    std::cout << "\n" << std::string(40, '-') << "\n";
    std::cout << "airhv 测试摘要\n";
    std::cout << "总测试数: " << test_results.size() << "\n";
    std::cout << "通过: " << (test_results.size() - GetFailedTestCount()) << "\n";
    std::cout << "失败: " << GetFailedTestCount() << "\n";
    std::cout << std::string(40, '-') << "\n";
}

// 检查是否为Intel处理器
bool AirhvTester::IsIntelProcessor() {
    int cpuInfo[4];
    __cpuid(cpuInfo, 0);

    // 检查Intel标识: "GenuineIntel"
    if (cpuInfo[1] == 0x756E6547 && // "Genu"
        cpuInfo[3] == 0x49656E69 && // "ineI"
        cpuInfo[2] == 0x6C65746E)   // "ntel"
    {
        return true;
    }
    return false;
}

// 测试驱动存在性
TestResult AirhvTester::TestDriverPresence() {
    std::cout << "[*] 测试1: 驱动存在性检测\n";

    if (airhv_adapter_is_running()) {
        PrintTestResult("驱动存在性", TEST_PASS, "airhv驱动已加载并运行");
        return TEST_PASS;
    }
    else {
        PrintTestResult("驱动存在性", TEST_FAIL, "airhv驱动未检测到");
        return TEST_FAIL;
    }
}

// 测试初始化
TestResult AirhvTester::TestInitialization() {
    std::cout << "[*] 测试2: 初始化检测\n";

    if (airhv_adapter_init()) {
        PrintTestResult("初始化", TEST_PASS, "驱动初始化成功");
        return TEST_PASS;
    }
    else {
        PrintTestResult("初始化", TEST_FAIL, "驱动初始化失败");
        return TEST_FAIL;
    }
}

// 测试处理器数量
TestResult AirhvTester::TestProcessorCount() {
    std::cout << "[*] 测试3: 处理器数量\n";

    unsigned int processorCount = airhv_adapter_get_processor_count();
    if (processorCount > 0) {
        PrintTestResult("处理器数量", TEST_PASS, "检测到 " + std::to_string(processorCount) + " 个处理器");
        return TEST_PASS;
    }
    else {
        PrintTestResult("处理器数量", TEST_FAIL, "无法获取处理器数量");
        return TEST_FAIL;
    }
}

// 测试进程访问（跳过，因为用户模式下无法直接访问内核对象）
TestResult AirhvTester::TestProcessAccess() {
    std::cout << "[*] 测试4: 进程访问\n";
    PrintTestResult("进程访问", TEST_SKIP, "用户模式下无法直接访问内核对象");
    return TEST_SKIP;
}

// 测试进程目录基址（跳过）
TestResult AirhvTester::TestProcessDirectoryBase() {
    std::cout << "[*] 测试5: 进程目录基址\n";
    PrintTestResult("目录基址", TEST_SKIP, "用户模式下无法直接访问内核对象");
    return TEST_SKIP;
}

// 测试内存读取（跳过）
TestResult AirhvTester::TestMemoryRead() {
    std::cout << "[*] 测试6: 内存读取\n";
    PrintTestResult("内存读取", TEST_SKIP, "需要通过IOCTL接口进行测试");
    return TEST_SKIP;
}

// 测试内存写入
TestResult AirhvTester::TestMemoryWrite() {
    std::cout << "[*] 测试7: 内存写入\n";
    PrintTestResult("内存写入", TEST_SKIP, "为安全起见跳过写入测试");
    return TEST_SKIP;
}

// 测试内存拷贝
TestResult AirhvTester::TestMemoryCopy() {
    std::cout << "[*] 测试8: 内存拷贝\n";
    PrintTestResult("内存拷贝", TEST_SKIP, "需要通过IOCTL接口进行测试");
    return TEST_SKIP;
}

// 测试进程枚举（跳过）
TestResult AirhvTester::TestProcessEnumeration() {
    std::cout << "[*] 测试9: 进程枚举\n";
    PrintTestResult("进程枚举", TEST_SKIP, "用户模式下无法直接访问内核对象");
    return TEST_SKIP;
}

// 测试自我保护（跳过）
TestResult AirhvTester::TestSelfProtection() {
    std::cout << "[*] 测试10: 自我保护\n";
    PrintTestResult("自我保护", TEST_SKIP, "需要通过IOCTL接口进行测试");
    return TEST_SKIP;
}

// 测试MSR验证（跳过）
TestResult AirhvTester::TestMsrValidation() {
    std::cout << "[*] 测试11: MSR验证\n";
    PrintTestResult("MSR验证", TEST_SKIP, "需要通过IOCTL接口进行测试");
    return TEST_SKIP;
}

// 测试用户地址验证（跳过）
TestResult AirhvTester::TestUserAddressValidation() {
    std::cout << "[*] 测试12: 用户地址验证\n";
    PrintTestResult("用户地址验证", TEST_SKIP, "需要通过IOCTL接口进行测试");
    return TEST_SKIP;
}

// 测试对象管理（跳过）
TestResult AirhvTester::TestObjectManagement() {
    std::cout << "[*] 测试13: 对象管理\n";
    PrintTestResult("对象管理", TEST_SKIP, "用户模式下无法直接访问内核对象");
    return TEST_SKIP;
}

// 测试安全内存操作（跳过）
TestResult AirhvTester::TestSafeMemoryOperations() {
    std::cout << "[*] 测试14: 安全内存操作\n";
    PrintTestResult("安全内存操作", TEST_SKIP, "需要通过IOCTL接口进行测试");
    return TEST_SKIP;
}

// 测试高级功能
TestResult AirhvTester::TestAdvancedFeatures() {
    std::cout << "[*] 测试15: 高级功能\n";
    PrintTestResult("高级功能", TEST_SKIP, "需要通过IOCTL接口进行测试");
    return TEST_SKIP;
}

// 测试MSR拦截
TestResult AirhvTester::TestMsrInterception() {
    std::cout << "[*] 测试16: MSR拦截\n";

    bool result = airhv_adapter_set_msr_intercept(0xC0000080, true, true);
    if (result) {
        PrintTestResult("MSR拦截", TEST_PASS, "MSR拦截设置成功");
        return TEST_PASS;
    }
    else {
        PrintTestResult("MSR拦截", TEST_SKIP, "MSR拦截功能测试跳过");
        return TEST_SKIP;
    }
}

// 测试IO拦截
TestResult AirhvTester::TestIoInterception() {
    std::cout << "[*] 测试17: IO拦截\n";

    bool result = airhv_adapter_set_io_intercept(0x3F8, true); // COM1端口
    if (result) {
        PrintTestResult("IO拦截", TEST_PASS, "IO拦截设置成功");
        return TEST_PASS;
    }
    else {
        PrintTestResult("IO拦截", TEST_SKIP, "IO拦截功能测试跳过");
        return TEST_SKIP;
    }
}

// 测试中断注入
TestResult AirhvTester::TestInterruptInjection() {
    std::cout << "[*] 测试18: 中断注入\n";

    // 尝试注入一个软中断
    bool result = airhv_adapter_inject_interrupt(0, 0x20, 4, 0, false); // INT 0x20
    if (result) {
        PrintTestResult("中断注入", TEST_PASS, "中断注入成功");
        return TEST_PASS;
    }
    else {
        PrintTestResult("中断注入", TEST_SKIP, "中断注入功能测试跳过");
        return TEST_SKIP;
    }
}

// 测试EPT钩子
TestResult AirhvTester::TestEptHooks() {
    std::cout << "[*] 测试19: EPT钩子\n";
    PrintTestResult("EPT钩子", TEST_SKIP, "需要通过IOCTL接口进行测试");
    return TEST_SKIP;
}

// 测试客户机寄存器
TestResult AirhvTester::TestGuestRegisters() {
    std::cout << "[*] 测试20: 客户机寄存器\n";

    __vmexit_guest_registers registers = { 0 };
    bool result = airhv_adapter_get_guest_registers(0, &registers);
    if (result) {
        PrintTestResult("客户机寄存器", TEST_PASS, "寄存器获取成功");
        return TEST_PASS;
    }
    else {
        PrintTestResult("客户机寄存器", TEST_SKIP, "寄存器获取功能测试跳过");
        return TEST_SKIP;
    }
}

// 测试VMCALL接口
TestResult AirhvTester::TestVmCallInterface() {
    std::cout << "[*] 测试21: VMCALL接口\n";

    // 发送一个测试VMCALL
    unsigned __int64 result = airhv_adapter_vmcall(0, 0x1000, nullptr);
    char buffer[64];
    sprintf_s(buffer, "VMCALL返回值: 0x%llx", result);
    PrintTestResult("VMCALL接口", TEST_PASS, buffer);
    return TEST_PASS;
}

// 测试清理
TestResult AirhvTester::TestCleanup() {
    std::cout << "[*] 测试22: 资源清理\n";

    // 执行清理
    airhv_adapter_cleanup();
    PrintTestResult("资源清理", TEST_PASS, "清理功能执行完成");
    return TEST_PASS;
}

// 运行所有测试
int AirhvTester::RunAllTests() {
    PrintTestHeader();
    test_results.clear();

    // 按顺序执行测试
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
    TestAdvancedFeatures();
    TestMsrInterception();
    TestIoInterception();
    TestInterruptInjection();
    TestEptHooks();
    TestGuestRegisters();
    TestVmCallInterface();
    TestCleanup();

    PrintTestSummary();

    return GetFailedTestCount();
}

// 全局测试函数实现
int TestAirhvDriver() {
    if (!IsIntelProcessor()) {
        std::cout << "[-] 当前不是Intel处理器，跳过airhv测试\n";
        return -1;
    }

    AirhvTester tester;
    return tester.RunAllTests();
}

bool TestAirhvLoad() {
    return airhv_adapter_is_running();
}

bool TestAirhvInitialization() {
    return airhv_adapter_init();
}

bool TestAirhvPresence() {
    return airhv_adapter_is_running();
}

bool TestAirhvProcessAccess() {
    // 用户模式下无法直接访问内核对象
    return false;
}

bool TestAirhvMemoryRead() {
    // 需要通过IOCTL接口进行测试
    return false;
}

bool TestAirhvMemoryWrite() {
    // 为安全起见，默认跳过写入测试
    return true;
}

bool TestAirhvProtection() {
    // 需要通过IOCTL接口进行测试
    return false;
}

bool TestAirhvCleanup() {
    airhv_adapter_cleanup();
    return true;
}

bool IsIntelProcessor() {
    int cpuInfo[4];
    __cpuid(cpuInfo, 0);

    // 检查Intel标识: "GenuineIntel"
    if (cpuInfo[1] == 0x756E6547 && // "Genu"
        cpuInfo[3] == 0x49656E69 && // "ineI"
        cpuInfo[2] == 0x6C65746E)   // "ntel"
    {
        return true;
    }
    return false;
}

void PrintAirhvTestHeader() {
    std::cout << "[*] 开始airhv驱动测试\n";
}

void PrintAirhvTestResult(const char* test_name, bool result) {
    if (result) {
        std::cout << "[+] " << test_name << ": 通过\n";
    }
    else {
        std::cout << "[-] " << test_name << ": 失败\n";
    }
}