#include "string_generator.h"

#include <algorithm>

StringGenerator::StringGenerator(const unsigned int seed) : generator_(seed) {}

const std::string& StringGenerator::alphabet() {
    static const std::string value =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789"
        "!@#%:;^&*()-";
    return value;
}

std::vector<std::string> StringGenerator::random_strings(const int size) {
    std::vector<std::string> values;
    values.reserve(size);

    for (int i = 0; i < size; ++i) {
        values.push_back(random_string());
    }

    return values;
}

std::vector<std::string> StringGenerator::reverse_sorted_strings(const int size) {
    std::vector<std::string> values = random_strings(size);
    std::ranges::sort(values, std::greater<>());
    return values;
}

std::vector<std::string> StringGenerator::nearly_sorted_strings(const int size, const int swap_count) {
    std::vector<std::string> values = random_strings(size);
    std::ranges::sort(values);

    if (values.empty()) {
        return values;
    }

    std::uniform_int_distribution index_distribution(0, size - 1);
    for (int i = 0; i < swap_count; ++i) {
        const int first = index_distribution(generator_);
        const int second = index_distribution(generator_);
        std::swap(values[first], values[second]);
    }

    return values;
}

std::vector<std::string> StringGenerator::prefix_heavy_strings(const int size, const int prefix_length) {
    std::string prefix;
    prefix.reserve(prefix_length);

    const std::string& symbols = alphabet();
    std::uniform_int_distribution char_distribution(0, static_cast<int>(symbols.length()) - 1);
    for (int i = 0; i < prefix_length; ++i) {
        prefix.push_back(symbols[char_distribution(generator_)]);
    }

    std::vector<std::string> values;
    values.reserve(size);
    for (int i = 0; i < size; ++i) {
        values.push_back(random_string_with_prefix(prefix));
    }

    return values;
}

std::string StringGenerator::random_string() {
    const std::string& symbols = alphabet();
    std::uniform_int_distribution length_distribution(10, 200);
    std::uniform_int_distribution char_distribution(0, static_cast<int>(symbols.length()) - 1);

    const int length = length_distribution(generator_);
    std::string value;
    value.reserve(length);

    for (int i = 0; i < length; ++i) {
        value.push_back(symbols[char_distribution(generator_)]);
    }

    return value;
}

std::string StringGenerator::random_string_with_prefix(const std::string& prefix) {
    const std::string& symbols = alphabet();
    const int min_length = std::max(10, static_cast<int>(prefix.length()));
    std::uniform_int_distribution length_distribution(min_length, 200);
    std::uniform_int_distribution char_distribution(0, static_cast<int>(symbols.length()) - 1);

    const int length = length_distribution(generator_);
    std::string value = prefix;
    value.reserve(length);

    while (static_cast<int>(value.length()) < length) {
        value.push_back(symbols[char_distribution(generator_)]);
    }

    return value;
}
