#pragma once

#include "string_sort_tester.h"

#include <string>
#include <vector>

namespace string_sorts {
    using SortFunction = void (*)(std::vector<std::string>&, SortMetrics&);

    struct Algorithm {
        const char* name;
        SortFunction sort;
    };

    std::vector<Algorithm> algorithms();

    void standard_quick_sort(std::vector<std::string>& values, SortMetrics& metrics);
    void standard_merge_sort(std::vector<std::string>& values, SortMetrics& metrics);
    void string_quick_sort(std::vector<std::string>& values, SortMetrics& metrics);
    void lcp_merge_sort(std::vector<std::string>& values, SortMetrics& metrics);
    void radix_sort(std::vector<std::string>& values, SortMetrics& metrics);
    void quick_radix_sort(std::vector<std::string>& values, SortMetrics& metrics);
} // namespace string_sorts
