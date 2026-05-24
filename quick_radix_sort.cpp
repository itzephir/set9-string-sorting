#include <array>
#include <iostream>
#include <string>
#include <vector>

constexpr int BUCKET_COUNT = 257;
constexpr int SWITCH_THRESHOLD = 74;

void quick_radix_sort_range(
    std::vector<std::string>& values,
    std::vector<std::string>& buffer,
    int left,
    int right,
    int index
);

int char_at(const std::string& value, const int index) {
    if (index >= static_cast<int>(value.length())) {
        return -1;
    }

    return static_cast<unsigned char>(value[index]);
}

int char_code(const std::string& value, const int index) {
    return char_at(value, index) + 1;
}

void quick_sort(
    std::vector<std::string>& values,
    const int left,
    const int right,
    const int index
) {
    if (left >= right) {
        return;
    }

    const int pivot_index = left + (right - left) / 2;
    std::swap(values[left], values[pivot_index]);

    int less = left;
    int greater = right;
    int current = left + 1;
    const int pivot = char_at(values[left], index);

    while (current <= greater) {
        if (const int symbol = char_at(values[current], index); symbol < pivot) {
            std::swap(values[less], values[current]);
            ++less;
            ++current;
        } else if (symbol > pivot) {
            std::swap(values[current], values[greater]);
            --greater;
        } else {
            ++current;
        }
    }

    quick_sort(values, left, less - 1, index);
    if (pivot >= 0) {
        quick_sort(values, less, greater, index + 1);
    }
    quick_sort(values, greater + 1, right, index);
}

void radix_sort(
    std::vector<std::string>& values,
    std::vector<std::string>& buffer,
    const int left,
    const int right,
    const int index
) {
    if (left >= right) {
        return;
    }

    std::array<int, BUCKET_COUNT> count{};
    for (int i = left; i <= right; ++i) {
        ++count[char_code(values[i], index)];
    }

    std::array<int, BUCKET_COUNT> start{};
    start[0] = left;
    for (int code = 1; code < BUCKET_COUNT; ++code) {
        start[code] = start[code - 1] + count[code - 1];
    }

    std::array<int, BUCKET_COUNT> position = start;
    for (int i = left; i <= right; ++i) {
        const int code = char_code(values[i], index);
        buffer[position[code]] = values[i];
        ++position[code];
    }

    for (int i = left; i <= right; ++i) {
        values[i] = buffer[i];
    }

    for (int code = 1; code < BUCKET_COUNT; ++code) {
        const int bucket_left = start[code];
        const int bucket_right = bucket_left + count[code] - 1;

        quick_radix_sort_range(values, buffer, bucket_left, bucket_right, index + 1);
    }
}

void quick_radix_sort_range(
    std::vector<std::string>& values,
    std::vector<std::string>& buffer,
    const int left,
    const int right,
    const int index
) {
    if (left >= right) {
        return;
    }

    if (right - left + 1 < SWITCH_THRESHOLD) {
        quick_sort(values, left, right, index);
        return;
    }

    radix_sort(values, buffer, left, right, index);
}

int main() {
    int n = 0;
    std::cin >> n;

    std::string line;
    std::getline(std::cin, line);

    std::vector<std::string> values(n);
    for (int i = 0; i < n; ++i) {
        std::getline(std::cin, values[i]);
    }

    std::vector<std::string> buffer(n);
    quick_radix_sort_range(values, buffer, 0, n - 1, 0);

    for (const std::string& value : values) {
        std::cout << value << '\n';
    }

    return 0;
}
