#include "string_sort_algorithms.h"

#include <algorithm>
#include <array>

namespace string_sorts {
    namespace {
        constexpr int BUCKET_COUNT = 257;
        constexpr int SWITCH_THRESHOLD = 74;

        int compare_strings(
            const std::string& left,
            const std::string& right,
            SortMetrics& metrics
        ) {
            int index = 0;
            const int limit = std::min(
                static_cast<int>(left.length()),
                static_cast<int>(right.length())
            );

            while (index < limit) {
                ++metrics.char_comparisons;
                if (left[index] < right[index]) {
                    return -1;
                }
                if (left[index] > right[index]) {
                    return 1;
                }
                ++index;
            }

            if (left.length() == right.length()) {
                return 0;
            }

            return left.length() < right.length() ? -1 : 1;
        }

        void standard_quick_sort(
            std::vector<std::string>& values,
            const int left,
            const int right,
            SortMetrics& metrics
        ) {
            if (left >= right) {
                return;
            }

            const int pivot_index = left + (right - left) / 2;
            std::swap(values[left], values[pivot_index]);

            int less = left;
            int greater = right;
            int current = left + 1;
            const std::string pivot = values[left];

            while (current <= greater) {
                const int order = compare_strings(values[current], pivot, metrics);
                if (order < 0) {
                    std::swap(values[less], values[current]);
                    ++less;
                    ++current;
                } else if (order > 0) {
                    std::swap(values[current], values[greater]);
                    --greater;
                } else {
                    ++current;
                }
            }

            standard_quick_sort(values, left, less - 1, metrics);
            standard_quick_sort(values, greater + 1, right, metrics);
        }

        void standard_merge(
            std::vector<std::string>& values,
            std::vector<std::string>& buffer,
            const int left,
            const int middle,
            const int right,
            SortMetrics& metrics
        ) {
            int first = left;
            int second = middle;
            int index = left;

            while (first < middle && second < right) {
                if (compare_strings(values[first], values[second], metrics) <= 0) {
                    buffer[index++] = values[first++];
                } else {
                    buffer[index++] = values[second++];
                }
            }

            while (first < middle) {
                buffer[index++] = values[first++];
            }

            while (second < right) {
                buffer[index++] = values[second++];
            }

            for (int i = left; i < right; ++i) {
                values[i] = buffer[i];
            }
        }

        void standard_merge_sort(
            std::vector<std::string>& values,
            std::vector<std::string>& buffer,
            const int left,
            const int right,
            SortMetrics& metrics
        ) {
            if (right - left <= 1) {
                return;
            }

            const int middle = left + (right - left) / 2;
            standard_merge_sort(values, buffer, left, middle, metrics);
            standard_merge_sort(values, buffer, middle, right, metrics);
            standard_merge(values, buffer, left, middle, right, metrics);
        }

        int lcp_compare(const std::string& left, const std::string& right, SortMetrics& metrics) {
            std::size_t lcp = 0;
            const std::size_t limit = std::min(left.size(), right.size());

            while (lcp < limit) {
                ++metrics.char_comparisons;
                if (left[lcp] != right[lcp]) {
                    break;
                }
                ++lcp;
            }

            if (lcp == left.size() && lcp == right.size()) {
                return 0;
            }
            if (lcp == left.size()) {
                return -1;
            }
            if (lcp == right.size()) {
                return 1;
            }

            ++metrics.char_comparisons;
            return left[lcp] < right[lcp] ? -1 : 1;
        }

        void merge(
            std::vector<std::string>& values,
            std::vector<std::string>& buffer,
            const int left,
            const int middle,
            const int right,
            SortMetrics& metrics
        ) {
            int first = left;
            int second = middle;
            int index = left;

            while (first < middle && second < right) {
                if (lcp_compare(values[first], values[second], metrics) <= 0) {
                    buffer[index++] = values[first++];
                } else {
                    buffer[index++] = values[second++];
                }
            }

            while (first < middle) {
                buffer[index++] = values[first++];
            }

            while (second < right) {
                buffer[index++] = values[second++];
            }

            for (int i = left; i < right; ++i) {
                values[i] = buffer[i];
            }
        }

        void merge_sort(
            std::vector<std::string>& values,
            std::vector<std::string>& buffer,
            const int left,
            const int right,
            SortMetrics& metrics
        ) {
            if (right - left <= 1) {
                return;
            }

            const int middle = left + (right - left) / 2;
            merge_sort(values, buffer, left, middle, metrics);
            merge_sort(values, buffer, middle, right, metrics);
            merge(values, buffer, left, middle, right, metrics);
        }

        int char_at(const std::string& value, const int index, SortMetrics& metrics) {
            ++metrics.char_inspections;
            if (index >= static_cast<int>(value.length())) {
                return -1;
            }

            return static_cast<unsigned char>(value[index]);
        }

        bool less_symbol(const int left, const int right, SortMetrics& metrics) {
            if (left >= 0 && right >= 0) {
                ++metrics.char_comparisons;
            }

            return left < right;
        }

        bool greater_symbol(const int left, const int right, SortMetrics& metrics) {
            if (left >= 0 && right >= 0) {
                ++metrics.char_comparisons;
            }

            return left > right;
        }

        void quick_sort(
            std::vector<std::string>& values,
            const int left,
            const int right,
            const int index,
            SortMetrics& metrics
        ) {
            if (left >= right) {
                return;
            }

            const int pivot_index = left + (right - left) / 2;
            std::swap(values[left], values[pivot_index]);

            int less = left;
            int greater = right;
            int current = left + 1;
            const int pivot = char_at(values[left], index, metrics);

            while (current <= greater) {
                if (const int symbol = char_at(values[current], index, metrics);
                    less_symbol(symbol, pivot, metrics)) {
                    std::swap(values[less], values[current]);
                    ++less;
                    ++current;
                } else if (greater_symbol(symbol, pivot, metrics)) {
                    std::swap(values[current], values[greater]);
                    --greater;
                } else {
                    ++current;
                }
            }

            quick_sort(values, left, less - 1, index, metrics);
            if (pivot >= 0) {
                quick_sort(values, less, greater, index + 1, metrics);
            }
            quick_sort(values, greater + 1, right, index, metrics);
        }

        int char_code(const std::string& value, const int index, SortMetrics& metrics) {
            ++metrics.char_inspections;
            if (index >= static_cast<int>(value.length())) {
                return 0;
            }

            return static_cast<unsigned char>(value[index]) + 1;
        }

        void radix_sort(
            std::vector<std::string>& values,
            std::vector<std::string>& buffer,
            const int left,
            const int right,
            const int index,
            SortMetrics& metrics
        ) {
            if (left >= right) {
                return;
            }

            ++metrics.radix_calls;
            std::array<int, BUCKET_COUNT> count{};
            for (int i = left; i <= right; ++i) {
                ++count[char_code(values[i], index, metrics)];
            }

            std::array<int, BUCKET_COUNT> start{};
            start[0] = left;
            for (int code = 1; code < BUCKET_COUNT; ++code) {
                start[code] = start[code - 1] + count[code - 1];
            }

            std::array<int, BUCKET_COUNT> position = start;
            for (int i = left; i <= right; ++i) {
                const int code = char_code(values[i], index, metrics);
                buffer[position[code]] = values[i];
                ++position[code];
            }

            for (int i = left; i <= right; ++i) {
                values[i] = buffer[i];
            }

            for (int code = 1; code < BUCKET_COUNT; ++code) {
                const int bucket_left = start[code];
                const int bucket_right = bucket_left + count[code] - 1;

                radix_sort(values, buffer, bucket_left, bucket_right, index + 1, metrics);
            }
        }

        void quick_radix_sort_range(
            std::vector<std::string>& values,
            std::vector<std::string>& buffer,
            int left,
            int right,
            int index,
            SortMetrics& metrics
        );

        void radix_sort_with_quick_switch(
            std::vector<std::string>& values,
            std::vector<std::string>& buffer,
            const int left,
            const int right,
            const int index,
            SortMetrics& metrics
        ) {
            if (left >= right) {
                return;
            }

            ++metrics.radix_calls;
            std::array<int, BUCKET_COUNT> count{};
            for (int i = left; i <= right; ++i) {
                ++count[char_code(values[i], index, metrics)];
            }

            std::array<int, BUCKET_COUNT> start{};
            start[0] = left;
            for (int code = 1; code < BUCKET_COUNT; ++code) {
                start[code] = start[code - 1] + count[code - 1];
            }

            std::array<int, BUCKET_COUNT> position = start;
            for (int i = left; i <= right; ++i) {
                const int code = char_code(values[i], index, metrics);
                buffer[position[code]] = values[i];
                ++position[code];
            }

            for (int i = left; i <= right; ++i) {
                values[i] = buffer[i];
            }

            for (int code = 1; code < BUCKET_COUNT; ++code) {
                const int bucket_left = start[code];
                const int bucket_right = bucket_left + count[code] - 1;

                quick_radix_sort_range(values, buffer, bucket_left, bucket_right, index + 1, metrics);
            }
        }

        void quick_radix_sort_range(
            std::vector<std::string>& values,
            std::vector<std::string>& buffer,
            const int left,
            const int right,
            const int index,
            SortMetrics& metrics
        ) {
            if (left >= right) {
                return;
            }

            if (right - left + 1 < SWITCH_THRESHOLD) {
                ++metrics.quick_switches;
                quick_sort(values, left, right, index, metrics);
                return;
            }

            radix_sort_with_quick_switch(values, buffer, left, right, index, metrics);
        }
    } // namespace

    std::vector<Algorithm> algorithms() {
        return {
            {"standard_quicksort", standard_quick_sort},
            {"standard_mergesort", standard_merge_sort},
            {"string_quicksort", string_quick_sort},
            {"lcp_mergesort", lcp_merge_sort},
            {"msd_radix_sort", radix_sort},
            {"quick_msd_radix_sort", quick_radix_sort},
        };
    }

    void standard_quick_sort(std::vector<std::string>& values, SortMetrics& metrics) {
        standard_quick_sort(values, 0, static_cast<int>(values.size()) - 1, metrics);
    }

    void standard_merge_sort(std::vector<std::string>& values, SortMetrics& metrics) {
        std::vector<std::string> buffer(values.size());
        standard_merge_sort(values, buffer, 0, static_cast<int>(values.size()), metrics);
    }

    void string_quick_sort(std::vector<std::string>& values, SortMetrics& metrics) {
        quick_sort(values, 0, static_cast<int>(values.size()) - 1, 0, metrics);
    }

    void lcp_merge_sort(std::vector<std::string>& values, SortMetrics& metrics) {
        std::vector<std::string> buffer(values.size());
        merge_sort(values, buffer, 0, static_cast<int>(values.size()), metrics);
    }

    void radix_sort(std::vector<std::string>& values, SortMetrics& metrics) {
        std::vector<std::string> buffer(values.size());
        radix_sort(values, buffer, 0, static_cast<int>(values.size()) - 1, 0, metrics);
    }

    void quick_radix_sort(std::vector<std::string>& values, SortMetrics& metrics) {
        std::vector<std::string> buffer(values.size());
        quick_radix_sort_range(values, buffer, 0, static_cast<int>(values.size()) - 1, 0, metrics);
    }
} // namespace string_sorts
