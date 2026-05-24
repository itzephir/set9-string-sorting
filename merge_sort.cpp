#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int lcpCompare(const std::string& left, const std::string& right) {
    std::size_t lcp = 0;
    const std::size_t limit = std::min(left.size(), right.size());

    while (lcp < limit && left[lcp] == right[lcp]) {
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

    return left[lcp] < right[lcp] ? -1 : 1;
}

void merge(
    std::vector<std::string>& values,
    std::vector<std::string>& buffer,
    const int left,
    const int middle,
    const int right
) {
    int first = left;
    int second = middle;
    int index = left;

    while (first < middle && second < right) {
        if (lcpCompare(values[first], values[second]) <= 0) {
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
    const int right
) {
    if (right - left <= 1) {
        return;
    }

    const int middle = left + (right - left) / 2;
    merge_sort(values, buffer, left, middle);
    merge_sort(values, buffer, middle, right);
    merge(values, buffer, left, middle, right);
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
    merge_sort(values, buffer, 0, n);

    for (const std::string& value : values) {
        std::cout << value << '\n';
    }

    return 0;
}
