#include <array>
#include <iostream>
#include <string>
#include <vector>

constexpr int BUCKET_COUNT = 257;

int char_code(const std::string& value, const int index) {
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

        radix_sort(values, buffer, bucket_left, bucket_right, index + 1);
    }
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
    radix_sort(values, buffer, 0, n - 1, 0);

    for (const std::string& value : values) {
        std::cout << value << '\n';
    }

    return 0;
}
