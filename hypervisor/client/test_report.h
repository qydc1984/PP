/**
 * test_report.h - ���Ա�������
 *
 * ���ɺ͹���hypervisor���Ե���ϸ����
 */

#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <map>
#include <chrono>

 // ���Խ��ö��
enum TestStatus {
    TEST_STATUS_PASS = 0,
    TEST_STATUS_FAIL = 1,
    TEST_STATUS_SKIP = 2,
    TEST_STATUS_ERROR = 3
};

// �������ö��
enum TestCategory {
    CATEGORY_INITIALIZATION = 0,
    CATEGORY_PROCESS_MANAGEMENT = 1,
    CATEGORY_MEMORY_OPERATIONS = 2,
    CATEGORY_ADVANCED_FEATURES = 3,
    CATEGORY_PERFORMANCE = 4,
    CATEGORY_SECURITY = 5,
    CATEGORY_COMPATIBILITY = 6,
    CATEGORY_STRESS = 7,
    CATEGORY_ERROR_HANDLING = 8
};

// ������������Ϣ
struct TestItem {
    std::string name;                    // ��������
    std::string description;             // ��������
    TestStatus status;                   // ����״̬
    TestCategory category;               // �������
    std::string error_message;           // ������Ϣ
    double execution_time;               // ִ��ʱ��(����)
    std::chrono::high_resolution_clock::time_point start_time; // ��ʼʱ��
    std::chrono::high_resolution_clock::time_point end_time;   // ����ʱ��
    SIZE_T memory_used;                  // ʹ���ڴ���
    int retry_count;                     // ���Դ���
};

// ����ָ��ṹ
struct PerformanceMetrics {
    double read_speed_mbps;              // ��ȡ�ٶ� (MB/s)
    double write_speed_mbps;             // д���ٶ� (MB/s)
    double copy_speed_mbps;              // �����ٶ� (MB/s)
    double average_latency_us;           // ƽ���ӳ� (΢��)
    double operations_per_second;        // ÿ�������
    SIZE_T peak_memory_usage;            // ��ֵ�ڴ�ʹ����
    double cpu_utilization;              // CPU������ (%)
};

// ϵͳ��Ϣ�ṹ
struct SystemInfo {
    std::string os_version;              // ����ϵͳ�汾
    std::string architecture;            // ϵͳ�ܹ�
    ULONG processor_count;               // ����������
    SIZE_T total_memory;                 // ���ڴ��С
    SIZE_T available_memory;             // �����ڴ�
    std::string hypervisor_type;         // Hypervisor����
    std::string driver_version;          // �����汾
    std::string test_environment;        // ���Ի���
};

// ��Դʹ��ͳ��
struct ResourceUsage {
    SIZE_T memory_allocated_current;     // ��ǰ�����ڴ�
    SIZE_T memory_allocated_peak;        // ��ֵ�����ڴ�
    ULONG handle_count;                  // �������
    ULONG thread_count;                  // �߳�����
    double io_read_bytes;                // IO��ȡ�ֽ�
    double io_write_bytes;               // IOд���ֽ�
};

// ���Ա���ṹ
struct TestReport {
    std::string report_id;               // ����ID
    std::string timestamp_start;         // ��ʼʱ���
    std::string timestamp_end;           // ����ʱ���
    double total_execution_time;         // ��ִ��ʱ��
    SystemInfo system_info;              // ϵͳ��Ϣ
    std::vector<TestItem> test_items;    // ������Ŀ�б�
    std::map<TestCategory, int> category_stats; // �����ͳ��
    PerformanceMetrics performance;       // ����ָ��
    ResourceUsage resources;             // ��Դʹ��
    int total_tests;                     // �ܲ�����
    int passed_tests;                    // ͨ����
    int failed_tests;                    // ʧ����
    int skipped_tests;                   // ������
    int error_tests;                     // ������
    double success_rate;                 // �ɹ���
    std::string summary;                 // ժҪ��Ϣ
    std::vector<std::string> recommendations; // ����
};

// ���Ա�����������
class TestReportGenerator {
private:
    TestReport report;
    bool report_generated;

public:
    TestReportGenerator();
    ~TestReportGenerator();

    // ���������
    void InitializeReport();
    void FinalizeReport();
    bool IsReportGenerated() const;
    void ResetReport();

    // ���������
    void AddTestItem(const TestItem& item);
    void UpdateTestItem(const std::string& name, const TestItem& updated_item);
    TestItem* FindTestItem(const std::string& name);

    // ϵͳ��Ϣ����
    void SetSystemInfo(const SystemInfo& info);
    void CollectSystemInfo();

    // ����ָ������
    void SetPerformanceMetrics(const PerformanceMetrics& metrics);
    void CollectPerformanceMetrics();

    // ��Դʹ������
    void SetResourceUsage(const ResourceUsage& usage);
    void CollectResourceUsage();

    // ͳ����Ϣ����
    void UpdateStatistics();
    void CalculateSuccessRate();

    // �����ȡ
    const TestReport& GetReport() const;
    TestReport& GetReport();

    // ���浼��
    bool SaveReportToFile(const std::string& filename);
    bool SaveReportToHtml(const std::string& filename);
    bool SaveReportToJson(const std::string& filename);
    bool SaveReportToXml(const std::string& filename);
    bool SaveReportToCsv(const std::string& filename);

    // �����ӡ
    void PrintConsoleReport();
    void PrintDetailedReport();
    void PrintSummaryReport();
    void PrintFailedTestsReport();
    void PrintPerformanceReport();

    // �������
    std::vector<TestItem> GetFailedTests() const;
    std::vector<TestItem> GetPassedTests() const;
    std::vector<TestItem> GetSkippedTests() const;
    std::vector<TestItem> GetTestsByCategory(TestCategory category) const;

    // ����Ƚ�
    static bool CompareReports(const TestReport& report1, const TestReport& report2);
    static double CalculateImprovement(const TestReport& old_report, const TestReport& new_report);

    // ������֤
    bool ValidateReport() const;
    std::vector<std::string> GetValidationErrors() const;

    // ���渨������
    std::string GetTestStatusString(TestStatus status) const;
    std::string GetTestCategoryString(TestCategory category) const;
    std::string FormatTimestamp(const std::chrono::high_resolution_clock::time_point& time) const;
    std::string GenerateReportId() const;

    // ��������
    void GenerateRecommendations();
    void AddRecommendation(const std::string& recommendation);

    // ��ʷ�������
    static bool LoadReportFromFile(const std::string& filename, TestReport& report);
    static std::vector<TestReport> LoadHistoricalReports(const std::string& directory);
    static bool CompareWithHistorical(const TestReport& current_report);
};

// ȫ�ֱ��溯��
TestReportGenerator& GetGlobalReportGenerator();
void InitializeGlobalReport();
void FinalizeGlobalReport();
TestReport GetFinalReport();

// ���ٱ��溯��
std::string GenerateQuickSummary();
std::string GeneratePerformanceSummary();
std::string GenerateErrorSummary();

// ���湤�ߺ���
std::string EscapeHtml(const std::string& text);
std::string FormatFileSize(SIZE_T size);
std::string FormatTime(double seconds);
std::string GetCurrentTimestamp();

// ��������
struct ReportConfiguration {
    bool include_detailed_results;
    bool include_performance_data;
    bool include_system_info;
    bool include_resource_usage;
    bool generate_html_report;
    bool generate_json_report;
    bool generate_csv_report;
    std::string output_directory;
    int max_report_history;
};

ReportConfiguration GetReportConfiguration();
void SetReportConfiguration(const ReportConfiguration& config);

// ����ģ��
class ReportTemplate {
public:
    static std::string GetHtmlTemplate();
    static std::string GetJsonTemplate();
    static std::string GetXmlTemplate();
    static std::string GetCsvHeader();
};

// �����¼��ص�
typedef void (*ReportCallback)(const TestReport& report, void* context);
void RegisterReportCallback(ReportCallback callback, void* context);
void UnregisterReportCallback(ReportCallback callback);

// ����֪ͨ
void NotifyReportGenerated(const TestReport& report);
void NotifyReportSaved(const std::string& filename);
void NotifyReportError(const std::string& error_message);

// ����鵵
bool ArchiveReport(const TestReport& report);
std::vector<TestReport> GetArchivedReports();
bool ClearArchivedReports();

// ���浼��ѡ��
struct ExportOptions {
    bool include_charts;
    bool include_detailed_logs;
    bool compress_output;
    std::string custom_css;
    std::string custom_header;
    std::string custom_footer;
};

bool ExportReportWithOptions(const TestReport& report, const std::string& filename, const ExportOptions& options);

// �����������
class ReportAnalyzer {
public:
    static double CalculateTrend(const std::vector<TestReport>& reports);
    static std::vector<TestItem> FindRegressions(const TestReport& current, const TestReport& baseline);
    static std::vector<TestItem> FindImprovements(const TestReport& current, const TestReport& baseline);
    static PerformanceMetrics CalculateAveragePerformance(const std::vector<TestReport>& reports);
};

// ���������
class ReportFilter {
public:
    static std::vector<TestItem> FilterByStatus(const std::vector<TestItem>& items, TestStatus status);
    static std::vector<TestItem> FilterByCategory(const std::vector<TestItem>& items, TestCategory category);
    static std::vector<TestItem> FilterByDuration(const std::vector<TestItem>& items, double min_duration, double max_duration);
    static std::vector<TestItem> FilterByMemoryUsage(const std::vector<TestItem>& items, SIZE_T min_memory, SIZE_T max_memory);
};

// ����������
class ReportSorter {
public:
    static void SortByExecutionTime(std::vector<TestItem>& items, bool ascending = true);
    static void SortByMemoryUsage(std::vector<TestItem>& items, bool ascending = true);
    static void SortByStatus(std::vector<TestItem>& items);
    static void SortByCategory(std::vector<TestItem>& items);
};