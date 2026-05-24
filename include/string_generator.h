#pragma once

#include <random>
#include <string>
#include <vector>

class StringGenerator {
public:
    explicit StringGenerator(unsigned int seed = 42);

    std::vector<std::string> random_strings(int size);
    std::vector<std::string> reverse_sorted_strings(int size);
    std::vector<std::string> nearly_sorted_strings(int size, int swap_count);
    std::vector<std::string> prefix_heavy_strings(int size, int prefix_length);

    static const std::string& alphabet();

private:
    std::string random_string();
    std::string random_string_with_prefix(const std::string& prefix);

    std::mt19937 generator_;
};
