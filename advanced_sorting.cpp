#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <functional> // Needed for std::cref
#include <chrono>
#include <numeric>    // Needed for std::iota
#include <random>
#include <algorithm> // Only for std::min, std::shuffle in data generation
#include <exception> // Include for std::exception
#include <iomanip>   // Include for std::setw, std::left

// *** Include the header with the CONTEMPORARY sorting algorithms ***
#include "advanced_sorting.h"


const int NUM_RUNS = 10;
const int MAX_CONCURRENT_THREADS = 20; 

std::mutex cout_mutex;

std::vector<int> generate_random_array(int size) { 
    std::vector<int> arr(size); std::mt19937 rng(std::random_device{}()); std::uniform_int_distribution<int> dist(0, 1'000'000); std::generate(arr.begin(), arr.end(), [&]() { return dist(rng); }); return arr; }
std::vector<int> generate_reverse_sorted_array(int size) { 
    std::vector<int> arr(size); std::iota(arr.rbegin(), arr.rend(), 0); return arr; }
std::vector<int> generate_ascending_sorted_array(int size) { 
    std::vector<int> arr(size); std::iota(arr.begin(), arr.end(), 0); return arr; }
std::vector<int> generate_partially_sorted_array(int size) { 
    std::vector<int> arr(size); std::iota(arr.begin(), arr.end(), 0); if (size > 1) { std::mt19937 shuffle_rng(std::random_device{}()); std::shuffle(arr.begin(), arr.begin() + std::min(size, (size / 2) + (size % 2)), shuffle_rng); } return arr; }


template <typename SortFunc>
double average_sort_time(SortFunc sort_func, const std::vector<int>& original) { 
    double total_time = 0; if (original.empty()) return 0.0;
    for (int i = 0; i < NUM_RUNS; ++i) {
        std::vector<int> arr = original; auto start = std::chrono::high_resolution_clock::now();
        sort_func(arr); auto end = std::chrono::high_resolution_clock::now();
        total_time += std::chrono::duration<double, std::milli>(end - start).count();
    } return total_time / NUM_RUNS; }


//  Worker Function
void process_dataset(int size, const std::string& label, const std::vector<int>& data) {
    double lib_time = 0.0, tim_time = 0.0, cock_time = 0.0;
    double comb_time = 0.0, tourn_time = 0.0, intro_time = 0.0;
    std::string note_lib = "(STUB - From Scratch Required)";
    std::string note_tim = "(Simplified - From Scratch Required)";

    try {
        { std::lock_guard<std::mutex> lock(cout_mutex); std::cout << "  [" << label << "," << size << "] Starting Library Sort " << note_lib << "..." << std::flush; }
        lib_time = average_sort_time(library_sort, data); // Calling the stub

        { std::lock_guard<std::mutex> lock(cout_mutex); std::cout << " Done.\n  [" << label << "," << size << "] Starting Timsort " << note_tim << "..." << std::flush; }
        tim_time = average_sort_time(tim_sort, data); // Calling the simplified implementation

        { std::lock_guard<std::mutex> lock(cout_mutex); std::cout << " Done.\n  [" << label << "," << size << "] Starting Cocktail Shaker Sort..." << std::flush; }
        cock_time = average_sort_time(cocktail_shaker_sort, data); // Calling custom implementation

        { std::lock_guard<std::mutex> lock(cout_mutex); std::cout << " Done.\n  [" << label << "," << size << "] Starting Comb Sort..." << std::flush; }
        comb_time = average_sort_time(comb_sort, data); // Calling custom implementation

        { std::lock_guard<std::mutex> lock(cout_mutex); std::cout << " Done.\n  [" << label << "," << size << "] Starting Tournament Sort (Heap)..." << std::flush; }
        tourn_time = average_sort_time(tournament_sort, data); // Calling custom implementation

        { std::lock_guard<std::mutex> lock(cout_mutex); std::cout << " Done.\n  [" << label << "," << size << "] Starting Introsort (Custom)..." << std::flush; }
        intro_time = average_sort_time(introsort, data); // Calling custom implementation

        { std::lock_guard<std::mutex> lock(cout_mutex); std::cout << " Done." << std::endl; }


        // Format and print results
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "\n-- Results for " << label << " Data (Size: " << size << ", Avg over " << NUM_RUNS << " runs) --\n";
        std::cout << std::fixed << std::setprecision(3); 

        auto print_time = [&](const std::string& name, double time_ms, const std::string& note = "") {
            std::cout << std::left << std::setw(30) << (name + " " + note + ":") 
                      << std::setw(12) << std::right << time_ms << " ms\n";
        };

        print_time("Library Sort", lib_time, note_lib);
        print_time("Timsort", tim_time, note_tim);
        print_time("Cocktail Shaker Sort", cock_time);
        print_time("Comb Sort", comb_time);
        print_time("Tournament Sort (Heap)", tourn_time);
        print_time("Introsort (Custom)", intro_time);

    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cerr << "\n!!! Error processing dataset (" << label << ", Size: " << size << "): " << e.what() << std::endl;
    } catch (...) {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cerr << "\n!!! Unknown error processing dataset (" << label << ", Size: " << size << ")" << std::endl;
    }
}

int main() {
    std::vector<int> sizes = {1000, 10000, 100000, 500000, 1000000}; 

    unsigned int hardware_cores = std::thread::hardware_concurrency();
    std::cout << "Detected " << hardware_cores << " hardware threads (logical cores)." << std::endl;
    std::cout << "Running up to " << MAX_CONCURRENT_THREADS << " concurrent dataset processing tasks (max 4 per size)." << std::endl;
    std::cout << "Benchmarking CONTEMPORARY sorting algorithms." << std::endl;
    std::cout << "All sorting algorithms implemented FROM SCRATCH (except where noted)." << std::endl;
    std::cout << "Each test will run " << NUM_RUNS << " times to calculate average execution time." << std::endl;
    std::cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    std::cout << "!!! WARNING: Running Cocktail Shaker Sort (O(n^2) worst)   !!!" << std::endl;
    std::cout << "!!! This may take a VERY LONG TIME for sizes >= 100k.      !!!" << std::endl;
    std::cout << "!!! Library Sort is a STUB requiring manual implementation.!!!" << std::endl;
    std::cout << "!!! Timsort is a SIMPLIFIED version requiring review/impl. !!!" << std::endl;
    std::cout << "!!! Verify code against assignment rules on AI usage.      !!!" << std::endl;
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" << std::endl;


    for (int size : sizes) {
        std::cout << "\n=== Generating Data for Array Size: " << size << " ===\n" << std::flush;
        std::vector<std::pair<std::string, std::vector<int>>> datasets;
        try {
             // Generate all 4 dataset types
             datasets = {
                 {"Random", generate_random_array(size)},
                 {"Reverse Sorted", generate_reverse_sorted_array(size)},
                 {"Ascending Sorted", generate_ascending_sorted_array(size)}, // Added Ascending
                 {"Partially Sorted", generate_partially_sorted_array(size)}
             };
        } catch (const std::bad_alloc& e) {
             std::lock_guard<std::mutex> lock(cout_mutex);
             std::cerr << "Failed to allocate memory for datasets of size " << size << ". Skipping." << std::endl;
             continue; // Skip to next size
        }

        std::cout << "--- Processing Size: " << size << " ---\n" << std::flush;
        std::vector<std::thread> threads;
        // Launch a thread for each of the 4 datasets
        for (auto const& [label, data_vec] : datasets) {
            threads.emplace_back(process_dataset, size, label, std::cref(data_vec));
        }

        // Wait for threads for this size (now 4 threads)
        std::cout << "Waiting for " << threads.size() << " dataset tasks for size " << size << " to complete...\n" << std::flush;
        for (auto& t : threads) {
             if (t.joinable()) {
                 t.join();
             }
        }
        std::cout << "--- Finished Processing Size: " << size << " ---\n" << std::flush;
    }

    std::cout << "\n=== All Benchmarks Complete ===\n" << std::flush;
    return 0;
}