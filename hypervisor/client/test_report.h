/**
 * test_report.h - 测试报告生成
 *
 * 生成和管理hypervisor测试的详细报告
 */

#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <map>
#include <chrono>

 // 测试结果枚举
enum TestStatus {
    TEST_STATUS_PASS = 0,
    TEST_STATUS_FAIL = 1,
    TEST_STATUS_SKIP = 2,
    TEST_STATUS_ERROR = 3
};

// 测试类别枚举
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

// 单个测试项信息
struct TestItem {
    std::string name;                    // 测试名称
    std::string description;             // 测试描述
    TestStatus status;                   // 测试状态
    TestCategory category;               // 测试类别
    std::string error_message;           // 错误信息
    double execution_time;               // 执行时间(毫秒)
    std::chrono::high_resolution_clock::time_point start_time; // 开始时间
    std::chrono::high_resolution_clock::time_point end_time;   // 结束时间
    SIZE_T memory_used;                  // 使用内存量
    int retry_count;                     // 重试次数
};

// 性能指标结构
struct PerformanceMetrics {
    double read_speed_mbps;              // 读取速度 (MB/s)
    double write_speed_mbps;             // 写入速度 (MB/s)
    double copy_speed_mbps;              // 拷贝速度 (MB/s)
    double average_latency_us;           // 平均延迟 (微秒)
    double operations_per_second;        // 每秒操作数
    SIZE_T peak_memory_usage;            // 峰值内存使用量
    double cpu_utilization;              // CPU利用率 (%)
};

// 系统信息结构
struct SystemInfo {
    std::string os_version;              // 操作系统版本
    std::string architecture;            // 系统架构
    ULONG processor_count;               // 处理器数量
    SIZE_T total_memory;                 // 总内存大小
    SIZE_T available_memory;             // 可用内存
    std::string hypervisor_type;         // Hypervisor类型
    std::string driver_version;          // 驱动版本
    std::string test_environment;        // 测试环境
};

// 资源使用统计
struct ResourceUsage {
    SIZE_T memory_allocated_current;     // 当前分配内存
    SIZE_T memory_allocated_peak;        // 峰值分配内存
    ULONG handle_count;                  // 句柄数量
    ULONG thread_count;                  // 线程数量
    double io_read_bytes;                // IO读取字节
    double io_write_bytes;               // IO写入字节
};

// 测试报告结构
struct TestReport {
    std::string report_id;               // 报告ID
    std::string timestamp_start;         // 开始时间戳
    std::string timestamp_end;           // 结束时间戳
    double total_execution_time;         // 总执行时间
    SystemInfo system_info;              // 系统信息
    std::vector<TestItem> test_items;    // 测试项目列表
    std::map<TestCategory, int> category_stats; // 各类别统计
    PerformanceMetrics performance;       // 性能指标
    ResourceUsage resources;             // 资源使用
    int total_tests;                     // 总测试数
    int passed_tests;                    // 通过数
    int failed_tests;                    // 失败数
    int skipped_tests;                   // 跳过数
    int error_tests;                     // 错误数
    double success_rate;                 // 成功率
    std::string summary;                 // 摘要信息
    std::vector<std::string> recommendations; // 建议
};

// 测试报告生成器类
class TestReportGenerator {
private:
    TestReport report;
    bool report_generated;

public:
    TestReportGenerator();
    ~TestReportGenerator();

    // 报告管理函数
    void InitializeReport();
    void FinalizeReport();
    bool IsReportGenerated() const;
    void ResetReport();

    // 测试项管理
    void AddTestItem(const TestItem& item);
    void UpdateTestItem(const std::string& name, const TestItem& updated_item);
    TestItem* FindTestItem(const std::string& name);

    // 系统信息设置
    void SetSystemInfo(const SystemInfo& info);
    void CollectSystemInfo();

    // 性能指标设置
    void SetPerformanceMetrics(const PerformanceMetrics& metrics);
    void CollectPerformanceMetrics();

    // 资源使用设置
    void SetResourceUsage(const ResourceUsage& usage);
    void CollectResourceUsage();

    // 统计信息更新
    void UpdateStatistics();
    void CalculateSuccessRate();

    // 报告获取
    const TestReport& GetReport() const;
    TestReport& GetReport();

    // 报告导出
    bool SaveReportToFile(const std::string& filename);
    bool SaveReportToHtml(const std::string& filename);
    bool SaveReportToJson(const std::string& filename);
    bool SaveReportToXml(const std::string& filename);
    bool SaveReportToCsv(const std::string& filename);

    // 报告打印
    void PrintConsoleReport();
    void PrintDetailedReport();
    void PrintSummaryReport();
    void PrintFailedTestsReport();
    void PrintPerformanceReport();

    // 报告分析
    std::vector<TestItem> GetFailedTests() const;
    std::vector<TestItem> GetPassedTests() const;
    std::vector<TestItem> GetSkippedTests() const;
    std::vector<TestItem> GetTestsByCategory(TestCategory category) const;

    // 报告比较
    static bool CompareReports(const TestReport& report1, const TestReport& report2);
    static double CalculateImprovement(const TestReport& old_report, const TestReport& new_report);

    // 报告验证
    bool ValidateReport() const;
    std::vector<std::string> GetValidationErrors() const;

    // 报告辅助函数
    std::string GetTestStatusString(TestStatus status) const;
    std::string GetTestCategoryString(TestCategory category) const;
    std::string FormatTimestamp(const std::chrono::high_resolution_clock::time_point& time) const;
    std::string GenerateReportId() const;

    // 建议生成
    void GenerateRecommendations();
    void AddRecommendation(const std::string& recommendation);

    // 历史报告管理
    static bool LoadReportFromFile(const std::string& filename, TestReport& report);
    static std::vector<TestReport> LoadHistoricalReports(const std::string& directory);
    static bool CompareWithHistorical(const TestReport& current_report);
};

// 全局报告函数
TestReportGenerator& GetGlobalReportGenerator();
void InitializeGlobalReport();
void FinalizeGlobalReport();
TestReport GetFinalReport();

// 快速报告函数
std::string GenerateQuickSummary();
std::string GeneratePerformanceSummary();
std::string GenerateErrorSummary();

// 报告工具函数
std::string EscapeHtml(const std::string& text);
std::string FormatFileSize(SIZE_T size);
std::string FormatTime(double seconds);
std::string GetCurrentTimestamp();

// 报告配置
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

// 报告模板
class ReportTemplate {
public:
    static std::string GetHtmlTemplate();
    static std::string GetJsonTemplate();
    static std::string GetXmlTemplate();
    static std::string GetCsvHeader();
};

// 报告事件回调
typedef void (*ReportCallback)(const TestReport& report, void* context);
void RegisterReportCallback(ReportCallback callback, void* context);
void UnregisterReportCallback(ReportCallback callback);

// 报告通知
void NotifyReportGenerated(const TestReport& report);
void NotifyReportSaved(const std::string& filename);
void NotifyReportError(const std::string& error_message);

// 报告归档
bool ArchiveReport(const TestReport& report);
std::vector<TestReport> GetArchivedReports();
bool ClearArchivedReports();

// 报告导出选项
struct ExportOptions {
    bool include_charts;
    bool include_detailed_logs;
    bool compress_output;
    std::string custom_css;
    std::string custom_header;
    std::string custom_footer;
};

bool ExportReportWithOptions(const TestReport& report, const std::string& filename, const ExportOptions& options);

// 报告分析工具
class ReportAnalyzer {
public:
    static double CalculateTrend(const std::vector<TestReport>& reports);
    static std::vector<TestItem> FindRegressions(const TestReport& current, const TestReport& baseline);
    static std::vector<TestItem> FindImprovements(const TestReport& current, const TestReport& baseline);
    static PerformanceMetrics CalculateAveragePerformance(const std::vector<TestReport>& reports);
};

// 报告过滤器
class ReportFilter {
public:
    static std::vector<TestItem> FilterByStatus(const std::vector<TestItem>& items, TestStatus status);
    static std::vector<TestItem> FilterByCategory(const std::vector<TestItem>& items, TestCategory category);
    static std::vector<TestItem> FilterByDuration(const std::vector<TestItem>& items, double min_duration, double max_duration);
    static std::vector<TestItem> FilterByMemoryUsage(const std::vector<TestItem>& items, SIZE_T min_memory, SIZE_T max_memory);
};

// 报告排序器
class ReportSorter {
public:
    static void SortByExecutionTime(std::vector<TestItem>& items, bool ascending = true);
    static void SortByMemoryUsage(std::vector<TestItem>& items, bool ascending = true);
    static void SortByStatus(std::vector<TestItem>& items);
    static void SortByCategory(std::vector<TestItem>& items);
};