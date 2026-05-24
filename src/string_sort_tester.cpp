#include "string_sort_tester.h"

#include "string_generator.h"
#include "string_sort_algorithms.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <stdexcept>
#include <tuple>
#include <utility>

namespace {
    struct DatasetCase {
        std::string name;
        std::vector<std::string> values;
    };

    struct SummaryAccumulator {
        double time_us = 0.0;
        double char_comparisons = 0.0;
        double char_inspections = 0.0;
        double radix_calls = 0.0;
        double quick_switches = 0.0;
        int count = 0;
    };

    std::string csv_escape(const std::string& value) {
        bool must_quote = false;
        for (const char symbol : value) {
            if (symbol == ',' || symbol == '"' || symbol == '\n' || symbol == '\r') {
                must_quote = true;
                break;
            }
        }

        if (!must_quote) {
            return value;
        }

        std::string escaped = "\"";
        for (const char symbol : value) {
            if (symbol == '"') {
                escaped += "\"\"";
            } else {
                escaped.push_back(symbol);
            }
        }
        escaped.push_back('"');
        return escaped;
    }

    std::vector<std::string> take_prefix(const std::vector<std::string>& values, const int size) {
        return {values.begin(), values.begin() + size};
    }
} // namespace

StringSortTester::StringSortTester(BenchmarkOptions options) : options_(std::move(options)) {}

void StringSortTester::run() const {
    std::filesystem::create_directories(options_.results_dir);
    std::filesystem::create_directories(options_.report_dir);

    const std::vector<BenchmarkRow> rows = run_measurements();
    const std::vector<SummaryRow> summary = summarize(rows);

    write_measurements(rows);
    write_summary(summary);
    write_readme();
}

std::vector<BenchmarkRow> StringSortTester::run_measurements() const {
    StringGenerator generator(options_.seed);
    const int nearly_swaps = std::max(1, options_.max_size * 3 / 100);

    const std::vector<DatasetCase> datasets = {
        {"random", generator.random_strings(options_.max_size)},
        {"reverse", generator.reverse_sorted_strings(options_.max_size)},
        {"nearly_sorted", generator.nearly_sorted_strings(options_.max_size, nearly_swaps)},
        {"prefix_25", generator.prefix_heavy_strings(options_.max_size, 25)},
        {"prefix_75", generator.prefix_heavy_strings(options_.max_size, 75)},
        {"prefix_150", generator.prefix_heavy_strings(options_.max_size, 150)},
    };

    std::vector<BenchmarkRow> rows;
    const std::vector<string_sorts::Algorithm> algorithms = string_sorts::algorithms();

    for (const auto& [name, values] : datasets) {
        for (int size = options_.step; size <= options_.max_size; size += options_.step) {
            const std::vector<std::string> original = take_prefix(values, size);
            std::vector<std::string> expected = original;
            std::sort(expected.begin(), expected.end());

            for (const string_sorts::Algorithm& algorithm : algorithms) {
                std::cerr << "benchmark " << name << " n=" << size
                    << " algorithm=" << algorithm.name << '\n';

                for (int run = 1; run <= options_.repeats; ++run) {
                    std::vector<std::string> basic_strings = original;
                    SortMetrics metrics;

                    const auto started = std::chrono::steady_clock::now();
                    algorithm.sort(basic_strings, metrics);
                    const auto finished = std::chrono::steady_clock::now();

                    if (basic_strings != expected) {
                        throw std::runtime_error(
                            "incorrect sort result for " + name + " / " + algorithm.name);
                    }

                    const auto elapsed =
                        std::chrono::duration_cast<std::chrono::microseconds>(finished - started)
                        .count();

                    rows.push_back({
                        name,
                        algorithm.name,
                        size,
                        run,
                        elapsed,
                        metrics.char_comparisons,
                        metrics.char_inspections,
                        metrics.radix_calls,
                        metrics.quick_switches,
                    });
                }
            }
        }
    }

    return rows;
}

std::vector<SummaryRow> StringSortTester::summarize(const std::vector<BenchmarkRow>& rows) {
    std::map<std::tuple<std::string, std::string, int>, SummaryAccumulator> grouped;

    for (const BenchmarkRow& row : rows) {
        auto& [
            time_us,
            char_comparisons,
            char_inspections,
            radix_calls,
            quick_switches,
            count
        ] = grouped[{row.dataset, row.algorithm, row.size}];
        time_us += static_cast<double>(row.time_us);
        char_comparisons += static_cast<double>(row.char_comparisons);
        char_inspections += static_cast<double>(row.char_inspections);
        radix_calls += static_cast<double>(row.radix_calls);
        quick_switches += static_cast<double>(row.quick_switches);
        ++count;
    }

    std::vector<SummaryRow> summary;
    summary.reserve(grouped.size());

    for (const auto& [key, accumulator] : grouped) {
        const auto& [dataset, algorithm, size] = key;
        summary.push_back({
            dataset,
            algorithm,
            size,
            accumulator.time_us / accumulator.count,
            accumulator.char_comparisons / accumulator.count,
            accumulator.char_inspections / accumulator.count,
            accumulator.radix_calls / accumulator.count,
            accumulator.quick_switches / accumulator.count,
        });
    }

    return summary;
}

void StringSortTester::write_measurements(const std::vector<BenchmarkRow>& rows) const {
    std::ofstream output(options_.results_dir + "/measurements.csv");
    output << "dataset,algorithm,size,run,time_us,char_comparisons,char_inspections,"
        "radix_calls,quick_switches\n";

    for (const BenchmarkRow& row : rows) {
        output << csv_escape(row.dataset) << ','
            << csv_escape(row.algorithm) << ','
            << row.size << ','
            << row.run << ','
            << row.time_us << ','
            << row.char_comparisons << ','
            << row.char_inspections << ','
            << row.radix_calls << ','
            << row.quick_switches << '\n';
    }
}

void StringSortTester::write_summary(const std::vector<SummaryRow>& rows) const {
    std::ofstream output(options_.results_dir + "/summary.csv");
    output << "dataset,algorithm,size,avg_time_us,avg_char_comparisons,"
        "avg_char_inspections,avg_radix_calls,avg_quick_switches\n";
    output << std::fixed << std::setprecision(3);

    for (const auto& [
             dataset,
             algorithm,
             size,
             avg_time_us,
             avg_char_comparisons,
             avg_char_inspections,
             avg_radix_calls,
             avg_quick_switches
         ] : rows) {
        output << csv_escape(dataset) << ','
            << csv_escape(algorithm) << ','
            << size << ','
            << avg_time_us << ','
            << avg_char_comparisons << ','
            << avg_char_inspections << ','
            << avg_radix_calls << ','
            << avg_quick_switches << '\n';
    }
}

void StringSortTester::write_readme() const {
    std::ofstream output(options_.results_dir + "/README.md");
    output << "# String Sorting Measurements\n\n"
        << "Generated by the benchmark target.\n\n"
        << "- `measurements.csv`: raw per-run measurements.\n"
        << "- `summary.csv`: averaged values grouped by dataset, algorithm, and size.\n"
        << "- Kotlin Notebook report: `../report/string_sorting_report.ipynb`.\n";
}

bool StringSortTester::is_sorted_like_std_sort(const std::vector<std::string>& original,
                                               const std::vector<std::string>& sorted) {
    std::vector<std::string> expected = original;
    std::sort(expected.begin(), expected.end());
    return expected == sorted;
}
