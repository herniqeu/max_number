#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <random>
#include <algorithm>
#include <string>
#include <limits>

class MaxThread {
private:
    const std::vector<int>& numbers;
    size_t start_index;
    size_t end_index;
    int partial_max;

public:
    MaxThread(const std::vector<int>& nums, size_t start, size_t end)
        : numbers(nums), start_index(start), end_index(end), partial_max(std::numeric_limits<int>::min()) {}

    void calculate() {
        partial_max = *std::max_element(numbers.begin() + start_index, numbers.begin() + end_index);
    }

    int getPartialMax() const { return partial_max; }
};

std::pair<int, double> parallel_max(const std::vector<int>& numbers, int num_threads) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    std::vector<std::thread> threads;
    std::vector<MaxThread> max_threads;
    
    size_t chunk_size = numbers.size() / num_threads;
    
    // Create and start threads
    for (int i = 0; i < num_threads; ++i) {
        size_t start_idx = i * chunk_size;
        size_t end_idx = (i == num_threads - 1) ? numbers.size() : (i + 1) * chunk_size;
        
        max_threads.emplace_back(numbers, start_idx, end_idx);
        threads.emplace_back(&MaxThread::calculate, &max_threads.back());
    }
    
    // Wait for all threads
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Calculate total max
    int total_max = std::numeric_limits<int>::min();
    for (const auto& max_thread : max_threads) {
        total_max = std::max(total_max, max_thread.getPartialMax());
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(end_time - start_time).count();
    
    return {total_max, duration};
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
        auto [parallel_result, parallel_time] = parallel_max(test_cases[i], num_threads);
        std::cout << "Parallel max: " << parallel_result << "\n";
        std::cout << "Parallel time: " << parallel_time << " seconds\n";
        std::cout << "Speed improvement: " << (regular_time / parallel_time) << "x\n";
    }
    
    return 0;
} 