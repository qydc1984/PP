/**
 * test_report.cpp - ���Ա�������ʵ��
 *
 * ʵ��hypervisor���Ե���ϸ�������ɹ���
 */

#include "test_report.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <filesystem>

 // ��̬ȫ�ֱ���������ʵ��
static TestReportGenerator g_GlobalReportGenerator;
static bool g_GlobalReportInitialized = false;

// ���캯��
TestReportGenerator::TestReportGenerator() : report_generated(false) {
    ResetReport();
}

// ��������
TestReportGenerator::~TestReportGenerator() {
    // ������Դ
}

// ��ʼ������
void TestReportGenerator::InitializeReport() {
    ResetReport();
    report_generated = false;

    // �ռ�ϵͳ��Ϣ
    CollectSystemInfo();

    // ���ÿ�ʼʱ��
    report.timestamp_start = GetCurrentTimestamp();
}

// ��ɱ���
void TestReportGenerator::FinalizeReport() {
    // ���ý���ʱ��
    report.timestamp_end = GetCurrentTimestamp();

    // ������ִ��ʱ��
    // �򻯴���ʵ��Ӧ�ü���ʱ���
    report.total_execution_time = 0.0;

    // ����ͳ����Ϣ
    UpdateStatistics();
    CalculateSuccessRate();

    // ���ɽ���
    GenerateRecommendations();

    report_generated = true;
}

// ��鱨���Ƿ�������
bool TestReportGenerator::IsReportGenerated() const {
    return report_generated;
}

// ���ñ���
void TestReportGenerator::ResetReport() {
    report = TestReport();
    report_generated = false;
}

// ��Ӳ�����
void TestReportGenerator::AddTestItem(const TestItem& item) {
    report.test_items.push_back(item);
}

// ���²�����
void TestReportGenerator::UpdateTestItem(const std::string& name, const TestItem& updated_item) {
    for (auto& item : report.test_items) {
        if (item.name == name) {
            item = updated_item;
            break;
        }
    }
}

// ���Ҳ�����
TestItem* TestReportGenerator::FindTestItem(const std::string& name) {
    for (auto& item : report.test_items) {
        if (item.name == name) {
            return &item;
        }
    }
    return nullptr;
}

// ����ϵͳ��Ϣ
void TestReportGenerator::SetSystemInfo(const SystemInfo& info) {
    report.system_info = info;
}

// �ռ�ϵͳ��Ϣ
void TestReportGenerator::CollectSystemInfo() {
    SystemInfo info;

    // ��ȡ����ϵͳ�汾���򻯴���
    info.os_version = "Windows 10/11";
    info.architecture = "x64";
    info.processor_count = std::thread::hardware_concurrency();
    info.total_memory = 0; // �򻯴���
    info.available_memory = 0; // �򻯴���
    info.hypervisor_type = "Unknown";
    info.driver_version = "1.0.0";
    info.test_environment = "Development";

    SetSystemInfo(info);
}

// ��������ָ��
void TestReportGenerator::SetPerformanceMetrics(const PerformanceMetrics& metrics) {
    report.performance = metrics;
}

// �ռ�����ָ��
void TestReportGenerator::CollectPerformanceMetrics() {
    PerformanceMetrics metrics = { 0 };
    // �򻯴���ʵ��Ӧ���ռ���ʵ����
    SetPerformanceMetrics(metrics);
}

// ������Դʹ��
void TestReportGenerator::SetResourceUsage(const ResourceUsage& usage) {
    report.resources = usage;
}

// �ռ���Դʹ��
void TestReportGenerator::CollectResourceUsage() {
    ResourceUsage usage = { 0 };
    // �򻯴���ʵ��Ӧ���ռ���ʵ����
    SetResourceUsage(usage);
}

// ����ͳ����Ϣ
void TestReportGenerator::UpdateStatistics() {
    report.total_tests = static_cast<int>(report.test_items.size());
    report.passed_tests = 0;
    report.failed_tests = 0;
    report.skipped_tests = 0;
    report.error_tests = 0;

    // �������ͳ��
    for (int i = 0; i <= CATEGORY_ERROR_HANDLING; i++) {
        report.category_stats[static_cast<TestCategory>(i)] = 0;
    }

    // ͳ�Ƹ�������
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

        // �������ͳ��
        report.category_stats[item.category]++;
    }
}

// ����ɹ���
void TestReportGenerator::CalculateSuccessRate() {
    if (report.total_tests > 0) {
        report.success_rate = (static_cast<double>(report.passed_tests) / report.total_tests) * 100.0;
    }
    else {
        report.success_rate = 0.0;
    }
}

// ��ȡ���棨�����汾��
const TestReport& TestReportGenerator::GetReport() const {
    return report;
}

// ��ȡ���棨�ǳ����汾��
TestReport& TestReportGenerator::GetReport() {
    return report;
}

// ���汨�浽�ļ�
bool TestReportGenerator::SaveReportToFile(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    // д�����������Ϣ
    file << "Hypervisor ���Ա���\n";
    file << "==================\n\n";

    file << "����ID: " << report.report_id << "\n";
    file << "��ʼʱ��: " << report.timestamp_start << "\n";
    file << "����ʱ��: " << report.timestamp_end << "\n";
    file << "��ִ��ʱ��: " << report.total_execution_time << " ��\n\n";

    // д��ϵͳ��Ϣ
    file << "ϵͳ��Ϣ:\n";
    file << "  ����ϵͳ: " << report.system_info.os_version << "\n";
    file << "  �ܹ�: " << report.system_info.architecture << "\n";
    file << "  ����������: " << report.system_info.processor_count << "\n";
    file << "  Hypervisor����: " << report.system_info.hypervisor_type << "\n";
    file << "  �����汾: " << report.system_info.driver_version << "\n\n";

    // д�����ͳ��
    file << "����ͳ��:\n";
    file << "  �ܲ�����: " << report.total_tests << "\n";
    file << "  ͨ��: " << report.passed_tests << "\n";
    file << "  ʧ��: " << report.failed_tests << "\n";
    file << "  ����: " << report.skipped_tests << "\n";
    file << "  ����: " << report.error_tests << "\n";
    file << "  �ɹ���: " << std::fixed << std::setprecision(2) << report.success_rate << "%\n\n";

    // д����ϸ���Խ��
    file << "��ϸ���Խ��:\n";
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

// ���汨�浽HTML
bool TestReportGenerator::SaveReportToHtml(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file << "<!DOCTYPE html>\n";
    file << "<html>\n<head>\n";
    file << "<title>Hypervisor ���Ա���</title>\n";
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

    file << "<h1>Hypervisor ���Ա���</h1>\n";

    // ������Ϣ
    file << "<h2>������Ϣ</h2>\n";
    file << "<p><strong>����ID:</strong> " << report.report_id << "</p>\n";
    file << "<p><strong>��ʼʱ��:</strong> " << report.timestamp_start << "</p>\n";
    file << "<p><strong>����ʱ��:</strong> " << report.timestamp_end << "</p>\n";
    file << "<p><strong>��ִ��ʱ��:</strong> " << report.total_execution_time << " ��</p>\n";

    // ϵͳ��Ϣ
    file << "<h2>ϵͳ��Ϣ</h2>\n";
    file << "<ul>\n";
    file << "<li><strong>����ϵͳ:</strong> " << report.system_info.os_version << "</li>\n";
    file << "<li><strong>�ܹ�:</strong> " << report.system_info.architecture << "</li>\n";
    file << "<li><strong>����������:</strong> " << report.system_info.processor_count << "</li>\n";
    file << "<li><strong>Hypervisor����:</strong> " << report.system_info.hypervisor_type << "</li>\n";
    file << "<li><strong>�����汾:</strong> " << report.system_info.driver_version << "</li>\n";
    file << "</ul>\n";

    // ����ͳ��
    file << "<h2>����ͳ��</h2>\n";
    file << "<table>\n";
    file << "<tr><th>��Ŀ</th><th>����</th></tr>\n";
    file << "<tr><td>�ܲ�����</td><td>" << report.total_tests << "</td></tr>\n";
    file << "<tr><td>ͨ��</td><td class='pass'>" << report.passed_tests << "</td></tr>\n";
    file << "<tr><td>ʧ��</td><td class='fail'>" << report.failed_tests << "</td></tr>\n";
    file << "<tr><td>����</td><td class='skip'>" << report.skipped_tests << "</td></tr>\n";
    file << "<tr><td>����</td><td class='error'>" << report.error_tests << "</td></tr>\n";
    file << "<tr><td>�ɹ���</td><td>" << std::fixed << std::setprecision(2) << report.success_rate << "%</td></tr>\n";
    file << "</table>\n";

    // ��ϸ���Խ��
    file << "<h2>��ϸ���Խ��</h2>\n";
    file << "<table>\n";
    file << "<tr><th>��������</th><th>״̬</th><th>������Ϣ</th></tr>\n";

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

// ���汨�浽JSON
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

    // ϵͳ��Ϣ
    file << "  \"system_info\": {\n";
    file << "    \"os_version\": \"" << report.system_info.os_version << "\",\n";
    file << "    \"architecture\": \"" << report.system_info.architecture << "\",\n";
    file << "    \"processor_count\": " << report.system_info.processor_count << ",\n";
    file << "    \"hypervisor_type\": \"" << report.system_info.hypervisor_type << "\",\n";
    file << "    \"driver_version\": \"" << report.system_info.driver_version << "\"\n";
    file << "  },\n";

    // ����ͳ��
    file << "  \"statistics\": {\n";
    file << "    \"total_tests\": " << report.total_tests << ",\n";
    file << "    \"passed_tests\": " << report.passed_tests << ",\n";
    file << "    \"failed_tests\": " << report.failed_tests << ",\n";
    file << "    \"skipped_tests\": " << report.skipped_tests << ",\n";
    file << "    \"error_tests\": " << report.error_tests << ",\n";
    file << "    \"success_rate\": " << std::fixed << std::setprecision(2) << report.success_rate << "\n";
    file << "  },\n";

    // ������
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

// ���汨�浽XML
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

    // ϵͳ��Ϣ
    file << "  <system_info>\n";
    file << "    <os_version>" << report.system_info.os_version << "</os_version>\n";
    file << "    <architecture>" << report.system_info.architecture << "</architecture>\n";
    file << "    <processor_count>" << report.system_info.processor_count << "</processor_count>\n";
    file << "    <hypervisor_type>" << report.system_info.hypervisor_type << "</hypervisor_type>\n";
    file << "    <driver_version>" << report.system_info.driver_version << "</driver_version>\n";
    file << "  </system_info>\n";

    // ����ͳ��
    file << "  <statistics>\n";
    file << "    <total_tests>" << report.total_tests << "</total_tests>\n";
    file << "    <passed_tests>" << report.passed_tests << "</passed_tests>\n";
    file << "    <failed_tests>" << report.failed_tests << "</failed_tests>\n";
    file << "    <skipped_tests>" << report.skipped_tests << "</skipped_tests>\n";
    file << "    <error_tests>" << report.error_tests << "</error_tests>\n";
    file << "    <success_rate>" << std::fixed << std::setprecision(2) << report.success_rate << "</success_rate>\n";
    file << "  </statistics>\n";

    // ������
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

// ���汨�浽CSV
bool TestReportGenerator::SaveReportToCsv(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    // CSVͷ��
    file << "Test Name,Status,Error Message\n";

    // ����������
    for (const auto& item : report.test_items) {
        file << "\"" << item.name << "\",";
        file << "\"" << GetTestStatusString(item.status) << "\",";
        file << "\"" << item.error_message << "\"\n";
    }

    file.close();
    return true;
}

// ��ӡ����̨����
void TestReportGenerator::PrintConsoleReport() {
    std::cout << "\n=== Hypervisor ���Ա��� ===\n";
    std::cout << "����ID: " << report.report_id << "\n";
    std::cout << "��ʼʱ��: " << report.timestamp_start << "\n";
    std::cout << "����ʱ��: " << report.timestamp_end << "\n";
    std::cout << "��ִ��ʱ��: " << report.total_execution_time << " ��\n";

    std::cout << "\nϵͳ��Ϣ:\n";
    std::cout << "  ����ϵͳ: " << report.system_info.os_version << "\n";
    std::cout << "  �ܹ�: " << report.system_info.architecture << "\n";
    std::cout << "  ����������: " << report.system_info.processor_count << "\n";
    std::cout << "  Hypervisor����: " << report.system_info.hypervisor_type << "\n";

    std::cout << "\n����ͳ��:\n";
    std::cout << "  �ܲ�����: " << report.total_tests << "\n";
    std::cout << "  ͨ��: " << report.passed_tests << "\n";
    std::cout << "  ʧ��: " << report.failed_tests << "\n";
    std::cout << "  ����: " << report.skipped_tests << "\n";
    std::cout << "  ����: " << report.error_tests << "\n";
    std::cout << "  �ɹ���: " << std::fixed << std::setprecision(2) << report.success_rate << "%\n";

    std::cout << "\n��ϸ���Խ��:\n";
    for (const auto& item : report.test_items) {
        std::cout << "  " << item.name << ": " << GetTestStatusString(item.status);
        if (!item.error_message.empty()) {
            std::cout << " - " << item.error_message;
        }
        std::cout << "\n";
    }
}

// ��ӡ��ϸ����
void TestReportGenerator::PrintDetailedReport() {
    PrintConsoleReport();
}

// ��ӡժҪ����
void TestReportGenerator::PrintSummaryReport() {
    std::cout << "\n=== ����ժҪ ===\n";
    std::cout << "�ܲ�����: " << report.total_tests << "\n";
    std::cout << "ͨ��: " << report.passed_tests << "\n";
    std::cout << "ʧ��: " << report.failed_tests << "\n";
    std::cout << "�ɹ���: " << std::fixed << std::setprecision(2) << report.success_rate << "%\n";
}

// ��ӡʧ�ܲ��Ա���
void TestReportGenerator::PrintFailedTestsReport() {
    std::vector<TestItem> failedTests = GetFailedTests();

    if (failedTests.empty()) {
        std::cout << "\nû��ʧ�ܵĲ��ԡ�\n";
        return;
    }

    std::cout << "\n=== ʧ�ܵĲ��� ===\n";
    for (const auto& item : failedTests) {
        std::cout << "  " << item.name << ": " << item.error_message << "\n";
    }
}

// ��ӡ���ܱ���
void TestReportGenerator::PrintPerformanceReport() {
    std::cout << "\n=== ���ܱ��� ===\n";
    std::cout << "��ȡ�ٶ�: " << report.performance.read_speed_mbps << " MB/s\n";
    std::cout << "д���ٶ�: " << report.performance.write_speed_mbps << " MB/s\n";
    std::cout << "�����ٶ�: " << report.performance.copy_speed_mbps << " MB/s\n";
    std::cout << "ƽ���ӳ�: " << report.performance.average_latency_us << " ΢��\n";
}

// ��ȡʧ�ܲ���
std::vector<TestItem> TestReportGenerator::GetFailedTests() const {
    return FilterByStatus(report.test_items, TEST_STATUS_FAIL);
}

// ��ȡͨ������
std::vector<TestItem> TestReportGenerator::GetPassedTests() const {
    return FilterByStatus(report.test_items, TEST_STATUS_PASS);
}

// ��ȡ��������
std::vector<TestItem> TestReportGenerator::GetSkippedTests() const {
    return FilterByStatus(report.test_items, TEST_STATUS_SKIP);
}

// ��������ȡ����
std::vector<TestItem> TestReportGenerator::GetTestsByCategory(TestCategory category) const {
    return ReportFilter::FilterByCategory(report.test_items, category);
}

// �Ƚϱ���
bool TestReportGenerator::CompareReports(const TestReport& report1, const TestReport& report2) {
    return (report1.success_rate == report2.success_rate) &&
        (report1.total_tests == report2.total_tests) &&
        (report1.passed_tests == report2.passed_tests);
}

// ����Ľ�
double TestReportGenerator::CalculateImprovement(const TestReport& old_report, const TestReport& new_report) {
    if (old_report.total_tests == 0) return 0.0;
    return new_report.success_rate - old_report.success_rate;
}

// ��֤����
bool TestReportGenerator::ValidateReport() const {
    // ������֤
    return !report.report_id.empty() && report.total_tests >= 0;
}

// ��ȡ��֤����
std::vector<std::string> TestReportGenerator::GetValidationErrors() const {
    std::vector<std::string> errors;

    if (report.report_id.empty()) {
        errors.push_back("����IDΪ��");
    }

    if (report.total_tests < 0) {
        errors.push_back("�ܲ�����Ϊ����");
    }

    return errors;
}

// ��ȡ����״̬�ַ���
std::string TestReportGenerator::GetTestStatusString(TestStatus status) const {
    switch (status) {
    case TEST_STATUS_PASS: return "ͨ��";
    case TEST_STATUS_FAIL: return "ʧ��";
    case TEST_STATUS_SKIP: return "����";
    case TEST_STATUS_ERROR: return "����";
    default: return "δ֪";
    }
}

// ��ȡ��������ַ���
std::string TestReportGenerator::GetTestCategoryString(TestCategory category) const {
    switch (category) {
    case CATEGORY_INITIALIZATION: return "��ʼ��";
    case CATEGORY_PROCESS_MANAGEMENT: return "���̹���";
    case CATEGORY_MEMORY_OPERATIONS: return "�ڴ����";
    case CATEGORY_ADVANCED_FEATURES: return "�߼�����";
    case CATEGORY_PERFORMANCE: return "����";
    case CATEGORY_SECURITY: return "��ȫ";
    case CATEGORY_COMPATIBILITY: return "������";
    case CATEGORY_STRESS: return "ѹ������";
    case CATEGORY_ERROR_HANDLING: return "������";
    default: return "δ֪";
    }
}

// ��ʽ��ʱ���
std::string TestReportGenerator::FormatTimestamp(const std::chrono::high_resolution_clock::time_point& time) const {
    // �򻯴���
    return "2024-01-01 00:00:00";
}

// ���ɱ���ID
std::string TestReportGenerator::GenerateReportId() const {
    // �򻯴���ʵ��Ӧ������ΨһID
    return "REPORT_" + std::to_string(std::time(nullptr));
}

// ���ɽ���
void TestReportGenerator::GenerateRecommendations() {
    report.recommendations.clear();

    if (report.failed_tests > 0) {
        report.recommendations.push_back("���ʧ�ܵĲ�����޸��������");
    }

    if (report.skipped_tests > 0) {
        report.recommendations.push_back("��������Ĳ����ȷ�����й��ܶ��õ�����");
    }

    if (report.success_rate < 90.0) {
        report.recommendations.push_back("���Գɹ��ʽϵͣ��������ȫ��Ĵ������");
    }
}

// ��ӽ���
void TestReportGenerator::AddRecommendation(const std::string& recommendation) {
    report.recommendations.push_back(recommendation);
}

// ���ر�����ļ�
bool TestReportGenerator::LoadReportFromFile(const std::string& filename, TestReport& report) {
    // �򻯴���ʵ��Ӧ�ý����ļ�����
    return false;
}

// ������ʷ����
std::vector<TestReport> TestReportGenerator::LoadHistoricalReports(const std::string& directory) {
    std::vector<TestReport> reports;
    // �򻯴���
    return reports;
}

// ����ʷ����Ƚ�
bool TestReportGenerator::CompareWithHistorical(const TestReport& current_report) {
    // �򻯴���
    return true;
}

// ȫ�ֱ��溯��ʵ��
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

// ���ٱ��溯��ʵ��
std::string GenerateQuickSummary() {
    TestReport report = GetFinalReport();
    std::ostringstream oss;
    oss << "�ܲ���: " << report.total_tests
        << ", ͨ��: " << report.passed_tests
        << ", ʧ��: " << report.failed_tests
        << ", �ɹ���: " << std::fixed << std::setprecision(2) << report.success_rate << "%";
    return oss.str();
}

std::string GeneratePerformanceSummary() {
    TestReport report = GetFinalReport();
    std::ostringstream oss;
    oss << "��ȡ: " << report.performance.read_speed_mbps << " MB/s, "
        << "д��: " << report.performance.write_speed_mbps << " MB/s";
    return oss.str();
}

std::string GenerateErrorSummary() {
    TestReport report = GetFinalReport();
    if (report.error_tests > 0) {
        return "���� " + std::to_string(report.error_tests) + " ������";
    }
    return "�޴���";
}

// ���湤�ߺ���ʵ��
std::string EscapeHtml(const std::string& text) {
    std::string result = text;
    // �򻯵�HTMLת��
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

// �������ú���ʵ��
ReportConfiguration GetReportConfiguration() {
    ReportConfiguration config = { 0 };
    // Ĭ������
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
    // �򻯴���
}

// ����ģ��ʵ��
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

// �����¼��ص�ʵ��
void RegisterReportCallback(ReportCallback callback, void* context) {
    // �򻯴���
}

void UnregisterReportCallback(ReportCallback callback) {
    // �򻯴���
}

// ����֪ͨʵ��
void NotifyReportGenerated(const TestReport& report) {
    // �򻯴���
}

void NotifyReportSaved(const std::string& filename) {
    // �򻯴���
}

void NotifyReportError(const std::string& error_message) {
    // �򻯴���
}

// ����鵵ʵ��
bool ArchiveReport(const TestReport& report) {
    // �򻯴���
    return true;
}

std::vector<TestReport> GetArchivedReports() {
    std::vector<TestReport> reports;
    // �򻯴���
    return reports;
}

bool ClearArchivedReports() {
    // �򻯴���
    return true;
}

// ���浼��ѡ��ʵ��
bool ExportReportWithOptions(const TestReport& report, const std::string& filename, const ExportOptions& options) {
    // �򻯴���
    return true;
}

// �����������ʵ��
double ReportAnalyzer::CalculateTrend(const std::vector<TestReport>& reports) {
    // �򻯴���
    return 0.0;
}

std::vector<TestItem> ReportAnalyzer::FindRegressions(const TestReport& current, const TestReport& baseline) {
    std::vector<TestItem> regressions;
    // �򻯴���
    return regressions;
}

std::vector<TestItem> ReportAnalyzer::FindImprovements(const TestReport& current, const TestReport& baseline) {
    std::vector<TestItem> improvements;
    // �򻯴���
    return improvements;
}

PerformanceMetrics ReportAnalyzer::CalculateAveragePerformance(const std::vector<TestReport>& reports) {
    PerformanceMetrics avg = { 0 };
    // �򻯴���
    return avg;
}

// ���������ʵ��
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

// ����������ʵ��
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