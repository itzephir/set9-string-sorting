#include "string_sort_tester.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
    void print_usage(const char* program) {
        std::cout << "Usage: " << program << " [options]\n"
            << "Options:\n"
            << "  --max-size N       Maximum dataset size (default: 3000)\n"
            << "  --step N           Size step (default: 100)\n"
            << "  --repeats N        Repeats per case (default: 30)\n"
            << "  --seed N           Random seed (default: 42)\n"
            << "  --results-dir DIR  CSV output directory (default: results)\n"
            << "  --report-dir DIR   Report output directory (default: report)\n"
            << "  --smoke            Fast local check: max-size=200, step=100, repeats=1\n";
    }

    std::string require_value(const int argc, char* argv[], int& index) {
        if (index + 1 >= argc) {
            throw std::runtime_error(std::string("missing value for ") + argv[index]);
        }

        ++index;
        return argv[index];
    }

    BenchmarkOptions parse_options(const int argc, char* argv[]) {
        BenchmarkOptions options;

        for (int i = 1; i < argc; ++i) {
            const std::string argument = argv[i];

            if (argument == "--help" || argument == "-h") {
                print_usage(argv[0]);
                std::exit(0);
            }
            if (argument == "--max-size") {
                options.max_size = std::stoi(require_value(argc, argv, i));
            } else if (argument == "--step") {
                options.step = std::stoi(require_value(argc, argv, i));
            } else if (argument == "--repeats") {
                options.repeats = std::stoi(require_value(argc, argv, i));
            } else if (argument == "--seed") {
                options.seed = static_cast<unsigned int>(std::stoul(require_value(argc, argv, i)));
            } else if (argument == "--results-dir") {
                options.results_dir = require_value(argc, argv, i);
            } else if (argument == "--report-dir") {
                options.report_dir = require_value(argc, argv, i);
            } else if (argument == "--smoke") {
                options.max_size = 200;
                options.step = 100;
                options.repeats = 1;
            } else {
                throw std::runtime_error("unknown option: " + argument);
            }
        }

        if (options.max_size <= 0 || options.step <= 0 || options.repeats <= 0) {
            throw std::runtime_error("max-size, step, and repeats must be positive");
        }
        if (options.max_size % options.step != 0) {
            throw std::runtime_error("max-size must be divisible by step");
        }

        return options;
    }
} // namespace

int main(int argc, char* argv[]) {
    try {
        const BenchmarkOptions options = parse_options(argc, argv);
        const StringSortTester tester(options);
        tester.run();

        std::cout << "benchmark finished.\n"
            << "Measurements: " << options.results_dir << "/measurements.csv\n"
            << "Summary: " << options.results_dir << "/summary.csv\n"
            << "Notebook: " << options.report_dir << "/string_sorting_report.ipynb\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "error: " << error.what() << '\n';
        return 1;
    }
}
