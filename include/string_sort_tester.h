#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct SortMetrics {
    std::int64_t char_comparisons = 0;
    std::int64_t char_inspections = 0;
    std::int64_t radix_calls = 0;
    std::int64_t quick_switches = 0;
};

struct BenchmarkOptions {
    int max_size = 3000;
    int step = 100;
    int repeats = 30;
    unsigned int seed = 42;
    std::string results_dir = "results";
    std::string report_dir = "report";
};

struct BenchmarkRow {
    std::string dataset;
    std::string algorithm;
    int size = 0;
    int run = 0;
    std::int64_t time_us = 0;
    std::int64_t char_comparisons = 0;
    std::int64_t char_inspections = 0;
    std::int64_t radix_calls = 0;
    std::int64_t quick_switches = 0;
};

struct SummaryRow {
    std::string dataset;
    std::string algorithm;
    int size = 0;
    double avg_time_us = 0.0;
    double avg_char_comparisons = 0.0;
    double avg_char_inspections = 0.0;
    double avg_radix_calls = 0.0;
    double avg_quick_switches = 0.0;
};

class StringSortTester {
public:
    explicit StringSortTester(BenchmarkOptions options = {});

    void run() const;

private:
    BenchmarkOptions options_;

    [[nodiscard]] std::vector<BenchmarkRow> run_measurements() const;
    static std::vector<SummaryRow> summarize(const std::vector<BenchmarkRow>& rows);

    void write_measurements(const std::vector<BenchmarkRow>& rows) const;
    void write_summary(const std::vector<SummaryRow>& rows) const;
    void write_readme() const;

    static bool is_sorted_like_std_sort(const std::vector<std::string>& original,
                                        const std::vector<std::string>& sorted);
};
