/**
 * test_report.cpp - 测试报告生成实现
 *
 * 实现hypervisor测试的详细报告生成功能
 */

#include "test_report.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <filesystem>

 // 静态全局报告生成器实例
static TestReportGenerator g_GlobalReportGenerator;
static bool g_GlobalReportInitialized = false;

// 构造函数
TestReportGenerator::TestReportGenerator() : report_generated(false) {
    ResetReport();
}

// 析构函数
TestReportGenerator::~TestReportGenerator() {
    // 清理资源
}

// 初始化报告
void TestReportGenerator::InitializeReport() {
    ResetReport();
    report_generated = false;

    // 收集系统信息
    CollectSystemInfo();

    // 设置开始时间
    report.timestamp_start = GetCurrentTimestamp();
}

// 完成报告
void TestReportGenerator::FinalizeReport() {
    // 设置结束时间
    report.timestamp_end = GetCurrentTimestamp();

    // 计算总执行时间
    // 简化处理，实际应该计算时间差
    report.total_execution_time = 0.0;

    // 更新统计信息
    UpdateStatistics();
    CalculateSuccessRate();

    // 生成建议
    GenerateRecommendations();

    report_generated = true;
}

// 检查报告是否已生成
bool TestReportGenerator::IsReportGenerated() const {
    return report_generated;
}

// 重置报告
void TestReportGenerator::ResetReport() {
    report = TestReport();
    report_generated = false;
}

// 添加测试项
void TestReportGenerator::AddTestItem(const TestItem& item) {
    report.test_items.push_back(item);
}

// 更新测试项
void TestReportGenerator::UpdateTestItem(const std::string& name, const TestItem& updated_item) {
    for (auto& item : report.test_items) {
        if (item.name == name) {
            item = updated_item;
            break;
        }
    }
}

// 查找测试项
TestItem* TestReportGenerator::FindTestItem(const std::string& name) {
    for (auto& item : report.test_items) {
        if (item.name == name) {
            return &item;
        }
    }
    return nullptr;
}

// 设置系统信息
void TestReportGenerator::SetSystemInfo(const SystemInfo& info) {
    report.system_info = info;
}

// 收集系统信息
void TestReportGenerator::CollectSystemInfo() {
    SystemInfo info;

    // 获取操作系统版本（简化处理）
    info.os_version = "Windows 10/11";
    info.architecture = "x64";
    info.processor_count = std::thread::hardware_concurrency();
    info.total_memory = 0; // 简化处理
    info.available_memory = 0; // 简化处理
    info.hypervisor_type = "Unknown";
    info.driver_version = "1.0.0";
    info.test_environment = "Development";

    SetSystemInfo(info);
}

// 设置性能指标
void TestReportGenerator::SetPerformanceMetrics(const PerformanceMetrics& metrics) {
    report.performance = metrics;
}

// 收集性能指标
void TestReportGenerator::CollectPerformanceMetrics() {
    PerformanceMetrics metrics = { 0 };
    // 简化处理，实际应该收集真实数据
    SetPerformanceMetrics(metrics);
}

// 设置资源使用
void TestReportGenerator::SetResourceUsage(const ResourceUsage& usage) {
    report.resources = usage;
}

// 收集资源使用
void TestReportGenerator::CollectResourceUsage() {
    ResourceUsage usage = { 0 };
    // 简化处理，实际应该收集真实数据
    SetResourceUsage(usage);
}

// 更新统计信息
void TestReportGenerator::UpdateStatistics() {
    report.total_tests = static_cast<int>(report.test_items.size());
    report.passed_tests = 0;
    report.failed_tests = 0;
    report.skipped_tests = 0;
    report.error_tests = 0;

    // 重置类别统计
    for (int i = 0; i <= CATEGORY_ERROR_HANDLING; i++) {
        report.category_stats[static_cast<TestCategory>(i)] = 0;
    }

    // 统计各项数据
    for (const auto& item : report.test_items) {
        switch (item.status) {
        case TEST_STATUS_PASS:
            report.passed_tests++;
            break;
        case TEST_STATUS_FAIL:
            report.failed_tests++;
            break;
        case TEST_STATUS_SKIP:
            report.skipped_tests++;
            break;
        case TEST_STATUS_ERROR:
            report.error_tests++;
            break;
        }

        // 更新类别统计
        report.category_stats[item.category]++;
    }
}

// 计算成功率
void TestReportGenerator::CalculateSuccessRate() {
    if (report.total_tests > 0) {
        report.success_rate = (static_cast<double>(report.passed_tests) / report.total_tests) * 100.0;
    }
    else {
        report.success_rate = 0.0;
    }
}

// 获取报告（常量版本）
const TestReport& TestReportGenerator::GetReport() const {
    return report;
}

// 获取报告（非常量版本）
TestReport& TestReportGenerator::GetReport() {
    return report;
}

// 保存报告到文件
bool TestReportGenerator::SaveReportToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    // 写入基本报告信息
    file << "Hypervisor 测试报告\n";
    file << "==================\n\n";

    file << "报告ID: " << report.report_id << "\n";
    file << "开始时间: " << report.timestamp_start << "\n";
    file << "结束时间: " << report.timestamp_end << "\n";
    file << "总执行时间: " << report.total_execution_time << " 秒\n\n";

    // 写入系统信息
    file << "系统信息:\n";
    file << "  操作系统: " << report.system_info.os_version << "\n";
    file << "  架构: " << report.system_info.architecture << "\n";
    file << "  处理器数量: " << report.system_info.processor_count << "\n";
    file << "  Hypervisor类型: " << report.system_info.hypervisor_type << "\n";
    file << "  驱动版本: " << report.system_info.driver_version << "\n\n";

    // 写入测试统计
    file << "测试统计:\n";
    file << "  总测试数: " << report.total_tests << "\n";
    file << "  通过: " << report.passed_tests << "\n";
    file << "  失败: " << report.failed_tests << "\n";
    file << "  跳过: " << report.skipped_tests << "\n";
    file << "  错误: " << report.error_tests << "\n";
    file << "  成功率: " << std::fixed << std::setprecision(2) << report.success_rate << "%\n\n";

    // 写入详细测试结果
    file << "详细测试结果:\n";
    for (const auto& item : report.test_items) {
        file << "  " << item.name << ": " << GetTestStatusString(item.status);
        if (!item.error_message.empty()) {
            file << " - " << item.error_message;
        }
        file << "\n";
    }

    file.close();
    return true;
}

// 保存报告到HTML
bool TestReportGenerator::SaveReportToHtml(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file << "<!DOCTYPE html>\n";
    file << "<html>\n<head>\n";
    file << "<title>Hypervisor 测试报告</title>\n";
    file << "<style>\n";
    file << "body { font-family: Arial, sans-serif; margin: 20px; }\n";
    file << "h1, h2 { color: #333; }\n";
    file << "table { border-collapse: collapse; width: 100%; }\n";
    file << "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n";
    file << "th { background-color: #f2f2f2; }\n";
    file << ".pass { color: green; }\n";
    file << ".fail { color: red; }\n";
    file << ".skip { color: orange; }\n";
    file << ".error { color: purple; }\n";
    file << "</style>\n";
    file << "</head>\n<body>\n";

    file << "<h1>Hypervisor 测试报告</h1>\n";

    // 基本信息
    file << "<h2>基本信息</h2>\n";
    file << "<p><strong>报告ID:</strong> " << report.report_id << "</p>\n";
    file << "<p><strong>开始时间:</strong> " << report.timestamp_start << "</p>\n";
    file << "<p><strong>结束时间:</strong> " << report.timestamp_end << "</p>\n";
    file << "<p><strong>总执行时间:</strong> " << report.total_execution_time << " 秒</p>\n";

    // 系统信息
    file << "<h2>系统信息</h2>\n";
    file << "<ul>\n";
    file << "<li><strong>操作系统:</strong> " << report.system_info.os_version << "</li>\n";
    file << "<li><strong>架构:</strong> " << report.system_info.architecture << "</li>\n";
    file << "<li><strong>处理器数量:</strong> " << report.system_info.processor_count << "</li>\n";
    file << "<li><strong>Hypervisor类型:</strong> " << report.system_info.hypervisor_type << "</li>\n";
    file << "<li><strong>驱动版本:</strong> " << report.system_info.driver_version << "</li>\n";
    file << "</ul>\n";

    // 测试统计
    file << "<h2>测试统计</h2>\n";
    file << "<table>\n";
    file << "<tr><th>项目</th><th>数量</th></tr>\n";
    file << "<tr><td>总测试数</td><td>" << report.total_tests << "</td></tr>\n";
    file << "<tr><td>通过</td><td class='pass'>" << report.passed_tests << "</td></tr>\n";
    file << "<tr><td>失败</td><td class='fail'>" << report.failed_tests << "</td></tr>\n";
    file << "<tr><td>跳过</td><td class='skip'>" << report.skipped_tests << "</td></tr>\n";
    file << "<tr><td>错误</td><td class='error'>" << report.error_tests << "</td></tr>\n";
    file << "<tr><td>成功率</td><td>" << std::fixed << std::setprecision(2) << report.success_rate << "%</td></tr>\n";
    file << "</table>\n";

    // 详细测试结果
    file << "<h2>详细测试结果</h2>\n";
    file << "<table>\n";
    file << "<tr><th>测试名称</th><th>状态</th><th>错误信息</th></tr>\n";

    for (const auto& item : report.test_items) {
        std::string status_class;
        switch (item.status) {
        case TEST_STATUS_PASS: status_class = "pass"; break;
        case TEST_STATUS_FAIL: status_class = "fail"; break;
        case TEST_STATUS_SKIP: status_class = "skip"; break;
        case TEST_STATUS_ERROR: status_class = "error"; break;
        default: status_class = "";
        }

        file << "<tr>\n";
        file << "<td>" << item.name << "</td>\n";
        file << "<td class='" << status_class << "'>" << GetTestStatusString(item.status) << "</td>\n";
        file << "<td>" << (item.error_message.empty() ? "-" : item.error_message) << "</td>\n";
        file << "</tr>\n";
    }

    file << "</table>\n";
    file << "</body>\n</html>\n";

    file.close();
    return true;
}

// 保存报告到JSON
bool TestReportGenerator::SaveReportToJson(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file << "{\n";
    file << "  \"report_id\": \"" << report.report_id << "\",\n";
    file << "  \"timestamp_start\": \"" << report.timestamp_start << "\",\n";
    file << "  \"timestamp_end\": \"" << report.timestamp_end << "\",\n";
    file << "  \"total_execution_time\": " << report.total_execution_time << ",\n";

    // 系统信息
    file << "  \"system_info\": {\n";
    file << "    \"os_version\": \"" << report.system_info.os_version << "\",\n";
    file << "    \"architecture\": \"" << report.system_info.architecture << "\",\n";
    file << "    \"processor_count\": " << report.system_info.processor_count << ",\n";
    file << "    \"hypervisor_type\": \"" << report.system_info.hypervisor_type << "\",\n";
    file << "    \"driver_version\": \"" << report.system_info.driver_version << "\"\n";
    file << "  },\n";

    // 测试统计
    file << "  \"statistics\": {\n";
    file << "    \"total_tests\": " << report.total_tests << ",\n";
    file << "    \"passed_tests\": " << report.passed_tests << ",\n";
    file << "    \"failed_tests\": " << report.failed_tests << ",\n";
    file << "    \"skipped_tests\": " << report.skipped_tests << ",\n";
    file << "    \"error_tests\": " << report.error_tests << ",\n";
    file << "    \"success_rate\": " << std::fixed << std::setprecision(2) << report.success_rate << "\n";
    file << "  },\n";

    // 测试项
    file << "  \"test_items\": [\n";
    for (size_t i = 0; i < report.test_items.size(); i++) {
        const auto& item = report.test_items[i];
        file << "    {\n";
        file << "      \"name\": \"" << item.name << "\",\n";
        file << "      \"status\": " << item.status << ",\n";
        file << "      \"error_message\": \"" << item.error_message << "\"\n";
        file << "    }" << (i < report.test_items.size() - 1 ? "," : "") << "\n";
    }
    file << "  ]\n";

    file << "}\n";

    file.close();
    return true;
}

// 保存报告到XML
bool TestReportGenerator::SaveReportToXml(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    file << "<test_report>\n";
    file << "  <report_id>" << report.report_id << "</report_id>\n";
    file << "  <timestamp_start>" << report.timestamp_start << "</timestamp_start>\n";
    file << "  <timestamp_end>" << report.timestamp_end << "</timestamp_end>\n";
    file << "  <total_execution_time>" << report.total_execution_time << "</total_execution_time>\n";

    // 系统信息
    file << "  <system_info>\n";
    file << "    <os_version>" << report.system_info.os_version << "</os_version>\n";
    file << "    <architecture>" << report.system_info.architecture << "</architecture>\n";
    file << "    <processor_count>" << report.system_info.processor_count << "</processor_count>\n";
    file << "    <hypervisor_type>" << report.system_info.hypervisor_type << "</hypervisor_type>\n";
    file << "    <driver_version>" << report.system_info.driver_version << "</driver_version>\n";
    file << "  </system_info>\n";

    // 测试统计
    file << "  <statistics>\n";
    file << "    <total_tests>" << report.total_tests << "</total_tests>\n";
    file << "    <passed_tests>" << report.passed_tests << "</passed_tests>\n";
    file << "    <failed_tests>" << report.failed_tests << "</failed_tests>\n";
    file << "    <skipped_tests>" << report.skipped_tests << "</skipped_tests>\n";
    file << "    <error_tests>" << report.error_tests << "</error_tests>\n";
    file << "    <success_rate>" << std::fixed << std::setprecision(2) << report.success_rate << "</success_rate>\n";
    file << "  </statistics>\n";

    // 测试项
    file << "  <test_items>\n";
    for (const auto& item : report.test_items) {
        file << "    <test_item>\n";
        file << "      <name>" << item.name << "</name>\n";
        file << "      <status>" << item.status << "</status>\n";
        file << "      <error_message>" << item.error_message << "</error_message>\n";
        file << "    </test_item>\n";
    }
    file << "  </test_items>\n";

    file << "</test_report>\n";

    file.close();
    return true;
}

// 保存报告到CSV
bool TestReportGenerator::SaveReportToCsv(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    // CSV头部
    file << "Test Name,Status,Error Message\n";

    // 测试项数据
    for (const auto& item : report.test_items) {
        file << "\"" << item.name << "\",";
        file << "\"" << GetTestStatusString(item.status) << "\",";
        file << "\"" << item.error_message << "\"\n";
    }

    file.close();
    return true;
}

// 打印控制台报告
void TestReportGenerator::PrintConsoleReport() {
    std::cout << "\n=== Hypervisor 测试报告 ===\n";
    std::cout << "报告ID: " << report.report_id << "\n";
    std::cout << "开始时间: " << report.timestamp_start << "\n";
    std::cout << "结束时间: " << report.timestamp_end << "\n";
    std::cout << "总执行时间: " << report.total_execution_time << " 秒\n";

    std::cout << "\n系统信息:\n";
    std::cout << "  操作系统: " << report.system_info.os_version << "\n";
    std::cout << "  架构: " << report.system_info.architecture << "\n";
    std::cout << "  处理器数量: " << report.system_info.processor_count << "\n";
    std::cout << "  Hypervisor类型: " << report.system_info.hypervisor_type << "\n";

    std::cout << "\n测试统计:\n";
    std::cout << "  总测试数: " << report.total_tests << "\n";
    std::cout << "  通过: " << report.passed_tests << "\n";
    std::cout << "  失败: " << report.failed_tests << "\n";
    std::cout << "  跳过: " << report.skipped_tests << "\n";
    std::cout << "  错误: " << report.error_tests << "\n";
    std::cout << "  成功率: " << std::fixed << std::setprecision(2) << report.success_rate << "%\n";

    std::cout << "\n详细测试结果:\n";
    for (const auto& item : report.test_items) {
        std::cout << "  " << item.name << ": " << GetTestStatusString(item.status);
        if (!item.error_message.empty()) {
            std::cout << " - " << item.error_message;
        }
        std::cout << "\n";
    }
}

// 打印详细报告
void TestReportGenerator::PrintDetailedReport() {
    PrintConsoleReport();
}

// 打印摘要报告
void TestReportGenerator::PrintSummaryReport() {
    std::cout << "\n=== 测试摘要 ===\n";
    std::cout << "总测试数: " << report.total_tests << "\n";
    std::cout << "通过: " << report.passed_tests << "\n";
    std::cout << "失败: " << report.failed_tests << "\n";
    std::cout << "成功率: " << std::fixed << std::setprecision(2) << report.success_rate << "%\n";
}

// 打印失败测试报告
void TestReportGenerator::PrintFailedTestsReport() {
    std::vector<TestItem> failedTests = GetFailedTests();

    if (failedTests.empty()) {
        std::cout << "\n没有失败的测试。\n";
        return;
    }

    std::cout << "\n=== 失败的测试 ===\n";
    for (const auto& item : failedTests) {
        std::cout << "  " << item.name << ": " << item.error_message << "\n";
    }
}

// 打印性能报告
void TestReportGenerator::PrintPerformanceReport() {
    std::cout << "\n=== 性能报告 ===\n";
    std::cout << "读取速度: " << report.performance.read_speed_mbps << " MB/s\n";
    std::cout << "写入速度: " << report.performance.write_speed_mbps << " MB/s\n";
    std::cout << "拷贝速度: " << report.performance.copy_speed_mbps << " MB/s\n";
    std::cout << "平均延迟: " << report.performance.average_latency_us << " 微秒\n";
}

// 获取失败测试
std::vector<TestItem> TestReportGenerator::GetFailedTests() const {
    return FilterByStatus(report.test_items, TEST_STATUS_FAIL);
}

// 获取通过测试
std::vector<TestItem> TestReportGenerator::GetPassedTests() const {
    return FilterByStatus(report.test_items, TEST_STATUS_PASS);
}

// 获取跳过测试
std::vector<TestItem> TestReportGenerator::GetSkippedTests() const {
    return FilterByStatus(report.test_items, TEST_STATUS_SKIP);
}

// 根据类别获取测试
std::vector<TestItem> TestReportGenerator::GetTestsByCategory(TestCategory category) const {
    return ReportFilter::FilterByCategory(report.test_items, category);
}

// 比较报告
bool TestReportGenerator::CompareReports(const TestReport& report1, const TestReport& report2) {
    return (report1.success_rate == report2.success_rate) &&
        (report1.total_tests == report2.total_tests) &&
        (report1.passed_tests == report2.passed_tests);
}

// 计算改进
double TestReportGenerator::CalculateImprovement(const TestReport& old_report, const TestReport& new_report) {
    if (old_report.total_tests == 0) return 0.0;
    return new_report.success_rate - old_report.success_rate;
}

// 验证报告
bool TestReportGenerator::ValidateReport() const {
    // 基本验证
    return !report.report_id.empty() && report.total_tests >= 0;
}

// 获取验证错误
std::vector<std::string> TestReportGenerator::GetValidationErrors() const {
    std::vector<std::string> errors;

    if (report.report_id.empty()) {
        errors.push_back("报告ID为空");
    }

    if (report.total_tests < 0) {
        errors.push_back("总测试数为负数");
    }

    return errors;
}

// 获取测试状态字符串
std::string TestReportGenerator::GetTestStatusString(TestStatus status) const {
    switch (status) {
    case TEST_STATUS_PASS: return "通过";
    case TEST_STATUS_FAIL: return "失败";
    case TEST_STATUS_SKIP: return "跳过";
    case TEST_STATUS_ERROR: return "错误";
    default: return "未知";
    }
}

// 获取测试类别字符串
std::string TestReportGenerator::GetTestCategoryString(TestCategory category) const {
    switch (category) {
    case CATEGORY_INITIALIZATION: return "初始化";
    case CATEGORY_PROCESS_MANAGEMENT: return "进程管理";
    case CATEGORY_MEMORY_OPERATIONS: return "内存操作";
    case CATEGORY_ADVANCED_FEATURES: return "高级功能";
    case CATEGORY_PERFORMANCE: return "性能";
    case CATEGORY_SECURITY: return "安全";
    case CATEGORY_COMPATIBILITY: return "兼容性";
    case CATEGORY_STRESS: return "压力测试";
    case CATEGORY_ERROR_HANDLING: return "错误处理";
    default: return "未知";
    }
}

// 格式化时间戳
std::string TestReportGenerator::FormatTimestamp(const std::chrono::high_resolution_clock::time_point& time) const {
    // 简化处理
    return "2024-01-01 00:00:00";
}

// 生成报告ID
std::string TestReportGenerator::GenerateReportId() const {
    // 简化处理，实际应该生成唯一ID
    return "REPORT_" + std::to_string(std::time(nullptr));
}

// 生成建议
void TestReportGenerator::GenerateRecommendations() {
    report.recommendations.clear();

    if (report.failed_tests > 0) {
        report.recommendations.push_back("检查失败的测试项并修复相关问题");
    }

    if (report.skipped_tests > 0) {
        report.recommendations.push_back("检查跳过的测试项，确保所有功能都得到测试");
    }

    if (report.success_rate < 90.0) {
        report.recommendations.push_back("测试成功率较低，建议进行全面的代码审查");
    }
}

// 添加建议
void TestReportGenerator::AddRecommendation(const std::string& recommendation) {
    report.recommendations.push_back(recommendation);
}

// 加载报告从文件
bool TestReportGenerator::LoadReportFromFile(const std::string& filename, TestReport& report) {
    // 简化处理，实际应该解析文件内容
    return false;
}

// 加载历史报告
std::vector<TestReport> TestReportGenerator::LoadHistoricalReports(const std::string& directory) {
    std::vector<TestReport> reports;
    // 简化处理
    return reports;
}

// 与历史报告比较
bool TestReportGenerator::CompareWithHistorical(const TestReport& current_report) {
    // 简化处理
    return true;
}

// 全局报告函数实现
TestReportGenerator& GetGlobalReportGenerator() {
    return g_GlobalReportGenerator;
}

void InitializeGlobalReport() {
    if (!g_GlobalReportInitialized) {
        g_GlobalReportGenerator.InitializeReport();
        g_GlobalReportInitialized = true;
    }
}

void FinalizeGlobalReport() {
    if (g_GlobalReportInitialized) {
        g_GlobalReportGenerator.FinalizeReport();
        g_GlobalReportInitialized = false;
    }
}

TestReport GetFinalReport() {
    return g_GlobalReportGenerator.GetReport();
}

// 快速报告函数实现
std::string GenerateQuickSummary() {
    TestReport report = GetFinalReport();
    std::ostringstream oss;
    oss << "总测试: " << report.total_tests
        << ", 通过: " << report.passed_tests
        << ", 失败: " << report.failed_tests
        << ", 成功率: " << std::fixed << std::setprecision(2) << report.success_rate << "%";
    return oss.str();
}

std::string GeneratePerformanceSummary() {
    TestReport report = GetFinalReport();
    std::ostringstream oss;
    oss << "读取: " << report.performance.read_speed_mbps << " MB/s, "
        << "写入: " << report.performance.write_speed_mbps << " MB/s";
    return oss.str();
}

std::string GenerateErrorSummary() {
    TestReport report = GetFinalReport();
    if (report.error_tests > 0) {
        return "发现 " + std::to_string(report.error_tests) + " 个错误";
    }
    return "无错误";
}

// 报告工具函数实现
std::string EscapeHtml(const std::string& text) {
    std::string result = text;
    // 简化的HTML转义
    return result;
}

std::string FormatFileSize(SIZE_T size) {
    std::ostringstream oss;
    if (size >= 1024 * 1024) {
        oss << std::fixed << std::setprecision(2) << (static_cast<double>(size) / (1024 * 1024)) << " MB";
    }
    else if (size >= 1024) {
        oss << std::fixed << std::setprecision(2) << (static_cast<double>(size) / 1024) << " KB";
    }
    else {
        oss << size << " B";
    }
    return oss.str();
}

std::string FormatTime(double seconds) {
    std::ostringstream oss;
    if (seconds >= 60) {
        int minutes = static_cast<int>(seconds / 60);
        double remaining = seconds - (minutes * 60);
        oss << minutes << "m " << std::fixed << std::setprecision(2) << remaining << "s";
    }
    else {
        oss << std::fixed << std::setprecision(3) << seconds << "s";
    }
    return oss.str();
}

std::string GetCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buffer);
}

// 报告配置函数实现
ReportConfiguration GetReportConfiguration() {
    ReportConfiguration config = { 0 };
    // 默认配置
    config.include_detailed_results = true;
    config.include_performance_data = true;
    config.include_system_info = true;
    config.include_resource_usage = true;
    config.generate_html_report = true;
    config.generate_json_report = true;
    config.generate_csv_report = true;
    config.output_directory = "./reports";
    config.max_report_history = 10;
    return config;
}

void SetReportConfiguration(const ReportConfiguration& config) {
    // 简化处理
}

// 报告模板实现
std::string ReportTemplate::GetHtmlTemplate() {
    return "<!DOCTYPE html><html><head><title>Report</title></head><body>{{content}}</body></html>";
}

std::string ReportTemplate::GetJsonTemplate() {
    return "{ \"report\": {{content}} }";
}

std::string ReportTemplate::GetXmlTemplate() {
    return "<?xml version=\"1.0\"?><report>{{content}}</report>";
}

std::string ReportTemplate::GetCsvHeader() {
    return "Name,Status,Message\n";
}

// 报告事件回调实现
void RegisterReportCallback(ReportCallback callback, void* context) {
    // 简化处理
}

void UnregisterReportCallback(ReportCallback callback) {
    // 简化处理
}

// 报告通知实现
void NotifyReportGenerated(const TestReport& report) {
    // 简化处理
}

void NotifyReportSaved(const std::string& filename) {
    // 简化处理
}

void NotifyReportError(const std::string& error_message) {
    // 简化处理
}

// 报告归档实现
bool ArchiveReport(const TestReport& report) {
    // 简化处理
    return true;
}

std::vector<TestReport> GetArchivedReports() {
    std::vector<TestReport> reports;
    // 简化处理
    return reports;
}

bool ClearArchivedReports() {
    // 简化处理
    return true;
}

// 报告导出选项实现
bool ExportReportWithOptions(const TestReport& report, const std::string& filename, const ExportOptions& options) {
    // 简化处理
    return true;
}

// 报告分析工具实现
double ReportAnalyzer::CalculateTrend(const std::vector<TestReport>& reports) {
    // 简化处理
    return 0.0;
}

std::vector<TestItem> ReportAnalyzer::FindRegressions(const TestReport& current, const TestReport& baseline) {
    std::vector<TestItem> regressions;
    // 简化处理
    return regressions;
}

std::vector<TestItem> ReportAnalyzer::FindImprovements(const TestReport& current, const TestReport& baseline) {
    std::vector<TestItem> improvements;
    // 简化处理
    return improvements;
}

PerformanceMetrics ReportAnalyzer::CalculateAveragePerformance(const std::vector<TestReport>& reports) {
    PerformanceMetrics avg = { 0 };
    // 简化处理
    return avg;
}

// 报告过滤器实现
std::vector<TestItem> ReportFilter::FilterByStatus(const std::vector<TestItem>& items, TestStatus status) {
    std::vector<TestItem> filtered;
    std::copy_if(items.begin(), items.end(), std::back_inserter(filtered),
        [status](const TestItem& item) { return item.status == status; });
    return filtered;
}

std::vector<TestItem> ReportFilter::FilterByCategory(const std::vector<TestItem>& items, TestCategory category) {
    std::vector<TestItem> filtered;
    std::copy_if(items.begin(), items.end(), std::back_inserter(filtered),
        [category](const TestItem& item) { return item.category == category; });
    return filtered;
}

std::vector<TestItem> ReportFilter::FilterByDuration(const std::vector<TestItem>& items, double min_duration, double max_duration) {
    std::vector<TestItem> filtered;
    std::copy_if(items.begin(), items.end(), std::back_inserter(filtered),
        [min_duration, max_duration](const TestItem& item) {
            return item.execution_time >= min_duration && item.execution_time <= max_duration;
        });
    return filtered;
}

std::vector<TestItem> ReportFilter::FilterByMemoryUsage(const std::vector<TestItem>& items, SIZE_T min_memory, SIZE_T max_memory) {
    std::vector<TestItem> filtered;
    std::copy_if(items.begin(), items.end(), std::back_inserter(filtered),
        [min_memory, max_memory](const TestItem& item) {
            return item.memory_used >= min_memory && item.memory_used <= max_memory;
        });
    return filtered;
}

// 报告排序器实现
void ReportSorter::SortByExecutionTime(std::vector<TestItem>& items, bool ascending) {
    if (ascending) {
        std::sort(items.begin(), items.end(),
            [](const TestItem& a, const TestItem& b) {
                return a.execution_time < b.execution_time;
            });
    }
    else {
        std::sort(items.begin(), items.end(),
            [](const TestItem& a, const TestItem& b) {
                return a.execution_time > b.execution_time;
            });
    }
}

void ReportSorter::SortByMemoryUsage(std::vector<TestItem>& items, bool ascending) {
    if (ascending) {
        std::sort(items.begin(), items.end(),
            [](const TestItem& a, const TestItem& b) {
                return a.memory_used < b.memory_used;
            });
    }
    else {
        std::sort(items.begin(), items.end(),
            [](const TestItem& a, const TestItem& b) {
                return a.memory_used > b.memory_used;
            });
    }
}

void ReportSorter::SortByStatus(std::vector<TestItem>& items) {
    std::sort(items.begin(), items.end(),
        [](const TestItem& a, const TestItem& b) {
            return a.status < b.status;
        });
}

void ReportSorter::SortByCategory(std::vector<TestItem>& items) {
    std::sort(items.begin(), items.end(),
        [](const TestItem& a, const TestItem& b) {
            return a.category < b.category;
        });
}