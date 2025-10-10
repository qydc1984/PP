/**
 * test_client.cpp - Hypervisor 测试客户端
 *
 * 用于分别测试 memhv 和 airhv 驱动的功能
 */

#include <iostream>
#include <windows.h>
#include <string>
#include <vector>

 // 包含我们的统一接口
#include "../common/hypervisor_interface.h"
#include "../common/hypervisor_factory.h"
#include "../common/hypervisor_utils.h"
#include "../common/hypervisor_config.h"

// 包含适配器接口
#include "../memhv/Source/memhv_adapter.h"
#include "../airhv/Source/airhv_adapter.h"

enum TestTarget {
    TARGET_AUTO = 0,
    TARGET_MEMHV = 1,
    TARGET_AIRHV = 2,
    TARGET_BOTH = 3
};

enum TestResult {
    TEST_PASS = 0,
    TEST_FAIL = 1,
    TEST_SKIP = 2
};

struct TestInfo {
    std::string name;
    std::string description;
    TestResult result;
    std::string error_message;
};

class HypervisorTester {
private:
    std::vector<TestInfo> test_results;
    HYPERVISOR_TYPE current_hypervisor;

public:
    HypervisorTester() : current_hypervisor(HYPERVISOR_TYPE_UNKNOWN) {}

    void PrintUsage() {
        std::cout << "Hypervisor 测试客户端\n";
        std::cout << "用法: test_client.exe [选项]\n";
        std::cout << "选项:\n";
        std::cout << "  --memhv     : 仅测试memhv驱动 (AMD SVM)\n";
        std::cout << "  --airhv     : 仅测试airhv驱动 (Intel VT-x)\n";
        std::cout << "  --both      : 依次测试两个驱动\n";
        std::cout << "  --auto      : 自动检测并测试 (默认)\n";
        std::cout << "  --help      : 显示帮助\n";
    }

    bool IsAMDProcessor() {
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

    void PrintTestHeader(const std::string& driver_name) {
        std::cout << "\n" << std::string(50, '=') << "\n";
        std::cout << "测试 " << driver_name << " 驱动\n";
        std::cout << std::string(50, '=') << "\n\n";
    }

    void PrintTestResult(const std::string& test_name, TestResult result, const std::string& message = "") {
        TestInfo info;
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

    TestResult TestDriverLoad(HYPERVISOR_TYPE type) {
        std::cout << "[*] 测试1: 驱动加载\n";

        NTSTATUS status = HypervisorInterfaceInitialize(type);
        if (NT_SUCCESS(status)) {
            PrintTestResult("驱动加载", TEST_PASS);
            current_hypervisor = type;
            return TEST_PASS;
        }
        else {
            PrintTestResult("驱动加载", TEST_FAIL, "初始化失败，状态码: 0x" + std::to_string(status));
            return TEST_FAIL;
        }
    }

    TestResult TestHypervisorPresence() {
        std::cout << "[*] 测试2: Hypervisor存在性检测\n";

        if (HypervisorIsPresent()) {
            PrintTestResult("存在性检测", TEST_PASS, "Hypervisor正在运行");
            return TEST_PASS;
        }
        else {
            PrintTestResult("存在性检测", TEST_FAIL, "Hypervisor未运行");
            return TEST_FAIL;
        }
    }

    TestResult TestProcessorCount() {
        std::cout << "[*] 测试3: 处理器数量检测\n";

        ULONG count = HypervisorGetProcessorCount();
        if (count > 0) {
            PrintTestResult("处理器检测", TEST_PASS, "检测到 " + std::to_string(count) + " 个处理器");
            return TEST_PASS;
        }
        else {
            PrintTestResult("处理器检测", TEST_FAIL, "无法获取处理器数量");
            return TEST_FAIL;
        }
    }

    TestResult TestProcessEnumeration() {
        std::cout << "[*] 测试4: 进程枚举\n";

        // 尝试获取System进程 (PID 4)
        PEPROCESS systemProcess = HypervisorGetProcessById((HANDLE)4);
        if (systemProcess) {
            PrintTestResult("进程枚举", TEST_PASS, "成功获取System进程");
            HypervisorDereferenceObject(systemProcess);
            return TEST_PASS;
        }
        else {
            PrintTestResult("进程枚举", TEST_FAIL, "无法获取System进程");
            return TEST_FAIL;
        }
    }

    TestResult TestProcessDirectoryBase() {
        std::cout << "[*] 测试5: 进程目录基址获取\n";

        PEPROCESS systemProcess = HypervisorGetProcessById((HANDLE)4);
        if (systemProcess) {
            ULONG64 dirBase = HypervisorGetProcessDirectoryBase(systemProcess);
            if (dirBase != 0) {
                PrintTestResult("目录基址获取", TEST_PASS, "System进程CR3: 0x" + std::to_string(dirBase));
                HypervisorDereferenceObject(systemProcess);
                return TEST_PASS;
            }
            else {
                PrintTestResult("目录基址获取", TEST_FAIL, "无法获取目录基址");
                HypervisorDereferenceObject(systemProcess);
                return TEST_FAIL;
            }
        }
        else {
            PrintTestResult("目录基址获取", TEST_SKIP, "System进程不可用");
            return TEST_SKIP;
        }
    }

    TestResult TestMemoryRead() {
        std::cout << "[*] 测试6: 内存读取\n";

        PEPROCESS systemProcess = HypervisorGetProcessById((HANDLE)4);
        if (systemProcess) {
            ULONG64 dirBase = HypervisorGetProcessDirectoryBase(systemProcess);
            if (dirBase != 0) {
                // 尝试读取一个已知的内核地址 (示例)
                UCHAR buffer[8] = { 0 };
                SIZE_T bytesRead = 0;
                NTSTATUS status = HypervisorReadProcessMemory(dirBase, 0xFFFFF80000000000, buffer, sizeof(buffer), &bytesRead);

                if (NT_SUCCESS(status)) {
                    PrintTestResult("内存读取", TEST_PASS, "成功读取 " + std::to_string(bytesRead) + " 字节");
                    HypervisorDereferenceObject(systemProcess);
                    return TEST_PASS;
                }
                else {
                    PrintTestResult("内存读取", TEST_FAIL, "读取失败，状态码: 0x" + std::to_string(status));
                    HypervisorDereferenceObject(systemProcess);
                    return TEST_FAIL;
                }
            }
            else {
                PrintTestResult("内存读取", TEST_SKIP, "目录基址不可用");
                HypervisorDereferenceObject(systemProcess);
                return TEST_SKIP;
            }
        }
        else {
            PrintTestResult("内存读取", TEST_SKIP, "System进程不可用");
            return TEST_SKIP;
        }
    }

    TestResult TestSelfProtection() {
        std::cout << "[*] 测试7: 自我保护功能\n";

        NTSTATUS status = HypervisorProtectSelf();
        if (NT_SUCCESS(status)) {
            PrintTestResult("自我保护", TEST_PASS, "保护功能启用成功");
            return TEST_PASS;
        }
        else {
            PrintTestResult("自我保护", TEST_FAIL, "保护功能启用失败，状态码: 0x" + std::to_string(status));
            return TEST_FAIL;
        }
    }

    TestResult TestCleanup() {
        std::cout << "[*] 测试8: 清理资源\n";

        try {
            HypervisorInterfaceCleanup();
            PrintTestResult("资源清理", TEST_PASS, "资源清理完成");
            return TEST_PASS;
        }
        catch (...) {
            PrintTestResult("资源清理", TEST_FAIL, "清理过程中发生异常");
            return TEST_FAIL;
        }
    }

    int RunTestsForHypervisor(HYPERVISOR_TYPE type) {
        std::string driver_name;
        switch (type) {
        case HYPERVISOR_TYPE_MEMHV:
            driver_name = "memhv (AMD SVM)";
            break;
        case HYPERVISOR_TYPE_AIRHV:
            driver_name = "airhv (Intel VT-x)";
            break;
        default:
            driver_name = "未知";
            break;
        }

        PrintTestHeader(driver_name);
        test_results.clear();

        // 执行所有测试
        int failed_tests = 0;

        if (TestDriverLoad(type) == TEST_FAIL) {
            std::cout << "[-] 驱动加载失败，跳过后续测试\n";
            failed_tests++;
        }
        else {
            if (TestHypervisorPresence() == TEST_FAIL) failed_tests++;
            if (TestProcessorCount() == TEST_FAIL) failed_tests++;
            if (TestProcessEnumeration() == TEST_FAIL) failed_tests++;
            if (TestProcessDirectoryBase() == TEST_FAIL) failed_tests++;
            if (TestMemoryRead() == TEST_FAIL) failed_tests++;
            if (TestSelfProtection() == TEST_FAIL) failed_tests++;
            if (TestCleanup() == TEST_FAIL) failed_tests++;
        }

        // 输出测试摘要
        std::cout << "\n" << std::string(30, '-') << "\n";
        std::cout << "测试摘要: " << driver_name << "\n";
        std::cout << "总测试数: " << test_results.size() << "\n";
        std::cout << "通过: " << (test_results.size() - failed_tests) << "\n";
        std::cout << "失败: " << failed_tests << "\n";
        std::cout << std::string(30, '-') << "\n";

        return failed_tests;
    }

    int TestMemhv() {
        return RunTestsForHypervisor(HYPERVISOR_TYPE_MEMHV);
    }

    int TestAirhv() {
        return RunTestsForHypervisor(HYPERVISOR_TYPE_AIRHV);
    }

    int TestAutoDetect() {
        if (IsAMDProcessor()) {
            std::cout << "[*] 检测到AMD处理器\n";
            return TestMemhv();
        }
        else if (IsIntelProcessor()) {
            std::cout << "[*] 检测到Intel处理器\n";
            return TestAirhv();
        }
        else {
            std::cout << "[-] 未识别的处理器类型\n";
            return -1;
        }
    }
};

int main(int argc, char* argv[]) {
    HypervisorTester tester;
    TestTarget target = TARGET_AUTO;

    // 解析命令行参数
    if (argc > 1) {
        if (strcmp(argv[1], "--memhv") == 0) {
            target = TARGET_MEMHV;
        }
        else if (strcmp(argv[1], "--airhv") == 0) {
            target = TARGET_AIRHV;
        }
        else if (strcmp(argv[1], "--both") == 0) {
            target = TARGET_BOTH;
        }
        else if (strcmp(argv[1], "--auto") == 0) {
            target = TARGET_AUTO;
        }
        else if (strcmp(argv[1], "--help") == 0) {
            tester.PrintUsage();
            return 0;
        }
        else {
            tester.PrintUsage();
            return -1;
        }
    }

    std::cout << "[*] Hypervisor 测试客户端\n";

    int result = 0;
    switch (target) {
    case TARGET_MEMHV:
        result = tester.TestMemhv();
        break;

    case TARGET_AIRHV:
        result = tester.TestAirhv();
        break;

    case TARGET_BOTH:
    {
        int memhv_result = tester.TestMemhv();
        std::cout << "\n" << std::string(60, '=') << "\n\n";
        int airhv_result = tester.TestAirhv();
        result = (memhv_result == 0 && airhv_result == 0) ? 0 : -1;
    }
    break;

    case TARGET_AUTO:
    default:
        result = tester.TestAutoDetect();
        break;
    }

    std::cout << "\n[*] 测试完成\n";
    return result;
}