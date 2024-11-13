#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>
#include <random>
#include <algorithm>
#include <limits>

int parallel_max(const std::vector<int>& numbers, int num_threads) {
    int total_max = std::numeric_limits<int>::min();

    #pragma omp parallel for num_threads(num_threads) reduction(max:total_max)
    for (size_t i = 0; i < numbers.size(); ++i) {
        total_max = std::max(total_max, numbers[i]);
    }

    return total_max;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <size> <num_threads>\n";
        return 1;
    }

    int size = std::stoi(argv[1]);
    int num_threads = std::stoi(argv[2]);

    // Test cases
    std::vector<std::vector<int>> test_cases;

    // Case 1: Random numbers
    std::vector<int> random_numbers(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 100);
    for (auto& num : random_numbers) {
        num = dis(gen);
    }
    test_cases.push_back(random_numbers);

    // Case 2: Uniform numbers
    std::vector<int> uniform_numbers(size, 1);
    test_cases.push_back(uniform_numbers);

    for (size_t i = 0; i < test_cases.size(); ++i) {
        std::cout << "\nTest Case " << i + 1 << ":\n";
        std::cout << "List size: " << test_cases[i].size() << "\n";

        // Regular max
        auto start = std::chrono::high_resolution_clock::now();
        int regular_max = *std::max_element(test_cases[i].begin(), test_cases[i].end());
        auto end = std::chrono::high_resolution_clock::now();
        double regular_time = std::chrono::duration<double>(end - start).count();

        std::cout << "Regular max: " << regular_max << "\n";
        std::cout << "Regular time: " << regular_time << " seconds\n";

        // Parallel max
        start = std::chrono::high_resolution_clock::now();
        int parallel_result = parallel_max(test_cases[i], num_threads);
        end = std::chrono::high_resolution_clock::now();
        double parallel_time = std::chrono::duration<double>(end - start).count();

        std::cout << "Parallel max: " << parallel_result << "\n";
        std::cout << "Parallel time: " << parallel_time << " seconds\n";
        std::cout << "Speed improvement: " << (regular_time / parallel_time) << "x\n";
    }

    return 0;
}