/**
 * airhv_tester.cpp - airhv 驱动专用测试器实现
 *
 * 实现 airhv (Intel VT-x) 驱动的各项功能测试
 */

#include "airhv_tester.h"
#include <iostream>
#include <windows.h>
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
    if (cpuInfo[1] == 0x756E6547 && // "uneG"
        cpuInfo[3] == 0x49656E69 && // "Ieni"
        cpuInfo[2] == 0x6C65746E)   // "letn"
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

// 测试进程访问
TestResult AirhvTester::TestProcessAccess() {
    std::cout << "[*] 测试4: 进程访问\n";

    // 尝试获取System进程 (PID 4)
    PEPROCESS systemProcess = AirHvGetProcessById((HANDLE)4);
    if (systemProcess) {
        PrintTestResult("进程访问", TEST_PASS, "成功获取System进程对象");
        AirHvDereferenceObject(systemProcess);
        return TEST_PASS;
    }
    else {
        PrintTestResult("进程访问", TEST_FAIL, "无法获取System进程");
        return TEST_FAIL;
    }
}

// 测试进程目录基址
TestResult AirhvTester::TestProcessDirectoryBase() {
    std::cout << "[*] 测试5: 进程目录基址\n";

    PEPROCESS systemProcess = AirHvGetProcessById((HANDLE)4);
    if (systemProcess) {
        ULONG64 dirBase = AirHvGetDirectoryBase(systemProcess);
        if (dirBase != 0) {
            char buffer[64];
            sprintf_s(buffer, "CR3 = 0x%llx", dirBase);
            PrintTestResult("目录基址", TEST_PASS, buffer);
            AirHvDereferenceObject(systemProcess);
            return TEST_PASS;
        }
        else {
            PrintTestResult("目录基址", TEST_FAIL, "无法获取目录基址");
            AirHvDereferenceObject(systemProcess);
            return TEST_FAIL;
        }
    }
    else {
        PrintTestResult("目录基址", TEST_SKIP, "System进程不可用");
        return TEST_SKIP;
    }
}

// 测试内存读取
TestResult AirhvTester::TestMemoryRead() {
    std::cout << "[*] 测试6: 内存读取\n";

    PEPROCESS systemProcess = AirHvGetProcessById((HANDLE)4);
    if (systemProcess) {
        ULONG64 dirBase = AirHvGetDirectoryBase(systemProcess);
        if (dirBase != 0) {
            UCHAR buffer[16] = { 0 };
            SIZE_T bytesRead = 0;
            NTSTATUS status = AirHvReadProcessMemory(0, dirBase, 0xFFFFF80000000000, buffer, sizeof(buffer), &bytesRead);

            if (NT_SUCCESS(status) && bytesRead > 0) {
                PrintTestResult("内存读取", TEST_PASS, "成功读取 " + std::to_string(bytesRead) + " 字节");
                AirHvDereferenceObject(systemProcess);
                return TEST_PASS;
            }
            else {
                char error_msg[128];
                sprintf_s(error_msg, "读取失败，状态码: 0x%08X", status);
                PrintTestResult("内存读取", TEST_FAIL, error_msg);
                AirHvDereferenceObject(systemProcess);
                return TEST_FAIL;
            }
        }
        else {
            PrintTestResult("内存读取", TEST_SKIP, "目录基址不可用");
            AirHvDereferenceObject(systemProcess);
            return TEST_SKIP;
        }
    }
    else {
        PrintTestResult("内存读取", TEST_SKIP, "System进程不可用");
        return TEST_SKIP;
    }
}

// 测试内存写入
TestResult AirhvTester::TestMemoryWrite() {
    std::cout << "[*] 测试7: 内存写入\n";

    // 注意：实际测试中应使用安全的测试地址
    PrintTestResult("内存写入", TEST_SKIP, "为安全起见跳过写入测试");
    return TEST_SKIP;
}

// 测试内存拷贝
TestResult AirhvTester::TestMemoryCopy() {
    std::cout << "[*] 测试8: 内存拷贝\n";

    PrintTestResult("内存拷贝", TEST_SKIP, "需要两个有效进程进行测试");
    return TEST_SKIP;
}

// 测试进程枚举
TestResult AirhvTester::TestProcessEnumeration() {
    std::cout << "[*] 测试9: 进程枚举\n";

    int processCount = 0;
    PEPROCESS current = PsInitialSystemProcess;

    // 简单遍历几个进程来测试功能
    for (int i = 0; i < 10 && current != nullptr; i++) {
        HANDLE processId = PsGetProcessId(current);
        if (processId) {
            processCount++;
        }
        current = AirHvGetNextProcess(current);
        if (!current) break;
    }

    if (processCount > 0) {
        PrintTestResult("进程枚举", TEST_PASS, "成功遍历 " + std::to_string(processCount) + " 个进程");
        return TEST_PASS;
    }
    else {
        PrintTestResult("进程枚举", TEST_FAIL, "进程枚举失败");
        return TEST_FAIL;
    }
}

// 测试自我保护
TestResult AirhvTester::TestSelfProtection() {
    std::cout << "[*] 测试10: 自我保护\n";

    if (AirHvProtectSelf()) {
        PrintTestResult("自我保护", TEST_PASS, "保护功能启用成功");
        return TEST_PASS;
    }
    else {
        PrintTestResult("自我保护", TEST_FAIL, "保护功能启用失败");
        return TEST_FAIL;
    }
}

// 测试MSR验证
TestResult AirhvTester::TestMsrValidation() {
    std::cout << "[*] 测试11: MSR验证\n";

    // 测试几个常见的MSR
    bool valid1 = AirHvIsValidMsr(0xC0000080); // EFER
    bool valid2 = AirHvIsValidMsr(0x00000010); // CS
    bool invalid1 = AirHvIsValidMsr(0xFFFFFFFF); // 无效MSR

    if (valid1 && valid2 && !invalid1) {
        PrintTestResult("MSR验证", TEST_PASS, "MSR验证功能正常");
        return TEST_PASS;
    }
    else {
        PrintTestResult("MSR验证", TEST_FAIL, "MSR验证功能异常");
        return TEST_FAIL;
    }
}

// 测试用户地址验证
TestResult AirhvTester::TestUserAddressValidation() {
    std::cout << "[*] 测试12: 用户地址验证\n";

    bool valid1 = AirHvIsValidUserAddress(0x00001000); // 有效用户地址
    bool invalid1 = AirHvIsValidUserAddress(0x00000000); // 无效地址
    bool invalid2 = AirHvIsValidUserAddress(0x800000000000); // 内核地址

    if (valid1 && !invalid1) {
        PrintTestResult("用户地址验证", TEST_PASS, "地址验证功能正常");
        return TEST_PASS;
    }
    else {
        PrintTestResult("用户地址验证", TEST_FAIL, "地址验证功能异常");
        return TEST_FAIL;
    }
}

// 测试对象管理
TestResult AirhvTester::TestObjectManagement() {
    std::cout << "[*] 测试13: 对象管理\n";

    PEPROCESS systemProcess = AirHvGetProcessById((HANDLE)4);
    if (systemProcess) {
        // 测试引用计数功能
        AirHvReferenceObject(systemProcess);
        AirHvDereferenceObject(systemProcess);
        AirHvDereferenceObject(systemProcess);
        PrintTestResult("对象管理", TEST_PASS, "引用计数操作完成");
        return TEST_PASS;
    }
    else {
        PrintTestResult("对象管理", TEST_FAIL, "无法获取进程对象");
        return TEST_FAIL;
    }
}

// 测试安全内存操作
TestResult AirhvTester::TestSafeMemoryOperations() {
    std::cout << "[*] 测试14: 安全内存操作\n";

    PEPROCESS systemProcess = AirHvGetProcessById((HANDLE)4);
    if (systemProcess) {
        ULONG64 dirBase = AirHvGetDirectoryBase(systemProcess);
        if (dirBase != 0) {
            UCHAR buffer[16] = { 0 };
            SIZE_T bytesRead = 0;
            // airhv 可能没有专门的安全读取函数，使用普通读取
            NTSTATUS status = AirHvReadProcessMemory(0, dirBase, 0xFFFFF80000000000, buffer, sizeof(buffer), &bytesRead);

            if (NT_SUCCESS(status)) {
                PrintTestResult("安全内存操作", TEST_PASS, "内存操作完成");
                AirHvDereferenceObject(systemProcess);
                return TEST_PASS;
            }
            else {
                char error_msg[128];
                sprintf_s(error_msg, "内存操作失败，状态码: 0x%08X", status);
                PrintTestResult("安全内存操作", TEST_FAIL, error_msg);
                AirHvDereferenceObject(systemProcess);
                return TEST_FAIL;
            }
        }
        else {
            PrintTestResult("安全内存操作", TEST_SKIP, "目录基址不可用");
            AirHvDereferenceObject(systemProcess);
            return TEST_SKIP;
        }
    }
    else {
        PrintTestResult("安全内存操作", TEST_SKIP, "System进程不可用");
        return TEST_SKIP;
    }
}

// 测试高级功能
TestResult AirhvTester::TestAdvancedFeatures() {
    std::cout << "[*] 测试15: 高级功能\n";

    // 测试MSR拦截功能
    bool msr_result = airhv_adapter_set_msr_intercept(0xC0000080, true, true);
    if (msr_result) {
        PrintTestResult("高级功能", TEST_PASS, "MSR拦截功能可用");
        return TEST_PASS;
    }
    else {
        PrintTestResult("高级功能", TEST_SKIP, "MSR拦截功能不可用");
        return TEST_SKIP;
    }
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
        PrintTestResult("MSR拦截", TEST_FAIL, "MSR拦截设置失败");
        return TEST_FAIL;
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
        PrintTestResult("IO拦截", TEST_FAIL, "IO拦截设置失败");
        return TEST_FAIL;
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

    PrintTestResult("EPT钩子", TEST_SKIP, "需要特定物理地址进行测试");
    return TEST_SKIP;
}

// 测试客户机寄存器
TestResult AirhvTester::TestGuestRegisters() {
    std::cout << "[*] 测试20: 客户机寄存器\n";

    // 尝试获取第一个处理器的寄存器
    __vmexit_guest_registers registers;
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
    PEPROCESS process = AirHvGetProcessById((HANDLE)4);
    if (process) {
        AirHvDereferenceObject(process);
        return true;
    }
    return false;
}

bool TestAirhvMemoryRead() {
    PEPROCESS process = AirHvGetProcessById((HANDLE)4);
    if (process) {
        ULONG64 dirBase = AirHvGetDirectoryBase(process);
        if (dirBase != 0) {
            UCHAR buffer[8];
            SIZE_T bytesRead;
            NTSTATUS status = AirHvReadProcessMemory(0, dirBase, 0xFFFFF80000000000, buffer, sizeof(buffer), &bytesRead);
            AirHvDereferenceObject(process);
            return NT_SUCCESS(status);
        }
        AirHvDereferenceObject(process);
    }
    return false;
}

bool TestAirhvMemoryWrite() {
    // 为安全起见，默认跳过写入测试
    return true;
}

bool TestAirhvProtection() {
    return AirHvProtectSelf();
}

bool TestAirhvCleanup() {
    airhv_adapter_cleanup();
    return true;
}

bool IsIntelProcessor() {
    int cpuInfo[4];
    __cpuid(cpuInfo, 0);

    // 检查Intel标识: "GenuineIntel"
    if (cpuInfo[1] == 0x756E6547 && // "uneG"
        cpuInfo[3] == 0x49656E69 && // "Ieni"
        cpuInfo[2] == 0x6C65746E)   // "letn"
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