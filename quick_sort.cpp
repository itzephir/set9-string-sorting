#include <iostream>
#include <string>
#include <vector>

int char_at(const std::string& value, const int index) {
    if (index >= static_cast<int>(value.length())) {
        return -1;
    }

    return static_cast<unsigned char>(value[index]);
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

int main() {
    int n = 0;
    std::cin >> n;

    std::string line;
    std::getline(std::cin, line);

    std::vector<std::string> values(n);
    for (int i = 0; i < n; ++i) {
        std::getline(std::cin, values[i]);
    }

    quick_sort(values, 0, n - 1, 0);

    for (const std::string& value : values) {
        std::cout << value << '\n';
    }

    return 0;
}
