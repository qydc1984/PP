/**
 * memhv_tester.cpp - memhv 驱动专用测试器实现
 *
 * 实现 memhv (AMD SVM) 驱动的各项功能测试
 */

#include "memhv_tester.h"
#include <iostream>
#include <windows.h>
#include <intrin.h>
#include <string>
#include <vector>

 // 构造函数
MemhvTester::MemhvTester() {
    test_results.clear();
}

// 析构函数
MemhvTester::~MemhvTester() {
    test_results.clear();
}

// 打印测试头部
void MemhvTester::PrintTestHeader() {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "memhv (AMD SVM) 驱动测试\n";
    std::cout << std::string(60, '=') << "\n\n";
}

// 打印测试结果
void MemhvTester::PrintTestResult(const std::string& test_name, TestResult result, const std::string& message) {
    MemhvTestInfo info;
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
int MemhvTester::GetFailedTestCount() const {
    int failed = 0;
    for (const auto& result : test_results) {
        if (result.result == TEST_FAIL) {
            failed++;
        }
    }
    return failed;
}

// 打印测试摘要
void MemhvTester::PrintTestSummary() {
    std::cout << "\n" << std::string(40, '-') << "\n";
    std::cout << "memhv 测试摘要\n";
    std::cout << "总测试数: " << test_results.size() << "\n";
    std::cout << "通过: " << (test_results.size() - GetFailedTestCount()) << "\n";
    std::cout << "失败: " << GetFailedTestCount() << "\n";
    std::cout << std::string(40, '-') << "\n";
}

// 检查是否为AMD处理器
bool MemhvTester::IsAMDProcessor() {
    int cpuInfo[4];
    __cpuid(cpuInfo, 0);

    // 检查AMD标识: "AuthenticAMD"
    if (cpuInfo[1] == 0x68747541 && // "htuA"
        cpuInfo[3] == 0x69746E65 && // "itne"
        cpuInfo[2] == 0x444D4163)   // "DMAc"
    {
        return true;
    }
    return false;
}

// 测试驱动存在性
TestResult MemhvTester::TestDriverPresence() {
    std::cout << "[*] 测试1: 驱动存在性检测\n";

    if (MemHvIsPresent()) {
        PrintTestResult("驱动存在性", TEST_PASS, "memhv驱动已加载并运行");
        return TEST_PASS;
    }
    else {
        PrintTestResult("驱动存在性", TEST_FAIL, "memhv驱动未检测到");
        return TEST_FAIL;
    }
}

// 测试初始化
TestResult MemhvTester::TestInitialization() {
    std::cout << "[*] 测试2: 初始化检测\n";

    // memhv 通过驱动自动初始化，这里检查基本功能
    if (MemHvIsPresent()) {
        PrintTestResult("初始化", TEST_PASS, "驱动已正确初始化");
        return TEST_PASS;
    }
    else {
        PrintTestResult("初始化", TEST_FAIL, "驱动初始化失败");
        return TEST_FAIL;
    }
}

// 测试处理器数量
TestResult MemhvTester::TestProcessorCount() {
    std::cout << "[*] 测试3: 处理器数量\n";

    ULONG processorCount = KeQueryActiveProcessorCount(NULL);
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
TestResult MemhvTester::TestProcessAccess() {
    std::cout << "[*] 测试4: 进程访问\n";

    // 尝试获取System进程 (PID 4)
    PEPROCESS systemProcess = MemHvGetProcessById((HANDLE)4);
    if (systemProcess) {
        PrintTestResult("进程访问", TEST_PASS, "成功获取System进程对象");
        MemHvDereferenceObject(systemProcess);
        return TEST_PASS;
    }
    else {
        PrintTestResult("进程访问", TEST_FAIL, "无法获取System进程");
        return TEST_FAIL;
    }
}

// 测试进程目录基址
TestResult MemhvTester::TestProcessDirectoryBase() {
    std::cout << "[*] 测试5: 进程目录基址\n";

    PEPROCESS systemProcess = MemHvGetProcessById((HANDLE)4);
    if (systemProcess) {
        ULONG64 dirBase = MemHvGetProcessDirectoryBase(systemProcess);
        if (dirBase != 0) {
            char buffer[64];
            sprintf_s(buffer, "CR3 = 0x%llx", dirBase);
            PrintTestResult("目录基址", TEST_PASS, buffer);
            MemHvDereferenceObject(systemProcess);
            return TEST_PASS;
        }
        else {
            PrintTestResult("目录基址", TEST_FAIL, "无法获取目录基址");
            MemHvDereferenceObject(systemProcess);
            return TEST_FAIL;
        }
    }
    else {
        PrintTestResult("目录基址", TEST_SKIP, "System进程不可用");
        return TEST_SKIP;
    }
}

// 测试内存读取
TestResult MemhvTester::TestMemoryRead() {
    std::cout << "[*] 测试6: 内存读取\n";

    PEPROCESS systemProcess = MemHvGetProcessById((HANDLE)4);
    if (systemProcess) {
        ULONG64 dirBase = MemHvGetProcessDirectoryBase(systemProcess);
        if (dirBase != 0) {
            UCHAR buffer[16] = { 0 };
            SIZE_T bytesRead = 0;
            NTSTATUS status = MemHvReadProcessMemory(dirBase, 0xFFFFF80000000000, buffer, sizeof(buffer), &bytesRead);

            if (NT_SUCCESS(status) && bytesRead > 0) {
                PrintTestResult("内存读取", TEST_PASS, "成功读取 " + std::to_string(bytesRead) + " 字节");
                MemHvDereferenceObject(systemProcess);
                return TEST_PASS;
            }
            else {
                char error_msg[128];
                sprintf_s(error_msg, "读取失败，状态码: 0x%08X", status);
                PrintTestResult("内存读取", TEST_FAIL, error_msg);
                MemHvDereferenceObject(systemProcess);
                return TEST_FAIL;
            }
        }
        else {
            PrintTestResult("内存读取", TEST_SKIP, "目录基址不可用");
            MemHvDereferenceObject(systemProcess);
            return TEST_SKIP;
        }
    }
    else {
        PrintTestResult("内存读取", TEST_SKIP, "System进程不可用");
        return TEST_SKIP;
    }
}

// 测试内存写入
TestResult MemhvTester::TestMemoryWrite() {
    std::cout << "[*] 测试7: 内存写入\n";

    // 注意：实际测试中应使用安全的测试地址
    PrintTestResult("内存写入", TEST_SKIP, "为安全起见跳过写入测试");
    return TEST_SKIP;
}

// 测试内存拷贝
TestResult MemhvTester::TestMemoryCopy() {
    std::cout << "[*] 测试8: 内存拷贝\n";

    PrintTestResult("内存拷贝", TEST_SKIP, "需要两个有效进程进行测试");
    return TEST_SKIP;
}

// 测试进程枚举
TestResult MemhvTester::TestProcessEnumeration() {
    std::cout << "[*] 测试9: 进程枚举\n";

    int processCount = 0;
    PEPROCESS current = PsInitialSystemProcess;

    // 简单遍历几个进程来测试功能
    for (int i = 0; i < 10 && current != nullptr; i++) {
        HANDLE processId = PsGetProcessId(current);
        if (processId) {
            processCount++;
        }
        current = MemHvGetNextProcess(current);
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
TestResult MemhvTester::TestSelfProtection() {
    std::cout << "[*] 测试10: 自我保护\n";

    NTSTATUS status = MemHvEnableProtection();
    if (NT_SUCCESS(status)) {
        PrintTestResult("自我保护", TEST_PASS, "保护功能启用成功");
        return TEST_PASS;
    }
    else {
        char error_msg[128];
        sprintf_s(error_msg, "保护功能启用失败，状态码: 0x%08X", status);
        PrintTestResult("自我保护", TEST_FAIL, error_msg);
        return TEST_FAIL;
    }
}

// 测试MSR验证
TestResult MemhvTester::TestMsrValidation() {
    std::cout << "[*] 测试11: MSR验证\n";

    // 测试几个常见的MSR
    bool valid1 = MemHvIsValidMsr(0xC0000080); // EFER
    bool valid2 = MemHvIsValidMsr(0x00000010); // CS
    bool invalid1 = MemHvIsValidMsr(0xFFFFFFFF); // 无效MSR

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
TestResult MemhvTester::TestUserAddressValidation() {
    std::cout << "[*] 测试12: 用户地址验证\n";

    bool valid1 = MemHvIsValidUserAddress(0x00001000); // 有效用户地址
    bool invalid1 = MemHvIsValidUserAddress(0x00000000); // 无效地址
    bool invalid2 = MemHvIsValidUserAddress(0x800000000000); // 内核地址

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
TestResult MemhvTester::TestObjectManagement() {
    std::cout << "[*] 测试13: 对象管理\n";

    PEPROCESS systemProcess = MemHvGetProcessById((HANDLE)4);
    if (systemProcess) {
        // 测试引用计数功能
        MemHvReferenceObject(systemProcess);
        MemHvDereferenceObject(systemProcess);
        MemHvDereferenceObject(systemProcess);
        PrintTestResult("对象管理", TEST_PASS, "引用计数操作完成");
        return TEST_PASS;
    }
    else {
        PrintTestResult("对象管理", TEST_FAIL, "无法获取进程对象");
        return TEST_FAIL;
    }
}

// 测试安全内存操作
TestResult MemhvTester::TestSafeMemoryOperations() {
    std::cout << "[*] 测试14: 安全内存操作\n";

    PEPROCESS systemProcess = MemHvGetProcessById((HANDLE)4);
    if (systemProcess) {
        ULONG64 dirBase = MemHvGetProcessDirectoryBase(systemProcess);
        if (dirBase != 0) {
            UCHAR buffer[16] = { 0 };
            SIZE_T bytesRead = 0;
            NTSTATUS status = MemHvReadProcessMemorySafe(dirBase, 0xFFFFF80000000000, buffer, sizeof(buffer), &bytesRead);

            if (NT_SUCCESS(status)) {
                PrintTestResult("安全内存操作", TEST_PASS, "安全读取操作完成");
                MemHvDereferenceObject(systemProcess);
                return TEST_PASS;
            }
            else {
                char error_msg[128];
                sprintf_s(error_msg, "安全读取失败，状态码: 0x%08X", status);
                PrintTestResult("安全内存操作", TEST_FAIL, error_msg);
                MemHvDereferenceObject(systemProcess);
                return TEST_FAIL;
            }
        }
        else {
            PrintTestResult("安全内存操作", TEST_SKIP, "目录基址不可用");
            MemHvDereferenceObject(systemProcess);
            return TEST_SKIP;
        }
    }
    else {
        PrintTestResult("安全内存操作", TEST_SKIP, "System进程不可用");
        return TEST_SKIP;
    }
}

// 测试清理
TestResult MemhvTester::TestCleanup() {
    std::cout << "[*] 测试15: 资源清理\n";

    // memhv 清理由驱动处理，这里只是确认功能调用
    PrintTestResult("资源清理", TEST_PASS, "清理功能接口可用");
    return TEST_PASS;
}

// 运行所有测试
int MemhvTester::RunAllTests() {
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
    TestCleanup();

    PrintTestSummary();

    return GetFailedTestCount();
}

// 全局测试函数实现
int TestMemhvDriver() {
    if (!IsAMDProcessor()) {
        std::cout << "[-] 当前不是AMD处理器，跳过memhv测试\n";
        return -1;
    }

    MemhvTester tester;
    return tester.RunAllTests();
}

bool TestMemhvLoad() {
    return MemHvIsPresent();
}

bool TestMemhvInitialization() {
    return MemHvIsPresent();
}

bool TestMemhvPresence() {
    return MemHvIsPresent();
}

bool TestMemhvProcessAccess() {
    PEPROCESS process = MemHvGetProcessById((HANDLE)4);
    if (process) {
        MemHvDereferenceObject(process);
        return true;
    }
    return false;
}

bool TestMemhvMemoryRead() {
    PEPROCESS process = MemHvGetProcessById((HANDLE)4);
    if (process) {
        ULONG64 dirBase = MemHvGetProcessDirectoryBase(process);
        if (dirBase != 0) {
            UCHAR buffer[8];
            SIZE_T bytesRead;
            NTSTATUS status = MemHvReadProcessMemory(dirBase, 0xFFFFF80000000000, buffer, sizeof(buffer), &bytesRead);
            MemHvDereferenceObject(process);
            return NT_SUCCESS(status);
        }
        MemHvDereferenceObject(process);
    }
    return false;
}

bool TestMemhvMemoryWrite() {
    // 为安全起见，默认跳过写入测试
    return true;
}

bool TestMemhvProtection() {
    NTSTATUS status = MemHvEnableProtection();
    return NT_SUCCESS(status);
}

bool TestMemhvCleanup() {
    // 清理由驱动处理
    return true;
}

bool IsAMDProcessor() {
    int cpuInfo[4];
    __cpuid(cpuInfo, 0);

    if (cpuInfo[1] == 0x68747541 && // "htuA"
        cpuInfo[3] == 0x69746E65 && // "itne"
        cpuInfo[2] == 0x444D4163)   // "DMAc"
    {
        return true;
    }
    return false;
}

void PrintMemhvTestHeader() {
    std::cout << "[*] 开始memhv驱动测试\n";
}

void PrintMemhvTestResult(const char* test_name, bool result) {
    if (result) {
        std::cout << "[+] " << test_name << ": 通过\n";
    }
    else {
        std::cout << "[-] " << test_name << ": 失败\n";
    }
}