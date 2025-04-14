#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <functional> // for std::cref
#include <chrono>
#include <numeric>    //  for std::iota
#include <random>
#include <algorithm>
#include <exception> // for std::exception
#include <iomanip>   //  std::setw, std::left (formatting output)

#include "sorting.h"

// Constants
const int NUM_RUNS = 10; 
const int MAX_CONCURRENT_THREADS = 20; 

// Mutex for protecting console output
std::mutex cout_mutex;


//  Function to generate random array
std::vector<int> generate_random_array(int size) {
    std::vector<int> arr(size);
    std::mt19937 rng(std::random_device{}()); 
    std::uniform_int_distribution<int> dist(0, 1'000'000);
    std::generate(arr.begin(), arr.end(), [&]() { return dist(rng); });
    return arr;
}

//  Function to generate descending array
std::vector<int> generate_reverse_sorted_array(int size) {
    std::vector<int> arr(size);
    std::iota(arr.rbegin(), arr.rend(), 0);
    return arr;
}

//  Function to generate ascending array
std::vector<int> generate_ascending_sorted_array(int size) {
    std::vector<int> arr(size);
    std::iota(arr.begin(), arr.end(), 0);
    return arr;
}

//  Function to generate shuffled array
std::vector<int> generate_partially_sorted_array(int size) {
    std::vector<int> arr(size);
    std::iota(arr.begin(), arr.end(), 0); // Start with sorted array
    if (size > 1) {
        // Shuffle the first half
        std::mt19937 shuffle_rng(std::random_device{}()); 
        std::shuffle(arr.begin(), arr.begin() + std::min(size, (size / 2) + (size % 2)), shuffle_rng);
    }
    return arr;
}


template <typename SortFunc>
double average_sort_time(SortFunc sort_func, const std::vector<int>& original) {
    double total_time = 0;
    if (original.empty()) return 0.0;

    for (int i = 0; i < NUM_RUNS; ++i) {
        std::vector<int> arr = original; // Necessary copy for sorting
        auto start = std::chrono::high_resolution_clock::now();
        sort_func(arr);
        auto end = std::chrono::high_resolution_clock::now();
        total_time += std::chrono::duration<double, std::milli>(end - start).count();
    }
    return total_time / NUM_RUNS;
}

void process_dataset(int size, const std::string& label, const std::vector<int>& data) {
    double bubble_time = 0.0, insertion_time = 0.0, selection_time = 0.0;
    double merge_time = 0.0, quick_time = 0.0, heap_time = 0.0;

    try {
        // --- Running ALL conventional sorting algorithms ---
        { std::lock_guard<std::mutex> lock(cout_mutex); std::cout << "  [" << label << "," << size << "] Starting Bubble Sort..." << std::flush; }
        bubble_time = average_sort_time(bubble_sort, data);

        { std::lock_guard<std::mutex> lock(cout_mutex); std::cout << " Done.\n  [" << label << "," << size << "] Starting Insertion Sort..." << std::flush; }
        insertion_time = average_sort_time(insertion_sort, data);

        { std::lock_guard<std::mutex> lock(cout_mutex); std::cout << " Done.\n  [" << label << "," << size << "] Starting Selection Sort..." << std::flush; }
        selection_time = average_sort_time(selection_sort, data);

        { std::lock_guard<std::mutex> lock(cout_mutex); std::cout << " Done.\n  [" << label << "," << size << "] Starting Merge Sort..." << std::flush; }
        merge_time = average_sort_time(
            [](std::vector<int>& arr) { if (!arr.empty()) mergeSort(arr, 0, arr.size() - 1); }, data);

        { std::lock_guard<std::mutex> lock(cout_mutex); std::cout << " Done.\n  [" << label << "," << size << "] Starting Quick Sort..." << std::flush; }
        quick_time = average_sort_time(
            [](std::vector<int>& arr) { if (!arr.empty()) quickSort(arr, 0, arr.size() - 1); }, data);

        { std::lock_guard<std::mutex> lock(cout_mutex); std::cout << " Done.\n  [" << label << "," << size << "] Starting Heap Sort..." << std::flush; }
        heap_time = average_sort_time(heapSort, data); 

        { std::lock_guard<std::mutex> lock(cout_mutex); std::cout << " Done." << std::endl; }


        // Format and print results
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "\n-- Results for " << label << " Data (Size: " << size << ") --\n";
        std::cout << std::fixed << std::setprecision(3);

        auto print_time = [&](const std::string& name, double time_ms) {
            std::cout << std::left << std::setw(16) << (name + ":")
                      << std::setw(12) << std::right << time_ms << " ms\n";
        };

        print_time("Bubble Sort", bubble_time);
        print_time("Insertion Sort", insertion_time);
        print_time("Selection Sort", selection_time);
        print_time("Merge Sort", merge_time);
        print_time("Quick Sort", quick_time);
        print_time("Heap Sort", heap_time);

    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cerr << "\n!!! Error processing dataset (" << label << ", Size: " << size << "): " << e.what() << std::endl;
    } catch (...) {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cerr << "\n!!! Unknown error processing dataset (" << label << ", Size: " << size << ")" << std::endl;
    }
}

// --- Main Function (Updated dataset list & info message) ---
int main() {
    std::vector<int> sizes = {1000, 10000, 100000, 500000, 1000000}; // Adjust as needed

    unsigned int hardware_cores = std::thread::hardware_concurrency();
    std::cout << "Detected " << hardware_cores << " hardware threads (logical cores)." << std::endl;
    // Update message: Now 4 concurrent tasks per size
    std::cout << "Running up to " << MAX_CONCURRENT_THREADS << " concurrent dataset processing tasks (max 4 per size)." << std::endl;
    std::cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    std::cout << "!!! WARNING: Running ALL sorts, including O(n^2) algorithms !!!" << std::endl;
    std::cout << "!!! (Bubble, Insertion, Selection) for all sizes.          !!!" << std::endl;
    std::cout << "!!! This will take an EXTREMELY LONG TIME for sizes >= 100k !!!" << std::endl;
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" << std::endl;


    for (int size : sizes) {
        std::cout << "\n=== Generating Data for Array Size: " << size << " ===\n" << std::flush;
        std::vector<std::pair<std::string, std::vector<int>>> datasets;
        try {
             datasets = {
                 {"Random", generate_random_array(size)},
                 {"Reverse Sorted", generate_reverse_sorted_array(size)},
                 {"Partially Sorted", generate_partially_sorted_array(size)},
                 {"Ascending Sorted", generate_ascending_sorted_array(size)} // New dataset type
             };
        } catch (const std::bad_alloc& e) {
             std::lock_guard<std::mutex> lock(cout_mutex);
             std::cerr << "Failed to allocate memory for datasets of size " << size << ". Skipping." << std::endl;
             continue; // Skip to next size
        }

        std::cout << "--- Processing Size: " << size << " ---\n" << std::flush;
        std::vector<std::thread> threads;
        // iterates through 4 datasets
        for (auto const& [label, data_vec] : datasets) {
            threads.emplace_back(process_dataset, size, label, std::cref(data_vec)); // Pass data by const ref
        }

      
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